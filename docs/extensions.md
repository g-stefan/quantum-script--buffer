# Using Buffer with other extensions

`Buffer` is the byte type shared by the `quantum-script--*` extensions. Each
of the extensions below loads `Buffer` itself when it is required, so
`Script.requireExtension("File")` is enough to use `Buffer` too.

Two families of functions exist:

- **Fill an existing buffer** (`readToBuffer`, `OpenSSL.sha256`, ...): you
  allocate the buffer once with the capacity you want, the function writes
  from index 0 and sets `length` to the bytes produced. Reuse the same
  buffer in a loop; read `length` (or the return value) after every call.
- **Return a new buffer** (`Shell.fileGetContentsBuffer`, `hashToBuffer`,
  `decodeToBuffer`, `Crypt.encrypt`, ...): `size == length` = the data.

Writers (`writeFromBuffer`, `filePutContentsBuffer`) send the first
`length` bytes.

## Summary

| Extension | Function | Buffer use |
|-----------|----------|------------|
| `File` | `file.readToBuffer(buffer, ln)` | fills `buffer`; returns bytes read, `0` at end of file |
| `File` | `file.writeFromBuffer(buffer)` | writes `length` bytes; returns bytes written |
| `Shell` | `Shell.fileGetContentsBuffer(file)` | returns a new buffer with the whole file, `undefined` on error |
| `Shell` | `Shell.filePutContentsBuffer(file, buffer)` | writes `length` bytes; `true` / `false`; `undefined` if not a buffer |
| `Socket` | `socket.readToBuffer(buffer, ln)` | fills `buffer`; returns bytes read, `0` at end of stream |
| `Socket` | `socket.writeFromBuffer(buffer)` | writes `length` bytes |
| `ProcessInteractive` | `process.readToBuffer(buffer, ln)` / `writeFromBuffer(buffer)` | same as `File` |
| `MD5`, `SHA256`, `SHA512` | `X.hashToBuffer(str)` | returns the raw digest: 16 / 32 / 64 bytes |
| `Base16`, `Base32`, `Base64` | `X.decodeToBuffer(str)` | returns the decoded bytes, `undefined` on invalid input |
| `Crypt` | `Crypt.encrypt(key, data)` / `Crypt.decrypt(key, data)` | `key` and `data`: String or Buffer; returns a new buffer (`undefined` on failure) |
| `OpenSSL` | `bioReadToBuffer(bio, buffer, ln)` / `bioWriteFromBuffer(bio, buffer)` | fill / write, like `File` |
| `OpenSSL` | `sha256(bufferIn, start, ln, hashOut)` / `sha512(...)` | hashes a range of `bufferIn` into `hashOut` (resized if too small); returns `true` / `false` |
| `OpenSSL` | `rsaPublicKey(buffer)`, `rsaPrivateKey(buffer)`, `rsaPublicEncrypt(rsa, buffer, bufferOut)`, ... | keys and data as buffers |

Functions that take text (`SHA256.hash(x)`, `Base64.encode(x)`,
`Shell.filePutContents(file, x)`, `file.write(x)`) accept a buffer as
well: it is converted with `toString()`, which keeps every byte.

## The `ln` argument of `readToBuffer`

For `File`, `Socket`, `ProcessInteractive` and `OpenSSL`:

| `ln` | Bytes requested |
|------|-----------------|
| missing | 32768, clipped to `size` (`File`, `Socket`, `ProcessInteractive`); `OpenSSL` requires it |
| a number | `ln`, clipped to `size` |
| `Infinity` | `size` |
| `0`, negative, `NaN` | nothing read: `length` is set to 0, returns 0 |

`File.readToBuffer` performs **one** read call (it may return fewer bytes
than requested before the end of the file); `ProcessInteractive`, `Socket`
and `OpenSSL` loop until `ln` bytes arrive or the stream ends.

## Recipes

### Copy a file in chunks

```javascript
Script.requireExtension("File");

function copyFile(from, to) {
	var src = new File();
	var dst = new File();
	var chunk = Buffer(65536);
	var n;
	src.openReadOnly(from);
	dst.openWrite(to);
	while ((n = src.readToBuffer(chunk, chunk.size)) > 0) {
		dst.writeFromBuffer(chunk);   // writes chunk.length == n bytes
	};
	src.close();
	dst.close();
};
```

### Read and write a whole binary file

```javascript
Script.requireExtension("Shell");

var data = Shell.fileGetContentsBuffer("input.bin");
if (!data) {
	throw "cannot read input.bin";
};
data.setU8(0, 0x7F);                               // patch the first byte
Shell.filePutContentsBuffer("output.bin", data);
```

### Hash digests as bytes or hex

```javascript
Script.requireExtension("SHA256");

var digest = SHA256.hashToBuffer("abc");   // 32 bytes
digest.length;                             // 32
digest.toHex() == SHA256.hash("abc");      // true
```

### Decode Base64 to bytes

```javascript
Script.requireExtension("Base64");

var bytes = Base64.decodeToBuffer("AP8=");   // 0x00, 0xff
bytes.toHex();                               // "00ff"
Base64.encode(bytes);                        // "AP8="
```

### Encrypt and decrypt

```javascript
Script.requireExtension("Crypt");

var secret = Crypt.encrypt("password", "message");   // Buffer
var hex = secret.toHex();                             // store as text
var plain = Crypt.decrypt("password", Buffer.fromHex(hex));
plain.toString();                                     // "message"
```

### Build a binary record

```javascript
Script.requireExtension("Buffer");

function u32le(buffer, pos, value) {
	buffer.setU8(pos, value & 0xFF);
	buffer.setU8(pos + 1, (value >> 8) & 0xFF);
	buffer.setU8(pos + 2, (value >> 16) & 0xFF);
	buffer.setU8(pos + 3, (value >> 24) & 0xFF);
};

var rec = Buffer(24);
rec.set(0, Buffer.fromString("NAME"), 0, 16, true);   // 16 byte zero padded field
u32le(rec, 16, 1234);
u32le(rec, 20, 5678);
rec.length;                                           // 24
```

### Append to a growing buffer

```javascript
function append(target, data) {          // data: String or Buffer
	var need = target.length + data.length;
	if (need > target.size) {
		var newSize = target.size * 2;
		if (newSize < need) {
			newSize = need;
		};
		target.resize(newSize);
	};
	target.set(target.length, data, 0, data.length);
	return target;
};

var out = Buffer(16);
append(out, "Hello, ");
append(out, Buffer.fromString("World"));
out.toString();                          // "Hello, World"
```

`data.length` of a String is its byte length, the same unit `set` uses.
