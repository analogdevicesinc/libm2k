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
using System.Threading;

namespace stream
{
    class Digital
    {
        public const int N_BITS = 16;
        public const uint IN_NO_SAMPLES = (1 << 16);
        public const int KERNEL_BUFFERS_COUNT = 64;
        public const long MAX_SAMPLE_RATE = 100000000;
        public const int SR_DIVIDER_STEP = 1;
        public const int SR_DIVIDER_START = 30;
        public const bool SHOW_BUFFERS = false;
        public const int NUMBER_OF_BUFFERS = 100;

        public const bool running = true;

        private static ManualResetEvent refill_reset_event = new ManualResetEvent(false);
        private static ManualResetEvent process_done_reset_event = new ManualResetEvent(false);

        private static VectorUS tmp_buffer = new VectorUS();
        private static uint[] values = new uint[IN_NO_SAMPLES * NUMBER_OF_BUFFERS];
        public static M2kDigital dig;


        public static void RefillThread()
        {
            for (int refills = 0; refills < NUMBER_OF_BUFFERS; refills++)
            {
                while (tmp_buffer.Count != 0)
                {
                    refill_reset_event.WaitOne();
                }

                if (tmp_buffer.Count == 0)
                {
                    tmp_buffer = dig.getSamples(IN_NO_SAMPLES);
                }
                refill_reset_event.Set();
                refill_reset_event.Reset();
            }
        }


        public static void ProcessThread()
        {
            for (int refills = 0; refills < NUMBER_OF_BUFFERS; refills++)
            {
                while (tmp_buffer.Count == 0)
                {
                    refill_reset_event.WaitOne();
                }

                if (tmp_buffer.Count > 0)
                {
                    for (int i = 0; i < IN_NO_SAMPLES; i++)
                    {
                        values[refills * IN_NO_SAMPLES + i] = tmp_buffer[i];
                    }
                    tmp_buffer.Clear();
                }
                refill_reset_event.Set();
                refill_reset_event.Reset();
            }
            process_done_reset_event.Set();
        }

        static int Main()
        {
            int elementSize = Marshal.SizeOf(typeof(IntPtr));
            M2k ctx = libm2k.m2kOpen();
            if (ctx == null)
            {
                Console.WriteLine("Connection Error: No ADALM2000 device available/connected to your PC.");
                return (1);
            }

            dig = ctx.getDigital();
            M2kHardwareTrigger trig = dig.getTrigger();

            long sr_divider = SR_DIVIDER_START;
            long sample_rate_in, sample_rate_out = MAX_SAMPLE_RATE;
            int k = 1;

            while (sr_divider > 1)
            {
                sample_rate_in = MAX_SAMPLE_RATE / sr_divider;
                sample_rate_out = MAX_SAMPLE_RATE / (sr_divider * 4);

                // set sample rates for in/out interface
                dig.setSampleRateIn(sample_rate_in + 1);
                dig.setSampleRateOut(sample_rate_out + 1);

                // set number of kernel buffers for the digital input interface
                dig.setKernelBuffersCountIn(KERNEL_BUFFERS_COUNT);
                trig.setDigitalStreamingFlag(true);


                for (uint j = 0; j < N_BITS; j++)
                {
                    dig.setDirection(j, DIO_DIRECTION.DIO_OUTPUT);
                    dig.enableChannel(j, true);
                }

                var bufferOut = new VectorUS();
                for (uint j = 0; j < (1 << N_BITS); j++)
                {
                    bufferOut.Add((ushort)j);
                }
                dig.setCyclic(true);
                dig.push(bufferOut);


                // Startup refill threads
                Thread refill_thread = new Thread(new ThreadStart(RefillThread));
                Thread process_thread = new Thread(new ThreadStart(ProcessThread));

                refill_thread.Start();
                process_thread.Start();

                refill_reset_event.Set();

                process_done_reset_event.WaitOne();

                refill_thread.Join();
                process_thread.Join();



                if (SHOW_BUFFERS)
                {
                    Console.WriteLine("===================== BUFFER =================== " + k);
                    foreach (var val in values)
                    {
                        Console.WriteLine((val));
                    }
                }

                bool stable = true;
                uint same_val = 0;
                uint same_val_cnt = 0;
                long dropped = 0;
                int i;

                for (i = 1; i < values.Length; i++)
                {
                    // find first transition
                    if (values[i] != values[i - 1])
                    {
                        same_val = values[i];
                        break;
                    }
                }

                for (; i < values.Length; i++)
                {
                    if (values[i] == same_val)
                        same_val_cnt++;
                    else
                    {
                        int divider = (int)(sample_rate_in / sample_rate_out);
                        if (same_val_cnt == divider)
                        {
                            {
                                same_val = values[i];
                                same_val_cnt = 1;
                            }

                        }
                        else
                        {
                            dropped = Math.Abs(values[i] - same_val) * divider + (same_val_cnt - divider);
                            stable = false;
                            break;
                        }
                    }
                }
                Console.Write("SR_DIVIDER: " + sr_divider +
                                " SR_IN: " + sample_rate_in +
                                " SR_OUT: " + sample_rate_out + " , " +
                                ((stable) ? "STABLE " : "UNSTABLE ") +
                                 " dropped: " + dropped + " samples");

                if (!stable)
                {
                    Console.WriteLine(" @ buffer " + i / IN_NO_SAMPLES +
                                    " prev val: " + (same_val) +
                                    " next val:  " + (values[i]));
                }
                Console.Write("\n");

                sr_divider -= SR_DIVIDER_STEP;


                dig.stopBufferOut();
                dig.stopAcquisition();
                k++;
            }

            libm2k.contextClose(ctx);
            var exit = Console.ReadLine();
            return (0);
        }
    }
}
