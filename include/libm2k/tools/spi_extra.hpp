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
#include <libm2k/tools/spi.hpp>

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
 * @brief SPI CS polarity
 */
typedef enum cs_polarity {
	ACTIVE_LOW = 0,
	ACTIVE_HIGH = 1
} cs_polarity;

/**
 * @brief SPI extra initial structure specific for ADALM2000
 */
typedef struct m2k_spi_init {
	uint8_t clock;
	uint8_t mosi;
	uint8_t miso;
	enum bit_numbering bit_numbering;
	enum cs_polarity cs_polarity;
	libm2k::context::M2k *context;
} m2k_spi_init;

/**
 * @brief SPI extra descriptor specific for ADALM2000
 */
typedef struct m2k_spi_desc {
	uint8_t clock;
	uint8_t mosi;
	uint8_t miso;
	enum bit_numbering bit_numbering;
	enum cs_polarity cs_polarity;
	libm2k::context::M2k *context;
	libm2k::digital::M2kDigital *digital;
	unsigned int sample_rate;
} m2k_spi_desc;

/**
* @private
*/
LIBM2K_API int32_t spi_init_write_only(struct spi_desc **desc,
				       const struct spi_init_param *param);

/**
* @private
*/
LIBM2K_API int32_t spi_write_only(struct spi_desc *desc,
				  uint8_t *data,
				  uint8_t bytes_number);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/**
 * @private
 */
LIBM2K_API std::vector<unsigned short> spi_create_buffer(struct spi_desc *desc, uint8_t *data, uint8_t bytes_number);

/**
 * @private
 */
LIBM2K_API int32_t spi_write_and_read_samples(struct spi_desc *desc, std::vector<unsigned short> samples,
						uint8_t *data, uint8_t bytes_number);

#endif //SPI_EXTRA_HPP
