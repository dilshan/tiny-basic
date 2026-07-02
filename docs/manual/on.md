# ON

**Category:** Statement  
**Syntax:** `ON expression, line1, line2, ...`

## Description

Performs a multi-way branch subroutine call (`GOSUB`). Evaluates the expression, truncates to an integer index, and executes a subroutine start to the corresponding line number in the comma-separated arguments list (0 indexed).

## Example

```basic
10 LET A = 1
20 ON A, 50, 70, 90
30 END
50 PRINT("Zero")
55 RETURN
70 PRINT("One")
75 RETURN
90 PRINT("Two")
95 RETURN
```
