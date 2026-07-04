# LBYTE

**Category:** Function  
**Syntax:** `LBYTE(num)`

## Description

Masks the input integer to extract the lower nibble of its lower byte (bitwise `AND` with `0x0F`).

## See also

- [HBYTE](hbyte.md)
- [BYTE](byte.md)

## Example

```basic
10 LET A = LBYTE(0xAE)
20 PRINT(HEX, A)
```
