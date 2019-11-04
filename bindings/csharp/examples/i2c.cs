/*
* In order to build the example, add libm2k-sharp.dll as a reference to the project.
* For example, in the command line: mcs analog.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*
* This example uses EVAL-ADT7420-PMDZ as a slave
* Hardware configuration:
* 	(ADALM2000) DIO_0 <---> Pin 1 (ADT7420) <--->  10 kilohms resistor <--- V+ (ADALM2000)
* 	(ADALM2000) DIO_1 <---> Pin 3 (ADT7420) <---> 10 kilohms resistor <--- V+ (ADALM2000)
* 	(ADALM2000) GND <---> Pin 5 (ADT7420)
* 	(ADALM2000) V+ ---> Pin 7 (ADT7420)
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
​
namespace examples
{
    class I2c
    {
        static float ConvertTemperature(byte[] data)
        {
            byte msb_temp = data[0];
            byte lsb_temp = data[1];
            UInt16 temp = 0;
            float temperature = 0;
​
            temp = (UInt16)((msb_temp << 8) + lsb_temp);
            if ((temp & 0x8000) != 0)
            {
                /*! Negative temperature */
                temperature = (float)((UInt32)temp - 65536) / 128;
            }
            else
            {
                /*! Positive temperature */
                temperature = (float)temp / 128;
            }
            return temperature;
        }
​
        static void Main()
        {
            M2k context = libm2k.m2kOpen("ip:192.168.2.1");
            if (context == null)
            {
                Console.WriteLine("Connection Error: No ADALM2000 device available/connected to your PC.");
                return;
            }
            Console.WriteLine("Calibrating ADC . . .");
            context.calibrateADC();
​
            M2kPowerSupply powerSupply = context.getPowerSupply();
​
            powerSupply.enableChannel(0, true);
            powerSupply.pushChannel(0, 3.3);
​
            var m2KI2CInit = new m2k_i2c_init();
            m2KI2CInit.scl = 0;
            m2KI2CInit.sda = 1;
            m2KI2CInit.context = context;

            var i2cInitParam = new i2c_init_param();
            i2cInitParam.max_speed_hz = 100000;
            i2cInitParam.slave_address = 0x48;
            i2cInitParam.extra = m2KI2CInit;
​
            var i2cDesc = libm2k.i2c_init(i2cInitParam);
            if (i2cDesc == null)
            {
                Console.WriteLine("I2C Error: Could not configure I2C");
                return;
            }
​
            Console.WriteLine("Initiating I2C transfer . . .");
            byte[] dataWriteConfig = { 0x0B };
            libm2k.i2c_write(i2cDesc, dataWriteConfig, 1, i2c_transfer_mode.i2c_general_call | i2c_transfer_mode.i2c_repeated_start)
​
            byte[] dataReadConfig = { 0 };
            libm2k.i2c_read(i2cDesc, dataReadConfig, 1, i2c_transfer_mode.i2c_general_call)
​
            Console.WriteLine("Reading the temperature . . .");
            byte[] dataWriteTemperature = { 0 };
            libm2k.i2c_write(i2cDesc, dataWriteTemperature, 1, i2c_transfer_mode.i2c_general_call | i2c_transfer_mode.i2c_repeated_start)
            byte[] dataReadTemperature = { 0, 0 };
            libm2k.i2c_read(i2cDesc, dataReadTemperature, 2, i2c_transfer_mode.i2c_general_call)
​
            float temperature = ConvertTemperature(dataReadTemperature);
            Console.WriteLine("Temperature: " + temperature + "°C");
​
            libm2k.i2c_remove(i2cDesc);
            libm2k.contextClose(context);
        }
    }
}
