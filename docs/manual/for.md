# FOR

**Category:** Statement  
**Syntax:** `FOR variable = start TO limit`
`FOR variable = start TO limit STEP step_num`

## Description

Starts a bounded counting loop block. The loop variable increments by 1 (or the value of `STEP` if defined). Loops continue until the variable value passes `limit`.

## Example

```basic
10 FOR I = 1 TO 5
20   PRINT(I)
30 NEXT I
40 FOR J = 10 TO 1 STEP -2
50   PRINT(J)
60 NEXT J
```
