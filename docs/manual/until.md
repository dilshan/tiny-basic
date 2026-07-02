# UNTIL

**Category:** Statement  
**Syntax:** `UNTIL boolean_expr`

## Description

Terminates a `REPEAT` loop block, testing the condition. If the expression is false, execution loops back to the start of `REPEAT`. If true, execution continues past the loop.

## Example

```basic
10 LET A = 5
20 REPEAT
30   PRINT(A)
40   LET A = A - 1
50 UNTIL A = 0
```
