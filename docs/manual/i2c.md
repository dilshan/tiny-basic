# I2C

**Category:** Function  
**Syntax:** `I2C(INIT)`
`I2C(START)`
`I2C(RESTART)`
`I2C(STOP)`
`I2C(WRITE, num)`
`I2C(READ, ack)`

## Description

Controls Inter-Integrated Circuit (I2C) communication bus peripherals. Supports commands for initializing the port, transmitting start/restart/stop conditions, writing bytes, or reading bytes.

## See also

- [SPI](spi.md)
- [READ](read.md)

## Example

```basic
10 I2C(START)
20 I2C(WRITE, 0xA0)
30 I2C(WRITE, 0x0A)
40 I2C(RESTART)
50 I2C(WRITE, 0xA1)
60 PRINT("VAL = ", I2C(READ, 0))
70 I2C(STOP)
```
