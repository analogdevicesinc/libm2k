/*
* In order to build the example, add libm2k-sharp.dll as a reference to the project.
* For example, in the command line: mcs digital.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace examples
{
    class Digital
    {
        static void Main()
        {
            int N_BITS = 4;
            M2k ctx = libm2k.m2kOpen();
            M2kDigital dig = ctx.getDigital();

            dig.setSampleRateIn(100000);
            dig.setSampleRateOut(100000);

            for (uint i = 0; i < N_BITS; i++)
            {
                dig.setDirection(i, DIO_DIRECTION.DIO_OUTPUT);
                dig.enableChannel(i, true);
            }

            VectorUS bufferOut = new VectorUS();
            VectorUS bufferIn = new VectorUS();
            
            for (ushort i = 0; i < (1 << N_BITS); i++)
            {
                bufferOut.Add(i);
            }
            dig.setCyclic(true);
            dig.push(bufferOut);

            bufferIn = dig.getSamples(1000);
            foreach (var val in bufferIn)
            {
                Console.Write(val + " \n");
            }
            dig.stop();
            libm2k.deviceClose(ctx);
        }
    }
}