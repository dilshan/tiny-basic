# WRITE

**Category:** Keyword / Command Modifier  
**Syntax:** `I2C(WRITE, data)`
`SPI(WRITE, data)`

## Description

Used within `I2C` or `SPI` function to specify data transmission bytes onto their respective communication channels.

## See also

- [READ](read.md)
- [SPI](spi.md)
- [I2C](i2c.md)

## Example

```basic
10 I2C(WRITE, 0xFF)
```
