# BYTE

**Category:** Function  
**Syntax:** `BYTE(num)`

## Description

Converts expression argument to an integer and masks the lower 8 bits (bitwise `AND` with `0xFF`).

## Example

```basic
10 LET A = BYTE(258)
20 PRINT(A)
```
