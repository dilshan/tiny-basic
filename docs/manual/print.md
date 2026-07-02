# PRINT

**Category:** Statement  
**Syntax:** `PRINT(expression_list)`
`PRINT(format, expression_list)`

## Description

Outputs strings, numbers, or expressions to the standard output. If multiple arguments are separated by commas, they are printed sequentially. Supports optional format modifiers to change mathematical output layouts (`HEX`, `BIN`, `BIN8`, `OCT`).

## Example

```basic
10 PRINT("Value of A is: ", A)
20 PRINT(HEX, A, B)
30 PRINT(BIN8, 1255)
```
