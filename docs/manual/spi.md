# SPI

**Category:** Statement / Function  
**Syntax:** `SPI(INIT)`
`SPI(START)`
`SPI(STOP)`
`SPI(WRITE, val)`
`SPI(READ)`

## Description

Manages Serial Peripheral Interface (SPI) bus operations, allowing devices to exchange data synchronously. `SPI(READ)` pulls from a buffer stored during last transactions.

## See also

- [I2C](i2c.md)
- [WRITE](write.md)

## Example

```basic
10 SPI(INIT)
20 SPI(START)
30 SPI(WRITE, 0xA1)
40 LET A = SPI(READ)
50 SPI(STOP)
60 PRINT(A)
```
