#ifndef __PLATFORM_SPI_HEADER__
#define __PLATFORM_SPI_HEADER__

void spiInit();

void spiSelect();
void spiDeselect();

unsigned char spiTransfer(unsigned char output_byte);

#endif /* __PLATFORM_SPI_HEADER__ */