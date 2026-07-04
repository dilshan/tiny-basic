# ANALOG

**Category:** Function  
**Syntax:** `ANALOG(pin)`

## Description

Reads the current voltage on the specified GPIO pin using the microcontroller's internal Analog-to-Digital Converter (ADC).

The function returns an integer corresponding to the raw ADC readout, rather than an absolute voltage value. The range of this return value depends on your specific MCU's hardware ADC resolution (for example, `0` to `1023` for a 10-bit ADC, or `0` to `4095` for a 12-bit ADC). 

Typically, a return value of `0` represents ground (0V), while the maximum integer value corresponds to the system's analog reference voltage (usually VCC, such as 3.3V or 5.0V).

## See also

- [PIN](pin.md)
- [OUT](out.md)

## Example

```basic
10 PIN(26, IN)
20 LET A = ANALOG(26)
30 PRINT("Analog read: ", A)
```
