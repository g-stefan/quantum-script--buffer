# Quantum Script Extension Buffer

Quantum Script extension
- A `Buffer` type for binary data in scripts: a block of bytes with a
capacity (`size`) and a used part (`length`), zero bytes allowed.
- Byte access (`getU8`, `setU8`), bulk copy from strings and buffers (`set`),
capacity changes (`resize`).
- Conversions to and from strings and hexadecimal text (`Buffer.fromString`,
`toString`, `Buffer.fromHex`, `toHex`).
- The byte type used by the other extensions: `File` / `Socket`
`readToBuffer`, `Shell.fileGetContentsBuffer`, `SHA256.hashToBuffer`,
`Base64.decodeToBuffer`, `Crypt.encrypt`, ...

```javascript
Script.requireExtension("Buffer");

Buffer(size);
Buffer.fromString(str);
Buffer.fromHex(str);
Buffer.prototype.length;
Buffer.prototype.size;
Buffer.prototype.getU8(pos);
Buffer.prototype.setU8(pos,value);
Buffer.prototype.set(start,str,strStart,ln,fill);
Buffer.prototype.toString();
Buffer.prototype.toHex();
Buffer.prototype.resize(size);
Buffer.prototype.setLength(ln);
```

Built on `quantum-script`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script, register in a C++ host, static builds
- [Buffer model](docs/buffer-model.md) - `size` and `length`, conversions, buffers in expressions
- [Script API](docs/script-api.md) - every function: exact behavior, edge cases
- [Using Buffer with other extensions](docs/extensions.md) - File, Shell, Socket, hashes, Base64, Crypt
- [C++ API](docs/cpp-api.md) - `VariableBuffer`, taking and returning buffers in native functions
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--buffer](.claude/skills/quantum-script--buffer/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
