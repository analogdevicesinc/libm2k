#capture 100 samples - binary format
m2kcli digital auto --capture buffer_size=100 format=binary

#capture 20 samples - csv format
m2kcli digital auto --capture buffer_size=20

#generate a cyclic signal for the given channels based on the samples located in file.csv
m2kcli digital auto --generate channel=0,1,4,7,13,15 cyclic=1 file=file.csv

#generate a non-cyclic signal for the given channels based on the samples fave as input
m2kcli digital auto --generate channel=1,2,3,4,5 cyclic=0 format=binary
# ~$1000
# ~$1000
# ~$1000
# ~$1000
# ~$CTRL + D
# ~$Press ENTER to stop the generation...
