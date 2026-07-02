# HBYTE

**Category:** Function  
**Syntax:** `HBYTE(num)`

## Description

Masks the input integer to extract the high nibble of its lower byte (bitwise `AND` with `0xF0`).

## Example

```basic
10 LET A = HBYTE(0xAE)
20 PRINT(HEX, A)
```
