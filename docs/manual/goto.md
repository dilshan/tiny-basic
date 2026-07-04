# GOTO

**Category:** Statement  
**Syntax:** `GOTO line_number`

## Description

Unconditionally jumps program execution to the specified program line number.

## See also

- [GOSUB](gosub.md)
- [ON](on.md)

## Example

```basic
10 GOTO 50
20 PRINT("Skipped")
50 PRINT("Reached here")
```
