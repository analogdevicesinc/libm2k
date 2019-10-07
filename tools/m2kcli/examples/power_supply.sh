#get the voltage for both channels
m2kcli power-supply auto --capture channel=0,1

#generate 4V on both channels
m2kcli power-supply auto --generate channel=0,1 value=4