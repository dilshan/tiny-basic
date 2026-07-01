# READ

**Category:** Keyword / Command Modifier  
**Syntax:** `I2C(READ, ack_expr)`
`SPI(READ)`

## Description

Used within `I2C` or `SPI` functions to command data retrieval from the bus.

## Example

```basic
10 LET A = SPI(READ)
20 PRINT(HEX, A)
```
