/*
* In order to build the example, add libm2k-sharp.dll as a reference to the project.
* For example, in the command line: mcs powersupply.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*
* This exammple assumes connection:
* 1+ to V+
* 1- to GND
* This example turns on the power supply and sets it to 1.7V and then reads back the value
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace examples
{
    class PowerSupply
    {
        static void Main()
        {
            M2k ctx = libm2k.m2kOpen();
            ctx.calibrateADC();

            // Will turn on the power supply if we need smth to measure
            M2kPowerSupply ps = ctx.getPowerSupply();
            ps.enableChannel(0, true);
            ps.pushChannel(0, 1.7);

            // Setup analog in
            M2kAnalogIn ain = ctx.getAnalogIn();
            ain.enableChannel(0, true);

            double voltage = ain.getVoltage(ANALOG_IN_CHANNEL.ANALOG_IN_CHANNEL_1);
            Console.WriteLine(voltage);
	    libm2k.contextClose(ctx);
        }
    }
}
