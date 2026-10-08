# Buffer model

## Size and length

A buffer is a block of `size` bytes, of which the first `length` are in use:

```
index   0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
      | H | e | l | l | o | 0 | 0 | 0 |      size   = 8   (allocated)
      +---+---+---+---+---+---+---+---+      length = 5   (in use)
      |<----- length ---->|
      |<------------- size ---------->|
```

| Property | Meaning | Changed by |
|----------|---------|------------|
| `size` | allocated bytes, the capacity | `Buffer(n)`, `resize(n)`, `Buffer.fromString`, `Buffer.fromHex`, extensions that fill a buffer (`Shell.fileGetContentsBuffer`, ...) |
| `length` | bytes holding data, `0 <= length <= size` | `setU8`, `set`, `resize` (clipped), extensions (`File.readToBuffer` sets it to the bytes read) |

Both are **read-only** numbers: `b.length = 3` throws `setPropertyBySymbol`.

- **Readers use `length`.** `toString()`, `toHex()`, `getU8()`, string
  concatenation and comparisons, `set` with a buffer source, and the
  `writeFromBuffer` functions of other extensions see bytes `0 .. length-1`.
- **Writers use `size`.** `setU8` and `set` can write anywhere below `size`
  and move `length` forward to cover what they wrote. Nothing grows the
  allocation except `resize` (and the functions that replace the whole
  content).
- **`length` only moves forward** with `setU8` / `set`; it goes down only
  with `resize` to a smaller size, or when an extension refills the buffer
  (`File.readToBuffer` sets `length` to the bytes just read, `0` at end of
  file).
- **Bytes between the old `length` and a later write keep their old value.**
  New buffers and the bytes added by `resize` are zero, so in practice the
  gap is zero unless the buffer held longer data before.

```javascript
var b = Buffer(8);          // size 8, length 0
b.set(4, "ab", 0, 2);       // writes indexes 4, 5
b.length;                   // 6
b.toHex();                  // "000000006162"
```

## Creating a buffer

| Expression | `size` | `length` | Content |
|------------|--------|----------|---------|
| `Buffer()` | 1024 | 0 | zero |
| `Buffer(n)` / `new Buffer(n)` | `n` (fraction dropped) | 0 | zero |
| `Buffer(-1)`, `Buffer(NaN)`, `Buffer(Infinity)` | 1024 | 0 | zero |
| `Buffer(0)` | 0 | 0 | none |
| `Buffer.fromString(s)` | bytes of `s` | bytes of `s` | the UTF-8 bytes of `s` |
| `Buffer.fromHex(h)` | `floor(h.length / 2)` | same | decoded bytes |

`Buffer` is a function that returns a new buffer; calling it with `new` gives
the same result (a native constructor's return value replaces the object
`new` created). The argument goes through number conversion, so
`Buffer("16")` is a 16 byte buffer.

## Bytes and positions

Positions and byte values are Quantum Script numbers (`double`):

| Value given | As a position (`getU8`, `setU8`) | As a byte value (`setU8`) |
|-------------|----------------------------------|---------------------------|
| `0 .. 255`, integer | used | stored |
| fraction (`2.7`) | truncated (`2`) | truncated |
| negative | ignored (`getU8` returns 0, `setU8` does nothing) | `0` |
| `NaN`, `undefined`, non numeric string | ignored | `0` |
| `Infinity` | ignored | `255` (`-Infinity`: `0`) |
| above 255 | — | the low byte is kept by the cast (`300` → `44`); mask it yourself (`v & 0xFF`) rather than rely on it |
| numeric string (`"66"`) | converted | converted |

`set` and `resize` use the engine's index conversion instead: `NaN`,
`Infinity`, negative and missing values all become `0`.

## Strings and encodings

A buffer holds bytes; a Quantum Script `String` holds bytes too (UTF-8 by
convention, but not validated). Conversions copy bytes unchanged in both
directions:

```javascript
var b = Buffer.fromString("ă");    // 2 bytes: c4 83 (UTF-8)
b.length;                          // 2
b.toString();                      // "ă"

var z = Buffer.fromHex("610062");  // 'a', 0x00, 'b'
z.toString().length;               // 3 — zero bytes are kept
```

- `Buffer.fromString(x)` converts `x` with `toString` first
  (`Buffer.fromString(12)` holds `"12"`).
- There is no character access by index on a buffer: `getU8` returns bytes,
  so a non-ASCII character spans several positions.
- For text with a known encoding other than UTF-8, build the bytes with
  `setU8` / `set` and keep them in the buffer; Quantum Script has no
  transcoding functions.

## Buffers in expressions

A buffer reports itself as string-like to the engine's operators, so in
expressions it behaves like `b.toString()`:

```javascript
var b = Buffer.fromString("Hello");
"[" + b + "]";                      // "[Hello]"
b == "Hello";                       // true
b === "Hello";                      // true (both string-like)
Buffer.fromString("a") < Buffer.fromString("b");   // true, byte-wise comparison
```

| Operation | Result |
|-----------|--------|
| `typeof(b)` | `"Buffer"` |
| `b instanceof Buffer` | `true` |
| `Script.isString(b)` | `false` — test with `typeof(b) == "Buffer"` |
| `if (b)`, `Convert.toBoolean(b)` | `true` when `length > 0` |
| `b + x`, `b == x`, `<`, `>` with a string or buffer | compares / joins the first `length` bytes as text |
| `JSON.encode({b: b})` | `{"b":null}` — encode `b.toHex()` or `b.toString()` instead |

**Numbers.** `Convert.toNumber(b)`, `b == 5`, `b < 10`, `b * 1` parse
`b.toString()` (the first `length` bytes) like a string: a leading number
is read, anything else gives `NaN`. An empty buffer (`Buffer(0)`,
`Buffer.fromHex("")`, any buffer with `length` 0) is `NaN`, so
`Buffer(0) == 0` is `false`.

```javascript
Convert.toNumber(Buffer.fromString("42"));   // 42
Buffer.fromString("21") * 2;                 // 42
Convert.toNumber(Buffer(0));                 // NaN
```

## Reference semantics

A buffer is an object: assignment and argument passing share it.

```javascript
var a = Buffer.fromString("x");
var c = a;
c.setU8(0, 65);
a.toString();          // "A"
```

To copy, make a new buffer and `set` the content into it:

```javascript
function copyBuffer(src) {
	var out = Buffer(src.size);
	out.set(0, src, 0, src.length);
	return out;
};
```

(or `Buffer.fromString(src.toString())`, which gives `size == length`).

## Prototype

Methods live on `Buffer.prototype`, which scripts can extend:

```javascript
Buffer.prototype.readU16LE = function(pos) {
	return this.getU8(pos) + this.getU8(pos + 1) * 256;
};
Buffer.fromHex("3412").readU16LE(0);   // 0x1234 = 4660
```

Own properties cannot be added to a buffer instance (`b.tag = 1` throws
`setPropertyBySymbol`); keep extra data next to it in an object.

## Not supported

| Not available | Use instead |
|---------------|-------------|
| `b[k]`, `b[k] = v` (read gives `undefined`, write throws `setPropertyByIndex`) | `getU8(k)`, `setU8(k, v)` |
| `b.length = n` | `b.setLength(n)` (see [Script API](script-api.md#setlengthln)), or `resize(n)` to also free memory |
| 16 / 32 bit, float, signed access | combine bytes with `getU8` / `setU8`, or add methods to `Buffer.prototype` |
| `slice`, `concat`, `indexOf`, `fill`, `xor` | `set` into a new buffer; loops over `getU8` / `setU8` |
| automatic growth | `resize` before writing past `size` |
| iteration with `for in` / `for of` | `for (k = 0; k < b.length; ++k) b.getU8(k)` |
