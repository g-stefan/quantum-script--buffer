# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...) and `quantum-script--console`
must be installed to the SDK first. From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 and test/test.02 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

Two libraries are produced:

| Project                         | Kind                                | Use it when                                         |
|---------------------------------|-------------------------------------|-----------------------------------------------------|
| `quantum-script--buffer`        | DLL / shared library (`dll-or-lib`) | scripts run by `quantum-script`, or a host using the engine DLL |
| `quantum-script--buffer.static` | static library, static CRT          | self-contained hosts built with `quantum-script.static` |

After `fabricare install`, `quantum-script--buffer.dll` (Windows) /
`libquantum-script--buffer.so` (Linux) sits in the SDK `bin` folder next to
`quantum-script.exe`, which is where `Script.requireExtension("Buffer")`
finds it.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("Buffer");

var b = Buffer.fromString("Hello");
Console.writeLn(b.length);       // 5
Console.writeLn(b.toHex());      // 48656c6c6f
Console.writeLn(b.getU8(0));     // 72

b.resize(8);                     // capacity 8, length still 5
b.setU8(5, 0x21);                // "!" at index 5, length becomes 6
Console.writeLn(b.toString());   // Hello!
```

Run it with:

```bash
quantum-script hello-buffer.js
```

`Script.requireExtension("Buffer")` looks for an external
`quantum-script--buffer` library first (the file as named, then every include
path folder: next to the interpreter, next to the script), then for an
internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "Buffer"`.

Extensions that work with buffers (`File`, `Shell`, `Socket`,
`ProcessInteractive`, `SHA256`, `Base64`, ...) load `Buffer` themselves, so
after `Script.requireExtension("File")` the `Buffer` global already exists.
Requiring it explicitly anyway is harmless and documents the dependency.

## 3. fabricare build scripts

`fabricare` registers `Buffer` as an internal extension, so build scripts can
use it without any DLL:

```javascript
Script.requireExtension("Console");
Script.requireExtension("Buffer");
Script.requireExtension("Shell");

var data = Shell.fileGetContentsBuffer("logo.png");   // Buffer, or undefined
if (data) {
	var head = Buffer(4);
	head.set(0, data, 0, 4);                           // first 4 bytes
	Console.writeLn("logo.png: " + data.length + " bytes, starts with " + head.toHex());
};
```

## 4. Register it in a C++ host

A host that embeds Quantum Script makes `Buffer` available as an internal
extension by registering it in the init callback (this is what
`test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"Buffer\");"
		        "Console.writeLn(Buffer.fromString(\"abc\").toHex());")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("Buffer")`. With the DLL build of the engine an
external `quantum-script--buffer.dll` found on the include path wins over
the internal one for `requireExtension`; use
`Script.requireInternalExtension("Buffer")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--buffer"
	]
}
```

## 5. Static builds

For a self-contained executable depend on the static variants and the static
CRT:

```json
{
	"name": "my-host.static",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script.static",
		"quantum-script--console.static",
		"quantum-script--buffer.static"
	],
	"crt": "static"
}
```

`quantum-script--buffer.static` exports the define
`XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY` to its consumers, which turns
`XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT` into nothing and leaves out the
`quantumScriptExtension` DLL entry point. A static host must register the
extension with `registerInternalExtension` (section 4): external DLLs cannot
be loaded into a host that does not use the engine DLL.

## 6. Threads

Each thread that runs scripts has its own engine and its own `Buffer`
prototype (`BufferContext` is a per thread singleton), so every thread loads
the extension itself with `Script.requireExtension("Buffer")`. A `Buffer`
value belongs to the thread that created it. Values passed to or returned
from a thread by the `Thread` extension are copied (`clone`): the other
thread receives an independent buffer with the same `size`, `length` and
bytes, and must have loaded `Buffer` before calling its methods.
