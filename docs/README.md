# Quantum Script Extension Base64 — Documentation

`quantum-script--base64` is the **Base64 encoding extension of Quantum
Script**. Loaded with `Script.requireExtension("Base64")`, it adds a `Base64`
object to scripts with three functions:

```javascript
Base64.encode(str);           // bytes  -> "SGVsbG8=" (RFC 4648 alphabet, "=" padded)
Base64.decode(str);           // Base64 -> String of bytes, undefined if invalid
Base64.decodeToBuffer(str);   // Base64 -> Buffer of bytes, undefined if invalid
```

Base64 (RFC 4648 section 4) writes every 3 bytes as 4 characters from the
alphabet `A-Z a-z 0-9 + /`. It is the most compact of the common text
encodings (about 1.33x the input) and the one most other software expects:
e-mail attachments (MIME), PEM keys and certificates, `data:` URLs, HTTP
`Basic` authorization, JSON and XML payloads that carry binary data, web
APIs.

- **Encode bytes, not characters.** The argument is converted to a string
  and its bytes are encoded: `"ă"` (UTF-8 `c4 83`) encodes to `"xIM="`.
  A `Buffer` argument is encoded byte for byte.
- **Canonical output.** `encode` always uses the standard alphabet (`+` and
  `/`, not the URL safe `-` and `_`), pads with `=` to a multiple of 4
  characters and never inserts line breaks.
- **Strict decoding.** `decode` accepts exactly that form: length a multiple
  of 4, only `A-Z a-z 0-9 + /`, `=` padding (one or two) only at the end of
  the last group. Missing padding, line breaks, spaces or the URL safe
  alphabet make it return `undefined`. There is no partial result.
- **Two output types.** `decode` returns a `String` (bytes kept as they
  are), `decodeToBuffer` a `Buffer` from the `Buffer` extension, which the
  binary APIs (`File.writeFromBuffer`, `Shell.filePutContentsBuffer`,
  `Crypt.decrypt`, ...) expect.

```
scripts: quantum-script .js, magnet, embedding hosts, ...
quantum-script--base64    <-- this extension: Base64.encode / decode / decodeToBuffer
quantum-script--buffer    (the Buffer type returned by decodeToBuffer, loaded automatically)
quantum-script            (Executive, Variable, Context)
xyo-encoding              (XYO::Encoding::Base64::encode / decode, the codec)
xyo-system, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `Base64` gives |
|------|---------------------|
| Put binary data (keys, digests, images, files) into text: JSON, XML, INI, command lines, logs | `Base64.encode(buffer)` — compact, printable, no quotes or `\` to escape |
| Talk to other software: MIME, PEM, `data:` URLs, HTTP `Authorization: Basic`, REST APIs | the standard RFC 4648 alphabet and padding everybody reads |
| Read Base64 produced by other tools | `Base64.decode(text)` after removing line breaks or mapping the URL safe alphabet (see [Script API](script-api.md#recipes)) |
| Validate Base64 input | `decode` returns `undefined` on any invalid input |
| Get bytes for binary APIs | `Base64.decodeToBuffer(text)` returns a `Buffer` |
| Same API as the other codecs | `Base16` and `Base32` have the same three functions |

Size compared to the input: Base16 2x, Base32 1.6x, **Base64 ~1.33x**.
Base64 is the smallest, but it is case-sensitive and uses `+` `/` `=`, so
it is not suited for case-insensitive file systems or DNS (use `Base32`)
or for text people type (use `Base16` or `Base32`).

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Base64");` | also loads `Buffer` |
| Encode a string | `Base64.encode("foo")` | `"Zm9v"` |
| Encode a buffer | `Base64.encode(buffer)` | first `length` bytes |
| Decode to a string | `Base64.decode("Zm9v")` | `"foo"`; `undefined` if invalid |
| Decode to a buffer | `Base64.decodeToBuffer("AP8Q")` | `Buffer`, `length` 3; `undefined` if invalid |
| Check for failure | `Script.isUndefined(r)` | not `!r`: decoding `""` gives `""`, which is falsy |
| Decode wrapped (MIME / PEM) input | remove `\r` and `\n` first | `decode("Zm9v\nYmFy")` is `undefined` |
| Decode URL safe / unpadded input | map `-` `_` to `+` `/`, re-pad with `=` | `decode("-_8Q")` is `undefined` |
| Output length | `Math.ceil(n / 3) * 4` characters for `n` bytes | always a multiple of 4 |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script, magnet and fabricare scripts, register it in a C++ host |
| [Script API](script-api.md) | Every function: argument conversion, exact validation rules, edge cases, recipes |
| [C++ API](cpp-api.md) | `registerInternalExtension`, `initExecutive`, the DLL entry point, using the codec directly from C++ |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the `Buffer` type in the `quantum-script--buffer` repository,
`docs/`; the codec in the `xyo-encoding` repository, `docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/Base64.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Base64.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Base64/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         encode / decode / decodeToBuffer
    Copyright / License / Version                        library metadata
test/test.01.cpp                                         C++ host registering Console, Buffer, Base64 as internal
test/test.01.js                                          RFC 4648 test vectors, encode and decode
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--base64/`](../.claude/skills/quantum-script--base64/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `Base64`
(Quantum Script tools, magnet scripts, other extensions).
