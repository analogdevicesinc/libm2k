// Copyright (c) 2019 Analog Devices Inc.
//
// This file is part of libm2k
// (see http://www.github.com/analogdevicesinc/libm2k).
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//

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
