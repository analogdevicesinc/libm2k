/***************************************************************************//**
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef UART_HPP
#define UART_HPP

#include <libm2k/m2kglobal.hpp>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
* @defgroup communication Communication
* @brief Contains communication protocols
* @{
*/

/**
 * @defgroup uart UART
 * @brief Contains the functionality of the UART
 * @{
 */

/**
 * @brief UART initial structure
 */
typedef struct uart_init_param {
	uint8_t		device_id;
	uint32_t 	baud_rate;
	void 		*extra;
}uart_init_param;

/**
 * @brief UART descriptor
 */
typedef struct uart_desc {
	uint8_t 	device_id;
	uint32_t 	baud_rate;
	void 		*extra;
}uart_desc;

/**
 * @brief Read data to UART
 * @param desc - The UART descriptor
 * @param data - The buffer with the received data
 * @param bytes_number - Number of bytes to read
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python 'data' parameter is a bytearray and 'bytes_number' parameter is not passed anymore
 */
LIBM2K_API int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number);

/**
 * @brief Write data to UART
 * @param desc - The UART descriptor
 * @param data - The buffer with the transmitted data
 * @param bytes_number - Number of bytes to write
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python 'data' parameter is a bytearray and 'bytes_number' parameter is not passed anymore
 */
LIBM2K_API int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number);

/**
 * @brief Initialize the UART communication peripheral
 * @param desc The UART descriptor
 * @param param The structure that contains the UART parameters
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python and C# 'desc' parameter is returned and it is not passed anymore as a parameter
 */
LIBM2K_API int32_t uart_init(struct uart_desc **desc, const struct uart_init_param *param);

/**
 * @brief Free the resources allocated by uart_init()
 * @param desc The UART descriptor
 * @return 0 in case of success, -1 otherwise
 */
LIBM2K_API int32_t uart_remove(struct uart_desc *desc);

/**
 * @brief Check if UART errors occurred.
 * @param desc The UART descriptor
 * @return Number of errors.
 */
LIBM2K_API uint32_t uart_get_errors(struct uart_desc *desc);

/**
* @}
* @}
*/

#ifdef __cplusplus
}
#endif
#endif //UART_HPP
