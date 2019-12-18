# example for accessing the terminal of another ADALM-2000
# GND < --- > GND
# UART RX (M2K 1) < --- > DIO_7 (M2K 2)
# UART TX (M2K 1) < --- > DIO_15 (M2K 2)

m2kcli uart-terminal auto -i baud_rate=115200 rx=15 tx=7 stop_bits=1 parity=none bits_number=8
