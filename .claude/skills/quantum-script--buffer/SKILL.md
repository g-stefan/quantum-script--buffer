---
name: quantum-script--buffer
description: >-
  How to use the Quantum Script Buffer extension (quantum-script--buffer),
  the byte buffer type loaded with Script.requireExtension("Buffer"):
  Buffer(size) (no argument = 1024), read-only size (capacity) and length
  (bytes in use), getU8 / setU8, set(start, source, sourceStart, ln, fill),
  toString / toHex, Buffer.fromString / fromHex, resize, setLength;
  buffers in expressions (string-like, numbers parsed from the text);
  extensions that take or return a Buffer (File / Socket
  readToBuffer / writeFromBuffer, Shell.fileGetContentsBuffer, hashToBuffer,
  decodeToBuffer, Crypt); the C++ side (VariableBuffer, newVariable,
  TIsType<VariableBuffer>, registerInternalExtension). Use when writing or
  reviewing Quantum Script or fabricare .js code that handles binary data or
  calls Buffer, C++ code that includes <XYO/QuantumScript.Extension/Buffer.hpp>,
  a fabricare.json depending on "quantum-script--buffer", or when working
  inside the quantum-script--buffer repository.
---

# quantum-script--buffer

Byte buffer extension of Quantum Script (see the `quantum-script` skill for
the language and its differences from JavaScript, and the `xyo-system`
skill for the underlying `XYO::System::Buffer`; their rules apply).
Purpose: **hold binary data in scripts** — file chunks, socket data,
digests, keys — where a UTF-8 `String` is the wrong type, and give every
native extension one shared byte type.

Full documentation: `docs/` in the quantum-script--buffer repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--buffer\docs` on this machine):
README, getting-started, **buffer-model** (size vs length, conversions,
expressions), **script-api** (exact behavior of every method, edge
cases), **extensions** (File / Shell / Socket / hashes / Crypt recipes),
cpp-api, reference. Read the matching page when you need more than this
summary. When in doubt read
`source/XYO/QuantumScript.Extension/Buffer/Library.cpp` (~310 lines).

## Script API

```javascript
Script.requireExtension("Buffer");      // File, Shell, Socket, SHA256, ... load it themselves

var b = Buffer(4096);                   // size 4096, length 0, zero bytes; new Buffer(n) is the same
var d = Buffer();                       // size 1024 (also for NaN / negative / Infinity)
var s = Buffer.fromString("Hello");     // UTF-8 bytes, size == length == 5
var h = Buffer.fromHex("48656C6c6f");   // pairs of hex digits, invalid digit -> 0, odd last char ignored

b.size; b.length;                       // read-only numbers (assigning throws)
b.getU8(k);                             // 0..255; returns 0 if k invalid or k >= length
b.setU8(k, v);                          // ignored if k >= size; length = max(length, k + 1)
b.set(start, src, srcStart, ln);        // src: String or Buffer; returns bytes copied; ln REQUIRED
b.set(start, src, 0, ln, true);         // + zero-pad up to start + ln (fixed width field)
b.toString();                           // first length bytes, unchanged (zero bytes kept)
b.toHex();                              // first length bytes, lowercase hex
b.resize(n);                            // realloc, keep content, clip length, new bytes zero; returns b
b.setLength(n);                         // length = n (n <= size), bytes unchanged; returns n, 0 if invalid
```

## Hard rules

1. **`size` is capacity, `length` is data.** Readers (`toString`, `toHex`,
   `getU8`, operators, `writeFromBuffer`) see `0 .. length-1`; writers
   (`setU8`, `set`) may write below `size` and push `length` forward.
   **Nothing grows the buffer automatically**: `resize` before writing past
   `size` — out of range writes are silently ignored / clipped, no error.
2. **Out of range reads return 0, not undefined.** `getU8(k)` with
   `k >= length` is `0`, indistinguishable from a zero byte: compare `k`
   with `b.length` yourself.
3. **`set` needs `ln`.** `b.set(0, "abc")` copies nothing (missing `ln` is
   0). Count = `min(ln, size - start, sourceLength - sourceStart)`; a
   buffer source contributes only its first `length` bytes.
4. **`setLength(n)` moves `length` only** (`0 <= n <= size`): bytes and
   allocation are unchanged, so extending exposes whatever is in memory.
   Returns `n`; invalid or `n > size` returns `0` and changes nothing.
   Use `resize(n)` to also free memory.
5. **`fill` pads from `length`, not from `start`.** After the copy, bytes
   `length .. min(start + ln, size) - 1` become zero; bytes between an old
   `length` and `start` are left as they are. String and Buffer sources
   behave the same.
6. **`xor`, `xorAvalancheEncode`, `xorAvalancheDecode` do not exist**
   (old README listed them). Loop with `getU8` / `setU8` and `^`.
7. **No indexing, no own properties.** `b[k]` is `undefined`, `b[k] = v`
   and `b.x = 1` throw. Methods can be added to `Buffer.prototype`.
8. **Buffers are string-like in operators**: `"" + b`, `b == "text"`,
   `b === "text"`, `<` use `b.toString()`. But `Script.isString(b)` is
   `false`; test with `typeof(b) == "Buffer"` or `b instanceof Buffer`.
   `if (b)` is `length > 0`.
9. **Numbers are parsed from the text**: `Convert.toNumber(b)`, `b == 5`,
   `b * 1` use `b.toString()` like a string; an empty buffer (`Buffer(0)`,
   `Buffer.fromHex("")`) is `NaN`, so `Buffer(0) == 0` is `false`.
10. **Reference semantics.** Assignment shares the buffer. Copy with
    `var c = Buffer(src.size); c.set(0, src, 0, src.length);`.
11. `JSON.encode` turns a buffer into `null`: store `toHex()` (or Base64)
    and rebuild with `Buffer.fromHex`.
12. `setU8` values: fractions truncated, negative / NaN -> 0, +Infinity ->
    255, > 255 cast to a byte; write `v & 0xFF` explicitly.
13. **Strings are bytes too**: `"ă".length` is 2, `Buffer.fromString` /
    `toString` never transcode. `String.length` and `set` counts use the
    same unit (bytes).
14. **One engine per thread**: each thread requires `Buffer` itself; the
    `Thread` extension passes buffers by `clone` (independent copy).

## Other extensions

```javascript
var chunk = Buffer(65536);                       // fill-an-existing-buffer functions: reuse it
while ((n = src.readToBuffer(chunk, chunk.size)) > 0) { dst.writeFromBuffer(chunk); };   // File
var data = Shell.fileGetContentsBuffer("x.bin");                 // new Buffer, undefined on error
Shell.filePutContentsBuffer("y.bin", data);                      // true / false; undefined if not a Buffer
socket.readToBuffer(buf);                                        // Socket: same ln rules as File, loops until ln bytes or end
SHA256.hashToBuffer(str);                                        // 32 raw bytes (MD5 16, SHA512 64)
Base64.decodeToBuffer(text);                                     // Buffer or undefined (Base16 / Base32 too)
Crypt.encrypt(key, data); Crypt.decrypt(key, buf);               // String or Buffer in, Buffer out
```

`readToBuffer(buffer, ln)` writes from index 0 and sets `length` to the
bytes read (0 at end); missing `ln` = 32768 (File, Socket, ProcessInteractive),
clipped to `size`; `ln` 0 / negative / NaN reads nothing. Text functions
(`SHA256.hash`, `Base64.encode`, `file.write`) accept a buffer via
`toString()`.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
using namespace XYO::QuantumScript;
using Extension::Buffer::VariableBuffer;

Extension::Buffer::registerInternalExtension(executive);   // host init callback; scripts still requireExtension

// argument
if (!TIsType<VariableBuffer>(arguments->index(0))) { throw(Error("invalid parameter")); };
XYO::System::Buffer &data = ((VariableBuffer *)(arguments->index(0)).value())->buffer;   // .buffer .length .size
// fill caller's buffer: write <= data.size bytes from 0, then data.length = n;
// grow first if needed: ((VariableBuffer *)...)->resize(32);

// return
return VariableBuffer::newVariable(ptr, size);              // copy
TPointer<Variable> r(VariableBuffer::newVariable(32));      // size 32, length 0
((VariableBuffer *)r.value())->buffer.length = 32;          // after writing the bytes
return VariableBuffer::newVariableFromString(str);
```

- Another extension using buffers: depend on `"quantum-script--buffer"` /
  `"quantum-script--buffer.static"` in `fabricare.json`, and in its
  `initExecutive` run
  `executive->compileStringX("Script.requireExtension(\"Buffer\");");`
  before anything creates buffers (otherwise the per thread
  `Buffer.prototype` is missing and method lookups crash).
- `buffer.buffer` may be `nullptr` (size 0); keep `length <= size`.
- Static build: `quantum-script--buffer.static` exports
  `XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY` (empty export macro, no
  `quantumScriptExtension` entry point); register it as internal.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01` and
  `test/test.02`, which register Console and Buffer as internal extensions
  and run `test/test.01.js` / `test/test.02.js`; run `make` first), `fabricare install` (see the `fabricare` skill).
  `quantum-script` and `quantum-script--console` must be installed first.
- Native methods live in `Buffer/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`,
  check `TIsType<VariableBuffer>(this_)` first, and are registered in
  `initExecutive` with `executive->setFunction2("Buffer.prototype.name(args)", name)`.
  Static functions: `"Buffer.name(args)"`.
- New methods: update `README.md`, `docs/script-api.md`, `docs/reference.md`
  and this skill.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
