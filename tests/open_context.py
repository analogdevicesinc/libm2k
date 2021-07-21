import libm2k
import time
import os


def calibrate(ctx):
    """Calibrates ADC and DAC
    
    Arguments:
        ctx  -- the available m2k context\n
    Returns:
        adc_calib-- Must be True if the calibration was successfull
        dac_calib--Must be True if the calibration was successfull
    """
    # ADC
    adc_calib = ctx.calibrateADC()
    # DAC
    dac_calib = ctx.calibrateDAC()
    return (adc_calib, dac_calib)


def open_context():
    """Opens the context and retrieves analog objects
    Returns:
        ctx-- M2k context opened
        ain-- AnalogIn object
        aout--Analog Out object
        trig--Trigger object
    """
    ctx = libm2k.m2kOpen("ip:192.168.2.1")
    if ctx is None:
        print("Connection Error: No ADALM2000 device available/connected to your PC.")
        exit(1)
    # define input and output
    try:
        ain = ctx.getAnalogIn()
    except:
        ain = 0
    try:
        aout = ctx.getAnalogOut()
    except:
        aout = 0
    # define trigger
    try:
        trig = ain.getTrigger()
    except:
        trig = 0

    return ctx, ain, aout, trig


def create_dir(timestamp):
    """Creates a new directory where all the plot files will be saved
    
    Arguments:
        timestamp -- Time and date when the program was run\n
    """
    path = os.getcwd()

    results_dir = "results" + str(timestamp)
    path = os.path.join(path, results_dir)
    os.mkdir(path)

    return results_dir


# open context
ctx_timeout = 5000  # initial value of the timeout
ctx, ain, aout, trig = open_context()  # open context
ctx.setTimeout(ctx_timeout)  # set ctx timeout
calibration = calibrate(ctx)
dig = ctx.getDigital()
d_trig = dig.getTrigger()
ps = ctx.getPowerSupply()
