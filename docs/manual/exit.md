# EXIT

**Category:** Statement  
**Syntax:** `EXIT`

## Description

Immediately terminates the execution of the innermost active loop construct (`FOR`, `WHILE`, or `REPEAT`) and jumps control flow to the instruction following the loop block.

## See also

- [STOP](stop.md)
- [END](end.md)

## Example

```basic
10 FOR I = 1 TO 10
20   IF I = 5 THEN EXIT
30   PRINT(I)
40 NEXT I
```
