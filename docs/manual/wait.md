# WAIT

**Category:** Statement  
**Syntax:** `WAIT(pin, digital_state)`

## Description

Blocks program execution until the digital level read from GPIO pin `pin` equals the expected `digital_state` (`HIGH` or `LOW`). Performs read checks recursively.

## See also

- [DELAY](delay.md)
- [SOUND](sound.md)

## Example

```basic
10 PIN(4, IN)
20 WAIT(4, HIGH)
30 PRINT("Button pressed")
```
