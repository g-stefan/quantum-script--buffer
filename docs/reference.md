# API reference

## Script

Available after `Script.requireExtension("Buffer")`.

### Global

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Buffer(size)` / `new Buffer(size)` | Buffer | `size` bytes, zero, `length` 0; invalid or missing `size` → 1024 |
| `Buffer.fromString(str)` | Buffer | bytes of `str`, `size == length` |
| `Buffer.fromHex(str)` | Buffer | pairs of hex digits; invalid digit → 0, odd last character ignored |

### Properties (read-only)

| Property | Value |
|----------|-------|
| `b.length` | bytes in use, `0 .. size` |
| `b.size` | bytes allocated |

### Methods

| Method | Returns | Behavior |
|--------|---------|----------|
| `b.getU8(pos)` | Number | byte at `pos`; `0` if `pos` invalid or `>= length` |
| `b.setU8(pos, value)` | `undefined` | writes a byte if `pos < size`; extends `length` to `pos + 1` |
| `b.set(start, source, sourceStart, ln, fill)` | Number | copies up to `ln` bytes of a String / Buffer `source` at `start`; returns bytes copied; `fill` zero-pads from `length` up to `min(start + ln, size)` |
| `b.toString()` | String | first `length` bytes |
| `b.toHex()` | String | first `length` bytes, lowercase hex |
| `b.resize(size)` | the buffer | reallocates, keeps content, clips `length`, new bytes zero |
| `b.setLength(ln)` | Number | sets `length` to `ln` (`<= size`), bytes unchanged; returns the new length, `0` if `ln` invalid or `> size` |

### Conversions

| Expression | Result |
|------------|--------|
| `typeof(b)` | `"Buffer"` |
| `b instanceof Buffer` | `true` |
| `Script.isString(b)` | `false` |
| `"" + b`, `b == "text"`, `b < other` | operate on `b.toString()` |
| `Convert.toBoolean(b)`, `if (b)` | `length > 0` |
| `Convert.toNumber(b)`, `b == 1`, `b * 1` | number parsed from `b.toString()`; `NaN` for an empty buffer |
| `b[k]` / `b[k] = v` | `undefined` / throws |
| `JSON.encode(b)` | `null` |

### Errors

| Message | Cause |
|---------|-------|
| `invalid parameter` | a method called with a `this` that is not a buffer |
| `setPropertyBySymbol` | assigning a property (`b.length = n`, `b.tag = 1`) |
| `setPropertyByIndex` | `b[k] = v` |
| `Unable to open "Buffer"` | the extension library was not found and no internal one is registered |

## C++

Namespace `XYO::QuantumScript::Extension::Buffer`, umbrella header
`<XYO/QuantumScript.Extension/Buffer.hpp>`.

### Library (`Buffer/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"Buffer"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### `VariableBuffer` (`Buffer/VariableBuffer.hpp`)

| Symbol | Notes |
|--------|-------|
| `XYO::System::Buffer buffer` | public storage: `buffer.buffer`, `buffer.length`, `buffer.size` |
| `static Variable *newVariable()` | empty, no allocation |
| `static Variable *newVariable(size_t size)` | `size` zero bytes, `length` 0 |
| `static Variable *newVariable(const uint8_t *data, size_t dataSize)` | copy of `data` |
| `static Variable *newVariableFromString(String str)` | copy of the string bytes |
| `void resize(size_t size)` | `buffer.setSize(size)` |
| `String getVariableType()` | `"Buffer"` |
| `TPointer<Variable> getPropertyBySymbol(Symbol)` | `length`, `size`, then prototype |
| `bool hasPropertyByVariable(Variable *)` | `length`, `size`, prototype members |
| `Variable *instancePrototype()` | `Buffer.prototype` of the current thread |
| `Variable *clone(SymbolList &)` | deep copy |
| `bool toBoolean()` | `length > 0` |
| `Number toNumber()` | parses `toString()`; `NaN` when `length` is 0 |
| `String toString()` | first `length` bytes |
| `bool isString()` | `true` |
| `static void initMemory()` | memory pool setup |
| `void activeDestructor()` | frees the bytes when the object is recycled |

### `BufferContext` (`Buffer/Context.hpp`)

| Symbol | Notes |
|--------|-------|
| `Symbol symbolFunctionBuffer` | symbol of the `Buffer` global |
| `TPointerX<Prototype> prototypeBuffer` | `Buffer.prototype` holder |
| `BufferContext *getContext()` | per thread singleton |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | from `version.json` |
| `Copyright::copyright()`, `Copyright::publisher()`, `Copyright::company()`, `Copyright::contact()` | |
| `License::license()`, `License::shortLicense()` | MIT text |

`Version`, `Copyright` and `License` exist in every XYO library: qualify them
(`Extension::Buffer::Version::versionWithBuild()`).

### Build configuration

| Name | Meaning |
|------|---------|
| `quantum-script--buffer` | fabricare project, DLL / shared library |
| `quantum-script--buffer.static` | fabricare project, static library, static CRT |
| `XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT` | export / import macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_INTERNAL` | defined while building the DLL (from `QUANTUM_SCRIPT__BUFFER_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY` | static build: empty export macro, no DLL entry point |
