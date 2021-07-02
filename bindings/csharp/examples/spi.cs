/*
* In order to build the example, add libm2k-sharp.dll as a reference to the project.
* For example, in the command line: mcs analog.cs -reference:libm2k-sharp.dll
* Please make sure that libm2k-sharp.dll and libm2k-sharp-cxx-wrap.dll are in your build folder.
* Please make sure that the location of libm2k is in the PATH.
*
* Please check Electronics Lab 14 on our wiki page: https://wiki.analog.com/university/courses/electronics/electronics-lab-14
* Configure the connections for 'Unipolar output operation' mode
*/

using System;
using System.Linq;

namespace examples
{
    class Spi
    {
        static double Voltage = 2.5;

        static byte[] GetRegisterData(double voltage)
        {
            byte[] data = new byte[2];
            if (voltage < 0)
            {
                data[0] = 0;
                data[1] = 0;
                return data;
            }

            if (voltage > 4.095)
            {
                data[0] = 0x0F;
                data[1] = 0xFF;
                return data;
            }

            var registerValue = (ushort) (voltage * 1000);
            data[0] = (byte) ((registerValue & 0xFF00) >> 8);
            data[1] = (byte) (registerValue & 0xFF);
            return data;
        }

        static void Main()
        {
            M2k context = libm2k.m2kOpen("ip:192.168.2.1");
            if (context == null)
            {
                Console.WriteLine("Connection Error: No ADALM2000 device available/connected to your PC.");
                return;
            }

            context.calibrateADC();

            M2kAnalogIn analogIn = context.getAnalogIn();
            M2kPowerSupply powerSupply = context.getPowerSupply();
            M2kDigital digital = context.getDigital();

            //setup analog in
            analogIn.setOversamplingRatio(1);
            analogIn.setSampleRate(1000000);
            analogIn.enableChannel(0, true);
            analogIn.enableChannel(1, false);
            analogIn.setRange((ANALOG_IN_CHANNEL) 0, M2K_RANGE.PLUS_MINUS_25V);

            //enable LDAC
            digital.setDirection(3, DIO_DIRECTION.DIO_OUTPUT);
            digital.enableChannel(3, true);

            //enable CLR
            digital.setDirection(4, DIO_DIRECTION.DIO_OUTPUT);
            digital.enableChannel(4, true);

            //setup SPI
            var m2kSpiInit = new m2k_spi_init();
            m2kSpiInit.clock = 1;
            m2kSpiInit.mosi = 2;
            m2kSpiInit.miso = 7; //dummy value - miso is not used in this example
            m2kSpiInit.bit_numbering = bit_numbering.MSB;
            m2kSpiInit.cs_polarity = cs_polarity.ACTIVE_LOW;
            m2kSpiInit.context = context;

            var spiInitParam = new spi_init_param();
            spiInitParam.max_speed_hz = 1000000;
            spiInitParam.mode = spi_mode.SPI_MODE_3;
            spiInitParam.chip_select = 0;
            spiInitParam.extra = m2kSpiInit;

            spi_desc spiDesc = libm2k.spi_init(spiInitParam);
            if (spiDesc == null)
            {
                Console.WriteLine("SPI Error: Could not configure SPI");
                return;
            }

            powerSupply.enableChannel(0, true);
            powerSupply.pushChannel(0, 5);

            //CLR and LDAC high as long as bits are transmitted
            digital.setValueRaw(3, DIO_LEVEL.HIGH);
            digital.setValueRaw(4, DIO_LEVEL.HIGH);

            byte[] data = GetRegisterData(Voltage);
            //transmitting data
            libm2k.spi_write_and_read(spiDesc, data, 2);

            //update with current shift register contents
            digital.setValueRaw(3, DIO_LEVEL.LOW);

            var samples = analogIn.getSamples(100);
            var average = samples[0].Average();
            Console.WriteLine("Average value: " + average);

            libm2k.spi_remove(spiDesc);
            libm2k.contextClose(context, true);
        }
    }
}
