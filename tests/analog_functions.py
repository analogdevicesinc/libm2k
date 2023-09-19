from scipy.stats import pearsonr
from scipy.signal import find_peaks
import numpy as np
import math
import matplotlib.pyplot as plt
import libm2k
import time
from multiprocessing.pool import ThreadPool
import threading
import os
from pandas import DataFrame
import pandas
import random
import sys
import reset_def_values as reset
from open_context import ctx_timeout, ctx
from create_files import results_file, results_dir, csv

# dicts that will be saved to csv files
shape_csv_vals = {}
ampl_csv_vals = {}
phasediff_csv_vals = {}
offset_csv_vals = {}
trig_csv_vals = {}
cyclic_csv_vals = {}
freq_csv_vals = {}
osr_csv_vals = {}
timeout_csv_vals = {}
gen_reports = True


def set_trig_for_signalshape_test(i, channel, trig, delay):
    # Set the trigger for verifying the signal shapes
    # Arguments:
    #    i  -- Number of the iteration, corresponds to the shape from:
    #    ['Sine','Square',  'Triangle','Rising_ramp','Falling_ramp']
    #    channel  -- Analog channel under test
    #    trig  -- Trigger object
    #    delay  -- Trigger delay

    if i == 0:
        set_trig(trig, channel, delay, libm2k.FALLING_EDGE_ANALOG, 0)
    elif i == 1:
        set_trig(trig, channel, delay, libm2k.FALLING_EDGE_ANALOG, 0)
    elif i == 2:
        set_trig(trig, channel, delay, libm2k.RISING_EDGE_ANALOG, -0.98)
    elif i == 3:
        set_trig(trig, channel, 1, libm2k.RISING_EDGE_ANALOG, -0.98)
    elif i == 4:
        set_trig(trig, channel, 1, libm2k.FALLING_EDGE_ANALOG, 0.98)

    return


def set_samplerates_for_shapetest(ain, aout):
    # Set the sample rates of DAC and ADC, and compute the number of samples in the input buffer
    # Arguments:
    #    ain -- AnalogIn object
    #    aout --AnalogOut object
    # Returns:
    #    out0_buffer_samples -- nr of samples in the output buffer on ch0
    #    out1_buffer_samples -- nr of samples in the output buffer on ch1
    #    ch0_sample_ratio -- ratio between DAC and ADC samplerates on ch0
    #    ch1_sample_ratio -- ratio between DAC and ADC samplerates on ch1
    #    in0_buffer_samples -- nr of samples in the input buffer on ch0
    #    in1_buffer_samples -- nr of samples in the input buffer on ch0

    adc_sample_rate = 1000000
    dac_a_sample_rate = 7500000
    dac_b_sample_rate = 7500000
    test_frequency = 1831
    out0_buffer_samples = int(dac_a_sample_rate / test_frequency)
    out1_buffer_samples = int(dac_b_sample_rate / test_frequency)

    ain.setSampleRate(adc_sample_rate)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_1, dac_a_sample_rate)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_2, dac_b_sample_rate)
    ch0_sample_ratio = dac_a_sample_rate / adc_sample_rate
    ch1_sample_ratio = dac_b_sample_rate / adc_sample_rate
    in0_buffer_samples = int(adc_sample_rate / test_frequency)
    in1_buffer_samples = int(adc_sample_rate / test_frequency)
    return out0_buffer_samples, out1_buffer_samples, ch0_sample_ratio, ch1_sample_ratio, in0_buffer_samples, in1_buffer_samples


def set_trig(trig, channel, delay, cond=None, level=None):
    # Set analog trigger as needed for specific operations
    # Arguments:
    #    trig  -- Trigger object
    #    channel  -- Analog channel under test
    #    delay  -- Delay value for the trigger
    # Keyword Arguments:
    #    cond  -- Trigger condition (default: {None})
    #    level  -- Trigger level value  (default: {None})

    trig.setAnalogMode(channel, libm2k.ANALOG)  # set analog trigger
    trig.setAnalogHysteresis(channel, 0)
    trig.setAnalogSource(channel)
    trig.setAnalogSourceChannel(channel)
    trig.setAnalogDelay(-delay)
    if cond is not None:
        trig.setAnalogCondition(channel, cond)
    if level is not None:
        trig.setAnalogLevel(channel, level)
    return


def test_calibration(ctx):
    # ADC
    adc_calib = ctx.calibrateADC()
    # DAC
    dac_calib = ctx.calibrateDAC()

    return adc_calib, dac_calib


def test_amplitude(out_data, ref_data, n, ain, aout, channel, trig):
    # Sends signals with different amplitudes and verify if the received data is as expected.
    # The amplitude multiplier is defined locally.
    # For each value of the amplitude multiplier is computed the maximum and the minimum value of the input signal.
    # Arguments:
    #    out_data -- Output data buffer
    #    ref_data -- Reference data buffer that will be compared with the input data
    #    n  -- Number of samples in the input buffer
    #    ain  -- AnalogIn object
    #    aout -- AnalogOut object
    #    channel -- The analog input channel currently under test
    #    trig -- Trigger object
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    corr_amplitude_max -- correlation coefficient between the vector that holds the maximum amplitude values in the
    #    input signal and the vector that holds the maximum amplitude values in the reference signal
    #    corr_amplitude_min -- correlation coefficient between the vector that holds the minimum amplitude values in the
    #    input signal and the vector that holds the maximum amplitude values in the reference signal

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "amplitude"
    data_string = []
    set_trig(trig, channel, 0, libm2k.FALLING_EDGE_ANALOG, 0)
    ain.setRange(channel, libm2k.PLUS_MINUS_25V)
    amplitude = np.arange(0.5, 5.5, 0.5)  # amplitude multiplier
    # arrays for maximum amplitude values of read data and reference data
    max_in = np.array([])
    max_ref = np.array([])

    # arrays for minimum amplitude values of read data and reference data
    min_in = np.array([])
    min_ref = np.array([])
    data_string.append("Amplitude values : \n")
    data_string.append(str(amplitude))
    for i in amplitude:
        aout.push(channel, (out_data * i))  # push the buffer
        # get samples for amplitude multiplied with i
        try:
            input_data = ain.getSamples(n)
        except:
            print('Timeout occured')
        ain.stopAcquisition()

        # generate the buffer that should be received and is compared to the actual input data
        ref_data_ampl = i * ref_data

        # gen max values from each data buffer
        max_in = np.append(max_in, np.amax(input_data[channel]))
        max_ref = np.append(max_ref, np.amax(ref_data_ampl))

        # gen min values from each data buffer
        min_in = np.append(min_in, np.amin(input_data[channel]))
        min_ref = np.append(min_ref, np.amin(ref_data_ampl))
        if gen_reports:
            if channel == 0:
                plot_to_file('Signal amplitudes channel 0', input_data[channel], dir_name, 'amplitudes_ch0.png')
                ampl_csv_vals['Amplitude ' + str(i) + ' ch0'] = input_data[channel]
                save_data_to_csv(ampl_csv_vals, csv_path + 'amplitude.csv')

            else:
                plot_to_file('Signal amplitudes channel 1', input_data[channel], dir_name, 'amplitudes_ch1.png')
                ampl_csv_vals['Amplitude ' + str(i) + ' ch1'] = input_data[channel]
                save_data_to_csv(ampl_csv_vals, csv_path + 'amplitude.csv')

    # compute correlation of max values
    corr_amplitude_max, _ = pearsonr(max_in, max_ref)
    corr_amplitude_min, _ = pearsonr(min_in, min_ref)
    data_string.append("Maximum amplitude reference:")
    data_string.append(str(max_ref))
    data_string.append("Maximum amplitude computed:")
    data_string.append(str(max_in))
    data_string.append("Minimum amplitude reference:")
    data_string.append(str(min_ref))
    data_string.append("Minimum amplitude computed:")
    data_string.append(str(min_in))
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    aout.stop(channel)
    return corr_amplitude_max, corr_amplitude_min


def test_shape(channel, out_data, ref_data, ain, aout, trig, ch_ratio, shapename):
    # Sends signal buffers created in shapefile.py  on analog channels. Reads the buffer and compares each result with
    # reference buffers.
    # Waveforms: ['Sine','Square',  'Triangle','Rising_ramp','Falling_ramp']
    # Arguments:
    #    channel  -- Analog channel under test
    #    out_data  -- Output data buffer
    #    ref_data  -- Reference data buffer
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    trig  -- Trigger object
    #    ch_ratio  -- Ratio between DAC and ADC  samplerate
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    corr_shape_vect-- vector that holds the correlation coefficients between the input data and the reference data
    #    for each signal shape
    #    phase_diff_vect-- vector that holds the phase difference between the input data and the reference data for each
    #    signal shape

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "shape"
    data_string = []
    # corr_shape_vect
    corr_shape_vect = []
    # phase difference vector for signals acquired on channel
    phase_diff_vect = []
    # set the trigger delay
    delay = 0

    for i in range(len(out_data)):
        ctx.setTimeout(5000)
        set_trig_for_signalshape_test(i, channel, trig, delay)
        n = round(len(out_data[i]) / ch_ratio)
        aout.push(channel, out_data[i])
        try:
            input_data = ain.getSamples(n)[channel]
        except:
            print('Timeout occured')

        ain.stopAcquisition()
        if gen_reports:
            if channel == 0:
                plot_to_file(str(shapename[i]) + ' signal  on channel 0', input_data, dir_name,
                             str(shapename[i]) + 'signal_ch0.png')
                shape_csv_vals[str(shapename[i]) + ' ch0'] = input_data
                save_data_to_csv(shape_csv_vals, csv_path + 'shapes.csv')
            else:
                plot_to_file(str(shapename[i]) + ' signal  on channel 1', input_data, dir_name,
                             str(shapename[i]) + 'signal_ch1.png')
                shape_csv_vals[str(shapename[i]) + ' ch1'] = input_data
                save_data_to_csv(shape_csv_vals, csv_path + 'shapes.csv')
            # compute pearson coefficient to determine that the generated and acquired signals are similar
        corr_shape, _ = pearsonr(ref_data[i], input_data)
        corr_shape_vect = np.append(corr_shape_vect, corr_shape)
        phase_diff = ((math.acos(corr_shape)) * 180) / np.pi
        phase_diff_vect = np.append(phase_diff_vect, phase_diff)
        data_string.append(
            "Correlation coefficient between " + shapename[i] + "signal and its reference:" + str(corr_shape))
        data_string.append("Phase difference between " + shapename[i] + "signal and its reference:" + str(phase_diff))
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    aout.stop(channel)

    return corr_shape_vect, phase_diff_vect


def phase_diff_ch0_ch1(aout, ain, trig):
    # Sends the same signal on both analog channel and computes the phase difference between the signal received on ch0
    # and the signal received on ch1
    # Arguments:
    #    out_data  -- Output data buffer
    #    n  --Nr of samples
    #    aout  -- AnalogOut object
    #    ain  -- AnalogIn object
    #    trig  -- Trigger object
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    phase_diff_between_channels-- the phase difference between channels in degrees
    #  
    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "phase_diff"
    data_string = []
    set_trig(trig, libm2k.ANALOG_IN_CHANNEL_1, 0, libm2k.RISING_EDGE_ANALOG, 0.0)
    dac_sr = 75000000
    adc_sr = [1000000, 10000000, 100000000]
    test_frequency = 100000
    out_samples = int(dac_sr / test_frequency)
    in_samples = 100000  # get a buffer long enough, the phase differences may appear in time
    sig = np.sin(np.linspace(-np.pi, np.pi, out_samples))  # test signal
    out_data = [sig, sig]
    aout.setSampleRate(0, dac_sr)
    aout.setSampleRate(1, dac_sr)

    aout.push(out_data)

    phasediff_csv = {}
    phase_diff_between_channels = []
    for sr in adc_sr:
        ain.setSampleRate(sr)

        try:
            input_data = ain.getSamples(in_samples)
        except:
            print("Timeout ocurred")
        ain.stopAcquisition()
        if gen_reports:
            plot_to_file('Same signal on both analog channels, ADC Sample rate:' + str(sr),
                         input_data[libm2k.ANALOG_IN_CHANNEL_1][90000:in_samples], dir_name,
                         'ch_phase_diff' + str(sr) + '.png',
                         data1=input_data[libm2k.ANALOG_IN_CHANNEL_2][90000:in_samples])
        corr, _ = pearsonr(input_data[libm2k.ANALOG_IN_CHANNEL_1], input_data[
            libm2k.ANALOG_IN_CHANNEL_2])  # compute correlation between the signals on the analog channels
        phase_diff_between_channels.append(((math.acos(corr)) * 180) / np.pi)  # compute the phase difference
        data_string.append("Phase difference between the channels for ADC Sample Rate " + str(sr) + ": " + str(
            phase_diff_between_channels[-1]))

        phasediff_csv['Ch0, ADCsr=' + str(sr)] = input_data[libm2k.ANALOG_IN_CHANNEL_1]
        phasediff_csv['Ch1, ADCsr=' + str(sr)] = input_data[libm2k.ANALOG_IN_CHANNEL_2]
    if gen_reports:
        channel = 0
        write_file(file, test_name, channel, data_string)
        save_data_to_csv(phasediff_csv, csv_path + 'ph_diff_channels.csv')
    aout.stop()

    return phase_diff_between_channels, adc_sr


def test_analog_trigger(channel, trig, aout, ain):
    # Test the analog trigger conditions
    # Arguments:
    #    channel  -- Analog channel under test
    #    trig  -- Trigger object
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    dir_name -- Directory where the plot files are saved
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    trig_test -- Vector that holds 1 for each trigger condition fulfilled and 0 otherwise
    #    condition_name-- Vector that holds names of the trigger conditions

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "analog_trigger"
    data_string = []
    low = 0.10
    th = 0.1
    high = 0.70
    level = 0.40
    delay = 0
    n = 4096 * 4
    condition = [libm2k.RISING_EDGE_ANALOG, libm2k.FALLING_EDGE_ANALOG, libm2k.LOW_LEVEL_ANALOG,
                 libm2k.HIGH_LEVEL_ANALOG]
    condition_name = ['rising edge', 'falling edge', 'low', 'high']
    trig_test = []
    set_trig(trig, channel, delay)
    # create a trapezoidal test signal
    test_signal = np.append(np.append(np.linspace(0, 0, int(n / 4)), np.linspace(0, 1, int(n / 4))),
                            np.append(np.linspace(1, 1, int(n / 4)), np.linspace(1, 0, int(n / 4))))
    aout.push(channel, test_signal)

    # go through all possible trigger conditions
    for i in condition:
        if i == libm2k.RISING_EDGE_ANALOG:
            trig.setAnalogCondition(channel, i)
            trig.setAnalogLevel(channel, level)

            try:
                input_data = ain.getSamples(round(n / 4))[channel]
            except:
                print('Timeout occured')
            ain.stopAcquisition()
            if gen_reports:
                if channel == 0:
                    plot_to_file('Trigger condition: Rising Edge channel 0, level=' + str(level), input_data, dir_name,
                                 'trig_RISING_EDGE_ANALOG_ch0.png')
                    trig_csv_vals['Rising Edge ch0'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')
                else:
                    plot_to_file('Trigger condition: Rising Edge channel1, level=' + str(level), input_data, dir_name,
                                 'trig_RISING_EDGE_ANALOG_ch1.png')
                    trig_csv_vals['Rising Edge ch1'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')

            if (level - th <= input_data[delay] <= level + th) and (
                    input_data[delay - 1] < input_data[delay + 1]):
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            data_string.append("Rising edge condition:")
            data_string.append("level set: " + str(level))
            data_string.append("level read: " + str(input_data[delay]))
        elif i == libm2k.FALLING_EDGE_ANALOG:
            trig.setAnalogCondition(channel, i)
            trig.setAnalogLevel(channel, level)

            try:
                input_data = ain.getSamples(round(n / 4))[channel]
            except:
                print('Timeout occured')
            ain.stopAcquisition()
            if gen_reports:
                if channel == 0:
                    plot_to_file('Trigger condition: Falling Edge channel 0, level=' + str(level), input_data, dir_name,
                                 'trig_falling_edge_ch0.png')
                    trig_csv_vals['Falling Edge ch0'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')
                else:
                    plot_to_file('Trigger condition: Falling Edge channel 1, level=' + str(level), input_data, dir_name,
                                 'trig_falling_edge_ch1.png')
                    trig_csv_vals['Falling Edge ch1'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')

            if (level - th <= input_data[delay] <= level + th) and (
                    input_data[delay - 1] > input_data[delay + 1]):

                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            data_string.append("Falling edge condition:")
            data_string.append("level set: " + str(level))
            data_string.append("level read: " + str(input_data[delay]))
        elif i == libm2k.LOW_LEVEL_ANALOG:
            trig.setAnalogCondition(channel, i)
            trig.setAnalogLevel(channel, low)

            try:
                input_data = ain.getSamples(round(n / 4))[channel]
            except:
                print('Timeout occured')
            ain.stopAcquisition()
            if gen_reports:
                if channel == 0:
                    plot_to_file('Trigger condition: Low Level channel 0, level=' + str(low), input_data, dir_name,
                                 'trig_LOW_LEVEL_ANALOG_ch0.png')
                    trig_csv_vals['Low level ch0'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')
                else:
                    plot_to_file('Trigger condition: Low Level channel 1, level=' + str(low), input_data, dir_name,
                                 'trig_LOW_LEVEL_ANALOG_ch1.png')
                    trig_csv_vals['Low level ch1'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')

            if input_data[delay + 1] < (low + th) and input_data[delay + 10] < (low + th):
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            data_string.append("Low level condition:")
            data_string.append("level set: " + str(low))
            data_string.append("\nlevel read: " + str(input_data[delay]))
        elif i == libm2k.HIGH_LEVEL_ANALOG:
            trig.setAnalogCondition(channel, i)
            trig.setAnalogLevel(channel, high)

            try:
                input_data = ain.getSamples(round(n / 4))[channel]
            except:
                print('Timeout occured')
            ain.stopAcquisition()
            if gen_reports:
                if channel == 0:
                    plot_to_file('Trigger condition: High Level channel 0, level=' + str(high), input_data, dir_name,
                                 'trig_HIGH_LEVEL_ANALOG_ch0.png')
                    trig_csv_vals['High level ch0'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')
                else:
                    plot_to_file('Trigger condition: High Level channel 1, level=' + str(high), input_data, dir_name,
                                 'trig_HIGH_LEVEL_ANALOG_ch1.png')
                    trig_csv_vals['High level ch1'] = input_data
                    save_data_to_csv(trig_csv_vals, csv_path + 'trigger.csv')

            if input_data[delay + 1] > (high - th) and input_data[delay + 10] > (high - th):
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            data_string.append("High level condition:")
            data_string.append("level set: " + str(high))
            data_string.append("\nlevel read: " + str(input_data[delay]))
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    aout.stop(channel)
    return trig_test, condition_name


def test_offset(out_data, n, ain, aout, trig, channel):
    # Sets different offsets to a signal and compares the offest of the received signal with the reference offset values
    # defined locally
    # Arguments:
    #    out_data  -- Output data buffer
    #    n  --Nr of samples
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    trig -- Trigger object
    #    channel -- Analog channel under test
    #    dir_name -- Directory where the plot files are saved
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns
    #    corr_offset -- Correlation coefficient between the computed offset vector and the defined offset vector

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "offset"
    data_string = []
    ain.setRange(channel, libm2k.PLUS_MINUS_25V)
    offset = np.arange(-4, 4.5, 0.5)  # offset values between -5 and 5 with a step of 0.5V
    in_offset = np.array([])

    data_string.append("Offset values set:")
    data_string.append(str(offset))
    data_string.append("Offset values computed:")
    for i in offset:
        ctx.setTimeout(5000)
        set_trig(trig, channel, 0, libm2k.FALLING_EDGE_ANALOG, i + 0.1)
        sum = 0
        o_buffer = i + out_data  # add the output data to the offset value
        aout.push(channel, o_buffer)  # push the buffer
        # get samples for amplitude multiplied with i
        try:
            input_data = ain.getSamples(n)
        except:
            print('Timeout occured')
        ain.stopAcquisition()

        for s in input_data[channel]:
            sum = sum + s
        average = round(sum / n, 1)  # compute the average value of a period of the acquired signal
        in_offset = np.append(in_offset, average)  # put all the average values in a vector
        if gen_reports:
            if channel == 0:
                plot_to_file('Signal offset  on channel 0', input_data[channel], dir_name, 'offsets_ch0.png')
                offset_csv_vals['Offset' + str(i) + 'ch0'] = input_data
                save_data_to_csv(trig_csv_vals, csv_path + 'offset.csv')

            else:
                plot_to_file('Signal offset  on channel 1', input_data[channel], dir_name, 'offsets_ch1.png')

    data_string.append(str(in_offset))
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    corr_offset, _ = pearsonr(offset, in_offset)  # compare the original offset vector with the average values obtained

    aout.stop(channel)

    return corr_offset


def test_voltmeter_functionality(channel, ain, aout, ctx):
    # Tests voltmeter functionality of analog input channels
    # Arguments:
    #    channel -- Analog channel under test
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    ctx-- M2k context opened
    #    file -- Text file where are saved reference and computed values during the test
    # Returns:
    #    voltmeter_ -- Vector thet holds 1 if the read voltage is in specified range and 0 otherwise

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)

    test_name = "voltmeter"
    data_string = []

    voltage = [random.uniform(0.2, 0.3), random.uniform(0.55, 0.75), random.uniform(1.2, 1.5), random.uniform(2.0, 2.2),
               random.uniform(2.8, 3.1), random.uniform(3.5, 3.9), random.uniform(4.1, 4.5),
               random.uniform(4.6, 5)]  # random voltage values
    t = 0.15  # threshold
    voltmeter_ = []  # voltmeter array
    nb_samples = 4096 * 2  # number of samples to be sent

    # ctx.calibrateDAC()
    aout.enableChannel(channel, True)
    for v_sent in voltage:
        if v_sent < 2.5:
            ain.setRange(channel, libm2k.PLUS_MINUS_2_5V)
        else:
            ain.setRange(channel, libm2k.PLUS_MINUS_25V)

        aout.push(channel, np.linspace(v_sent, v_sent, nb_samples))  # send the voltage value on the output channels
        time.sleep(0.2)  # wait for the DAC output to settle
        Vin = ain.getVoltage()[channel]  # read the voltage
        data_string.append("Voltage sent: " + str(v_sent))
        data_string.append("Voltage read: " + str(Vin))

        if (v_sent - t) < Vin < (v_sent + t):
            voltmeter_ = np.append(voltmeter_, 1)  # the  voltage is in the correct range
        else:
            voltmeter_ = np.append(voltmeter_, 0)  # the voltage is out of range
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    aout.stop(channel)

    return voltmeter_


def set_trig_for_cyclicbuffer_test(trig, delay, level=0.0):
    #  Sets the trigger for checking the cyclic buffer set to False
    # Arguments:
    #    trig--Trigger object

    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ANALOG)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_2, 1)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_1, libm2k.LOW_LEVEL_ANALOG)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_2, libm2k.LOW_LEVEL_ANALOG)
    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_1, level)
    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_2, level)
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_1, 0)  # value for hysteresis is set in raw
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_2, 0)  # 63 raw corresponds to 100mV
    trig.setAnalogDelay(-delay)
    return


def cyclic_buffer_test(aout, ain, channel, trig):
    # Test if multiple buffers of data are sent and received when the cyclic buffer is set to true.
    # Arguments:
    #    aout  -- AnalogOut object
    #    ain  -- AnalogIn object
    #    channel -- Analog channel under test
    #    trig -- Trigger object
    # Returns:
    #    cyclic_false -- Must be 1 if a single buffer was sent and succesfully recieved, 0 otherwise

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []
    dac_sr = 75000
    adc_sr = 10000
    min_periods = 3
    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)
    aout.setCyclic(True)
    ain.setSampleRate(adc_sr)
    aout.setSampleRate(channel, dac_sr)
    set_trig(trig, channel, 0, libm2k.FALLING_EDGE_ANALOG, 0.0)

    out_samples = 4096
    out_data = np.sin(np.linspace(-np.pi, np.pi, out_samples))

    pool = ThreadPool(processes=1)  # start a paralel process

    async_result = pool.apply_async(get_samples_notcyclic, args=[out_samples, ain, channel])
    aout.push(channel, out_data)
    return_val = async_result.get()  # get the data acquired in the parallel thread
    peak_indices, peak_properties = find_peaks(return_val, height=0.97, width=5)

    if len(peak_indices) >= min_periods:
        cyclic_test = 1
    else:
        cyclic_test = 0

    return cyclic_test


def noncyclic_buffer_test(aout, ain, channel, trig, ctx):
    # Test if a single buffer of data is sent and received when the cyclic buffer is set to false.
    # This function sets the trigger as needed for this operation then starts a parralel thread  where it waits for
    # samples. The  buffer of samples returned by get_samples_not_cyclic() is compared with a reference buffer
    # corresponding to the single buffer sent. After the length of a single buffer the signal must be 0.
    # Arguments:
    #    aout  -- AnalogOut object
    #    ain  -- AnalogIn object
    #    channel -- Analog channel under test
    #    trig -- Trigger object
    #    dir_name -- Directory where the plot files are saved
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    cyclic_false -- Must be 1 if a single buffer was sent and succesfully recieved, 0 otherwise

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    # test the cyclic buffer condition
    ctx.setTimeout(10000000)  # set large value of timeout so single buffer is captured
    aout.setCyclic(False)
    ain.setSampleRate(1000000)
    aout.setSampleRate(channel, 75000000)
    trig.setAnalogSource(channel)
    trig.setAnalogSourceChannel(channel)
    delay = 0
    set_trig_for_cyclicbuffer_test(trig, delay, level=-0.25)
    out_samples = 4096
    out_data = np.sin(np.linspace(-np.pi, np.pi, out_samples))

    ain.startAcquisition(out_samples * 4)

    aout.push(channel, out_data)

    return_val = ain.getSamples(out_samples * 4)[channel]
    ain.stopAcquisition()
    ch_ratio = aout.getSampleRate(
        channel) / ain.getSampleRate()  # ratio between the samplerate on the output channel and the input channel
    single_buffer_length = round(out_samples / ch_ratio)
    ref_data = (np.sin(np.linspace(-np.pi, np.pi,
                                   single_buffer_length)))  # generate a reference signal to compare to read signal
    corr, _ = pearsonr(ref_data, return_val[
                                 delay:single_buffer_length + delay])  # correlation btw. reference and the input data
    if corr > 0.9:
        for i in range(out_samples):
            if i > out_samples / ch_ratio + delay:
                if -0.01 < round(return_val[i]) < 0.01:
                    cyclic_false = 1
                else:
                    cyclic_false = 0
    else:
        cyclic_false = 0
    if gen_reports:
        if channel == 0:
            plot_to_file('Cyclic buffer set to False, channel 0', return_val, dir_name, 'cyclic_buffer_plot_ch0.png')
            plot_to_file('Cyclic buffer set to False, channel 0', return_val[
                                                                  delay:single_buffer_length + delay], dir_name,
                         'cyclic_buffer_plot_ch0_2.png')
            cyclic_csv_vals['Channel 0'] = return_val
            save_data_to_csv(cyclic_csv_vals, csv_path + 'cyclic_buffer.csv')
        else:
            plot_to_file('Cyclic buffer set to False, channel 1', return_val, dir_name, 'cyclic_buffer_plot_ch1.png')
            cyclic_csv_vals['Channel 1'] = return_val
            save_data_to_csv(cyclic_csv_vals, csv_path + 'cyclic_buffer.csv')
    aout.stop(channel)
    aout.setCyclic(True)
    ctx.setTimeout(ctx_timeout)  # set the timeout to the initial value

    return cyclic_false


def get_samples_notcyclic(n_samples, ain, channel):
    # Get samples in the parallel thread of the cyclic buffer test
    # Arguments:
    #    n_samples -- Number of samples
    #    ain -- AnalogIn object
    #    channel -- Analog channel under test
    # Returns:
    #    data -- Data read on analog channel under test after the trigger condition was fulfilled

    # get samples in the paralel thread
    try:
        data = ain.getSamples(n_samples)[channel]
    except:
        print('Timeout occured')
    ain.stopAcquisition()
    return data


def compute_frequency(channel, ain, aout, trig):
    # Loops trough available ADC and DAC samplerates and set the number of samples to be sent according to
    # these samplerates. Computes  the frequency of the signal corresponding to the output buffer.
    # Reads the buffer from the specified channel and computes the frequency of the singnal corresponding to this buffer
    # Arguments:
    #    channel -- Analog channel under test
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    trig -- Trigger object
    #    file -- Text file where are saved reference and computed values during the test
    #    csv_path -- Path to the csv file where are saved the samples
    # Returns:
    #    ofreqs-- Vector that holds the frequencies of the output buffers
    #    ifreqs-- Vector that holds the frequencies of the input buffers

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    test_name = "freq"
    data_string = []

    set_trig(trig, channel, 0, libm2k.FALLING_EDGE_ANALOG)
    # available adc samplerates
    adc_samplerates = [10000, 100000, 1000000, 10000000, 100000000]
    # available dac sampelrates
    dac_a_samplerates = [750, 7500, 75000, 750000, 7500000, 75000000]
    dac_b_samplerates = [750, 7500, 75000, 750000, 7500000, 75000000]
    ofreqs = []
    ifreqs = []

    if channel == 0:
        dac_samplerates = dac_a_samplerates
    else:
        dac_samplerates = dac_b_samplerates
    # loop through sample rates combinations
    for adc_sr in adc_samplerates:
        for i in range(len(dac_samplerates)):
            ain.setSampleRate(adc_sr)  # set sample rate for ain channels
            aout.setSampleRate(channel, dac_samplerates[i])  # set sample rate for DAC
            ch_sampleratio = dac_samplerates[i] / adc_sr  # compute ratio between adc and dac sampelrates

            if 7.5 <= ch_sampleratio <= 750:  # make sure  the ratio has an acceptable value
                data_string.append("ADC samplerate:" + str(adc_sr))
                data_string.append("DAC samplerate:" + str(dac_samplerates[i]))

                if ch_sampleratio == 7.5:  # set the number of output samples to be sent
                    out_nr_samples = 2 ** 12  # according to the ratio between dac and adc samplerates
                elif ch_sampleratio == 75:
                    out_nr_samples = 2 ** 13
                else:
                    out_nr_samples = 2 ** 15
                in_nr_samples = round(out_nr_samples / ch_sampleratio)  # compute the number of input samples
                out_data = np.sin(np.linspace(-np.pi, np.pi, out_nr_samples))  # generate test output signal
                ref_data = np.sin(np.linspace(-np.pi, np.pi,
                                              in_nr_samples))  # generate a reference signal  to be compared with the
                # acquired input signal
                out_freq = (dac_samplerates[i] / out_nr_samples)  # compute the frequency of the signal sent bu the dac
                ofreqs = np.append(ofreqs, out_freq)
                aout.push(channel, out_data)
                try:
                    input_data = ain.getSamples(round(in_nr_samples))[channel]
                except:
                    print('Timeout occured')
                ain.stopAcquisition()
                corr, _ = pearsonr(ref_data, input_data)  # compare the acquired signal with the expected signal

                in_freq = adc_sr / len(input_data)

                if corr > 0.8:  # a correlation coeff>0.7 => strong correlation
                    in_freq = (adc_sr / len(input_data))  # if the signal is ok, compute its frequency
                    ifreqs = np.append(ifreqs, in_freq)
                data_string.append("Number of samples in the output buffer: " + str(out_nr_samples))
                data_string.append("Number of samples in the input buffer:" + str(in_nr_samples))
                data_string.append("Out signal frequency:" + str(out_freq))
                data_string.append("In singal frequency:" + str(in_freq))
    if gen_reports:
        write_file(file, test_name, channel, data_string)

    return ofreqs, ifreqs


def compare_in_out_frequency(channel, ain, aout, trig):
    # Compares the frequency of the signal in the output buffer with the frequency of the signal in the input buffer.
    # Frequencies are computed in compute_frequency(channel, ain, aout, trig, file)
    # Arguments:
    #   channel -- Analog channel under test
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    trig -- Trigger object
    #    file -- Text file where are saved reference and computed values during the test\n
    # Returns:
    #    freq_test--Vector that holds 1 if the corresponding in and out frequencies are equal and 0 otherwise

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    freq_test = []  # create an array that will hold 1 if the frequencies are equal and 0 otherwise
    out_freq, in_freq = compute_frequency(channel, ain, aout, trig)  # compute input and output frequencies
    if len(out_freq) == len(in_freq):  # make sure that for each out freq there is a corresponding input freq
        for i in range(len(out_freq)):
            if in_freq[i] < 100:  # according to the frequency value, check their equality
                if round(out_freq[i]) == round(in_freq[i]):
                    freq_test = np.append(freq_test, 1)
                else:
                    freq_test = np.append(freq_test, 0)
            elif in_freq[i] < 1000:
                if round(out_freq[i] / 10) == round(in_freq[i] / 10):
                    freq_test = np.append(freq_test, 1)
                else:
                    freq_test = np.append(freq_test, 0)
            elif in_freq[i] < 10000:
                if round(out_freq[i] / 100) == round(in_freq[i] / 100):
                    freq_test = np.append(freq_test, 1)
                else:
                    freq_test = np.append(freq_test, 0)
            elif in_freq[i] < 100000:
                if round(out_freq[i] / 1000) == round(in_freq[i] / 1000):
                    freq_test = np.append(freq_test, 1)
                else:
                    freq_test = np.append(freq_test, 0)
    aout.stop(channel)
    return freq_test


def test_oversampling_ratio(channel, ain, aout, trig):
    # Sets different values for Ain oversampling ratios, sends a buffer at the output.
    # From the corresponding input buffer, computes the oversampling ratio and compares it with the value previously set
    # Arguments:
    #        channel -- Analog channel under test\n
    #        ain  -- AnalogIn object \n
    #        aout  -- AnalogOut object\n
    #        trig -- Trigger object \n
    #        file-- Text file where are saved reference and computed values during the test\n
    #        csv_path -- Path to the csv file where are saved the samples\n
    # Returns:
    #        test_osr -- Must be 1 if the computed oversampling ratio is equal with the set oversampling ratio
    #        and 0 otherwise

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    test_name = "osr"
    data_string = []

    set_trig(trig, channel, 0, libm2k.FALLING_EDGE_ANALOG, 0.0)
    osr = [1, random.randint(3, 7), random.randint(7, 11), random.randint(11, 15),
           random.randint(16, 20), random.randint(20, 23)]  # random.randint(1, 3)some values for oversampling ratio
    verify_osr = []
    adc_sr = 100000000
    dac_sr = 75000000
    out_nr_samples = 512  # a smaller number of sample so the zero rising edge crossing  is easier to detect

    ain.setSampleRate(adc_sr)
    aout.setSampleRate(channel, dac_sr)
    ch_sampleratio = dac_sr / adc_sr  # ratio between dac sample rate and adc sample rate
    in_nr_samples = round(out_nr_samples / ch_sampleratio)  # round(out_nr_samples +(out_nr_samples%4))
    out_data = np.sin(np.linspace(-np.pi, np.pi, out_nr_samples))
    data_string.append("Oversampling ratios set:\n" + str(osr))
    for j in osr:
        ain.setOversamplingRatio(j)
        aout.push(channel, out_data)
        try:
            input_data = ain.getSamples(in_nr_samples)[channel]
        except:
            print("Timeout ocurred")
        ain.stopAcquisition()

        c = 0  # set the counter of rising edge zero crossings to 0
        for i in range(10, len(input_data) - 10):
            if round(input_data[i - 1], 3) <= 0 < round(input_data[i],
                                                        3):  # test if there is a zero crossing on the rising edge (
                # middle of a sine period)
                c = c + 1  # count how many periods of a sine wave are acquired at the input
        verify_osr = np.append(verify_osr,
                               c)  # append the counted crossings for each oversampling ratio in the verify array
        osr_csv_vals['Oversamplingratio:' + str(j) + ' ch' + str(channel)] = input_data
        save_data_to_csv(osr_csv_vals, csv_path + 'ain_oversampling_ratio.csv')

        if gen_reports:
            save_data_to_csv(osr_csv_vals, csv_path + 'ain_oversampling_ratio.csv')

    data_string.append("Oversampling ratios computed: \n" + str(verify_osr))
    if gen_reports:
        write_file(file, test_name, channel, data_string)
    test_osr = 1
    for i in range(len(osr)):
        if osr[i] != verify_osr[i]:
            test_osr = 0
    aout.stop(channel)
    return test_osr


def plot_to_file(title, data, dir_name, filename, xlabel=None, ylabel=None, data1=None, data_marked=None):
    # Saves the plots in a separate folder
    # Arguments:
    #    title  -- Title of the plot
    #    data  -- Data to be plotted
    #    filename  -- Name of the file with the plot
    # Keyword Arguments:
    #    xlabel  -- Label of x-Axis (default: {None})
    #    ylabel  -- Label of y-Axis(default: {None})
    #    data1  --  Data that should be plotted on the same plot(default: {None})
    #    data_marked -- Data that represents specific points on the plot(default: {None})
    # plot the signals in a separate folder
    plt.title(title)
    if xlabel is not None:  # if xlabel and ylabel are not specified there will be default values
        plt.xlabel(xlabel)
    else:
        plt.xlabel('Samples')
    if ylabel is not None:
        plt.ylabel(ylabel)
    else:
        plt.ylabel('Voltage [V]')
    plt.grid(visible=True)
    plt.plot(data)  # if a second set of data must be printed (for ch0 and ch1 phase difference in this case)
    if data1 is not None:
        plt.plot(data1)
    if data_marked is not None:
        plt.plot(data_marked, data[data_marked], 'xr')        
    plt.savefig(dir_name + "/" + filename)
    plt.close()
    return


def save_data_to_csv(csv_vals, csv_file):
    df = DataFrame(csv_vals)
    df.to_csv(csv_file)
    return


def channels_diff_in_samples(trig, channel, aout, ain):
    # Find if there is a sample delay between channels for the same signal and trigger
    # Arguments:
    #    trig  -- Trigger object
    #    channel  -- Channel source for the trigger
    #    aout  -- Analog Out object
    #    ain  -- Analog In object
    #    file  -- File where are printed the results
    #    csv_path  -- Path for csv file with results
    # Returns:
    #    diff_adc_sr-- 2-D array, holds the difference in samples for each ADC sample rate
    #    and Dac oversampling ratio combination
    #    adc_sr-- sample rate set for Ain
    #    dac_osr-- oversampling ratios set for Aout
    #    freq -- frequency of the signals used for the test

    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []
    reset.analog_in(ain)
    reset.analog_out(aout)  # /len(samples_diff1[i])
    reset.trigger(trig)

    data_string = []
    test_name = "phase_dif_samples"

    n = 150  # nr of samples, for max samplerate=> 1Mhz frequency for 75 samples in a buffer
    delay = 10
    max_dac_samplerate = 75000000
    aout.setSampleRate(0, max_dac_samplerate)
    aout.setSampleRate(1, max_dac_samplerate)
    adc_sr = [1000000, 10000000, 100000000]
    dac_osr = [1, 10, 100, 1000, 10000]
    set_trig(trig, channel, delay, cond=libm2k.RISING_EDGE_ANALOG, level=0)
    test_signal = np.append(np.linspace(-1, -1, int(n / 2)), np.linspace(1, 1, int(n / 2)))
    diff_adc_sr = []
    diff_in_samples0 = {}
    diff_in_samples1 = {}
    freq = []
    input_data = []
    for i in dac_osr:
        freq.append(max_dac_samplerate / (n * i))
    for sr in adc_sr:
        ain.setSampleRate(sr)
        diff_osr = []
        for osr in dac_osr:
            aout.setOversamplingRatio(0, osr)
            aout.setOversamplingRatio(1, osr)
            aout.push([test_signal, test_signal])
            try:
                input_data = ain.getSamples(delay * 2)
            except:
                print("Timeout occurred")
            input_data0 = input_data[libm2k.ANALOG_IN_CHANNEL_1][delay - 5:delay + 5]
            input_data1 = input_data[libm2k.ANALOG_IN_CHANNEL_2][delay - 5:delay + 5]

            diff_in_samples0['ADC sr' + str(sr) + '; DAC osr:' + str(osr)] = input_data[0]
            diff_in_samples1['ADC sr' + str(sr) + '; DAC osr:' + str(osr)] = input_data[1]
            ain.stopAcquisition()
            for i in range(len(input_data0) - 1):
                if (input_data0[i] <= 0 < input_data0[i + 1]) or (input_data0[i] >= 0 > input_data0[i + 1]):
                    p0 = i  # position of trigger on channel 0
                if (input_data1[i] <= 0 < input_data1[i + 1]) or (input_data1[i] >= 0 > input_data1[i + 1]):
                    p1 = i  # position of trigger on channel 1
            diff_osr.append(p1 - p0)

        diff_adc_sr.append(diff_osr)
        data_string.append('ADC sample rate: ' + str(sr))
        data_string.append('DAC sample rate: ' + str(max_dac_samplerate))
        data_string.append('Frequency: ' + str(freq))
        data_string.append('DAC oversampling ratio' + str(dac_osr))
        data_string.append('Samples difference: ' + str(diff_osr))

    if gen_reports:
        write_file(file, test_name, channel, data_string)
        save_data_to_csv(diff_in_samples0, csv_path + 'diffSamples_ch0_trigSrc_' + str(channel) + '.csv')
        save_data_to_csv(diff_in_samples1, csv_path + 'diffSamples_ch1_trigSrc_' + str(channel) + '.csv')
    aout.stop()
    return diff_adc_sr, adc_sr, dac_osr, freq


def write_file(file, test_name, channel, data_string):
    if test_name == "phase_dif_samples":
        file.write("\n\n Difference in samples between Analog channels, Trigger source: Ch " + str(channel) + '\n')
    elif test_name == "osr":
        file.write("\n\n Ain oversampling ratio test on channel: " + str(channel) + "\n")
    elif test_name == "freq":
        file.write("\n\nFrequency test on channel " + str(channel) + "\n")
    elif test_name == "voltmeter":
        file.write("\n\nVoltmeter functionality on channel " + str(channel) + ": \n")
    elif test_name == "offset":
        file.write("\n\n Offset test on channel " + str(channel) + "\n")
    elif test_name == "analog_trigger":
        file.write("\n\nTrigger conditions on channel " + str(channel) + ":\n")
    elif test_name == "phase_diff":
        file.write("\n\n The same buffer sent simultaneously on both analog channels:\n")
    elif test_name == "shape":
        file.write("\n\nTest of analog input and output with different signal shapes:\n")
    elif test_name == "amplitude":
        file.write("\n\nAmplitude test on channel " + str(channel) + ": \n")
    elif test_name == "buffer_transition_glitch":
        file.write("\n\nTest buffer transition glitch on channel " + str(channel) + ": \n")
    for i in range(len(data_string)):
        file.write(str(data_string[i]) + '\n')


def test_timeout(ctx, ain, aout, trig, channel, dir_name, file, csv_path):
    # Set timeout, set trigger. Acquire data and if timeout occurs, set timeout to 0 and
    # reset trigger then acquire data again. The signal has an offset so if you compute the mean it is different than 0.
    # Arguments:
    #    ctx-Context
    #    ain  -- AnalogIn object
    #    aout  -- AnalogOut object
    #    trig -- Trigger object
    #    channel -- Analog channel under test
    #    dir_name -- Directory where the plot files are saved
    #    file -- Text file where are saved reference and computed values during the test
    #   csv_path -- Path to the csv file where are saved the samples
    # Returns
    #    offset -- value of the average set
    #    average-- value of the computed average
    #    t_occ-- False by default, True if timeout occurred

    reset.analog_in(ain)
    reset.analog_out(aout)
    reset.trigger(trig)

    timeout_val = 2  # value of the timeout, small value so timeout occurrs during test
    t_occ = False
    in_samples = 4096
    out_samples = 4096
    offset = 0.5  # average value of the signal
    suma = 0
    average = 0
    # gen out_data buffer
    x = offset + np.sin(np.linspace(-np.pi, np.pi, out_samples))
    out_data = [x, x]  # output buffer, same signal on both analog channels
    set_trig(trig, 0, 0, libm2k.FALLING_EDGE_ANALOG, 0)  # set trigger condition
    ctx.setTimeout(timeout_val)  # set timeout
    ain.stopAcquisition()  # flush buffer so previous values will not influence average
    # send data
    aout.push(out_data)
    try:
        input_data = ain.getSamples(in_samples)  # get data
    except:
        # timeout occurred
        t_occ = True
        ctx.setTimeout(0)  # set timeout to 0
        reset.trigger(trig)  # reset trigger
        input_data = ain.getSamples(in_samples)  # getdata after timeout and trigger reset

    for s in input_data[channel]:  # compute average value of the acquired data
        suma = suma + s
    average = suma / in_samples

    ctx.setTimeout(ctx_timeout)  # set timeout to the initial value

    if gen_reports:
        file.write("\n\n Timeout test on channel " + str(channel) + "\n")
        file.write("Timeout occured:\n" + str(t_occ) + "\n")
        file.write("Average of the data set:" + str(offset) + "\n Average of the data read: " + str(average) + "\n")
        if channel == 0:
            plot_to_file('Timeout: ' + str(t_occ) + ' on ch' + str(channel), input_data[channel], dir_name,
                         'timeout' + str(t_occ) + '_ch0.png')
            timeout_csv_vals['Timeout: ' + str(t_occ) + ' ch0'] = input_data[channel]
            save_data_to_csv(timeout_csv_vals, csv_path + 'timeout.csv')

        else:
            plot_to_file('Timeout: ' + str(t_occ) + ' on ch' + str(channel), input_data[channel], dir_name,
                         'timeout' + str(t_occ) + '_ch1.png')
            timeout_csv_vals['Timeout: ' + str(t_occ) + ' ch1'] = input_data[channel]
            save_data_to_csv(timeout_csv_vals, csv_path + 'timeout.csv')
        plt.close()
    aout.stop()
    return offset, average, t_occ

def test_kernel_buffers(ain, trig, nb_kernel_buffers):
    error = False
    reset.analog_in(ain)
    reset.trigger(trig)

    ain.setSampleRate(1000000)
    ain.startAcquisition(10000)
    ain.stopAcquisition()
    try:
        ain.setKernelBuffersCount(nb_kernel_buffers)
    except:
        error = True
    return error

def compute_percentage_diff(v1, v2):
    # https://www.oracle.com/webfolder/technetwork/data-quality/edqhelp/content/processor_library/matching/comparisons/percent_difference.htm
    vals = sorted([v1, v2])
    percentage_diff = 0
    try:
        percentage_diff = ((vals[1] - vals[0]) / vals[1])
    except:
        pass    
    return percentage_diff

def compute_y_distance(v1, v2):
    lens = sorted([v1, v2])
    distance = lens[1] - lens[0]
    return distance

def is_spike(data, peak, threshold = 0.25):
    # for sampling_frequency_in = 1_000_000 the center of of the glitch is at 75 samples distance with repect to the peak
    dx_small = 75 
    dx_large = 200
    
    prev_sample, next_sample = data[peak - dx_small], data[peak + dx_small]
    step_inside_glitch_range = compute_y_distance(prev_sample, next_sample)
    prev_sample, next_sample = data[peak - dx_large], data[peak + dx_large]
    step_outside_glitch_range = compute_y_distance(prev_sample, next_sample)
    
    percentage_dif = compute_percentage_diff(step_inside_glitch_range, step_outside_glitch_range)
    return percentage_dif > threshold

def test_buffer_transition_glitch(channel, ain, aout, trig, waveform, amplitude=1):
    if gen_reports:
        from create_files import results_file, results_dir, csv, open_files_and_dirs
        if results_file is None:
            file, dir_name, csv_path = open_files_and_dirs()
        else:
            file = results_file
            dir_name = results_dir
            csv_path = csv
    else:
        file = []
    
    BUFFER_SIZE = 5_00_000
        
    reset.analog_in(ain)
    reset.analog_out(aout) 
    reset.trigger(trig)
    
    test_name = "buffer_transition_glitch"
    data_string = []
    
    dac_sr = 75_000
    adc_sr = 1_000_000
    
    ain.setSampleRate(adc_sr)
    ain.setRange(channel, libm2k.PLUS_MINUS_2_5V)
    
    set_trig(trig, channel, 0, libm2k.RISING_EDGE_ANALOG, 0.1)
    
    aout.setSampleRate(channel, dac_sr)
    aout.enableChannel(channel, True)
    aout.setCyclic(True)
    ctx.setTimeout(10000)
    
    out_samples = 4096
    if waveform == 'sine':
        offset = 0
        data_high = amplitude* np.sin(np.linspace(offset, 2*np.pi + offset, out_samples ))
        data_low = -amplitude* np.sin(np.linspace(offset, 2*np.pi + offset, out_samples))
    if waveform == 'dc':
        data_high = [amplitude] * out_samples
        data_low = [-amplitude] * out_samples
    
    ain.startAcquisition(BUFFER_SIZE)
    for _ in range(5):
        aout.push(channel, data_high)
        time.sleep(0.1)
        aout.push(channel, data_low)
        time.sleep(0.1)
    try:
        data = np.array(ain.getSamples(BUFFER_SIZE)[channel][int(BUFFER_SIZE* 0.05):]) 
    except:
        print('Timeout occured')
    
    aout.stop()
    ain.stopAcquisition()
    
    param_args = {
        'sine': {
            'threshold': 0.1,
            'find_peaks_args': {
                'prominence': (0.25, 1),
                'height': (0.1, amplitude),
            },
        },
        'dc': {
            'threshold': 0.4,
            'find_peaks_args': {
                'prominence': 1,
                'height': 0.1,
            },
        },
    }
    
    peaks_pos, _ = find_peaks(data, **param_args[waveform]["find_peaks_args"]) 
    peaks_neg, _ = find_peaks(-data, **param_args[waveform]["find_peaks_args"]) 
    
    peaks = np.concatenate((peaks_pos, peaks_neg)) 
    filtered_peaks = list(filter(lambda peak: is_spike(data, peak, param_args[waveform]["threshold"]), peaks))  
    num_peaks = len(filtered_peaks)
    
    data_string.append(
            "Number of glitch peaks found in " + waveform + " signal :" + str(num_peaks))
    
    if gen_reports:
        write_file(file, test_name, channel, data_string)    
        plot_to_file(f'Buffer Glitch , channel{channel}', 
                     data, 
                     dir_name, 
                     f'buffer_glitch_plot_ch{channel}_{waveform}.png',
                     data_marked=filtered_peaks)

    return num_peaks