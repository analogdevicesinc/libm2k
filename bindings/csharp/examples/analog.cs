/*
* In order to build the example, add libm2k-sharp.dll as a reference to the project.
* For example, in the command line: mcs analog.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
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

            // setup analog trigger
            trig.setAnalogSource(M2K_TRIGGER_SOURCE.CHANNEL_1);
            trig.setAnalogCondition(0, M2K_TRIGGER_CONDITION.RISING_EDGE);
            trig.setAnalogLevel(0, 0.5);
            trig.setAnalogDelay(0);
            trig.setAnalogMode(0, M2K_TRIGGER_MODE.ALWAYS);

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
            libm2k.deviceClose(ctx);
        }
    }
}