# IF

**Category:** Statement  
**Syntax:** `IF boolean_expr THEN statement`
`IF boolean_expr THEN`
`...`
`[ELSE]`
`...`
`ENDIF`

## Description

Evaluates a condition and branches execution. Tiny BASIC supports both a single-line `IF-THEN` statement and a multi-line `IF-THEN-ELSE-ENDIF` block.

## See also

- [THEN](then.md)
- [ELSE](else.md)

## Example

```basic
10 LET A = 5
20 IF A < 10 THEN PRINT("Small")
30 IF A = 0 THEN
40   PRINT("Zero")
50 ELSE
60   PRINT("Non-zero")
70 ENDIF
```
