# ELSE

**Category:** Statement  
**Syntax:** `ELSE`

## Description

Defines the alternative execution branch in a multi-line `IF` statement block. Statements following `ELSE` are executed if the condition is false.

## Example

```basic
10 LET A = 10
20 LET B = 20
30 IF A > B THEN
40   LET C = A
50 ELSE
60   LET C = B
70 ENDIF
80 PRINT(C)
```
