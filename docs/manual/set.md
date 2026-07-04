# SET

**Category:** Statement  
**Syntax:** `SET(pin, value)`

## Description

Sets the digital output level of the GPIO pin. Values are defined as `HIGH` (1) or `LOW` (0).

## See also

- [LET](let.md)
- [PRINT](print.md)

## Example

```basic
10 PIN(13, OUT)
20 SET(13, HIGH)
30 DELAY(500)
40 SET(13, LOW)
```
