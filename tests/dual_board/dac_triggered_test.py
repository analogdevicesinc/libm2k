#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

"""
This script performs synchronization and interaction tests between two ADALM2000 boards using the libm2k library. 
The MASTER board measures signals and generates trigger events, while the SLAVE board generates signals in response 
to these events. The script configures both boards, generates waveforms, triggers the signals, and captures data 
to evaluate the delay between trigger and response. Results, including plots and configuration details, are saved 
in a timestamped results folder.

Requirements: x2 ADALM2000 devices ()
======================================
    - MASTER: measure the signal and generate trigger events.
    - SLAVE: generate a signal at the output interface  when trigger event is detected.
        - use firmware >= 0.33

Connections:
============
    - (MASTER)    GND     <->    GND            (SLAVE)
    - (MASTER)    1+      <->    W1             (SLAVE)    --> Measure slave signal
    - (MASTER)    2+      <->    TI/1+/DIO_0    (SLAVE)    --> Trigger sources
    - (MASTER)    DIO_0   <->    TI/1+/DIO_0    (SLAVE)    --> Trigger signal - used to synchronize measurements
"""
import libm2k
import numpy as np
import matplotlib.pyplot as plt
import time
import os
import datetime
#  Configurations:
URI_MASTER = "ip:192.168.2.1"
URI_SLAVE = "ip:192.168.2.2"

CONVERT_TO_DIGITAL = False  # The plot will display waveform as digital signal with 3 states
TIMEOUT: int = 10_000
DELAY = 1  # [s] # delay between push and trigger generation -> ensure start on trigger

AMPLITUDE = 5
NUM_SAMPLES_PER_PERIOD = 1024
TRIGGER_THRESHOLD = 2  # [V]
INPUT_BUFFER_SIZE = 16_000  # 80k samples

SET_CYCLIC = True
ACTIONS = {"START": libm2k.START, "STOP": libm2k.STOP}
TRIGGER_SRC = {
    "TI": libm2k.TRIGGER_TI,
    "ADC": libm2k.TRIGGER_ADC,
    "LA": libm2k.TRIGGER_LA,
}
SET_AUTO_REARM = False
# Output plot
COLUMNS = ["TRIG_ACTION", "TRIG_SRC", "SR_DAC", "SR_ADC", "CYCLIC", "DELAY"]


def generate_square_wave(num_samples: int, amplitude: int):
    return amplitude * np.sign(np.sin(np.linspace(0, 2 * np.pi, num_samples)))


def to_logic(sample):
    # Digital signal with 3 states: -1, 0, 1
    # We send a square wave of +/- AMPLITUDE. When the channel stops the output goes back to 0.
    if sample >= TRIGGER_THRESHOLD:
        return 1
    if sample <= -TRIGGER_THRESHOLD:
        return -1
    return 0


def get_sample_rate_display_format(sample_rate):
    if sample_rate < 1e3:
        return f"{sample_rate:.2f} Hz"
    if sample_rate < 1e6:
        return f"{sample_rate/1e3:.2f} KHz"
    if sample_rate < 1e9:
        return f"{sample_rate/1e6:.2f} MHz"
    return f"{sample_rate/1e9:.2f} GHz"


def get_time_format(time):
    if time < 1e-6:
        return f"{time*1e9:.2f} ns"
    if time < 1e-3:
        return f"{time*1e6:.2f} us"
    if time < 1:
        return f"{time*1e3:.2f} ms"
    return f"{time:.2f} s"


def compute_delay(trigger, signal, sample_rate, action, to_digital):
    # Ensure analog signal is converted to digital to compute delay between edges
    if not to_digital:
        trig_signal = list(map(to_logic, trigger))
        dac_signal = list(map(to_logic, signal))
    else:
        trig_signal = trigger
        dac_signal = signal

    trig_edges = []
    dac_edges = []
    for i in range(1, len(trig_signal)):
        if trig_signal[i] != trig_signal[i - 1]:
            trig_edges.append(i)
        if dac_signal[i] != dac_signal[i - 1]:
            dac_edges.append(i)

    if len(trig_edges) == 0 or len(dac_edges) == 0:
        raise ValueError("No trigger or dac edge found")

    trigger_edge_sample = trig_edges[0]
    if action == "START":
        dac_edge_sample = dac_edges[0]
    if action == "STOP":
        dac_edge_sample = dac_edges[-1]

    print(f"Trigger edge sample: {trigger_edge_sample}")
    print(f"DAC edge sample: {dac_edge_sample}")

    delta = np.abs(dac_edge_sample - trigger_edge_sample) * (1 / sample_rate)
    return get_time_format(delta)


def configure_m2k_master(ctx: libm2k.M2k):
    ain: libm2k.M2kAnalogIn = ctx.getAnalogIn()
    aout: libm2k.M2kAnalogOut = ctx.getAnalogOut()
    dig: libm2k.M2kDigital = ctx.getDigital()
    trig: libm2k.M2kHardwareTrigger = ain.getTrigger()

    # 1+ : captures waveform
    # 2+ : captures trigger signal
    ain.setSampleRate(ain.getMaximumSamplerate())
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, -10, 10)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, -10, 10)
    ain.setKernelBuffersCount(1)

    # Pin used to send trigger signal
    dig.setDirection(libm2k.DIO_CHANNEL_0, libm2k.DIO_OUTPUT)
    dig.setOutputMode(libm2k.DIO_CHANNEL_0, libm2k.DIO_PUSHPULL)
    dig.enableChannel(libm2k.DIO_CHANNEL_0, True)
    dig.setValueRaw(libm2k.DIO_CHANNEL_0, libm2k.LOW)

    aout.setSampleRate(0, aout.getMaximumSamplerate(0))
    aout.setSampleRate(1, aout.getMaximumSamplerate(1))
    aout.enableChannel(0, True)
    aout.enableChannel(1, True)

    # Acquisition starts when trigger is received on 2+
    trig.setAnalogSource(libm2k.ANALOG_IN_CHANNEL_2)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_2, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_2, 2)
    trig.setAnalogDelay(max(-(int)(INPUT_BUFFER_SIZE * 0.3), -8192))
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_2, libm2k.ANALOG)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ANALOG)
    print(f"Master analog delay: {trig.getAnalogDelay()}")


def configure_m2k_slave(ctx: libm2k.M2k):
    ain: libm2k.M2kAnalogIn = ctx.getAnalogIn()
    aout: libm2k.M2kAnalogOut = ctx.getAnalogOut()
    dig: libm2k.M2kDigital = ctx.getDigital()
    trig: libm2k.M2kHardwareTrigger = ain.getTrigger()  # trigger is shared

    aout.setSampleRate(0, aout.getMaximumSamplerate(0))
    aout.setSampleRate(1, aout.getMaximumSamplerate(1))
    aout.enableChannel(0, True)
    aout.enableChannel(1, True)

    # ADC trigger source response time depens on the sample rate
    ain.setSampleRate(ain.getMaximumSamplerate())

    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, -10, 10)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, -10, 10)

    aout.setCyclic(False)  # NOTE: this was changed in updated version

    dig.setSampleRateIn(75000000)
    dig.setSampleRateOut(75000000)


def configure_dac_trigger(ctx: libm2k.M2k, trigger_source, trigger_action):
    trig: libm2k.M2kHardwareTrigger = ctx.getAnalogOut().getTrigger()

    status = ACTIONS.get(trigger_action)
    if status is None:
        raise ValueError(f"No action defined for trigger action: {trigger_action}")

    match trigger_source:
        case "TI":
            print("Setting TI as trigger source for DAC")
            trig.setAnalogOutTriggerSource(libm2k.TRIGGER_TI)
            trig.setAnalogOutTriggerCondition(libm2k.RISING_EDGE_OUT)
            trig.setAnalogOutTriggerStatus(status)
        case "ADC":
            print("Setting ADC as trigger source for DAC")
            ain: libm2k.M2kAnalogIn = ctx.getAnalogIn()
            ain.setSampleRate(ain.getMaximumSamplerate())

            trig.setAnalogSource(libm2k.ANALOG_IN_CHANNEL_1)
            trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_1, libm2k.RISING_EDGE_ANALOG)
            trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_1, TRIGGER_THRESHOLD)
            trig.setAnalogDelay(0)
            trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ANALOG)

            trig.setAnalogOutTriggerSource(libm2k.TRIGGER_ADC)
            trig.setAnalogOutTriggerStatus(status)
        case "LA":
            print("Setting LA as trigger source for DAC")
            dig: libm2k.M2kDigital = ctx.getDigital()
            dig.setDirection(libm2k.DIO_CHANNEL_0, libm2k.DIO_INPUT)
            dig.enableChannel(libm2k.DIO_CHANNEL_0, True)
            dig.setOutputMode(libm2k.DIO_CHANNEL_0, libm2k.DIO_PUSHPULL)

            trig.setDigitalCondition(libm2k.DIO_CHANNEL_0, libm2k.RISING_EDGE_DIGITAL)

            trig.setAnalogOutTriggerSource(libm2k.TRIGGER_LA)
            trig.setAnalogOutTriggerStatus(status)
        case _:
            raise ValueError(f"No source defined for trigger source: {trigger_source}")


def generate_trigger_signal(ctx: libm2k.M2k):
    dig: libm2k.M2kDigital = ctx.getDigital()
    dig.setValueRaw(libm2k.DIO_CHANNEL_0, libm2k.HIGH)


def main():
    folder_name = "results_" + datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    os.makedirs(folder_name, exist_ok=True)

    try:
        ctx_master: libm2k.M2k = libm2k.m2kOpen(URI_MASTER)
        ctx_slave: libm2k.M2k = libm2k.m2kOpen(URI_SLAVE)

        if ctx_master is None:
            raise ValueError(f"Failed to establish connection with MASTER device")
        if ctx_slave is None:
            raise ValueError(f"Failed to establish connection with SLAVE device")

        ain_master: libm2k.M2kAnalogIn = ctx_master.getAnalogIn()

        for trigger_action in ACTIONS.keys():
            print(f"Current action: {trigger_action}")
            for trigger_source in TRIGGER_SRC.keys():
                print(f"\tCurrent source: {trigger_source}")

                ctx_master.reset()
                ctx_slave.reset()

                ctx_master.calibrateADC()
                ctx_master.calibrateDAC()
                ctx_slave.calibrateADC()
                ctx_slave.calibrateDAC()

                configure_m2k_master(ctx_master)
                ain_master.startAcquisition(INPUT_BUFFER_SIZE)

                # we expect to receive the trigger signal in max 10 seconds
                ctx_master.setTimeout(TIMEOUT)

                configure_m2k_slave(ctx_slave)
                configure_dac_trigger(ctx_slave, trigger_source, trigger_action)

                waveform = generate_square_wave(NUM_SAMPLES_PER_PERIOD, AMPLITUDE)
                aout_slave: libm2k.M2kAnalogOut = ctx_slave.getAnalogOut()
                aout_slave.setCyclic(SET_CYCLIC)
                aout_slave.setBufferRearmOnTrigger(SET_AUTO_REARM)
                aout_slave.push([waveform, waveform])

                print(f"Delay for {DELAY} seconds")
                time.sleep(DELAY)
                print(f"\tMaster generates trigger signal")
                generate_trigger_signal(ctx_master)

                # Data processing
                print(f"\tPlotting data")
                master_data = ain_master.getSamples(INPUT_BUFFER_SIZE)
                dac_output = master_data[0]
                trigger_signal = master_data[1]

                if CONVERT_TO_DIGITAL:
                    dac_output = list(map(to_logic, dac_output))
                    trigger_signal = list(map(to_logic, trigger_signal))

                cell_text = [
                    trigger_action,
                    trigger_source,
                    get_sample_rate_display_format(ain_master.getSampleRate()),
                    get_sample_rate_display_format(aout_slave.getSampleRate(0)),
                    SET_CYCLIC,
                    compute_delay(
                        trigger_signal,
                        dac_output,
                        ain_master.getSampleRate(),
                        trigger_action,
                        CONVERT_TO_DIGITAL,
                    ),
                ]

                plt.plot(dac_output, label="DAC output")
                plt.plot(trigger_signal, label="Trigger signal")
                plt.legend()

                plt.grid(True)
                plt.yticks([])

                plt.table(
                    cellText=[cell_text],
                    colLabels=COLUMNS,
                    loc="top",
                    cellLoc="center",
                    fontsize=18,
                )

                plt.savefig(f"{folder_name}/{trigger_action}_{trigger_source}.png")
                plt.clf()

        print("Done successfully")
    except Exception as e:
        print(f"Error: {e}")
    except:
        print("Unexpected error")

    print("Closing connections")
    libm2k.contextClose(ctx_master)
    libm2k.contextClose(ctx_slave)


if __name__ == "__main__":
    main()
