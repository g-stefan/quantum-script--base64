# API reference

## Script

Available after `Script.requireExtension("Base64")` (which also loads
`Buffer`).

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Base64.encode(str)` | String | bytes of `toString(str)` in RFC 4648 Base64, `A-Z a-z 0-9 + /`, `=` padded to a multiple of 4, one line; never fails |
| `Base64.decode(str)` | String or `undefined` | length a multiple of 4, `A-Z a-z 0-9 + /` (case-sensitive), groups `xxxx` / `xxx=` / `xx==`, padding in the last group only; otherwise `undefined` |
| `Base64.decodeToBuffer(str)` | Buffer or `undefined` | same validation as `decode`; `size == length` = decoded bytes |

### Edge cases

| Expression | Result |
|------------|--------|
| `Base64.encode("")` | `""` |
| `Base64.encode("Hello")` | `"SGVsbG8="` |
| `Base64.encode(buffer)` | Base64 of the first `length` bytes |
| `Base64.encode(Buffer.fromHex("fbff"))` | `"+/8="` |
| `Base64.encode(255)` | `"MjU1"` (the text `"255"`) |
| `Base64.encode()` | `"dW5kZWZpbmVk"` (the text `"undefined"`) |
| `Base64.decode("")` | `""` |
| `Base64.decode("SGVsbG8")` | `undefined` (not padded) |
| `Base64.decode("Zm9v\n")` | `undefined` (line break) |
| `Base64.decode("-_8Q")` | `undefined` (URL safe alphabet) |
| `Base64.decode("Z===")` | `undefined` (3 `=`) |
| `Base64.decode("Zg==Zm9v")` | `undefined` (padding before the last group) |
| `Base64.decode("ZM9V")` | 3 bytes, not `"foo"` (case is data) |
| `Base64.decode("Zh==")` | `"f"` (unused low bits are not checked) |
| `Base64.decode("YQBi").length` | `3` (zero byte kept) |
| `Base64.decodeToBuffer("")` | empty Buffer (`length` 0, falsy) |

### Errors

| Message | Cause |
|---------|-------|
| `Unable to open "Base64"` | the extension library was not found and no internal one is registered (for example in `fabricare` scripts) |

The three functions themselves never throw: invalid input to `decode` /
`decodeToBuffer` gives `undefined`.

## C++

Namespace `XYO::QuantumScript::Extension::Base64`, umbrella header
`<XYO/QuantumScript.Extension/Base64.hpp>`.

### Library (`Base64/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"Base64"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | from `version.json` |
| `Copyright::copyright()`, `Copyright::publisher()`, `Copyright::company()`, `Copyright::contact()` | |
| `License::license()`, `License::shortLicense()` | MIT text |

`Version`, `Copyright` and `License` exist in every XYO library: qualify them
(`Extension::Base64::Version::versionWithBuild()`).

### Build configuration

| Name | Meaning |
|------|---------|
| `quantum-script--base64` | fabricare project, `dll-or-lib` |
| `test.01` | fabricare test project, runs `test/test.01.js` |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_EXPORT` | export / import macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_INTERNAL` | defined while building the DLL (from `QUANTUM_SCRIPT__BASE64_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY` | static build: empty export macro, no DLL entry point |

### Codec (`xyo-encoding`)

| Symbol | Notes |
|--------|-------|
| `XYO::Encoding::Base64::encode(const String &)` | RFC 4648, `=` padded |
| `XYO::Encoding::Base64::decode(const String &, String &out)` | `bool`; strict, standard alphabet only, padding required |
