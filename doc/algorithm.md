# Doublespace decompression algorithm

## Basic operation

The doublespace decompression algorithm is based on the well-known LZ77 algorithm,
meaing decompression works on tuples containing a length and an offset, or an literal.

If the tuple is an literal, its value is placed directly inside the output buffer. Otherwise,
the data already inside the output buffer is copied according to the length and offset values.
The length specifies how many bytes should be copied, while the offset specifies the starting
point for the data to be copied from (start = current position - offset).
Should the copy operation exceed the current position inside the output buffer, then it will
wrap around.

There are a couple of special offset values:
* `0x0`: Invalid offset, should be ignored
* `0x113f`: Special offset used for synchronization, usually occurs every 512 bytes and at the
            end of output data

## Bit stream format

### Header

The header consists of the following information:
* a magic number containing the two letters `DS` (`0x5344` little endian)
* a 16 bit big endian number containg the version of the algorithm (1 - 3 are known to work)

### Tuple stream

The first two bits of each tuple describe the content of the following data:
* `0x0`: Standard length/offet tuple
* `0x1`: Big literal
* `0x2`: Small literal
* `0x3`: Extended length/offset tuple

**All length/offset values are in little endian!**

#### Literals

Each literal contains 7 bits of data. A small literal has the 8th bit set to 0, while a big
literal has the 8th bit set to 1.

#### Standard length/offset tuple

Such a tuple contain a 6 bit offset followed by the length value.

#### Extended length/offset tuple

Such a tuple starts with a special bit. If this bit is true, then it is followed by a 12 bit offset,
otherwise it is followed by a 8 bit offset. Both offsets need an additional offset applied onto them
(320 for the 12 bit offset and 64 for the 8 bit offset). The offset value is then followed by a length
value.

A raw 12-bit offset value with all bits set indicates that 512 bytes of output data should have been
produced so far. It can also indicate that decoding of the bitstream should stop if fewer than 16 bits
remain. In both cases said offset has no associated length value.

#### Length value

The length value starts with a number of zero bits (between 0 and 8), terminated by a one bit. This the base value,
which determines the length of the next value (value length in bits = number of zero bits in base value). The sum
of both values plus one forms the final length value.

### Format (bits from left to right)

```
Bitstream: <Header> <Payload>

Header: <Magic> <Version>

Magic: <Magic Low Byte> <Magic High Byte>
Magic High Byte: 1 1 0 0 1 0 1 0 ('S')
Magic Low Byte: 0 0 1 0 0 0 1 0 ('D')

Version <High Byte> <Low Byte>

Payload: [<Tuple> <Up to 15 bits for stuffing>]
Tuple: 0 0 <Standard Tuple> | 1 0 <Big Literal> | 0 1 <Small Literal> | 1 1 <Extended Tuple>

Big/Small Literal: <b> <b> <b> <b> <b> <b> <b>

Standard Tuple: <Small Offset> <Length>
Small Offset: <b> <b> <b> <b> <b> <b>

Extended Tuple: 0 <Medium Offset> <Length> | 1 <Large Offset> <Length>
Medium Offset: <b> <b> <b> <b> <b> <b> <b> <b>
Large Offset: <b> <b> <b> <b> <b> <b> <b> <b> <b> <b> <b> <b>

Length: <Base n> <Value n>
Base n: 0^n 1
Value n: <b>^n

Byte: <b> <b> <b> <b> <b> <b> <b> <b>
b: 0 | 1
```
