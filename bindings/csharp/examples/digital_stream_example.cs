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
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

//#define SR_DIVIDER 67
//#define SAMPLE_RATE_IN (int)(MAX_SAMPLE_RATE/SR_DIVIDER)
//#define SAMPLE_RATE_OUT  (int)(MAX_SAMPLE_RATE/(SR_DIVIDER*4))

namespace stream
{
    class Digital
    {
        public const int N_BITS = 6;
        public const int IN_NO_SAMPLES = 1000;
        public const int KERNEL_BUFFERS_COUNT = 4;
        public const long MAX_SAMPLE_RATE = 100000000;
        public const int SAMPLE_RATE_IN = 1000000;
        public const int SAMPLE_RATE_OUT = SAMPLE_RATE_IN / 4;
        public const bool SHOW_BUFFERS = false;


        public const int NUMBER_OF_BUFFERS = 100;

        static int Main()
        {
            int elementSize = Marshal.SizeOf(typeof(IntPtr));
            M2k ctx = libm2k.m2kOpen();
            if (ctx == null)
            {
                Console.WriteLine("Connection Error: No ADALM2000 device available/connected to your PC.");
                return (1);
            }

            M2kDigital dig = ctx.getDigital();
            M2kHardwareTrigger trig = dig.getTrigger();

            // set sample rates for in/out interface
            dig.setSampleRateIn(SAMPLE_RATE_IN);
            dig.setSampleRateOut(SAMPLE_RATE_OUT);
            // set number of kernel buffers for the digital input interface
            dig.setKernelBuffersCountIn(KERNEL_BUFFERS_COUNT);


            for (uint j = 0; j < N_BITS; j++)
            {
                dig.setDirection(j, DIO_DIRECTION.DIO_OUTPUT);
                dig.enableChannel(j, true);
            }


            var bufferOut = new VectorUS();
            var bufferIn = new VectorUS();
            var buffers = new VectorVectorUS();

            for (ushort j = 0; j < (1 << N_BITS); j++)
            {
                bufferOut.Add(j);
            }
            dig.setCyclic(true);
            dig.push(bufferOut);


            for (int j = 0; j < NUMBER_OF_BUFFERS; j++)
            {
                // always get same number of samples so buffer configuration does not reset
                //IntPtr val = dig.getSamplesP(IN_NO_SAMPLES);
                //for (int k=0; k < IN_NO_SAMPLES; k++)
                //{
                 //   Console.WriteLine(Marshal.ReadIntPtr(val, k * elementSize));
                //}

                
                //buffers.Add(new VectorUS(val,val+IN_NO_SAMPLES));
                buffers.Add(dig.getSamples(IN_NO_SAMPLES));
            }

            int i = 0;
            if (SHOW_BUFFERS)
            {
                foreach (var buf in buffers)
                {
                    foreach (var val in buf)
                    {
                        Console.WriteLine((val));
                    }
                    Console.WriteLine("-------- BUFFER " + i + "--------");
                    i++;
                }
            }

            bool stable = true;
            var values = new VectorUS();
            uint same_val = 0;
            uint same_val_cnt = 0;
            

            foreach (var buf in buffers) {
                foreach (var val in buf)
                {
                    values.Add(val);
                }
            }

            for (i = 1; i < values.Count; i++)
            {
                // find first transition
                if (values[i] != values[i - 1])
                {
                    same_val = values[i];
                    break;
                }
            }
            
            for (; i < values.Count; i++)
            {
                if (values[i] == same_val)
                    same_val_cnt++;
                else
                {
                    if (same_val_cnt == (int)(SAMPLE_RATE_IN / SAMPLE_RATE_OUT))
                    {
                        {
                            same_val = values[i];
                            same_val_cnt = 1;
                        }

                    }
                    else
                    {

                        stable = false;
                        break;
                    }
                }
            }
            Console.WriteLine(((stable) ? "STABLE " : "UNSTABLE ") + i);

            dig.stopBufferOut();
            dig.flushBufferIn();
            libm2k.contextClose(ctx);


            var exit = Console.ReadLine();
            return(0);
        }
    }
}