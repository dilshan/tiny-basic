# ANALOG

**Category:** Function  
**Syntax:** `ANALOG(pin)`

## Description

Reads the analog voltage from the specified GPIO pin. Returns a value representing the voltage (analog-to-digital converter readout).

## Example

```basic
10 PIN(26, IN)
20 LET A = ANALOG(26)
30 PRINT("Analog read: ", A)
```
