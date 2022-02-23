import matplotlib.pyplot as plt
import numpy as np
import libm2k
from multiprocessing.pool import ThreadPool
import threading
from pandas import DataFrame
import random
import time
from open_context import ctx
from scipy.signal import find_peaks

sample = random.randint(0, 255)
gen_reports = True


def result_files(gen_reports):
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
        dir_name = []
        csv_path = []

    return file, dir_name, csv_path


def dig_reset(dig):
    # Reset digital object
    # Arguments:
    #    dig  -- Digital object

    dig.setSampleRateIn(10000)
    dig.setSampleRateOut(10000)
    dig.setCyclic(True)
    dig.setDirection(0b1111111111111111)
    for i in range(16):
        dig.setOutputMode(i, 1)
    dig.enableAllOut(True)
    return


def set_digital_trigger(dig):
    # Set the digital trigger
    # Arguments:
    #    dig  -- Digital object

    d_trig = dig.getTrigger()
    d_trig.setDigitalMode(0)
    d_trig.setDigitalStreamingFlag(True)
    for i in range(16):
        d_trig.setDigitalCondition(i, 0)
    return d_trig


def check_digital_channels_state(dig, channel):

    dig.reset()
    # enable channel under test
    dig.setDirection(channel, libm2k.DIO_OUTPUT)
    dig.enableChannel(channel, True)
    dig.setValueRaw(channel, libm2k.LOW)
    # enable 8 input channels
    ch1 = []
    for level in [libm2k.HIGH, libm2k.LOW]:
        dig.setValueRaw(channel, level)
        val = dig.getValueRaw(channel)
        if val == level:
            ch1.append(1)
        else:
            ch1.append(0)

    return ch1


def check_digital_output(dig):
    dig.stopAcquisition()
    dig.stopBufferOut()
    dig.reset()
    d_trig = dig.getTrigger()
    d_trig.setDigitalMode(0)
    d_trig.setDigitalStreamingFlag(False)
    for i in range(16):
        d_trig.setDigitalCondition(i, libm2k.NO_TRIGGER_DIGITAL)
    dig.setCyclic(True)

    for i in range(16):
        dig.setDirection(i, libm2k.DIO_OUTPUT)
        dig.enableChannel(i, True)
    ch2 = []
    for i in range(16):
        # get the signal from the digital channel i
        buff = [(2 ** i)] * 16
        dig.push(buff)
        print(buff)
        dig.startAcquisition(16)
        data = dig.getSamples(16)
        dig.stopAcquisition()
        print(data)
        val = data[0]
        if val & (2 ** i) == 2 ** i:
            ch2.append(1)
        else:
            ch2.append(0)
    return ch2


def get_data_to_check_trig_condition(dig, channel, i, buff):
    ch = []
    n = 1024
    delay = 1
    dig.stopBufferOut()
    ctx.setTimeout(5000)
    d_trig = dig.getTrigger()
    d_trig.reset()
    d_trig.setDigitalMode(0)
    d_trig.setDigitalStreamingFlag(False)
    for j in range(16):
        d_trig.setDigitalCondition(j, libm2k.NO_TRIGGER_DIGITAL)
    d_trig.setDigitalCondition(channel, i)
    d_trig.setDigitalDelay(-delay)
    dig.setDirection(channel, libm2k.DIO_OUTPUT)
    dig.enableChannel(channel, True)
    dig.setCyclic(True)
    dig.push(buff)
    data = dig.getSamples(n)
    dig.stopAcquisition()

    for val in data:
        if val & (2 ** channel) == 2 ** channel:
            ch = np.append(ch, 1)
        else:
            ch = np.append(ch, 0)

    dig.stopAcquisition()
    dig.stopBufferOut()

    return ch


def check_digital_trigger(channel, dig, d_trig):
    file_name, dir_name, csv_path = result_files(gen_reports)
    dig_reset(dig)
    delay = 1

    condition = [libm2k.RISING_EDGE_DIGITAL, libm2k.FALLING_EDGE_DIGITAL, libm2k.LOW_LEVEL_DIGITAL,
                 libm2k.HIGH_LEVEL_DIGITAL, libm2k.ANY_EDGE_DIGITAL, libm2k.NO_TRIGGER_DIGITAL]
    trig_test = []
    dig.setSampleRateIn(1000000)
    dig.setSampleRateOut(1000000)
    # create square buffer containing 2 periods
    duty = 256 / 2  # 50%
    signal = np.arange(256) < duty
    buff = list(map(lambda s: int(s) << channel, signal))
    buff.extend(buff)
    # go through all possible trigger conditions
    for i in condition:
        if i == libm2k.RISING_EDGE_DIGITAL:
            ch = get_data_to_check_trig_condition(dig, channel, i, buff)
            if ch[delay] <= ch[delay + 3]:
                print("rising")
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)

            if gen_reports:
                plot_to_file("Rising Edge Trigger on ch" + str(channel), ch, dir_name,
                             "digital_trigger_rising_edge_ch" + str(channel) + ".png")
        elif i == libm2k.FALLING_EDGE_DIGITAL:
            ch = get_data_to_check_trig_condition(dig, channel, i, buff)
            if ch[delay] >= ch[delay + 3]:
                print("falling")
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            if gen_reports:
                plot_to_file("Falling Edge Trigger on ch" + str(channel), ch, dir_name,
                             "digital_trigger_falling_edge_ch" + str(channel) + ".png")
        elif i == libm2k.LOW_LEVEL_DIGITAL:
            d_trig.reset()
            ch = get_data_to_check_trig_condition(dig, channel, i, buff)
            if ch[delay] == 0:
                print("low")
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)
            if gen_reports:
                plot_to_file("Low Level Trigger on ch" + str(channel), ch, dir_name,
                             "digital_trigger_low_level_ch" + str(channel) + ".png")
        elif i == libm2k.HIGH_LEVEL_DIGITAL:
            d_trig.reset()
            ch = get_data_to_check_trig_condition(dig, channel, i, buff)
            if ch[delay] == 1:
                print("high")
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)

            if gen_reports:
                plot_to_file("High Level Trigger on ch" + str(channel), ch, dir_name,
                             "digital_trigger_high_level_ch" + str(channel) + ".png")

        elif i == libm2k.ANY_EDGE_DIGITAL:
            ch = get_data_to_check_trig_condition(dig, channel, i, buff)
            if ch[delay] <= ch[delay + 3]:
                print("rising")
                trig_test = np.append(trig_test, 1)
            elif ch[delay] >= ch[delay + 3]:
                print("falling")
                trig_test = np.append(trig_test, 1)
            else:
                trig_test = np.append(trig_test, 0)

            if gen_reports:
                plot_to_file("Any Edge Trigger on ch" + str(channel), ch, dir_name,
                             "digital_trigger_any_edge_ch" + str(channel) + ".png")

    return trig_test


def check_open_drain_mode(dig, channel):
    # TBD
    dig.enableChannel(channel, True)
    dig.enableChannel(channel + 8, True)
    dig.setSampleRateIn(1000)
    dig.setSampleRateOut(1000)  # set sample rate and buffer length for a frequency smaller than 450HZ
    dig.setOutputMode(channel, libm2k.DIO_OPENDRAIN)  # OD, PP
    dig.setDirection(channel, libm2k.DIO_OUTPUT)
    dig.setDirection(channel + 8, libm2k.DIO_INPUT)
    ch = []
    buff = [1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1]  # 100Hz frequency
    dig.push(buff)
    data = dig.getSamples(100)
    for val in data:
        print((val))
        if val & (2 ** channel) == 2 ** channel:
            ch = np.append(ch, 1)
        else:
            ch = np.append(ch, 0)

    return


def task1(nb_samples, dig):
    # Parallel process where data is read
    # Arguments:
    #    nb_samples  -- Number of samples
    #    dig  -- Digital object

    print("Task 1 assigned to thread: {}".format(threading.current_thread().name))
    data = dig.getSamples(nb_samples)
    dig.stopAcquisition()
    return data


def test_digital_cyclic_buffer(dig, d_trig, channel):
    file, dir_name, csv_path = result_files(gen_reports)
    dig.setDirection(channel, libm2k.DIO_OUTPUT)
    dig.enableChannel(channel, True)
    d_trig.setDigitalCondition(channel, libm2k.LOW_LEVEL_DIGITAL)
    dig.setCyclic(True)
    dig.setSampleRateOut(1000000)
    dig.setSampleRateIn(1000000)
    n = 1024
    duty = 256 / 2  # 50%
    signal = np.arange(256) < duty
    buff = list(map(lambda s: int(s) << channel, signal))
    buff.extend(buff)
    ch = []
    pool = ThreadPool(processes=1)
    print("Main thread name: {}".format(threading.main_thread().name))

    async_result = pool.apply_async(task1, args=[n, dig])
    dig.push(buff)
    return_val = async_result.get()
    for val in return_val:
        if val & (2 ** channel) == 2 ** channel:
            ch = np.append(ch, 1)
        else:
            ch = np.append(ch, 0)
    dig.stopBufferOut()
    dig.stopAcquisition()
    nb_read_buffers = find_peaks(ch)
    if gen_reports:
        plot_to_file("Cyclic Buffer on Digital Channel " + str(channel), ch, dir_name,
                     "cyclic_buff_ch" + str(channel) + ".png")
        save_data_to_csv(ch, csv_path)
    if len(nb_read_buffers) >= 3:
        passed = True
    else:
        passed = False
    return passed


def plot_to_file(title, data, dir_name, filename, xlabel=None, ylabel=None, data1=None):
    # Saves the plots in a separate folder
    # Arguments:
    #    title  -- Title of the plot\n
    #    data  -- Data to be plotted\n
    #    filename  -- Name of the file with the plot\n
    # Keyword Arguments:
    #    xlabel  -- Label of x-Axis (default: {None})
    #    ylabel  -- Label of y-Axis(default: {None})
    #    data1  --  Data that should be plotted on the same plot(default: {None})

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
    plt.grid(b=True)
    plt.plot(data)  # if a second set of data must be printed (for ch0 and ch1 phase difference in this case)
    if data1 is not None:
        plt.plot(data1)
    plt.savefig(dir_name + "/" + filename)
    plt.close()
    return


def save_data_to_csv(csv_vals, csv_file):
    df = DataFrame(csv_vals)
    df.to_csv(csv_file)
    return
