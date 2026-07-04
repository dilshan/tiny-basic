# STEP

**Category:** Statement / Keyword Modifier  
**Syntax:** `FOR variable = start TO limit STEP step_num`

## Description

Optionally defines the increment/decrement value applied to the loop counter on each iteration of a `FOR` loop.

## See also

- [FOR](for.md)
- [NEXT](next.md)

## Example

```basic
10 FOR I = 0 TO 100 STEP 10
20   PRINT(I)
30 NEXT I
```
