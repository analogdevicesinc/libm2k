# Please check our wiki page: https://wiki.analog.com/university/courses/electronics/m2k-uart-debug

# first terminal
m2kcli uart ip:192.168.2.1 -i device=1 baud_rate=9600 parity=none bits_number=8 stop_bits=1 -r bytes_number=3 format=text

#second terminal
m2kcli uart ip:192.168.2.1 -i device=0 baud_rate=9600 parity=none bits_number=8 stop_bits=1 -w data=ADI
