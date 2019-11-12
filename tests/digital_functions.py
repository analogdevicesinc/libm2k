import matplotlib.pyplot as plt
import numpy as np
import libm2k
from multiprocessing.pool import ThreadPool
import threading
def dig_reset(dig):
    """ Reset digital object
    
    Arguments:
        dig  -- Digital object\n
    """
    dig.setSampleRateIn(10000)
    dig.setSampleRateOut(10000)
    dig.setCyclic(True)
    dig.setDirection(0b1111111111111111)
    for i in range(16):
        dig.setOutputMode(i,1)
    dig.enableAllOut(True)
    return

def set_digital_trigger(dig):
    """Set the digital trigger
    
    Arguments:
        dig  -- Digital object\n
    """
    d_trig=dig.getTrigger()
    d_trig.setDigitalMode(0)
    d_trig.setDigitalStreamingFlag(True)
    for i in range(16):
        d_trig.setDigitalCondition(i,5)
    return d_trig

def ch_0_7_digital_output(dig, buff):
    """Channels 0 to 7 are set as digital output and channels 8-to 16 are set as digital input.
    A data buffer is sent to 0-7 and received on 8-16. In ch1 are saved signals for each corresponding channel which will be plotter further
    
    Arguments:
        dig  -- Digital object\n
        buff  -- Data buffer to be sent\n
    """
    #enable 8 output channels
    for i in range(8):
        dig.setDirection(i,libm2k.DIO_OUTPUT)
        dig.enableChannel(i,True)
    #enable 8 input channels
    for i in range(8,16):
        dig.setDirection(i,libm2k.DIO_INPUT)
        dig.enableChannel(i, True)
    dig.push(buff)
    ch1=[[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
    data = dig.getSamples(100)
    for i in range(16):

        if (dig.getDirection(i))==libm2k.DIO_INPUT:
            for val in data:
                if(val&(2**i)==2**i):
                    ch1[i]=np.append(ch1[i],1)
                else:
                    ch1[i]=np.append(ch1[i],0)
    return ch1

def ch_8_16_digital_output(dig,buff):
    """Channels 8 to 16 are set as digital output and channels 0-to 7 are set as digital input.
    A data buffer is sent to 8-16 and received on 0-7. In ch1 are saved signals for each corresponding channel which will be plotter further
    
    Arguments:
        dig  -- Digital object\n
        buff  -- Data buffer to be sent\n
    """
    #enable 8 output channels
    for i in range(8,16):
        dig.setDirection(i,libm2k.DIO_OUTPUT)
        dig.enableChannel(i,True)
    #enable 8 input channels
    for i in range(8):
        dig.setDirection(i,libm2k.DIO_INPUT)
        dig.enableChannel(i, True)
    dig.push(buff)
    ch2=[[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
    data = dig.getSamples(100)
    for i in range(16):
        if (dig.getDirection(i))==libm2k.DIO_INPUT:
            for val in data:
                #get the signal from the digital channel i
                if(val&(2**i)==2**i):
                    ch2[i]=np.append(ch2[i],1)
                else:
                    ch2[i]=np.append(ch2[i],0)
    return ch2

def get_data_to_check_trig_condition(d_trig,dig, channel,i,buff):
    """Set trigger condition and send a buffer. Read and build the signal on the desired channel. The result wil be further used to verify the trigger conditions
    
    Arguments:
        d_trig  -- Digital trigger\n
        dig  -- Digital object\n
        channel  --Channel under test\n
        i  -- Trigger condition\n
        buff  -- Data buffer \n
    """
    ch=[]
    n=50
    d_trig.setDigitalCondition(channel,i)
    dig.push(buff)
    data = dig.getSamples(n)

    for val in data:
        #print((val))
        if(val&(2**channel)==2**channel):
            ch=np.append(ch,1)
        else:
            ch=np.append(ch,0)

    return ch

def check_digital_trigger(channel,dig, d_trig):
    """ Verify the digital trigger for the 'rising edge, falling edge, low level, high level, any edge and no trigger' conditions
    
    Arguments:
        channel  -- Digital channel under test\n
        dig  -- Digital object\n
        d_trig  -- Digital trigger\n
    """
    dig_reset(dig)
    # low=0
    # high=1
    # level=0.5
    delay=1

    condition=[libm2k.RISING_EDGE,libm2k.FALLING_EDGE,libm2k.LOW_LEVEL,libm2k.HIGH_LEVEL,libm2k.ANY_EDGE,libm2k.NO_TRIGGER]
    trig_test=[]
    dig.setDirection(channel,libm2k.DIO_INPUT)
    dig.enableChannel(channel,True)
    dig.setDirection(channel+8,libm2k.DIO_OUTPUT)
    dig.enableChannel(channel+8,True)
    buff=[512,512,512,0,0,0,0,512,512,512]

    d_trig.setDigitalDelay(-delay)

    #go through all possible trigger conditions
    for i in condition:
        if i==libm2k.RISING_EDGE:
            ch=get_data_to_check_trig_condition(d_trig, dig,channel,i, buff)
            #print(ch)
            #plt.figure(1)
            #plt.plot(ch)

            if ch[delay]<=ch[delay+3]:
                print("rising")
                trig_test=np.append(trig_test,1)
            else:
                trig_test=np.append(trig_test,0)

        elif i==libm2k.FALLING_EDGE:
            ch=get_data_to_check_trig_condition(d_trig, dig,channel,i, buff)
            # print(ch)
            # plt.figure(2)
            # plt.plot(ch)
            if ch[delay]>=ch[delay+3]:
                print("falling")
                trig_test=np.append(trig_test,1)
            else:
                trig_test=np.append(trig_test,0)

        elif i==libm2k.LOW_LEVEL:
            ch=get_data_to_check_trig_condition(d_trig, dig,channel,i, buff)
            # print(ch)

            # plt.figure(3)
            # plt.plot(ch)
            if ch[delay]==0 :
                print("low")
                trig_test=np.append(trig_test,1)
            else:
                trig_test=np.append(trig_test,0)

        elif i==libm2k.HIGH_LEVEL:
            ch=get_data_to_check_trig_condition(d_trig, dig,channel,i, buff)
            # print(ch)
            # plt.figure(4)
            # plt.plot(ch)
            if ch[delay]==1:
                print("high")
                trig_test=np.append(trig_test,1)
            else:
                trig_test=np.append(trig_test,0)

        elif i==libm2k.ANY_EDGE:
            ch=get_data_to_check_trig_condition(d_trig, dig,channel,i, buff)
            # print(ch)
            # plt.figure(5)
            # plt.plot(ch)

            if ch[delay]<=ch[delay+3]:
                print("rising")
                trig_test=np.append(trig_test,1)
            elif ch[delay]>=ch[delay+3]:
                print("falling")
                trig_test=np.append(trig_test,1)
            else:
                trig_test=np.append(trig_test,0)

        #elif i==5:

    plt.show()
    return trig_test

def check_open_drain_mode(dig, channel):
    """Should verify if the digital channel selected works well in open drain mode
    
    Arguments:
        dig  -- Digital object\n
        channel  -- Digital Channel under test\n
    """
    dig.enableChannel(channel,True)
    dig.enableChannel(channel+8,True)
    dig.setSampleRateIn(1000)
    dig.setSampleRateOut(1000) # set sample rate and buffer length for a frequency smaller than 450HZ
    dig.setOutputMode(channel,libm2k.DIO_OPENDRAIN)#OD, PP
    dig.setDirection(channel,libm2k.DIO_OUTPUT)
    dig.setDirection(channel+8,libm2k.DIO_INPUT)
    ch=[]
    buff=[1,1,1,1,1,0,0,0,0,0,1]#100Hz frequency
    dig.push(buff)
    data = dig.getSamples(100)
    for val in data:
            print((val))
            if(val&(2**channel)==2**channel):
                ch=np.append(ch,1)
            else:
                ch=np.append(ch,0)
    #dig.setValueRaw(channel,libm2k.HIGH)            
    #print("raw value of the channel")
    #print(dig.getValueRaw(channel))
    # plt.figure(15)
    # plt.plot(ch)
    # plt.show()

    return

def task1(nb_samples, dig):
    """Parallel process where data is read
    
    Arguments:
        nb_samples  -- Number of samples\n
        dig  -- Digital object\n
    """
    print("Task 1 assigned to thread: {}".format(threading.current_thread().name))
    data = dig.getSamples(nb_samples)


    return data

def test_digital_cyclic_buffer(dig, d_trig, channel):
    """Test for the digital cyclic buffer set to false. 
    Arguments:
        dig  -- Digital object\n
        d_trig  -- Digital trigger\n
        channel  -- Digital channel under test\n
    """
    dig.setDirection(channel,libm2k.DIO_INPUT)
    dig.enableChannel(channel,True)
    dig.setDirection(channel+8,libm2k.DIO_OUTPUT)
    dig.enableChannel(channel+8,True)
    d_trig.setDigitalCondition(channel,libm2k.LOW_LEVEL)
    dig.setCyclic(True)
    n=50
    buff=[512,512,0,0,0,0,512,512]
    ch=[]
    pool = ThreadPool(processes=1)
    print("Main thread name: {}".format(threading.main_thread().name))

    async_result = pool.apply_async(task1, args=[n, dig])
    dig.push(buff)
    return_val = async_result.get()
    for val in return_val:
            #print((val))
        if(val&(2**channel)==2**channel):
            ch=np.append(ch,1)
        else:
            ch=np.append(ch,0)
    plt.plot(ch)
    plt.show()
    return ch
