# AND

**Category:** Logical Operator  
**Syntax:** `expr1 AND expr2`

## Description

Performs a short-circuiting logical AND operation. Returns `1` (`True`) if both expressions are non-zero, otherwise returns `0` (`False`).

## See also

- [OR](or.md)
- [NOT](not.md)

## Example

```basic
10 LET A = (5 > 2) AND (3 < 10)
20 PRINT(A)
```
