#calibrate the DAC
m2kcli analog-out auto --calibrate

#generate a cyclic signal based on the samples located in file.csv - csv format
m2kcli analog-out auto --generate channel=0 cyclic=1 raw=0 buffer_size=1024 < file.csv

#generate a cyclic signal based on the samples located in file.bin - binary format
m2kcli analog-out auto -9 channel=0,1 cyclic=1 raw=0 format=binary < file.bin
