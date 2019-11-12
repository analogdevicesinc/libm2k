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

#ifndef UART_EXTRA_HPP
#define UART_EXTRA_HPP

#include <libm2k/m2k.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup uart
* @brief Contains the extra UART structures specific for ADALM2000
* @{
*/

typedef enum uart_parity {
	NO_PARITY = 0,
	ODD = 1,
	EVEN = 2,
	MARK = 3,
	SPACE = 4
}uart_parity;

typedef enum uart_stop_bits {
	ONE = 2,
	ONE_AND_A_HALF = 3,
	TWO = 4
}uart_stop_bits;

/**
* @brief UART extra initial structure specific for ADALM2000
*/
typedef struct m2k_uart_init {
	enum uart_parity parity;
	uint8_t bits_number;
	enum uart_stop_bits stop_bits;
	libm2k::contexts::M2k *context;
} m2k_uart_init;

/**
 * @brief UART extra descriptor specific for ADALM2000
 */
typedef struct m2k_uart_desc {
	enum uart_parity parity;
	uint8_t bits_number;
	enum uart_stop_bits stop_bits;
	libm2k::contexts::M2k *context;
	libm2k::digital::M2kDigital *digital;
	unsigned int sample_rate;
	unsigned int total_error_count;
} m2k_uart_desc;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //UART_EXTRA_HPP
