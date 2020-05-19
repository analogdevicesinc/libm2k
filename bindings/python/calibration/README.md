# ADALM2000 calibrated using predefined values

## Generating your own calibration file
A calibration file is a file used at boot time to write inside the
context some predefined values for the calibration process. The values
will be stored until they will be overwritten.

This file must be called 'm2k-calib-temp-lut.ini' and it should contain
only a row having the following format:

cal,temp_lut=[\<temperature>,<adc_offset1>,<adc_offset2>,<adc_gain1> <adc_gain2>,
<dac_offset1>,<dac_offset2>,<dac_gain1>,<dac_gain2> [...]]

### Command
    generate_temperature_calib_lut <uri> [-h] [-t max_temp] [-T min] [-v nb_values] [-f path] [-a]

### Examples
Create the ini file 'm2k-calib-temp-lut.ini'. The process can be stopped by
pressing 'CTRL + C', otherwise the process will stop when the temperature
of the board rises up to 75 °C or after 30 minutes

    python3 generate_temperature_calib_lut.py ip:192.168.2.1

Create the ini file 'm2k-calib-temp-lut.ini'. The process can be stopped by
pressing 'CTRL + C' or when the temperature of the board rises up to 54 °C

    python3 generate_temperature_calib_lut.py auto -t 54

Create the ini file 'm2k-calib-temp-lut.ini'. Extract 5 values from all
computed calibration parameters.

    python3 generate_temperature_calib_lut.py auto -v 5 -t 54

## Calibrating using ini file
The purpose of this calibration type is to automate and to make the calibration
process more efficient. Please make sure to have the following minimum requirements:
* A valid calibration file. The file can be generated using the script mentioned above.
* Firmware v0.26 on your board (https://github.com/analogdevicesinc/m2k-fw/releases/tag/v0.26).
* libm2k v0.3.1

After the board booted, copy the ini file inside the m2k drive. Then eject the drive and wait
for ADALM2000 to boot. Once booted, the temperature calibration lookup table will be usable from libm2k.

In order to perform a fast calibration call
the fallowing context method: calibrateFromContext()

For more information, please visit out wiki page: https://wiki.analog.com/university/tools/m2k/libm2k/calibration
