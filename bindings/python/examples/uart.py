import libm2k
import time
import multiprocessing

'''
Please check our wiki page: https://wiki.analog.com/university/courses/electronics/m2k-uart-debug
'''


def print_read_data(uart_desc, size):
    data_read = bytearray(size)
    libm2k.uart_read(uart_desc, data_read)
    print(data_read.decode('utf-8'))


def main():
    context = libm2k.m2kOpen('ip:192.168.2.1')
    if context is None:
        print('Connection Error: No ADALM2000 device available/connected to your PC.')
        exit(1)

    m2k_uart_init = libm2k.m2k_uart_init()
    m2k_uart_init.bits_number = 8
    m2k_uart_init.parity = libm2k.NO_PARITY
    m2k_uart_init.stop_bits = libm2k.ONE
    m2k_uart_init.context = context

    uart_init_param = libm2k.uart_init_param()
    uart_init_param.device_id = 0
    uart_init_param.baud_rate = 9600
    uart_init_param.extra = m2k_uart_init

    uart_desc_write = libm2k.uart_init(uart_init_param)

    uart_init_param.device_id = 1
    uart_desc_read = libm2k.uart_init(uart_init_param)

    if uart_desc_write is None or uart_desc_read is None:
        print('UART Error: Could not configure UART')
        exit(1)

    read_process = multiprocessing.Process(target=print_read_data, args=(uart_desc_read, 3))
    read_process.start()
    time.sleep(1)
    libm2k.uart_write(uart_desc_write, bytearray('ADI', 'utf-8'))
    read_process.join()

    libm2k.uart_remove(uart_desc_write)
    libm2k.uart_remove(uart_desc_read)
    libm2k.contextClose(context)


if __name__ == "__main__":
    main()
