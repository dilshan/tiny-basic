# IFF

**Category:** Function  
**Syntax:** `IFF(bool_expr, true_expr, false_expr)`

## Description

Inline IF logical evaluation. Evaluates `bool_expr` and returns the evaluation of `true_expr` if the condition is true, or `false_expr` if false.

## Example

```basic
10 LET A = 10
20 LET B = IFF(A > 5, 1, 0)
30 PRINT(B)
```
