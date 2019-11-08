import libm2k

'''
This example uses EVAL-ADT7420-PMDZ as a slave
Hardware configuration:
    (ADALM2000) DIO_0 <---> Pin 1 (ADT7420) <--->  10 kilohms resistor <--- V+ (ADALM2000)
    (ADALM2000) DIO_1 <---> Pin 3 (ADT7420) <---> 10 kilohms resistor <--- V+ (ADALM2000)
    (ADALM2000) GND <---> Pin 5 (ADT7420)
    (ADALM2000) V+ ---> Pin 7 (ADT7420)
'''

def convert_temperature(data):
    msb_temp = data[0]
    lsb_temp = data[1]

    temperature = (msb_temp << 8) + lsb_temp
    if temperature & 0x8000:
        temperature = (temperature - 65536) / 128
    else:
        temperature = temperature / 128
    return temperature


def main():
    context = libm2k.m2kOpen("ip:192.168.2.1")
    if context is None:
        print('Connection Error: No ADALM2000 device available/connected to your PC.')
        exit(1)

    print('Calibrating ADC . . .')
    context.calibrateADC()

    power_supply = context.getPowerSupply()

    power_supply.enableChannel(0, True)
    power_supply.pushChannel(0, 3.3)

    m2k_i2c_init = libm2k.m2k_i2c_init()
    m2k_i2c_init.scl = 0
    m2k_i2c_init.sda = 1
    m2k_i2c_init.context = context

    i2c_init_param = libm2k.i2c_init_param()
    i2c_init_param.max_speed_hz = 100000
    i2c_init_param.slave_address = 0x48
    i2c_init_param.extra = m2k_i2c_init

    i2c_desc = libm2k.i2c_init(i2c_init_param)
    if i2c_desc is None:
        print('I2C Error: Could not configure I2C')
        exit(1)

    print('Initiating I2C transfer . . .')
    if libm2k.i2c_write(i2c_desc, bytearray([0x0B]), libm2k.i2c_general_call | libm2k.i2c_repeated_start) == -1:
        exit(1)

    data_read_config = bytearray(1)
    libm2k.i2c_read(i2c_desc, data_read_config, libm2k.i2c_general_call)
    if len(data_read_config) == 0:
        exit(1)

    print('Reading the temperature . . .')
    if libm2k.i2c_write(i2c_desc, bytearray([0]), libm2k.i2c_general_call | libm2k.i2c_repeated_start) == -1:
        exit(1)

    data_read_temperature = bytearray(2)
    libm2k.i2c_read(i2c_desc, data_read_temperature, libm2k.i2c_general_call)
    if len(data_read_temperature) == 0:
        exit(1)

    temperature = convert_temperature(data_read_temperature)
    print('Temperature: ' + str(temperature) + '\u2103')

    libm2k.i2c_remove(i2c_desc)
    libm2k.contextClose(context, True)


if __name__ == "__main__":
    main()
