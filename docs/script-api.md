# Script API

Everything the extension defines after `Script.requireExtension("Base64")`.
`Base64` is a plain object holding three native functions; it has no
constructor and no state.

Every function takes one argument and converts it to a string with the
engine's `toString` first. A Quantum Script string is a sequence of bytes
(UTF-8 by convention, zero bytes allowed), and Base64 works on those bytes.

## The format

Base64 (RFC 4648 section 4) splits the input into groups of 3 bytes (24
bits) and writes each group as 4 characters of 6 bits each, from the
alphabet:

```
value  0-25   26-51   52-61   62   63
char   A-Z    a-z     0-9     +    /
```

The last group is completed with `=` so that the output is always a
multiple of 4 characters. The amount of padding depends on how many bytes
the last group holds:

| Bytes in last group | Characters | Padding | Example |
|---------------------|------------|---------|---------|
| 1 | 2 | `==` (2) | `"f"` → `"Zg=="` |
| 2 | 3 | `=` (1) | `"fo"` → `"Zm8="` |
| 3 | 4 | none | `"foo"` → `"Zm9v"` |

Encoded length for `n` bytes: `Math.ceil(n / 3) * 4`.

## `Base64.encode(str)`

Returns a `String` with the bytes of `str` in Base64: standard alphabet
`A-Z a-z 0-9 + /`, padded with `=` as in the table above, all on one line
(no line breaks, whatever the length). It never fails.

```javascript
Base64.encode("");               // ""
Base64.encode("f");              // "Zg=="
Base64.encode("foobar");         // "Zm9vYmFy"
Base64.encode("Hello");          // "SGVsbG8="
Base64.encode("Hello, World!");  // "SGVsbG8sIFdvcmxkIQ=="
Base64.encode("ă");              // "xIM="  (UTF-8 bytes c4 83)
```

Argument conversion — anything that is not a string is encoded as its text
form, which is rarely what you want:

| Argument | Encoded text | Result |
|----------|--------------|--------|
| `Buffer` | its first `length` bytes | `Base64.encode(Buffer.fromHex("00ff10"))` → `"AP8Q"` |
| number | decimal text | `Base64.encode(255)` → `"MjU1"` (the text `"255"`) |
| `true` / `null` | `"true"` / `"null"` | `"dHJ1ZQ=="` / `"bnVsbA=="` |
| missing / `undefined` | `"undefined"` | `"dW5kZWZpbmVk"` — no error |
| array | elements joined with `,` | `Base64.encode([1, 2])` → `"MSwy"` |

To encode binary data or the value of a number as bytes, put it into a
buffer: `var b = Buffer(1); b.setU8(0, 255); Base64.encode(b);` → `"/w=="`.
String literals cannot hold a zero byte (`"a\0b"` ends at the `\0`); build
such data with `Buffer.fromHex("610062")` (→ `"YQBi"`).

## `Base64.decode(str)`

Decodes Base64 text and returns the bytes as a `String`, or **`undefined`**
when the text is not valid. Validation is strict — the input must look
exactly like `encode` output:

- the length is a **multiple of 4** (unpadded input is rejected);
- every character before the padding is in `A-Z a-z 0-9 + /`, letter case
  matters (spaces, `\r`, `\n`, tabs, the URL safe `-` and `_` are
  rejected);
- a group of 4 is `xxxx`, `xxx=` or `xx==` — `x===` and `====` cannot come
  from any input and are rejected;
- after the first `=` there are only `=`, and only the **last** group of 4
  may contain padding (no concatenated padded values).

```javascript
Base64.decode("Zm9v");              // "foo"
Base64.decode("SGVsbG8=");          // "Hello"
Base64.decode("");                  // ""  (valid, empty)
Base64.decode("SGVsbG8");           // undefined: not padded to 4
Base64.decode("Zg");                // undefined: not padded to 4
Base64.decode("Zm9 v");             // undefined: space
Base64.decode("Zm9v\n");            // undefined: line break
Base64.decode("Zm9v\r\nYmFy");      // undefined: MIME / PEM line break
Base64.decode("-_8Q");              // undefined: URL safe alphabet
Base64.decode("Z===");              // undefined: 3 "=" is not a valid padding
Base64.decode("====");              // undefined
Base64.decode("Z=g=");              // undefined: letters after "="
Base64.decode("Zg==Zm9v");          // undefined: padding before the last group
Base64.decode();                    // undefined: "undefined" has 9 characters
```

- Decoding is all or nothing: there is no partial result and no error
  message, only `undefined`.
- **Letter case is data.** `"ZM9V"` is valid Base64 too, it just decodes to
  different bytes than `"Zm9v"`. Never change the case of Base64 text.
- Zero bytes are kept: `Base64.decode("YQBi").length` is `3`
  (`a`, `0x00`, `b`).
- The result is not checked for UTF-8: decoding a digest gives a string of
  raw bytes. Use `decodeToBuffer` when the data is binary.
- The unused low bits of the last character are ignored, not checked:
  `"Zh=="` decodes to `"f"` just like `"Zg=="`. So
  `Base64.encode(Base64.decode(t)) == t` holds for `encode` output but not
  for every accepted `t`; compare decoded bytes, not text, when that
  matters.
- **Test for failure with `Script.isUndefined(r)`** (or `r === undefined`),
  not with `!r`: a valid empty input decodes to `""`, which is falsy too.

## `Base64.decodeToBuffer(str)`

Same validation as `decode`, but returns the bytes as a **`Buffer`** (from
the `Buffer` extension), or `undefined` when the text is not valid Base64.

```javascript
var b = Base64.decodeToBuffer("AP8Q");
typeof(b);       // "Buffer"
b.length;        // 3
b.size;          // 3
b.getU8(1);      // 255
b.toHex();       // "00ff10"

Base64.decodeToBuffer("xyz");  // undefined
Base64.decodeToBuffer("");     // empty Buffer, length 0
```

- The buffer is new: `size == length ==` number of decoded bytes.
- An empty buffer is falsy (`if (b)` tests `length > 0`): use
  `Script.isUndefined(b)` to detect invalid input here too.
- Use it to feed binary APIs: `File.writeFromBuffer`, `Socket.writeFromBuffer`,
  `Shell.filePutContentsBuffer`, `Crypt.decrypt`, `OpenSSL` functions.

## Recipes

`String.prototype.replace` replaces every occurrence, and
`substring(start, length)` takes a length, not an end index.

### Decode MIME / PEM text (line breaks)

E-mail attachments and PEM files wrap Base64 at 64 or 76 characters per
line. Remove the line breaks before decoding:

```javascript
var flat = text.replace("\r", "").replace("\n", "");
var bytes = Base64.decodeToBuffer(flat);
```

For a PEM file, also drop the `-----BEGIN ...-----` / `-----END ...-----`
lines first.

### Write wrapped output

`encode` returns one line. To produce MIME style lines of 76 characters:

```javascript
function wrap(text, width) {
	var out = "";
	for (var i = 0; i < text.length; i += width) {
		out += text.substring(i, width) + "\r\n";
	};
	return out;
};

wrap(Base64.encode(data), 76);
```

### URL safe Base64 (JWT, URLs, file names)

The URL safe variant (RFC 4648 section 5) uses `-` and `_` instead of `+`
and `/`, and usually drops the padding. Convert both ways:

```javascript
function toUrlSafe(bytes) {
	return Base64.encode(bytes).replace("+", "-").replace("/", "_").replace("=", "");
};

function fromUrlSafe(text) {
	text = text.replace("-", "+").replace("_", "/");
	var rest = text.length % 4;
	if (rest == 1) {
		return undefined;   // no input length gives this
	};
	if (rest) {
		text += "==".substring(0, 4 - rest);
	};
	return Base64.decodeToBuffer(text);
};

toUrlSafe(Buffer.fromHex("fbff10ab"));   // "-_8Qqw"
fromUrlSafe("-_8Qqw").toHex();           // "fbff10ab"
```

### HTTP Basic authorization

```javascript
var header = "Authorization: Basic " + Base64.encode(user + ":" + password);
// user "user", password "pass" -> "Authorization: Basic dXNlcjpwYXNz"
```

Base64 is an encoding, not encryption: anyone who sees the header can
decode it.

### `data:` URL from a file

```javascript
Script.requireExtension("Shell");
Script.requireExtension("Base64");

var png = Shell.fileGetContentsBuffer("icon.png");
var url = "data:image/png;base64," + Base64.encode(png);
```

### Store binary data in JSON

`JSON.encode` turns a `Buffer` into `null`; store its Base64 form instead.

```javascript
Script.requireExtension("JSON");
Script.requireExtension("Base64");

var key = Base64.decodeToBuffer("iMQBUg==");          // 4 bytes 88 c4 01 52
var text = JSON.encode({key: Base64.encode(key)});    // {"key":"iMQBUg=="}
var back = Base64.decodeToBuffer(JSON.decode(text).key);
```

### Validate a key of a given size

```javascript
function parseKey(text, bytes) {
	var key = Base64.decodeToBuffer(text);
	if (Script.isUndefined(key) || key.length != bytes) {
		throw "expected a Base64 key of " + bytes + " bytes";
	};
	return key;
};

var key = parseKey("SSm9QMOayXTLouyf/kRjOkrZkkqpMNtKI8jV3IMFjuc=", 32);
```

### Write a binary file from Base64

```javascript
Script.requireExtension("Shell");
Script.requireExtension("Base64");

Shell.filePutContentsBuffer("magic.bin", Base64.decodeToBuffer("iVBORw0KGgo="));
// the 8 byte PNG signature 89 50 4e 47 0d 0a 1a 0a
```

### Round trip

For any string `s`: `Base64.decode(Base64.encode(s)) == s`. For any `t`
produced by `encode`: `Base64.encode(Base64.decode(t)) == t`.

## Related extensions

| Extension | Same three functions | Output alphabet | Decoding |
|-----------|----------------------|-----------------|----------|
| `Base16` | `encode`, `decode`, `decodeToBuffer` | `0-9 A-F`, no padding | case-insensitive |
| `Base32` | `encode`, `decode`, `decodeToBuffer` | `A-Z 2-7`, `=` padding | uppercase only, padding required |
| `Base64` | `encode`, `decode`, `decodeToBuffer` | `A-Z a-z 0-9 + /`, `=` padding | case-sensitive, padding required |
| `Buffer` | `Buffer.fromHex(str)`, `b.toHex()` | lowercase hex | lenient (invalid digit → 0) |
