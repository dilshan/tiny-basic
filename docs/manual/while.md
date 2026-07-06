# WHILE

**Category:** Statement  
**Syntax:** `WHILE boolean_expr`

## Description

Initiates a conditional pre-test loop. The block of statements between `WHILE` and `WEND` executes repeatedly as long as the conditional expression is true.

## See also

- [REPEAT](repeat.md)
- [WEND](wend.md)
- [EXIT](exit.md)

## Example

```basic
10 LET A = 1
20 WHILE A <= 3
30   PRINT(A)
40   LET A = A + 1
50 WEND
```
