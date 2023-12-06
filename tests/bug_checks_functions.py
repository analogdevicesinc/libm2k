from scipy.signal import find_peaks
import matplotlib.pyplot as plt
import libm2k

from open_context import ctx_timeout, ctx, ain, aout, trig

gen_reports = True


def test_dac_artifact(ain, aout, trig, chn):
    # check for DAC artifacts at slow sample rates
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
    buffer_size = 4000
    sampling_frequency_in = 1000000
    sampling_frequency_out = 750000
    # setup adc
    ain.setSampleRate(sampling_frequency_in)
    ain.setRange(chn, libm2k.PLUS_MINUS_2_5V)
    ain.enableChannel(chn, True)


    # setup trigger
    trig.setAnalogSource(chn)
    trig.setAnalogCondition(chn, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogLevel(chn, 0.5)  # Set trigger level at 0.5
    trig.setAnalogDelay(0)  # Trigger is centered
    trig.setAnalogMode(chn, libm2k.ANALOG)

    # setup dac
    aout.stop()
    aout.setCyclic(False)
    aout.enableChannel(chn, True)
    aout.setSampleRate(chn, sampling_frequency_out)

    buffer = generate_clock_signal()

    ain.startAcquisition(buffer_size)
    aout.push(chn, buffer)
    data = ain.getSamples(buffer_size)[chn]

    peaks = find_peaks(data, height=2.5)
    if peaks == 0:
        dac_artifact = 0
    else:
        dac_artifact = 1
    if gen_reports:
        plot_to_file('Spikes on chn '+ str(chn), data, dir_name, 'spikes_ch'+str(chn)+'.png')
    return dac_artifact


def generate_clock_signal():
    buffer = []
    for i in range(256):
        buffer.append(1)
    for i in range(256):
        buffer.append(0)
    return buffer


def plot_to_file(title, data, dir_name, filename, xlabel=None, ylabel=None, data1=None):
    # Saves the plots in a separate folder
    # Arguments:
    #    title  -- Title of the plot
    #    data  -- Data to be plotted
    #    filename  -- Name of the file with the plot
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
    plt.grid(visible=True)
    plt.plot(data)  # if a second set of data must be printed (for ch0 and ch1 phase difference in this case)
    if data1 is not None:
        plt.plot(data1)
    plt.savefig(dir_name + "/" + filename)
    plt.close()
    return
