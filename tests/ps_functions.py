import libm2k
import numpy as np
import time
from open_context_and_files import ain, ctx
import random

def config_for_ps_test(ps,ain):
    """Retrieve the Power supply object and enabe the power supply channels
    Arguments:
        ps-- Power Supply object
        ain  -- AnalogIn object\n
    """
    ctx.calibrateADC()
    #enable channels
    ps.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ps.enableChannel(libm2k.ANALOG_IN_CHANNEL_2,True)
    if ain.isChannelEnabled(libm2k.ANALOG_IN_CHANNEL_1)==False:
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    if ain.isChannelEnabled(libm2k.ANALOG_IN_CHANNEL_2)==False:
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    return

def ps_test_positive(ps,ain, file):
    """Tests the positive power supply
    Arguments:
        ps -- Power Supply object
        ain -- AnalogIn object
    Returns:
        pos_supply-- Vector that  holds 1 if the  voltage value read on the channel equals the voltage sent
    """
    file.write("\n\nPositive power supply test:\n")
    voltage=0
    t=0.1 #threshold value 
    pos_supply=[]
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1,libm2k.PLUS_MINUS_25V)
    while voltage<=5:
        ps.pushChannel(libm2k.ANALOG_IN_CHANNEL_1, voltage)
        time.sleep(0.2)
        read_voltage=(ain.getVoltage()[libm2k.ANALOG_IN_CHANNEL_1])
        file.write("Sent voltage: "+str(voltage)+"\n")
        file.write("Read voltage: "+str(read_voltage)+"\n")
        if(read_voltage>=(voltage-t) and read_voltage<=(voltage+t)):
            pos_supply=np.append(pos_supply,1)
        else:
            pos_supply=np.append(pos_supply,0)
        voltage=voltage+random.uniform(0.4,0.5) #add a random value to the previous voltage value
    return pos_supply


def ps_test_negative(ps,ain, file):
    """Tests the negativepower supply
    Arguments:
        ps -- Power Supply object
        ain -- AnalogIn object
    Returns:
        neg_supply-- Vector that  holds 1 if the  voltage value read on the channel equals the voltage sent
    """
    file.write("\n\nNegative power supply test:\n")
    voltage=0
    neg_supply=[]
    t=0.2 #threshold value
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2,libm2k.PLUS_MINUS_25V)
    while voltage>=-5:
        ps.pushChannel(libm2k.ANALOG_IN_CHANNEL_2, voltage)
        time.sleep(0.2)
        read_voltage=(ain.getVoltage()[libm2k.ANALOG_IN_CHANNEL_2])
        file.write("Sent voltage: "+str(voltage)+"\n")
        file.write("Read voltage: "+str(read_voltage)+"\n")
        if(read_voltage<=(voltage+t) and read_voltage>=(voltage-t)):
            neg_supply=np.append(neg_supply,1)
        else:
            neg_supply=np.append(neg_supply,0)
        voltage=voltage-random.uniform(0.4, 0.5) #subtract a random value from the previous voltage value
    
    
    
    return neg_supply

