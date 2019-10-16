/*
 * Copyright 2019 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SPI_EXTRA_HPP
#define SPI_EXTRA_HPP

#include <libm2k/m2k.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup spi
* @brief Contains the extra SPI structures specific for ADALM2000
* @{
*/

/**
 * @brief SPI bit numbering
 */
typedef enum bit_numbering {
	LSB = 1,
	MSB = -1
} bit_numbering;

/**
 * @brief SPI extra initial structure specific for ADALM2000
 */
typedef struct m2k_spi_init {
	uint8_t clock;
	uint8_t mosi;
	uint8_t miso;
	enum bit_numbering bit_numbering;
	libm2k::contexts::M2k *context;
} m2k_spi_init;

/**
 * @brief SPI extra descriptor specific for ADALM2000
 */
typedef struct m2k_spi_desc {
	uint8_t clock;
	uint8_t mosi;
	uint8_t miso;
	enum bit_numbering bit_numbering;
	libm2k::contexts::M2k *context;
	libm2k::digital::M2kDigital *digital;
	unsigned int sample_rate;
} m2k_spi_desc;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //SPI_EXTRA_HPP
