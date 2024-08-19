from scipy.signal import find_peaks
import matplotlib.pyplot as plt
import libm2k

from helpers import get_result_files, plot_to_file
from open_context import ctx_timeout, ctx, ain, aout, trig

gen_reports = True


def test_dac_artifact(ain, aout, trig, chn):
    # check for DAC artifacts at slow sample rates
    file_name, dir_name, csv_path = get_result_files(gen_reports)

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