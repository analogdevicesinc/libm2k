import argparse
import libm2k
import signal
import time
import math
import sys
import os


PREFIX = 'cal,temp_lut='

example = '''\
Examples:
    |============================================================================|
    |Create the ini file 'm2k-calib-temp-lut.ini'. The process can be stopped by |
    |pressing 'CTRL + C', otherwise the process will stop when the temperature   |
    |of the board rises up to 75 °C or after 30 minutes                          |
    |                                                                            |
    |    python3 generate_temperature_calib_lut.py ip:192.168.2.1                |
    |                                                                            |
    |============================================================================|
    |Create the ini file 'm2k-calib-temp-lut.ini'. The process can be stopped by |
    |pressing 'CTRL + C' or when the temperature of the board rises up to 54 °C  |
    |                                                                            |
    |    python3 generate_temperature_calib_lut.py auto -t 54                    |
    |                                                                            |
    |============================================================================|
    |Create the ini file 'example.ini'. Extract 5 values from all computed       |
    |calibration parameters. The maximum temperature will be 54 °C               |
    |                                                                            |
    | python3 generate_temperature_calib_lut.py auto -v 5 -t 54 -f "example.ini" |
    |                                                                            |
    |============================================================================|
'''


def create_file(calibration_values, file, append):
    if os.path.isfile(file) is False:
        f = open(file, 'w+')
        f.close()

    if append:
        with open(file, mode='r') as file:
            content = file.read()
            content = content.replace(PREFIX, '')
            if not content:
                return
            values = content.split(',')

            while len(values) > 0:
                key = float(values.pop(0))
                parameters = []
                for i in range(8):
                    val = float(values.pop(0))
                    if val.is_integer():
                        parameters.append(int(val))
                    else:
                        parameters.append(val)
                calibration_values[key] = parameters


def write_in_file(calibration_values, nb_values, file_name):
    keys = list(calibration_values.keys())
    if 0 < nb_values < len(keys):
        length = float(len(keys))
        valid_keys = []
        for i in range(nb_values):
            valid_keys.append(keys[math.ceil(i * length / nb_values)])

        newdict = {k: calibration_values[k] for k in valid_keys}
        calibration_values = newdict

    with open(file_name, mode='w') as file:
        file.write(PREFIX)

        content = ''
        for key, values in sorted(calibration_values.items()):
            content += ('{0},{1},'.format(key, ','.join(str(round(v, 6)) for v in values)))
        content = content[:-1]
        file.write(content)
    print('The calibration values were written to the file: ' + file_name)
    print('Copy the file into the device and eject it (do not unplug!) in order to apply the values. After ejecting, '
          'the device will reboot. Once booted, the temperature calibration look-up table will be usable from libm2k')
    print('IMPORTANT: if you are using the -f or --file option you should rename the file to '
          '\'m2k-calib-temp-lut.ini\' before copying it to the device.')


def get_key_values(context):
    key = context.getDMM('ad9963').readChannel('temp0').value
    parameters = [context.getAdcCalibrationOffset(0), context.getAdcCalibrationOffset(1),
                  context.getAdcCalibrationGain(0), context.getAdcCalibrationGain(1),
                  context.getDacCalibrationOffset(0), context.getDacCalibrationOffset(1),
                  context.getDacCalibrationGain(0), context.getDacCalibrationGain(1)]

    return key, parameters


def get_new_temperatures(visited, new):
    new_temperatures = []
    for key in new.keys():
        if key not in visited:
            new_temperatures.append(key)
    return new_temperatures


def print_msg(time_left, new_temperatures, calibration_values):
    displayed_temp = get_new_temperatures(new_temperatures, calibration_values)
    if displayed_temp:
        print('New calibration temperatures found: ' + ", ".join(str(x) for x in displayed_temp))
        new_temperatures.extend(displayed_temp)
    print('Time left: ' + str(time_left) + ' min')


def generate_file(ctx, calibration_values, max_temperature, timeout, nb_values, file, append):
    print("'CTRL + C' to stop the calibration data extraction process")
    create_file(calibration_values, file, append)
    start_time = time.time()
    time_left = 1
    i = 0
    new_temperatures = []
    print_msg(timeout, new_temperatures, calibration_values)
    while True:
        current_time = time.time()
        if (current_time - start_time) // 60 >= timeout:
            break

        if (current_time - start_time) / 60 >= time_left:
            print_msg(timeout - time_left, new_temperatures, calibration_values)
            time_left += 1

        ctx.calibrateADC()
        ctx.calibrateDAC()

        key, parameters = get_key_values(ctx)
        if key >= max_temperature:
            break

        calibration_values[key] = parameters
        i += 1
    write_in_file(calibration_values, nb_values, file)


def main():
    parser = argparse.ArgumentParser(prog='generate_temperature_calib_lut',
                                     formatter_class=argparse.RawDescriptionHelpFormatter,
                                     description='Generate the temperature calibration lookup table',
                                     epilog=example)

    parser.add_argument('uri',
                        action='store',
                        metavar='uri',
                        type=str,
                        help='describe the context location ')

    parser.add_argument('-t', '--temperature',
                        action='store',
                        metavar='max_temp',
                        type=int,
                        default=75,
                        help='the maximum temperature (default: 75)')

    parser.add_argument('-T', '--timeout',
                        action='store',
                        metavar='min',
                        type=int,
                        default=30,
                        help='the number of minutes after timeout occur (default: 30)')

    parser.add_argument('-v', '--values',
                        action='store',
                        metavar='nb_values',
                        type=int,
                        default=-1,
                        help='the maximum number of values to be extracted (default: unlimited)')

    parser.add_argument('-f', '--file',
                        action='store',
                        metavar='path',
                        type=str,
                        default="m2k-calib-temp-lut.ini",
                        help='file path (default: m2k-calib-temp-lut.ini)')

    parser.add_argument('-a', '--append',
                        action='store_true',
                        help='append the values to the file, otherwise override it')

    parser.set_defaults(func=parse_arguments)

    args = parser.parse_args()
    args.func(args)


def parse_arguments(args):
    try:
        if args.uri == 'auto':
            ctx = libm2k.m2kOpen()
        else:
            ctx = libm2k.m2kOpen(args.uri)
        if ctx is None:
            raise Exception('Invalid uri')
        print('Connection established')

        def signal_handler(sig, frame):
            nonlocal calibration_values
            write_in_file(calibration_values, args.values, args.file)
            libm2k.contextClose(ctx)
            sys.exit(0)
        signal.signal(signal.SIGINT, signal_handler)

        calibration_values = dict()
        generate_file(ctx, calibration_values, args.temperature, args.timeout, args.values, args.file, args.append)

        libm2k.contextClose(ctx)
        return 0
    except Exception as error:
        print(error)
        return -1


if __name__ == "__main__":
    main()
