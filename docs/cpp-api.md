# C++ API

For hosts that embed Quantum Script and for maintainers of the extension.
Read the `quantum-script` repository's `docs/embedding.md` and
`docs/writing-extensions.md` first: native functions, `Variable` and
`TPointer` work the same way here.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/Base64.hpp>   // Library.hpp

using namespace XYO::QuantumScript;
```

Namespace: `XYO::QuantumScript::Extension::Base64`. Export macro:
`XYO_QUANTUMSCRIPT_EXTENSION_BASE64_EXPORT`:

| Define | Effect |
|--------|--------|
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_INTERNAL` (or `QUANTUM_SCRIPT__BASE64_INTERNAL`, set by fabricare while building the DLL) | export macro = `XYO_PLATFORM_LIBRARY_EXPORT` |
| none | export macro = `XYO_PLATFORM_LIBRARY_IMPORT` (consumers of the DLL) |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY` | export macro empty, no `quantumScriptExtension` entry point (sources compiled into another library) |
| `XYO_PLATFORM_COMPILE_STATIC` (static platforms) | `XYO_PLATFORM_LIBRARY_EXPORT` / `IMPORT` are empty |

The `quantumScriptExtension` entry point is compiled only when
`XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` is defined and
`XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY` is not.

## Registering the extension

```cpp
void Extension::Base64::registerInternalExtension(Executive *executive);
void Extension::Base64::initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension` registers `"Base64"` as an internal extension;
  call it from the host's init callback, together with
  `Extension::Buffer::registerInternalExtension` (see
  [Getting started](getting-started.md#4-register-it-in-a-c-host)).
- `initExecutive` is the extension's init function, run by the engine when a
  script first requires `Base64` in a thread. It sets the extension name,
  info (license text), version and public flag, then:

  ```cpp
  executive->compileStringX("Script.requireExtension(\"Buffer\");");
  executive->compileStringX("var Base64={};");
  executive->setFunction2("Base64.encode(str)", encode);
  executive->setFunction2("Base64.decode(str)", decode);
  executive->setFunction2("Base64.decodeToBuffer(str)", decodeToBuffer);
  ```

  Do not call it directly.
- The DLL build also exports
  `extern "C" void quantumScriptExtension(Executive *, void *)`, which
  forwards to `initExecutive`; it is what `Script.requireExtension` looks up
  in `quantum-script--base64.dll`.

## The native functions

All three are `static` in `Base64/Library.cpp` and wrap the codec in
`xyo-encoding`:

| Script function | C++ | Returns |
|-----------------|-----|---------|
| `Base64.encode(str)` | `XYO::Encoding::Base64::encode(arguments->index(0)->toString())` | `VariableString` |
| `Base64.decode(str)` | `XYO::Encoding::Base64::decode(text, result)` | `VariableString`, or `Context::getValueUndefined()` on failure |
| `Base64.decodeToBuffer(str)` | same decode | `Extension::Buffer::VariableBuffer::newVariableFromString(result)`, or undefined |

With `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` defined each function prints a trace
line (`- base64-encode`, `- base64-decode`, `- base64-decode-to-buffer`).

## Using the codec without the script engine

C++ code that only needs Base64 conversion should call `xyo-encoding`
directly instead of going through a script:

```cpp
#include <XYO/Encoding.hpp>

XYO::Encoding::String text = XYO::Encoding::Base64::encode("foo");   // "Zm9v"

XYO::Encoding::String bytes;
if (XYO::Encoding::Base64::decode(text, bytes)) {
	// bytes == "foo"
};
```

| Function | Behavior |
|----------|----------|
| `String encode(const String &toEncode)` | RFC 4648 alphabet `A-Z a-z 0-9 + /`, `=` padded to a multiple of 4, no line breaks |
| `bool decode(const String &toDecode, String &out)` | `false` (and `out` unchanged) when the length is not a multiple of 4, a character is outside `A-Z a-z 0-9 + / =`, a group has fewer than 2 data characters (`x===`, `====`), a data character follows `=`, or padding appears before the last group; decoding uses a 256 entry reverse lookup table |

The result `String` can contain zero bytes: use `length()` and compare with
`memcmp`, not C string functions.

## Notes for maintainers

- New functions: register them in `initExecutive` with
  `executive->setFunction2("Base64.name(args)", name)`, then update
  `README.md`, `docs/script-api.md`, `docs/reference.md`, the skill in
  `.claude/skills/quantum-script--base64/` and `test/test.01.js`.
- Keep the API in step with `quantum-script--base16` and
  `quantum-script--base32`, which expose the same three functions.
- Validation rules live in `xyo-encoding` (`XYO/Encoding/Base64.cpp`); a
  change there (for example accepting unpadded input or the URL safe
  alphabet) changes this extension's behavior, so update
  `docs/script-api.md` and the skill with it.
- `test/test.01.js` checks the RFC 4648 test vectors (`""`, `"f"`, ...,
  `"foobar"`) in both directions; run it with `fabricare test`.
