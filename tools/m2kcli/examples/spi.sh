#Please check Electronics Lab 14 on our wiki page: https://wiki.analog.com/university/courses/electronics/electronics-lab-14
#Configure the connections for 'Unipolar output operation' mode

m2kcli spi auto -i frequency=1000000 clk=1 mosi=2 miso=7 bit_numbering=MSB cs=0 mode=3 cs_polarity=0 -w data=0x09,0xC4
