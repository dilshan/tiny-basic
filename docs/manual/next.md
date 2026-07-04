# NEXT

**Category:** Statement  
**Syntax:** `NEXT variable`

## Description

Marks the end of a `FOR` loop block. Increments the loop variable and jumps back to the code segment within the loop if the limit condition is not yet met.

## See also

- [FOR](for.md)
- [STEP](step.md)

## Example

```basic
10 FOR I = 1 TO 3
20   PRINT(I)
30 NEXT I
```
