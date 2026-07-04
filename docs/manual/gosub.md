# GOSUB

**Category:** Statement  
**Syntax:** `GOSUB line_number`

## Description

Subroutine invocation. Causes program execution to branch to the specified line number; when the `RETURN` command is encountered, execution branches to the command immediately following the most recent `GOSUB` command.

## See also

- [GOTO](goto.md)
- [RETURN](return.md)

## Example

```basic
10 GOSUB 100
20 END
100 PRINT("Within Subroutine")
110 RETURN
```
