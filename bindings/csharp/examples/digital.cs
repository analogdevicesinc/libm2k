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
* For example, in the command line: mcs digital.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*
* This example will generate a binary counter on the first N_BITS of the
* digital interface and read them back - no additional connection required
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
	    libm2k.contextClose(ctx);
        }
    }
}
