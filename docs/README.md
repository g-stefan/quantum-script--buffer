# Quantum Script Extension Buffer — Documentation

`quantum-script--buffer` is the **byte buffer extension of Quantum Script**.
Loaded with `Script.requireExtension("Buffer")`, it adds a `Buffer` type to
scripts: a fixed capacity block of raw bytes that can hold binary data
(including zero bytes), be read and written byte by byte, filled from strings
or other buffers, and converted to and from hexadecimal text.

Quantum Script strings are UTF-8 text. `Buffer` is the type the other
extensions use when the data is **bytes, not text**: `File.readToBuffer`,
`Socket.writeFromBuffer`, `Shell.fileGetContentsBuffer`,
`SHA256.hashToBuffer`, `Base64.decodeToBuffer`, `Crypt.encrypt`, ... all take
or return a `Buffer`.

- **A capacity and a length.** Every buffer has a `size` (allocated bytes)
  and a `length` (bytes in use, `0 .. size`). Reading functions see the
  first `length` bytes; writing functions may write anywhere below `size`
  and move `length` forward. Both are read-only properties.
- **Byte access.** `getU8(pos)` / `setU8(pos, value)` read and write one
  byte (`0 .. 255`). There is no `buffer[k]` indexing.
- **Bulk copy.** `set(start, source, sourceStart, ln, fill)` copies a range of
  a `String` or another `Buffer` into the buffer, clipped to both sides.
- **Conversions.** `Buffer.fromString(str)` / `toString()` move bytes between
  a string and a buffer unchanged; `Buffer.fromHex(str)` / `toHex()` encode
  them as hexadecimal text.
- **Capacity changes.** `resize(size)` grows or shrinks the allocation,
  keeping the content.

```
scripts: fabricare build scripts, quantum-script .js, ...
quantum-script--file, --socket, --shell, --sha256, --crypt, ...   (take / return Buffer)
quantum-script--buffer    <-- this extension: the Buffer type
quantum-script            (Executive, Variable, Context)
xyo-system                (XYO::System::Buffer, the storage)
xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `Buffer` gives |
|------|---------------------|
| Binary data in a script (file chunks, network packets, hashes, keys) | bytes `0 .. 255`, zero bytes allowed, no UTF-8 interpretation |
| Read a file or a socket in chunks without building strings | `File.readToBuffer(buffer)` fills an existing buffer, reuse it in a loop |
| Look at or patch individual bytes | `getU8` / `setU8` |
| Show or store binary data as text | `toHex()` / `Buffer.fromHex(str)` |
| A common byte type for all native extensions | C++ side: `VariableBuffer` with a public `XYO::System::Buffer buffer` member |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Buffer");` | once per script, before the first use of `Buffer` |
| New empty buffer | `var b = Buffer(4096);` or `new Buffer(4096)` | `size` 4096, `length` 0, bytes zero; no argument means **1024** |
| Buffer from a string | `Buffer.fromString("text")` | the UTF-8 bytes, `size` = `length` = byte count |
| Buffer from hex | `Buffer.fromHex("48656c6c6f")` | upper or lower case |
| Capacity / bytes used | `b.size`, `b.length` | read-only |
| Read a byte | `b.getU8(k)` | `0` past `length` |
| Write a byte | `b.setU8(k, 0x41)` | ignored past `size`; extends `length` to `k + 1` |
| Copy into the buffer | `b.set(start, strOrBuffer, srcStart, ln)` | returns the bytes copied; never grows the buffer |
| As text | `b.toString()`, `"" + b` | the first `length` bytes, unchanged |
| As hex | `b.toHex()` | lowercase, 2 characters per byte |
| Change capacity | `b.resize(n)` | content kept, new bytes zero, returns `b` |
| Type test | `typeof(b) == "Buffer"`, `b instanceof Buffer` | `Script.isString(b)` is `false` |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script, fabricare scripts, register it in a C++ host, static builds |
| [Buffer model](buffer-model.md) | `size` and `length`, how numbers and strings are converted, buffers in expressions, reference semantics, what is not supported |
| [Script API](script-api.md) | Every function and property: arguments, exact behavior, edge cases, return values |
| [Using Buffer with other extensions](extensions.md) | File, Shell, Socket, ProcessInteractive, hashes, Base16/32/64, Crypt, OpenSSL: patterns and examples |
| [C++ API](cpp-api.md) | `VariableBuffer`, `registerInternalExtension`, taking and returning buffers in your own native functions |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the underlying `XYO::System::Buffer` in the `xyo-system`
repository, `docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/Buffer.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Buffer.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Buffer/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         Buffer() and every native function
    Context.hpp                                          BufferContext: the Buffer prototype per thread
    VariableBuffer[.hpp/.cpp]                            the script value type (wraps XYO::System::Buffer)
    Copyright / License / Version                        library metadata
test/test.01.cpp                                         C++ host registering Console and Buffer as internal
test/test.01.js                                          string -> hex -> string round trip
test/test.02.cpp                                         same host, runs test/test.02.js
test/test.02.js                                          setLength, set (with and without fill), number conversion
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--buffer/`](../.claude/skills/quantum-script--buffer/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `Buffer`
(fabricare scripts, Quantum Script tools, other extensions).
