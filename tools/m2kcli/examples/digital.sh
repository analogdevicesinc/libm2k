#capture 1000 samples - binary format
m2kcli digital auto --capture buffer_size=100 nb_samples=1000 format=binary

#continuously samples capturing - csv format
m2kcli digital auto --capture buffer_size=1024

#generate a cyclic signal for the given channels based on the samples located in file.csv
m2kcli digital auto --generate channel=0,1,4,7,13,15 cyclic=1 < file.csv

#generate a non-cyclic signal for the given channels based on the samples gave as input
m2kcli digital auto --generate channel=1,2,3,4,5 cyclic=0 format=binary
# ~$1000
# ~$1000
#  ...
# ~$1000
# ~$1000
# ~$CTRL + C