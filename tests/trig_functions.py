import libm2k
import reset_def_values as reset
from analog_functions import set_trig, save_data_to_csv
import numpy as np
from pandas import DataFrame
from open_context_and_files import ain, aout,trig, ctx, results_dir, results_file, csv_path
import logging

def trigger_jitter(buffers, trig, channel, signal, trig_cond, ain, aout, file, csv_path):
    file.write("\n\nTrigger jitter test for channel "+str(channel)+"\n")
    max_dac_samplerate=75000000
    aout.setSampleRate(channel, max_dac_samplerate)
    aout.enableChannel(channel,True)
    ain.enableChannel(channel, True)
    delay=20
    level=0
    trigs_counted=[[],[],[]]
    adc_sr=[1000000,10000000, 100000000]
    set_trig(trig, channel, delay, cond=trig_cond, level=0)
    nr_samples, sine_signals, square_signals =create_test_signals()
    if signal=='square':
        test_signal=square_signals

    elif signal=='sine':
        test_signal=sine_signals

    for  sr in adc_sr:
        ain.stopAcquisition()
        ain.setSampleRate(sr)
        file.write('ADC sample rate:' +str(sr)+'; Test signal:'+signal+"; Trigger condition:" +str(trig_cond) +'\n')
        for l in range(len(test_signal)):
            file.write("Frequency: "+str(max_dac_samplerate/len(test_signal[l])) + '\n')
            counter=count_trigger_events(channel,buffers, delay, level, trig_cond, ain, test_signal[l])
            trigs_counted[l].append(counter)
            file.write(str(counter)+ " Trigger events in "+str(buffers)+ " buffers \n")
    return trigs_counted, adc_sr, nr_samples, max_dac_samplerate

def count_trigger_events(channel,buffers,delay, level, trig_cond, ain, test_signal):
    """Counts how many trigger events happened in x buffers. The counter of trigger events should result equal with the number of buffers.
    
    Arguments:
        channel  -- Channel under test\n
        buffers  -- Number of buffers\n
        delay  -- Trigger delay (position in the buffer)\n
        level  -- Trigger level\n
        trig_cond  -- Trigger condition\n
        ain  -- AnalogIn object\n
        test_signal  -- Test signal \n
    
    Returns:
        counter -- Nr of trigger events in buffers\n
    """    
    counter=0
    aout.push(channel, test_signal)
    for i in range(buffers):
        try:
            input_data=ain.getSamples(delay*2)[channel]
        except:
            print("Timeout ocurred")    
        if(trig_cond==libm2k.RISING_EDGE_ANALOG):
            if input_data[delay]<level and input_data[delay+1]>=level:
                counter=counter+1
        elif(trig_cond==libm2k.FALLING_EDGE_ANALOG):
            if input_data[delay]>=level and input_data[delay+1]<level:
                counter=counter+1
    return counter

def create_test_signals():
    """Create a set of sine and square signals to test the trigger jitter. Signals are created based on nr_samples
    
    Returns:
        nr_samples- number of samples in the output buffer, determines signal frequency\n
        sine_signals-set of sinusoidal signals to be used for the test\n
        square_signals-set of square seignals to be used for the test\n
    """    
    nr_samples=[150, 1500, 15000]
    sine_signals=[]
    square_signals=[]
    for i in range(len(nr_samples)):
        sine_signals.append(np.sin(np.linspace(-np.pi, np.pi, nr_samples[i])))
        square_signals.append(np.append(np.linspace(-1,-1,int(nr_samples[i]/2)),np.linspace(1,1,int(nr_samples[i]/2))))
    return nr_samples, sine_signals, square_signals
