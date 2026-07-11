# PWM

**Category:** Function  
**Syntax:** `PWM(pin, level)`

## Description

The PWM function generates a Pulse Width Modulation signal on a specified pin. To use it, provide the specific hardware `pin` you want to control (checking your hardware documentation first to ensure that pin supports PWM output), followed by the desired output `level`. This `level` dictates the intensity of the signal, typically ranging from `0` (completely off) up to `255` (fully on).

## See also

- [ANALOG](analog.md)
- [PIN](pin.md)
- [SET](set.md)

## Example

```basic
10 PIN(13, OUT)
20 PWM(13, 50)
```