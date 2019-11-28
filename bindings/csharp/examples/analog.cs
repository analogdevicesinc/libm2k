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
* For example, in the command line: mcs analog.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*
* This example assumes the following connections:
* W1 -> 1+
* W2 -> 2+
* GND -> 1-
* GND -> 2-
*
* The application will generate a sine and triangular wave on W1 and W2. The signal is fed back into the analog input
* and the voltage values are displayed on the screen
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace examples
{
    class Analog
    {
        static void Main()
        {
            M2k ctx = libm2k.m2kOpen();
            ctx.calibrateADC();
            ctx.calibrateDAC();

            M2kAnalogIn ain = ctx.getAnalogIn();
            M2kAnalogOut aout = ctx.getAnalogOut();
            M2kHardwareTrigger trig = ain.getTrigger();

            // Setup analog in
            ain.enableChannel(0, true);
            ain.enableChannel(1, true);
            ain.setSampleRate(100000);
            ain.setRange((ANALOG_IN_CHANNEL)0, -10.0, 10.0); // nu are idxchannel
            ain.setRange((ANALOG_IN_CHANNEL)1, M2K_RANGE.PLUS_MINUS_25V);
            
	    // Uncomment the following block to enable triggering
            // trig.setAnalogLevel(0, 0.5);
            // trig.setAnalogDelay(0);
            // trig.setAnalogMode(0, M2K_TRIGGER_MODE.ALWAYS);
	    // trig.setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG.CHANNEL_1);
	    // trig.setAnalogCondition(0, M2K_TRIGGER_CONDITION_ANALOG.RISING_EDGE_ANALOG);

            // setup analog output
            aout.setSampleRate(0, 750000);
            aout.setSampleRate(1, 750000);
            aout.enableChannel(0, true);
            aout.enableChannel(1, true);

            // create output buffers
            var ch1 = new VectorD();
            var ch2 = new VectorD();
            var both = new VectorVectorD();

            for (int i = 0; i < 1024; i++)
            {
                double val = 4.0;
                ch1.Add(val);
                ch2.Add(3.0);
            }

            both.Add(ch1);
            both.Add(ch2);

            aout.setCyclic(true);
            aout.push(both);

            var data = ain.getSamples(1024);

            for(int i = 0; i < 10; i++)
            {
                Console.WriteLine(data[0][i] + " ");
            }

            for (int i = 0; i < 10; i++)
            {
                Console.WriteLine(data[1][i] + " ");
            }

            aout.stop();
	    libm2k.contextClose(ctx);
        }
    }
}
