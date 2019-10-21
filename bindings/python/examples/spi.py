import libm2k

'''
Please check Electronics Lab 14 on our wiki page: https://wiki.analog.com/university/courses/electronics/electronics-lab-14
Configure the connections for 'Unipolar output operation' mode
'''

VOLTAGE = 2.5

def get_register_data(voltage):
    if voltage < 0:
        return [0, 0]
    if voltage > 4.095:
        return [0x0F, 0xFF]

    register_value = int(voltage * 1000)
    return [(register_value >> 8) & 0xFF, register_value & 0xFF]


def main():
    context = libm2k.m2kOpen('ip:192.168.2.1')
    if context is None:
        print('Connection Error: No ADALM2000 device available/connected to your PC.')
        exit(1)

    context.calibrateADC()
    analog_in = context.getAnalogIn()
    power_supply = context.getPowerSupply()
    digital = context.getDigital()

    # setup analog in
    analog_in.setOversamplingRatio(1)
    analog_in.setSampleRate(1000000)
    analog_in.enableChannel(0, True)
    analog_in.enableChannel(1, False)
    analog_in.setRange(libm2k.ANALOG_IN_CHANNEL_1, libm2k.PLUS_MINUS_25V)

    # enable LDAC
    digital.setDirection(3, libm2k.DIO_OUTPUT)
    digital.enableChannel(3, True)

    # enable CLR
    digital.setDirection(4, libm2k.DIO_OUTPUT)
    digital.enableChannel(4, True)

    # setup SPI
    m2k_spi_init = libm2k.m2k_spi_init()
    m2k_spi_init.clock = 1
    m2k_spi_init.mosi = 2
    m2k_spi_init.miso = 7  # dummy value - miso is not used in this example
    m2k_spi_init.bit_numbering = libm2k.MSB
    m2k_spi_init.context = context

    spi_init_param = libm2k.spi_init_param()
    spi_init_param.max_speed_hz = 1000000
    spi_init_param.mode = libm2k.SPI_MODE_3
    spi_init_param.chip_select = 0
    spi_init_param.extra = m2k_spi_init

    spi_desc = libm2k.spi_init(spi_init_param)
    if spi_desc is None:
        print('SPI Error: Could not configure SPI')
        exit(1)

    power_supply.enableChannel(0, True)
    power_supply.pushChannel(0, 5)

    # CLR and LDAC high as long as bits are transmitted
    digital.setValueRaw(3, libm2k.HIGH)
    digital.setValueRaw(4, libm2k.HIGH)

    data = bytearray(get_register_data(VOLTAGE))
    # transmitting data
    libm2k.spi_write_and_read(spi_desc, data)
    if len(data) == 0:
        print('SPI Error: Could not transmit the data')
        exit(1)

    # update with current shift register contents
    digital.setValueRaw(3, libm2k.LOW)

    samples = analog_in.getSamples(100)
    average = sum(samples[0]) / len(samples[0])
    print('Average value: ' + str(average))

    libm2k.spi_remove(spi_desc)
    libm2k.contextClose(context, True)


if __name__ == "__main__":
    main()
