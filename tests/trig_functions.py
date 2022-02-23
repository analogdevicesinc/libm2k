import libm2k
import reset_def_values as reset
import analog_functions
import numpy as np
from pandas import DataFrame
from open_context import ain, aout, trig, ctx

gen_reports = True


def trigger_jitter(buffers, trig, channel, signal, trig_cond, ain, aout):

    if gen_reports:
        from create_files import results_file, open_files_and_dirs
        if results_file is None:
            file, dir, csv = open_files_and_dirs()
        else:
            file = results_file
    else:
        file = []

    max_dac_samplerate = 75000000
    aout.setSampleRate(channel, max_dac_samplerate)
    aout.enableChannel(channel, True)
    ain.enableChannel(channel, True)
    delay = 20
    level = 0
    trigs_counted = [[], [], []]
    adc_sr = [1000000, 10000000, 100000000]
    analog_functions.set_trig(trig, channel, delay, cond=trig_cond, level=0)
    nr_samples, sine_signals, square_signals = create_test_signals()
    data_string = []
    if signal == 'square':
        test_signal = square_signals

    elif signal == 'sine':
        test_signal = sine_signals

    for sr in adc_sr:
        ain.stopAcquisition()
        ain.setSampleRate(sr)
        data_string.append('ADC sample rate:' + str(sr) + '; Test signal:' + signal + "; Trigger condition:" + str(
                    trig_cond))
        for l in range(len(test_signal)):
            counter = count_trigger_events(channel, buffers, delay, level, trig_cond, ain, test_signal[l])
            trigs_counted[l].append(counter)
            data_string.append("Frequency: " + str(max_dac_samplerate / len(test_signal[l])))
            data_string.append(str(counter) + " Trigger events in " + str(buffers) + " buffers")

    if gen_reports:
        write_file(file, channel, data_string)

    return trigs_counted, adc_sr, nr_samples, max_dac_samplerate


def count_trigger_events(channel, buffers, delay, level, trig_cond, ain, test_signal):
    # Counts how many trigger events happened in x buffers. The counter of trigger events should result equal with
    # the number of buffers.
    # Arguments:
    #   channel  -- Channel under test
    #   buffers  -- Number of buffers
    #   delay  -- Trigger delay (position in the buffer)
    #   level  -- Trigger level
    #   trig_cond  -- Trigger condition
    #   ain  -- AnalogIn object
    #   test_signal  -- Test signal
    # Returns: counter -- Nr of trigger events in buffers
    counter = 0
    aout.push(channel, test_signal)
    ctx.setTimeout(5000)
    for i in range(buffers):
        try:
            input_data = ain.getSamples(delay * 2)[channel]
        except:
            print("Timeout ocurred")
        if trig_cond == libm2k.RISING_EDGE_ANALOG:
            if input_data[delay] < level <= input_data[delay + 1]:
                counter = counter + 1
        elif trig_cond == libm2k.FALLING_EDGE_ANALOG:
            if input_data[delay] >= level > input_data[delay + 1]:
                counter = counter + 1
    ain.stopAcquisition()
    return counter


def create_test_signals():
    # Create a set of sine and square signals to test the trigger jitter. Signals are created based on nr_samples
    # Returns:
    #    nr_samples- number of samples in the output buffer, determines signal frequency
    #    sine_signals- set of sinusoidal signals to be used for the test
    #    square_signals- set of square signals to be used for the test

    nr_samples = [150, 1500, 15000]
    sine_signals = []
    square_signals = []
    for i in range(len(nr_samples)):
        sine_signals.append(np.sin(np.linspace(-np.pi, np.pi, nr_samples[i])))
        square_signals.append(
            np.append(np.linspace(-1, -1, int(nr_samples[i] / 2)), np.linspace(1, 1, int(nr_samples[i] / 2))))
    return nr_samples, sine_signals, square_signals


def write_file(file, channel, data_string):
    file.write("\n\nTrigger jitter test for channel " + str(channel) + "\n")
    for i in range(len(data_string)):
        file.write(str(data_string[i])+'\n')

