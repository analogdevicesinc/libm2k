/***************************************************************************//**
*   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef I2C_HPP
#define I2C_HPP

#include <libm2k/m2kglobal.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
* @addtogroup communication
* @{
* @defgroup i2c I2C
* @brief Contains the functionality of the I2C
* @{
*/

/**
 * @enum i2c_transfer_mode
 * @brief I2C transfer modes
 *
 * i2c_general_call: 7-bit addressing
*/
typedef enum i2c_transfer_mode {
	i2c_general_call =	0x01,
	i2c_repeated_start =	0x02,
	i2c_10_bit_transfer =	0x04
} i2c_transfer_mode;

/**
* @brief I2C initial structure
*/
typedef struct i2c_init_param {
	uint32_t max_speed_hz;
	uint16_t slave_address;
	void *extra;
} i2c_init_param;

/**
 * @brief I2C descriptor
 */
typedef struct i2c_desc {
	uint32_t max_speed_hz;
	uint16_t slave_address;
	void *extra;
} i2c_desc;

/**
 * @brief Initialize the I2C communication peripheral
 * @param desc The I2C descriptor
 * @param param The structure that contains the I2C parameters
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python and C# 'desc' parameter is returned and it is not passed anymore as a parameter
 */
LIBM2K_API int32_t i2c_init(struct i2c_desc **desc,
			    const struct i2c_init_param *param);

/**
 * @brief Free the resources allocated by i2c_init()
 * @param desc The I2C descriptor
 * @return 0 in case of success, -1 otherwise
 */
LIBM2K_API int32_t i2c_remove(struct i2c_desc *desc);

/**
 * @brief Write data to a slave device
 * @param desc - The I2C descriptor
 * @param data - The buffer with the transmitted data
 * @param bytes_number - Number of bytes to write
 * @param option I2C transfer mode
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python 'data' parameter is a bytearray and 'bytes_number' parameter is not passed anymore
 */
LIBM2K_API int32_t i2c_write(struct i2c_desc *desc,
			     uint8_t *data,
			     uint8_t bytes_number,
			     uint8_t option);

/**
 * @brief Read data from a slave device
 * @param desc - The I2C descriptor
 * @param data - The buffer with the received data
 * @param bytes_number - Number of bytes to read
 * @param option I2C transfer mode
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python 'data' parameter is a bytearray and 'bytes_number' parameter is not passed anymore
 */
LIBM2K_API int32_t i2c_read(struct i2c_desc *desc,
			    uint8_t *data,
			    uint8_t bytes_number,
			    uint8_t option);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif //I2C_HPP
