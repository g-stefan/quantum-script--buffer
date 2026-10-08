# Script API

Everything the extension defines after `Script.requireExtension("Buffer")`.
"Index conversion" below means the engine's `toIndex`: the value is converted
to a number, and `NaN`, `±Infinity`, negative or missing values become `0`;
fractions are truncated.

Methods called on something that is not a buffer
(`Buffer.prototype.toHex.call({})`) throw `Error: invalid parameter`.

## Constructor

### `Buffer(size)` / `new Buffer(size)`

Returns a new buffer with `size` bytes allocated, all zero, and `length` 0.

- `size` is converted to a number and truncated: `Buffer(2.7)` has size 2,
  `Buffer("16")` size 16.
- Missing, `NaN`, `±Infinity` or negative `size` gives **1024**.
- `Buffer(0)` is valid (size 0, no memory allocated); it converts to the
  number `NaN`, like any empty buffer (see
  [Buffer model](buffer-model.md#buffers-in-expressions)).

```javascript
var b = Buffer(4096);
b.size;     // 4096
b.length;   // 0
```

## Static functions

### `Buffer.fromString(str)`

Returns a new buffer holding the bytes of `str` (converted with `toString`
first). `size` and `length` both equal the byte length of the string; UTF-8
characters take several bytes, zero bytes are kept.

```javascript
Buffer.fromString("Hello").toHex();   // "48656c6c6f"
Buffer.fromString("ă").length;        // 2
Buffer.fromString("").size;           // 0
```

### `Buffer.fromHex(str)`

Returns a new buffer decoded from hexadecimal text: each pair of characters
becomes one byte. `size` and `length` are `floor(str.length / 2)`.

- Digits `0-9`, `a-f`, `A-F` are accepted; **any other character counts as
  0** (no error): `"ZZ"` decodes to `0x00`.
- An odd final character is ignored.
- No separators or prefixes: `"0x41"` decodes as `0x00`, `0x41`; remove
  spaces and `0x` before decoding.
- An empty string gives an empty buffer with no allocation.

```javascript
Buffer.fromHex("48656C6c6f").toString();   // "Hello"
Buffer.fromHex("abc").length;              // 1 (0xab, "c" ignored)
```

## Properties

### `length`

Number of bytes in use (`0 .. size`). Read-only.

### `size`

Number of bytes allocated. Read-only.

## Methods

### `getU8(pos)`

Returns the byte at `pos` as a number `0 .. 255`.

- `pos` is truncated (`getU8(1.9)` reads index 1).
- Returns **`0`** — not `undefined`, no error — when `pos` is negative,
  `NaN`, `Infinity`, or `>= length`. A zero byte and an out of range read
  look the same: check `pos < b.length` yourself when it matters.

### `setU8(pos, value)`

Writes one byte. Returns `undefined`.

- `pos` is truncated; negative, `NaN` or `Infinity` positions and positions
  `>= size` are **silently ignored** (no error, nothing written). Grow the
  buffer with `resize` first.
- If `pos >= length`, `length` becomes `pos + 1`; bytes between the old
  `length` and `pos` are not touched (zero in a new buffer).
- `value`: `NaN` → 0, `+Infinity` → 255, negative → 0, fractions truncated.
  Values above 255 are cast to a byte (the low 8 bits in practice: `300` →
  `44`); pass `value & 0xFF` to make that explicit.

```javascript
var b = Buffer(4);
b.setU8(2, 0x41);
b.length;        // 3
b.toHex();       // "000041"
b.setU8(4, 1);   // ignored: 4 >= size
```

### `set(start, source, sourceStart, ln, fill)`

Copies up to `ln` bytes from `source`, starting at `sourceStart`, into this
buffer at `start`. Returns the number of bytes copied.

- `source` must be a **String** or a **Buffer**; anything else copies
  nothing and returns `0`. For a buffer source, only its first `length`
  bytes can be copied; for a string, its bytes.
- `start`, `sourceStart`, `ln` use index conversion. **`ln` is required**:
  `b.set(0, "abc")` copies 0 bytes. Pass `src.length` to copy everything.
- The count is clipped to what fits:
  `min(ln, size - start, sourceLength - sourceStart)`. If `start` is past
  `size` or `sourceStart` is past the end of the source, nothing is copied
  and `0` is returned.
- The buffer **never grows**; `resize` first when `start + ln > size`.
- `length` becomes `max(length, start + copied)`. Bytes between the old
  `length` and `start` are not touched.
- `fill` (optional, converted to boolean): pad with zero bytes. After the
  copy, if `length` is below `min(start + ln, size)`, the bytes from `length`
  up to that point are set to zero and `length` moves there. Use it to write
  fixed width fields: the field always ends at `start + ln`, whatever the
  length of the source. `fill` never shrinks `length`. String and buffer
  sources behave the same.

```javascript
var b = Buffer(8);
b.set(0, "abcdef", 1, 3);                // 3, b = "bcd", length 3
b.set(6, "xyz", 0, 3);                   // 2 (clipped at size), length 8

var rec = Buffer(16);
rec.set(0, Buffer.fromString("ab"), 0, 8, true);   // 2 copied, length 8, "ab" + 6 zero bytes
rec.set(8, "cd", 0, 8, true);                     // 2 copied, length 16

Buffer(8).set(4, "ab", 0, 10, true);               // 2; toHex() "0000000061620000", length 8
```

### `toString()`

Returns the first `length` bytes as a `String`, unchanged (zero bytes kept,
no UTF-8 validation). An empty buffer gives `""`. The same conversion is
used when a buffer appears in a string expression (`"" + b`).

### `toHex()`

Returns the first `length` bytes as lowercase hexadecimal text, two
characters per byte, no separators. An empty buffer gives `""`.

```javascript
Buffer.fromString("Hi!").toHex();   // "486921"
```

### `resize(size)`

Changes the allocation to `size` bytes (index conversion; `NaN` or negative
means 0). Returns the buffer itself, so calls can be chained.

- The first `min(length, size)` bytes are kept; `length` is clipped to the
  new `size`; added bytes are zero.
- Resizing to the current size does nothing.
- Every resize to a different size allocates a new block and copies: grow in
  large steps (double the size) when appending in a loop.

```javascript
var b = Buffer.fromString("Hello");   // size 5, length 5
b.resize(8);                          // size 8, length 5
b.resize(3).toString();               // "Hel", size 3, length 3
```

### `setLength(ln)`

Sets `length` to `ln` without touching the bytes or the allocation, and
returns the new length.

- `ln` is converted to a number; a fraction is truncated.
- `ln` may shrink or extend `length`, up to `size`. Extending exposes the
  bytes already in memory (zero for a new buffer, old data after a shrink).
- `NaN`, `±Infinity`, negative or `ln > size`: nothing changes, returns `0`.

```javascript
var b = Buffer.fromString("Hello");   // size 5, length 5
b.setLength(2);                       // 2, b.toString() == "He"
b.setLength(5);                       // 5, "Hello" again
b.setLength(6);                       // 0, past size: length stays 5
```

## Functions listed elsewhere that do not exist

Older documentation (including earlier versions of this repository's
`README.md`) lists `Buffer.prototype.xor(buffer, start, ln)`,
`xorAvalancheEncode()` and `xorAvalancheDecode()`. They are **not
registered** by this extension: `typeof(Buffer.prototype.xor)` is
`"undefined"`. Write the loop with `getU8` / `setU8`:

```javascript
function xorInPlace(target, key) {
	var k;
	for (k = 0; k < target.length; ++k) {
		target.setU8(k, target.getU8(k) ^ key.getU8(k % key.length));
	};
	return target;
};
```
