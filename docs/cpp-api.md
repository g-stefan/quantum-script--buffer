# C++ API

For hosts that embed Quantum Script and for extensions that take or return
buffers. Read the `quantum-script` repository's `docs/embedding.md` and
`docs/writing-extensions.md` first: native functions, `Variable` and
`TPointer` work the same way here.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>                  // Library.hpp + VariableBuffer.hpp
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>   // just the value type
#include <XYO/QuantumScript.Extension/Buffer/Context.hpp>          // BufferContext (rarely needed)

using namespace XYO::QuantumScript;
using Extension::Buffer::VariableBuffer;
```

Namespace: `XYO::QuantumScript::Extension::Buffer`. Export macro:
`XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT` (empty when
`XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY` is defined, i.e. static
builds).

## Registering the extension

```cpp
void Extension::Buffer::registerInternalExtension(Executive *executive);
void Extension::Buffer::initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension` registers `"Buffer"` as an internal extension;
  call it from the host's init callback (see
  [Getting started](getting-started.md#4-register-it-in-a-c-host)).
- `initExecutive` is the extension's init function, run by the engine when a
  script first requires `Buffer` in a thread. It sets the extension name,
  info (license text) and version, creates the `Buffer` global function and
  its prototype, and registers the native methods. Do not call it directly.
- The DLL build also exports
  `extern "C" void quantumScriptExtension(Executive *, void *)`, which
  forwards to `initExecutive`; it is what `Script.requireExtension` looks up
  in `quantum-script--buffer.dll`.

## `VariableBuffer`

```cpp
class VariableBuffer : public Variable {
	public:
		XYO::System::Buffer buffer;      // the bytes: buffer.buffer, buffer.length, buffer.size

		static Variable *newVariable();                                   // size 0, no allocation
		static Variable *newVariable(size_t size);                        // size bytes, zero, length 0
		static Variable *newVariable(const uint8_t *data, size_t dataSize);  // copy, size == length == dataSize
		static Variable *newVariableFromString(String str);               // copy of the string bytes

		void resize(size_t size);        // buffer.setSize: keeps content, new bytes zero, clips length

		// Variable overrides
		String getVariableType();        // "Buffer"
		TPointer<Variable> getPropertyBySymbol(Symbol);   // length, size, then the prototype
		bool hasPropertyByVariable(Variable *);
		Variable *instancePrototype();   // Buffer.prototype of the current thread
		Variable *clone(SymbolList &);   // independent copy, same size, length and bytes
		bool toBoolean();                // length > 0
		Number toNumber();               // parses toString(); NaN when length is 0
		String toString();               // first length bytes
		bool isString();                 // true: operators treat a buffer as a string
};
```

The storage is a public `XYO::System::Buffer` (see the `xyo-system`
repository, `docs/`):

| Member | Meaning |
|--------|---------|
| `uint8_t *buffer` | the bytes; `nullptr` when nothing was ever allocated (`newVariable()`, size 0) |
| `size_t size` | allocated bytes |
| `size_t length` | bytes in use; keep `length <= size` |
| `setSize(n)` | reallocate keeping content, new bytes zero |
| `allocate(n)` | reallocate without keeping content, `length = 0` (fast path before a full overwrite) |
| `set(data, n)`, `fromString(s)`, `fromHex(s)`, `copy(other)` | replace the whole content |
| `toString()`, `toHex()` | read the first `length` bytes |

`VariableBuffer` objects come from an active memory pool
(`TMemoryPoolActive`): when a buffer is recycled, `activeDestructor` frees
its bytes. Always create them with the `newVariable*` functions and hold
them in `TPointer<Variable>`.

`VariableBuffer::toNumber` parses `toString()` (the first `length` bytes,
zero terminated) and returns `NaN` for an empty buffer, so numeric
conversion is safe even when `buffer.buffer` is `nullptr`.

## Taking a buffer argument

Check the type with `TIsType<VariableBuffer>`, then use the storage directly.
This is the pattern of `File.prototype.writeFromBuffer`:

```cpp
static TPointer<Variable> deviceWrite(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	TPointerX<Variable> &bufferV(arguments->index(0));
	if (!TIsType<VariableBuffer>(bufferV)) {
		throw(Error("invalid parameter"));
	};
	XYO::System::Buffer &data = ((VariableBuffer *)bufferV.value())->buffer;
	size_t written = myDeviceWrite(data.buffer, data.length);    // only the first length bytes
	return VariableNumber::newVariable((Number)written);
};
```

To accept either a String or a Buffer (as `Crypt.encrypt` does), test both
types and take a pointer and a size from whichever matches.

## Filling a caller's buffer

Write from index 0, never past `size`, then set `length` (the
`readToBuffer` pattern):

```cpp
static TPointer<Variable> deviceRead(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	TPointerX<Variable> &bufferV(arguments->index(0));
	if (!TIsType<VariableBuffer>(bufferV)) {
		throw(Error("invalid parameter"));
	};
	XYO::System::Buffer &out = ((VariableBuffer *)bufferV.value())->buffer;
	size_t n = myDeviceRead(out.buffer, out.size);   // capacity chosen by the script
	out.length = n;
	return VariableNumber::newVariable((Number)n);
};
```

When the result has a fixed size, grow the caller's buffer first, as
`OpenSSL.sha256` does: `if (out.size < 32) ((VariableBuffer *)bufferV.value())->resize(32);`.

## Returning a new buffer

```cpp
// copy existing bytes
return VariableBuffer::newVariable(data, dataSize);

// or allocate and write in place (the hashToBuffer pattern)
TPointer<Variable> retV(VariableBuffer::newVariable(32));
VariableBuffer *out = (VariableBuffer *)retV.value();
computeDigest(input, out->buffer.buffer);   // writes 32 bytes
out->buffer.length = 32;
return retV;

// or fill an XYO::System::Buffer through an API that takes one
TPointer<Variable> retV(VariableBuffer::newVariable());
if (XYO::System::Shell::fileGetContents(fileName, ((VariableBuffer *)retV.value())->buffer)) {
	return retV;
};
return Context::getValueUndefined();
```

## Depending on Buffer from another extension

1. In `fabricare.json`, add `"quantum-script--buffer"` to the DLL project's
   dependencies and `"quantum-script--buffer.static"` to the static one.
2. Include `<XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>`.
3. In your `initExecutive`, load the extension before anything that may
   create or receive buffers, so `Buffer.prototype` exists in that thread:

```cpp
void initExecutive(Executive *executive, void *extensionId) {
	executive->setExtensionName(extensionId, "Device");
	// ...
	executive->compileStringX("Script.requireExtension(\"Buffer\");");
	executive->compileStringX("var Device={};");
	executive->setFunction2("Device.read(buffer)", deviceRead);
	executive->setFunction2("Device.write(buffer)", deviceWrite);
};
```

A `VariableBuffer` created in a thread where `Buffer` was never loaded has
no prototype: reading a method or `in` on it dereferences an empty
`BufferContext::prototypeBuffer`.

## `BufferContext`

```cpp
class BufferContext : public Object {
	public:
		Symbol symbolFunctionBuffer;            // the "Buffer" global
		TPointerX<Prototype> prototypeBuffer;   // Buffer.prototype
};
BufferContext *getContext();                    // TSingleton<BufferContext>: one per thread
```

Filled by `initExecutive`, cleared by the extension's delete-context hook
when the engine ends. Use `getContext()->prototypeBuffer->prototype` if you
need `Buffer.prototype` from C++.

## Notes for maintainers

- Every native method checks `TIsType<VariableBuffer>(this_)` and throws
  `Error("invalid parameter")` otherwise (`Buffer.fromHex` and
  `Buffer.fromString` are static and take no `this`).
- `set` handles String and Buffer sources with one helper, `setFrom`
  (source bytes and length), so both share the clipping and `fill` rules
  (see [Script API](script-api.md#setstart-source-sourcestart-ln-fill)).
- `test/test.02.js` covers `setLength`, `set` with `fill` and number
  conversion of empty buffers; extend it when changing these.
