# GET

**Category:** Function  
**Syntax:** `GET(pin)`

## Description

Reads the digital state input logic level of GPIO pin `pin`. Returns `HIGH` (1) or `LOW` (0).

## See also

- [INPUT](input.md)
- [READ](read.md)

## Example

```basic
10 PIN(12, IN)
20 LET A = GET(12)
30 IF A = HIGH THEN PRINT("High state detected")
```
