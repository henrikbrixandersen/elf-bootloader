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

#include <xparameters.h>

/*
 * Device ID of the SPI controller to use
 */
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID

/*
 * Slave select pin for the SPI flash
 */
#define SPI_FLASH_SLAVE_SELECT	1

/*
 * Number of dummy bytes (derived from number of dummy cycles)
 * required for the SPI flash at the given frequency
 */
#define SPI_FLASH_NDUMMY_BYTES	4

/*
 * Base address of the ELF image in the SPI flash
 */
#define ELF_IMAGE_BASEADDR		0x219000

/*
 * Maximum number of bytes to read from flash in one go.
 * Must be large enough to accommodate the ELF32 header (52 bytes).
 */
#define EFFECTIVE_READ_BUFFER_SIZE		256

/*
 * SPI read operation to use
 */
#define SPI_READ_OPERATION				0x6B

/*
 * Byte offset for the valid bytes in ReadBuffer
 */
#define SPI_VALID_DATA_OFFSET			(4 + SPI_FLASH_NDUMMY_BYTES)

/*
 * Enable debug for the ELF loader
 */
//#define DEBUG_ELF_LOADER
