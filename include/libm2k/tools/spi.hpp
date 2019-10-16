/***************************************************************************//**
 *   @author DBogdan (dragos.bogdan@analog.com)
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

#ifndef SPI_HPP
#define SPI_HPP

#include <libm2k/m2kglobal.hpp>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define        SPI_CPHA        0x01
#define        SPI_CPOL        0x02

/**
* @defgroup communication Communication
* @brief Contains communication protocols
* @{
*/

/**
 * @defgroup spi SPI
 * @brief Contains the functionality of the SPI
 * @{
 */

/**
 * @brief SPI modes
 */
typedef enum spi_mode {
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPI_CPHA),
	SPI_MODE_2 = (SPI_CPOL | 0),
	SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
} spi_mode;


/**
 * @brief SPI initial structure
 */
typedef struct spi_init_param {
	uint32_t max_speed_hz;
	uint8_t chip_select;
	enum spi_mode mode;
	void *extra;
} spi_init_param;

/**
 * @brief SPI descriptor
 */
typedef struct spi_desc {
	uint32_t max_speed_hz;
	uint8_t chip_select;
	enum spi_mode mode;
	void *extra;
} spi_desc;

/**
 * @brief Initialize the SPI communication peripheral
 * @param desc The SPI descriptor
 * @param param The structure that contains the SPI parameters
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python and C# 'desc' parameter is returned and it is not passed anymore as a parameter
 */
LIBM2K_API int32_t spi_init(struct spi_desc **desc,
			    const struct spi_init_param *param);

/**
 * @brief Free the resources allocated by spi_init()
 * @param desc The SPI descriptor
 * @return 0 in case of success, -1 otherwise
 */
LIBM2K_API int32_t spi_remove(struct spi_desc *desc);

/**
 * @brief Write and read data to/from SPI
 * @param desc - The SPI descriptor
 * @param data - The buffer with the transmitted/received data
 * @param bytes_number - Number of bytes to write/read
 * @return 0 in case of success, -1 otherwise
 *
 * @note In Python 'data' parameter is a bytearray and 'bytes_number' parameter is not passed anymore
 */
LIBM2K_API int32_t spi_write_and_read(struct spi_desc *desc,
				      uint8_t *data,
				      uint8_t bytes_number);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif //SPI_HPP
