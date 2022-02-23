import libm2k


# set default values for analogin
def analog_in(ain):
    #  Sets default values for ain parameters
    # Arguments:
    #    ain  -- AnalogIn object

    ain.stopAcquisition()
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    ain.setVerticalOffset(libm2k.ANALOG_IN_CHANNEL_1, 0)
    ain.setVerticalOffset(libm2k.ANALOG_IN_CHANNEL_2, 0)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, libm2k.PLUS_MINUS_2_5V)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, libm2k.PLUS_MINUS_2_5V)
    ain.setOversamplingRatio(1)
    ain.setSampleRate(100000)
    ain.setKernelBuffersCount(1)

    return


# set default values for analog out
def analog_out(aout):
    # Sets default values for aout parameters
    # Arguments:
    #    aout  -- AnalogOut object

    aout.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    aout.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_1, 750000)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_2, 750000)
    aout.setOversamplingRatio(libm2k.ANALOG_IN_CHANNEL_1, 1)
    aout.setOversamplingRatio(libm2k.ANALOG_IN_CHANNEL_2, 1)
    aout.setCyclic(True)
    return


# set default values for hardware trigger
def trigger(trig):
    # Sets default values for trigger parameters
    # Arguments:
    #    trig  -- Trigger object

    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_1, 0.0)
    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_2, 0.0)
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_1, 0)
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_2, 0)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_1, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_2, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ALWAYS)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ALWAYS)
    trig.setAnalogSource(libm2k.CHANNEL_1)
    trig.setAnalogSourceChannel(libm2k.ANALOG_IN_CHANNEL_1)
    trig.setAnalogDelay(0)

    return
