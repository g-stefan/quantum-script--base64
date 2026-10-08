# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...), `quantum-script--console` and
`quantum-script--buffer` must be installed to the SDK first. From the
repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

`fabricare.json` declares two projects:

| Project | Kind | Purpose |
|---------|------|---------|
| `quantum-script--base64` | `dll-or-lib`: shared library in a dynamic build, static library in a static build | the extension |
| `test.01` | executable, category `test` | runs `test/test.01.js` with the extension registered as internal |

After `fabricare install`, `quantum-script--base64.dll` (Windows) /
`libquantum-script--base64.so` (Linux) sits in the SDK `bin` folder next to
`quantum-script.exe`, which is where `Script.requireExtension("Base64")`
finds it.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("Base64");

var text = Base64.encode("Hello");
Console.writeLn(text);                         // SGVsbG8=
Console.writeLn(Base64.decode(text));          // Hello
Console.writeLn(Base64.encode("foo"));         // Zm9v  (3 bytes, no padding)

var r = Base64.decode("SGVsbG8");              // not padded: invalid
if (Script.isUndefined(r)) {
	Console.writeLn("not valid Base64");
};

var bytes = Base64.decodeToBuffer("AP8Q");     // Buffer
Console.writeLn(bytes.length);                 // 3
Console.writeLn(bytes.getU8(1));               // 255
Console.writeLn(bytes.toHex());                // 00ff10
```

Run it with:

```bash
quantum-script hello-base64.js
```

`Script.requireExtension("Base64")` looks for an external
`quantum-script--base64` library first (the file as named, then every include
path folder: next to the interpreter, next to the script), then for an
internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "Base64"`.

Loading `Base64` also loads `Buffer`: the `Buffer` global exists afterwards
even if the script never required it.

## 3. magnet and fabricare scripts

`magnet` (through `quantum-script--magnet`) registers `Base64` as an
internal extension, so its scripts can use it without any DLL:

```javascript
Script.requireExtension("Console");
Script.requireExtension("Base64");
Script.requireExtension("SHA256");

var digest = SHA256.hashToBuffer("release-1.0");   // 32 raw bytes
Console.writeLn(Base64.encode(digest));            // 44 characters, ends with "="
```

`fabricare` does **not** include `Base64`: it is a static executable with a
fixed set of internal extensions (`Console`, `Buffer`, `Shell`, `File`,
`JSON`, `SHA512`, ...), and a static host cannot load extension DLLs, so
`Script.requireExtension("Base64")` fails in a build script. Use hex there
instead: `Buffer.fromString(s).toHex()` / `Buffer.fromHex(h)`.

## 4. Register it in a C++ host

A host that embeds Quantum Script makes `Base64` available as an internal
extension by registering it in the init callback. `Base64` requires
`Buffer` when it is loaded, so register `Buffer` too (this is what
`test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Base64.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::Base64::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"Base64\");"
		        "Console.writeLn(Base64.encode(\"abc\"));")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("Base64")`. With the DLL build of the engine an
external `quantum-script--base64.dll` found on the include path wins over
the internal one for `requireExtension`; use
`Script.requireInternalExtension("Base64")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--base64"
	]
}
```

`quantum-script--base64` depends on `quantum-script`,
`quantum-script--console` and `quantum-script--buffer`; fabricare resolves
them transitively.

## 5. Static builds

The project is `dll-or-lib`: on a static platform (for example
`win64-msvc-2026.static`, which sets `XYO_PLATFORM_COMPILE_STATIC`) the same
project builds a static library. The export macros become empty and the
`quantumScriptExtension` DLL entry point is left out (it is compiled only
with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY`). Defining
`XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY` has the same effect when the
sources are compiled into another library.

A static host must register the extension with `registerInternalExtension`
(section 4): external DLLs cannot be loaded into a host that does not use
the engine DLL.

## 6. Threads

Each thread that runs scripts has its own engine, so every thread loads the
extension itself with `Script.requireExtension("Base64")`. The functions
keep no state: they are safe to call from any thread that loaded the
extension.
