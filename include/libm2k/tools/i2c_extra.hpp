/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef I2C_EXTRA_HPP
#define I2C_EXTRA_HPP

#include <libm2k/m2k.hpp>
#include <libm2k/tools/i2c.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup i2c
* @brief Contains the extra I2C structures specific for ADALM2000
* @{
*/

/**
* @brief I2C extra initial structure specific for ADALM2000
*/
typedef struct m2k_i2c_init {
	uint8_t scl;
	uint8_t sda;
	libm2k::context::M2k *context;
} m2k_i2c_init;

/**
 * @brief I2C extra descriptor specific for ADALM2000
 */
typedef struct m2k_i2c_desc {
	uint8_t scl;
	uint8_t sda;
	libm2k::context::M2k *context;
	libm2k::digital::M2kDigital *digital;
	unsigned int sample_rate;
} m2k_i2c_desc;

/**
* @private
*/
LIBM2K_API int32_t i2c_init_write_only(struct i2c_desc **desc,
				       const struct i2c_init_param *param);

/**
* @private
*/
LIBM2K_API int32_t i2c_write_only(struct i2c_desc *desc,
				  uint8_t *data,
				  uint8_t bytes_number,
				  uint8_t option);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //I2C_EXTRA_HPP
