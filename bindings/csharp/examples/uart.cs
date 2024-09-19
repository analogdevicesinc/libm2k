/*
 * Copyright (c) 2024 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
* Please check our wiki page: https://wiki.analog.com/university/courses/electronics/m2k-uart-debug
*/

using System;
using System.Text;
using System.Threading;

namespace examples
{
    class Uart
    {
        public static void Main()
        {
            M2k context = libm2k.m2kOpen("ip:192.168.2.1");
            if (context == null)
            {
                Console.WriteLine("Connection Error: No ADALM2000 device available/connected to your PC.");
                return;
            }

            var m2kUartInit = new m2k_uart_init();
            m2kUartInit.bits_number = 8;
            m2kUartInit.parity = uart_parity.NO_PARITY;
            m2kUartInit.stop_bits = uart_stop_bits.ONE;
            m2kUartInit.context = context;

            var uartInitParam = new uart_init_param();
            uartInitParam.device_id = 0;
            uartInitParam.baud_rate = 9600;
            uartInitParam.extra = m2kUartInit;

            var uartDescWrite = libm2k.uart_init(uartInitParam);

            uartInitParam.device_id = 1;
            var uartDescRead = libm2k.uart_init(uartInitParam);

            byte[] dataRead = new byte[3];
            var threadRead = new Thread(() =>
            {
                Thread.CurrentThread.IsBackground = true;
                lock (dataRead)
                {
                    libm2k.uart_read(uartDescRead, dataRead, (byte)dataRead.Length);
                }
            });
            threadRead.Start();

            byte[] dataWrite = Encoding.ASCII.GetBytes("ADI");
            Thread.Sleep(100);
            libm2k.uart_write(uartDescWrite, dataWrite, (byte) dataWrite.Length);
            threadRead.Join();

            foreach (var c in dataRead)
            {
                Console.Write((char)c);
            }
            Console.WriteLine();

            libm2k.uart_remove(uartDescWrite);
            libm2k.uart_remove(uartDescRead);
            libm2k.contextClose(context);
        }
    }
}
