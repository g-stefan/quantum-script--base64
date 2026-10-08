---
name: quantum-script--base64
description: >-
  How to use the Quantum Script Base64 extension (quantum-script--base64),
  the RFC 4648 Base64 codec loaded with Script.requireExtension("Base64"):
  Base64.encode(str) (bytes -> standard alphabet A-Z a-z 0-9 + /, "="
  padded to a multiple of 4, one line, never fails, non-strings are encoded
  as their text: encode(255) is "MjU1"), Base64.decode(str) (strict: length
  a multiple of 4, case-sensitive, groups xxxx / xxx= / xx==, padding in the
  last group only, no line breaks, no URL safe - _, otherwise undefined;
  returns a String of bytes) and Base64.decodeToBuffer(str) (same, returns a
  Buffer); removing MIME / PEM line breaks, converting URL safe / unpadded
  Base64 (JWT), HTTP Basic auth, data: URLs, binary in JSON; testing failure
  with Script.isUndefined (decode("") is "" which is falsy); Base16 / Base32
  siblings; which hosts have it (quantum-script, magnet; NOT fabricare build
  scripts); the C++ side (registerInternalExtension together with Buffer,
  initExecutive, quantumScriptExtension entry point,
  XYO::Encoding::Base64). Use when writing or reviewing Quantum Script code
  that converts bytes to / from Base64, C++ code that includes
  <XYO/QuantumScript.Extension/Base64.hpp>, a fabricare.json depending on
  "quantum-script--base64", or when working inside the
  quantum-script--base64 repository.
---

# quantum-script--base64

Base64 (RFC 4648 section 4) codec extension of Quantum Script (see the
`quantum-script` skill for the language and its differences from
JavaScript, and the `quantum-script--buffer` skill for the `Buffer` type;
their rules apply). Purpose: **turn binary data into compact printable text
and back** — for JSON / XML / INI payloads, MIME and PEM, `data:` URLs,
HTTP `Basic` authorization, web APIs — with strict validation on input.
Output is ~1.33x the input (Base16 2x, Base32 1.6x).

Full documentation: `docs/` in the quantum-script--base64 repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--base64\docs` on this machine):
README (purpose), getting-started (build, load, hosts, C++ registration),
**script-api** (format, exact validation, argument conversion, recipes),
cpp-api, reference. When in doubt read
`source/XYO/QuantumScript.Extension/Base64/Library.cpp` (~75 lines) and
`XYO::Encoding::Base64` (`source/XYO/Encoding/Base64.cpp`) in the
xyo-encoding repository.

## Script API

```javascript
Script.requireExtension("Base64");          // also loads Buffer

Base64.encode("foo");                       // "Zm9v"      A-Z a-z 0-9 + /, "=" padded, one line, never fails
Base64.encode(buffer);                      // Base64 of the first buffer.length bytes
Base64.decode("SGVsbG8=");                  // "Hello"     String of bytes, or undefined
Base64.decodeToBuffer("AP8Q");              // Buffer 00 ff 10 (size == length == 3), or undefined
```

Padding by bytes in the last 3-byte group: 1 → `==`, 2 → `=`, 3 → none.
Length for `n` bytes: `Math.ceil(n / 3) * 4`.

## Hard rules

1. **Decoding is strict, failure is `undefined`.** Valid input is exactly
   what `encode` produces: length a multiple of 4, only `A-Z a-z 0-9 + /`,
   each group `xxxx`, `xxx=` or `xx==`, padding only in the last group.
   Unpadded text, spaces, `\r` / `\n`, URL safe `-` `_`, `x===`, letters
   after `=`, or two padded values concatenated → `undefined`, no
   exception, no partial result.
2. **Normalize foreign input first.** MIME / PEM: `replace("\r", "")` and
   `replace("\n", "")` (replace replaces all; drop PEM `-----BEGIN/END`
   lines too). URL safe (JWT): `replace("-", "+").replace("_", "/")`, then
   re-pad with `=` to a multiple of 4; a remainder of 1 is invalid.
3. **Letter case is data.** Never upper- or lower-case Base64 text:
   `"ZM9V"` is valid but is not `"foo"`.
4. **Test failure with `Script.isUndefined(r)`**, never `!r`:
   `Base64.decode("")` is `""` and `Base64.decodeToBuffer("")` is an empty
   Buffer — both falsy but valid.
5. **The argument is converted with `toString` first.** `encode(255)` is
   `"MjU1"` (the text "255"); `encode()` encodes the text `"undefined"`;
   `decode()` is `undefined` (9 characters). To encode raw bytes, use a
   Buffer: `var b = Buffer(1); b.setU8(0, 255); Base64.encode(b);` →
   `"/w=="`. String literals cannot hold `\0`; use `Buffer.fromHex("610062")`.
6. **Bytes, not characters.** `Base64.encode("ă")` is `"xIM="` (UTF-8);
   `decode` keeps zero bytes and does not validate UTF-8. Use
   `decodeToBuffer` when the result is binary.
7. **Unused trailing bits are not checked**: `"Zh=="` and `"Zg=="` both
   decode to `"f"`. Compare decoded bytes, not Base64 text, when checking
   equality of foreign input.
8. **`encode` never wraps lines.** For 76-character MIME lines split it
   yourself; `substring(start, length)` takes a length.
9. **Not available in fabricare build scripts.** fabricare is a static host
   without `Base64`; `requireExtension("Base64")` throws
   `Unable to open "Base64"` there. Use hex (`Buffer.fromString(s).toHex()`
   / `Buffer.fromHex(h)`) instead. Available in the `quantum-script`
   interpreter (DLL), `magnet`, and hosts that register it.
10. `JSON.encode` turns a Buffer into `null`: store `Base64.encode(buffer)`
    and restore with `Base64.decodeToBuffer`.
11. Same three functions exist in `Base16` and `Base32`; keep usage
    symmetric when switching codecs. Base64 is not encryption.
12. One engine per thread: each thread requires `Base64` itself; the
    functions are stateless.

## Recipes

```javascript
var bytes = Base64.decodeToBuffer(pem.replace("\r", "").replace("\n", ""));   // MIME / PEM body

function fromUrlSafe(text) {                                 // JWT, URL tokens
	text = text.replace("-", "+").replace("_", "/");
	var rest = text.length % 4;
	if (rest == 1) { return undefined; };
	if (rest) { text += "==".substring(0, 4 - rest); };
	return Base64.decodeToBuffer(text);
};
var token = Base64.encode(bytes).replace("+", "-").replace("/", "_").replace("=", "");   // to URL safe

var auth = "Authorization: Basic " + Base64.encode(user + ":" + password);
var url = "data:image/png;base64," + Base64.encode(Shell.fileGetContentsBuffer("icon.png"));
JSON.encode({key: Base64.encode(key)});                      // binary in JSON

var key = Base64.decodeToBuffer(text);                       // validate a 32 byte key
if (Script.isUndefined(key) || key.length != 32) { throw "expected a Base64 key of 32 bytes"; };
```

## C++

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Base64.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                 // host init callback
	Extension::Buffer::registerInternalExtension(executive);  // Base64 requires Buffer
	Extension::Base64::registerInternalExtension(executive);  // scripts still requireExtension("Base64")
};

// without the script engine:
XYO::Encoding::String text = XYO::Encoding::Base64::encode(bytes);   // "=" padded
XYO::Encoding::String out;
bool ok = XYO::Encoding::Base64::decode(text, out);                 // strict, out unchanged on failure
```

- fabricare.json dependency: `"quantum-script--base64"` (`dll-or-lib`: DLL
  on dynamic platforms, static lib on `*.static` platforms). It pulls in
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer`.
- DLL entry point `extern "C" quantumScriptExtension(Executive *, void *)`
  only with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` and without
  `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY`. Static hosts must register
  the extension as internal.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, which
  registers Console, Buffer and Base64 as internal extensions and checks the
  RFC 4648 vectors in `test/test.01.js`; run `make` first),
  `fabricare install` (see the `fabricare` skill). `quantum-script`,
  `quantum-script--console` and `quantum-script--buffer` must be installed
  first. With the SDK installed, a script can also be run directly:
  `quantum-script test/test.01.js` (uses the installed DLL).
- Native functions live in `Base64/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`
  and are registered in `initExecutive` with
  `executive->setFunction2("Base64.name(args)", name)`.
- Validation rules come from `xyo-encoding`; if they change there, update
  `docs/script-api.md`, `docs/reference.md` and this skill.
- New functions: update `README.md`, `docs/script-api.md`,
  `docs/reference.md`, `test/test.01.js` and this skill; keep the API in
  step with quantum-script--base16 / --base32.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
