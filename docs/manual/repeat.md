# REPEAT

**Category:** Statement  
**Syntax:** `REPEAT`
`  ...`
`UNTIL boolean_expr`

## Description

Begins a conditional post-test loop. Statements within the repeat block execute at least once and continue iterating until the condition in the matching `UNTIL` evaluates to true.

## See also

- [UNTIL](until.md)
- [WHILE](while.md)
- [EXIT](exit.md)

## Example

```basic
10 LET A = 1
20 REPEAT
30   PRINT(A)
40   LET A = A + 1
50 UNTIL A > 3
```
