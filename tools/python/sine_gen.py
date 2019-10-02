import libm2k
import math

available_sample_rates= [750, 7500, 75000, 750000, 7500000, 75000000]
max_rate = available_sample_rates[-1] # last sample rate = max rate
min_nr_of_points=10
max_buffer_size = 500000
uri = "ip:192.168.3.2"

def get_best_ratio(ratio):
    max_it=max_buffer_size/ratio
    best_ratio=ratio
    best_fract=1

    for i in range(1,int(max_it)):
        new_ratio = i*ratio
        (new_fract, integral) = math.modf(new_ratio)
        if new_fract < best_fract:
            best_fract = new_fract
            best_ratio = new_ratio
        if new_fract == 0:
            break

    return best_ratio,best_fract


def get_samples_count(rate, freq):
    ratio = rate/freq
    if ratio<min_nr_of_points and rate < max_rate:
        return 0
    if ratio<2:
        return 0

    ratio,fract = get_best_ratio(ratio)
    # ratio = number of periods in buffer
    # fract = what is left over - error

    size=int(ratio)
    while size & 0x03:
        size=size<<1
    while size < 1024:
        size=size<<1
    return size

def get_optimal_sample_rate(freq):
    for rate in available_sample_rates:
        buf_size = get_samples_count(rate,freq)
        if buf_size:
            return rate

def sine_buffer_generator(channel, freq, ampl, offset, phase):

    buffer = []

    sample_rate = get_optimal_sample_rate(freq)
    nr_of_samples = get_samples_count(sample_rate, freq)
    samples_per_period = sample_rate / freq
    phase_in_samples = ((phase/360) * samples_per_period)

    #print("sample_rate:",sample_rate)
    #print("number_of_samples",nr_of_samples)
    #print("samples_per_period",samples_per_period)
    #print("phase_in_samples",phase_in_samples)

    for i in range(nr_of_samples):
        buffer.append(offset + ampl * (math.sin(((i + phase_in_samples)/samples_per_period) * 2*math.pi) ))

    return sample_rate, buffer

def main():
    ctx=libm2k.m2kOpen(uri)
    ctx.calibrateADC()
    ctx.calibrateDAC()

    siggen=ctx.getAnalogOut()

    #call buffer generator, returns sample rate and buffer
    samp0,buffer0 = sine_buffer_generator(0,200000,2,0,90)
    samp1,buffer1 = sine_buffer_generator(1,200000,2,0,0)

    siggen.enableChannel(0, True)
    siggen.enableChannel(1, True)

    siggen.setSampleRate(0, samp0)
    siggen.setSampleRate(1, samp1)

    siggen.push([buffer0,buffer1])

    input( " Press any key to stop generation ")

    siggen.stop()
    libm2k.deviceClose(ctx)

main()
