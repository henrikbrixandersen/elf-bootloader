/* Copyright (c) 2015-2017 Henrik Brix Andersen <henrik@brixandersen.dk>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include <mb_interface.h>
#include <xil_cache.h>
#include <xil_printf.h>
#include <xspi.h>
#include <xparameters.h>

#include "elf32.h"
#include "eb-config.h"

static XSpi Spi;
static u8 ReadBuffer[EFFECTIVE_READ_BUFFER_SIZE + SPI_VALID_DATA_OFFSET];

/*
 * Reduce code size on Microblaze (no interrupts used)
 */
#ifdef __MICROBLAZE__
void _interrupt_handler() {}
void _exception_handler() {}
void _hw_exception_handler() {}
#endif

/**
 * Simple wrapper function for reading a number of bytes from SPI flash.
 */
int spi_flash_read(XSpi *InstancePtr, u32 FlashAddress, u8 *RecvBuffer, unsigned int ByteCount)
{
	RecvBuffer[0] = SPI_READ_OPERATION;
	RecvBuffer[1] = (FlashAddress >> 16) & 0xFF;
	RecvBuffer[2] = (FlashAddress >> 8) & 0xFF;
	RecvBuffer[3] = FlashAddress & 0xFF;

	return XSpi_Transfer(InstancePtr, RecvBuffer, RecvBuffer, ByteCount + SPI_VALID_DATA_OFFSET);
}

int main()
{
	elf32_hdr hdr;
	elf32_phdr phdr;
	u32 addr;
	int ret, i, j;

	/*
	 * Disable caches
	 */
#if (XPAR_MICROBLAZE_USE_DCACHE == 1)
	Xil_DCacheInvalidate()
	Xil_DCacheDisable();
#endif
#if (XPAR_MICROBLAZE_USE_ICACHE == 1)
	Xil_ICacheInvalidate();
	Xil_ICacheDisable();
#endif

	print("\r\nSPI ELF Bootloader\r\n");

	/*
	 * Initialize the SPI controller in polled mode
	 * and enable the flash slave select
	 */
	ret = XSpi_Initialize(&Spi, SPI_DEVICE_ID);
	if (ret != XST_SUCCESS) {
		if (ret == XST_DEVICE_IS_STARTED) {
			if (XSpi_Stop(&Spi) == XST_DEVICE_BUSY) {
				print("SPI device is busy\r\n");
				return -1;
			}
		} else if (ret == XST_DEVICE_NOT_FOUND) {
			print("SPI device not found\r\n");
			return -1;
		} else {
			print("Unknown error\r\n");
			return -1;
		}
	}

	ret = XSpi_SetOptions(&Spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
	if (ret != XST_SUCCESS) {
		if (ret == XST_DEVICE_BUSY) {
			print("SPI device is busy\r\n");
			return -1;
		} else if (ret == XST_SPI_SLAVE_ONLY) {
			print("SPI device is slave-only\r\n");
			return -1;
		} else {
			print("Unknown error\r\n");
			return -1;
		}
	}

	ret = XSpi_SetSlaveSelect(&Spi, SPI_FLASH_SLAVE_SELECT);
	if (ret != XST_SUCCESS) {
		if (ret == XST_DEVICE_BUSY) {
			print("SPI device is busy\r\n");
			return ret;
		} else if (ret == XST_SPI_TOO_MANY_SLAVES) {
			print("Too many SPI slave devices\r\n");
			return ret;
		} else {
			print("Unknown error\r\n");
			return ret;
		}
	}

	XSpi_Start(&Spi);
	XSpi_IntrGlobalDisable(&Spi);

	print("Copying ELF image from SPI flash @ 0x");
	putnum(ELF_IMAGE_BASEADDR);
	print(" to RAM\r\n");

	/*
	 * Read ELF header
	 */
	ret = spi_flash_read(&Spi, ELF_IMAGE_BASEADDR, ReadBuffer, sizeof(hdr));
	if (ret == XST_SUCCESS) {
		memcpy(&hdr, ReadBuffer + SPI_VALID_DATA_OFFSET, sizeof(hdr));

#ifdef DEBUG_ELF_LOADER
		print("hdr.ident:\r\n");
		for (i = 0; i < HDR_IDENT_NBYTES; i++) {
			putnum(hdr.ident[i]);
			print("\r\n");
		}
#endif

	} else {
		print("Failed to read ELF header");
		return -1;
	}

	/*
	 * Validate ELF header
	 */
	if (hdr.ident[0] != HDR_IDENT_MAGIC_0 ||
			hdr.ident[1] != HDR_IDENT_MAGIC_1 ||
			hdr.ident[2] != HDR_IDENT_MAGIC_2 ||
			hdr.ident[3] != HDR_IDENT_MAGIC_3) {
		print("Invalid ELF header");
		return -1;
	}

	/**
	 * Read ELF program headers
	 */
	for (i = 0; i < hdr.phnum; i++) {
		ret = spi_flash_read(&Spi, ELF_IMAGE_BASEADDR + hdr.phoff + i * sizeof(phdr), ReadBuffer, sizeof(phdr));
		if (ret == XST_SUCCESS) {
			memcpy(&phdr, ReadBuffer + SPI_VALID_DATA_OFFSET, sizeof(phdr));

			if (phdr.type == PHDR_TYPE_LOAD) {
				/*
				 * Copy program segment from flash to RAM
				 */
				for (addr = 0; addr < phdr.filesz; addr += EFFECTIVE_READ_BUFFER_SIZE) {
					if (addr + EFFECTIVE_READ_BUFFER_SIZE > phdr.filesz) {
						ret = spi_flash_read(&Spi, ELF_IMAGE_BASEADDR + phdr.offset + addr,
								ReadBuffer, phdr.filesz - addr);
#ifdef DEBUG_ELF_LOADER
						print("End of section: ");
						putnum(i);
						print("\r\n");

						print("filesz: ");
						putnum(phdr.filesz);
						print("\r\n");

						print("addr: ");
						putnum(addr);
						print("\r\n");

						print("segment end:\r\n");
						for (j = 15; j >= 0; j--) {
							putnum(ReadBuffer[SPI_VALID_DATA_OFFSET + phdr.filesz - addr - j]);
							print(" ");
						}
						print("\r\n");
#endif

					} else {
						ret = spi_flash_read(&Spi, ELF_IMAGE_BASEADDR + phdr.offset + addr,
								ReadBuffer, EFFECTIVE_READ_BUFFER_SIZE);
#ifdef DEBUG_ELF_LOADER
						if (addr == 0) {
							print("segment start:\r\n");
							for (j = 0; j < EFFECTIVE_READ_BUFFER_SIZE; j++) {
								putnum(ReadBuffer[SPI_VALID_DATA_OFFSET + j]);
								print(" ");
							}
							print("\r\n");
						}
#endif
					}

					if (ret == XST_SUCCESS) {
						if (addr + EFFECTIVE_READ_BUFFER_SIZE > phdr.filesz) {
							memcpy((void*)phdr.paddr + addr, ReadBuffer + SPI_VALID_DATA_OFFSET, phdr.filesz - addr);
						} else {
							memcpy((void*)phdr.paddr + addr, ReadBuffer + SPI_VALID_DATA_OFFSET, EFFECTIVE_READ_BUFFER_SIZE);
						}

						if (addr % 1024 == 0) {
							print(".");
						}
					} else {
						print("Failed to read ELF program segment");
						return -1;
					}
				}

				/*
				 * Fill remaining segment in RAM with zeros
				 */
				if (phdr.memsz > phdr.filesz) {
					memset((void*)(phdr.paddr + phdr.filesz), 0, phdr.memsz - phdr.filesz);
				}
			}
		} else {
			print("Failed to read ELF program header");
			return -1;
		}
	}

	/**
	 * Jump to ELF entry address
	 */
	print("\r\nTransferring execution to program @ 0x");
	putnum(hdr.entry);
	print("\r\n");
	((void (*)())hdr.entry)();

	// Never reached
	return 0;
}
