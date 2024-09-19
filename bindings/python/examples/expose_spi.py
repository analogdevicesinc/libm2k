#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

import libm2k

'''
SPI Config:
    CLK - DIO_0
    MOSI - DIO_1
    MISO - DIO_3
    CS - DIO_2
'''


def main():
    context = libm2k.m2kOpen()
    if context is None:
        print('Connection Error: No ADALM2000 device available/connected to your PC.')
        exit(1)

    digital = context.getDigital()

    # setup SPI
    m2k_spi_init = libm2k.m2k_spi_init()
    m2k_spi_init.clock = 0
    m2k_spi_init.mosi = 1
    m2k_spi_init.miso = 3
    m2k_spi_init.bit_numbering = libm2k.MSB
    m2k_spi_init.cs_polarity = libm2k.ACTIVE_LOW
    m2k_spi_init.context = context

    spi_init_param = libm2k.spi_init_param()
    spi_init_param.max_speed_hz = 1000000
    spi_init_param.mode = libm2k.SPI_MODE_0
    spi_init_param.chip_select = 2
    spi_init_param.extra = m2k_spi_init

    spi_desc = libm2k.spi_init(spi_init_param)
    if spi_desc is None:
        print('SPI Error: Could not configure SPI')
        exit(1)

    data = bytearray([0x4A, 0xF1])
    buffer = libm2k.spi_create_buffer(spi_desc, data)
    print(f'Digital buffer: {buffer}')

    # here can be modified the data inside the digital buffer

    # transmitting data
    libm2k.spi_write_and_read_samples(spi_desc, buffer, data)
    print(f'Received data: {data}')  # received data
    libm2k.spi_remove(spi_desc)
    libm2k.contextClose(context, True)


if __name__ == "__main__":
    main()
