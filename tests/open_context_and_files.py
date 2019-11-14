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
    #ADC
    adc_calib=ctx.calibrateADC()
    #DAC
    dac_calib=ctx.calibrateDAC()
    ctx.setTimeout(1000)
    return (adc_calib, dac_calib)

def open_context():
    """Opens the context and retrieves analog objects
    Returns:
        ctx-- M2k context opened
        ain-- AnalogIn object
        aout--Analog Out object
        trig--Trigger object
    """
    ctx=libm2k.m2kOpen("ip:192.168.2.1")
    if ctx is None:
        print("Connection Error: No ADALM2000 device available/connected to your PC.")
        exit(1)
    #define input and output
    try:
        ain=ctx.getAnalogIn()
    except:
        ain=0
    try:
        aout=ctx.getAnalogOut()
    except:
        aout=0
    #define trigger
    try:
        trig=ain.getTrigger()
    except:
        trig=0

    return ctx, ain, aout, trig

def create_dir(timestamp):
    """Creates a new directory where all the plot files will be saved
    
    Arguments:
        timestamp -- Time and date when the program was run\n
    """
    path=os.getcwd()

    results_dir="results"+str(timestamp)
    path=path+"\\"+results_dir
    os.mkdir(path)
    
    return results_dir


#open context
ctx, ain, aout, trig = open_context()
calibration=calibrate(ctx)
dig=ctx.getDigital()
d_trig=dig.getTrigger()
ps=ctx.getPowerSupply()

timestamp=time.strftime("_%H-%M-%S_%Y-%m-%d")#reate timestamp
results_dir=create_dir(timestamp) #create new directory 
results_file=open(str(results_dir)+'/results_'+timestamp+'.txt','w') #create new file where the computed values can be found
csv_path=str(results_dir)+'/csv_'
results_file.write("\nADALM2000 Libm2k Tests- Result Values \n\n")


