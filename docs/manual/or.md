# OR

**Category:** Logical Operator  
**Syntax:** `expr1 OR expr2`

## Description

Performs a short-circuiting logical OR operation. Returns `True` if either/both expressions are `True`, otherwise returns `False`.

## See also

- [AND](and.md)
- [XOR](xor.md)

## Example

```basic
10 LET A = (5 < 2) OR (3 < 10)
20 PRINT(A)
```
