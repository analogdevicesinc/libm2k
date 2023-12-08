/*
 * Copyright (c) 2023 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software: you can redistribute it and/or modify
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
// This example assumes the following connections:
// W1 -> 1+
// W2 -> 2+
// GND -> 1-
// GND -> 2-
//
// The application is a demo that will output a voltage on W1 and W2. The signal is fed back into the analog input.
// The voltage can be set either as raw value or as voltage value. The voltage values are displayed on the screen.

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

void usingSetVoltage(M2kAnalogIn *, M2kAnalogOut *);
void usingSetVoltageRaw(M2kAnalogIn *, M2kAnalogOut *);

int main(int argc, char *argv[])
{
    M2k *ctx = m2kOpen("ip:192.168.2.1");
    if (!ctx)
    {
        std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
        return 1;
    }

    // Get the analog input and output interfaces and calibrate
    M2kAnalogIn *ain = ctx->getAnalogIn();
    M2kAnalogOut *aout = ctx->getAnalogOut();
    ain->reset();
    aout->reset();
    ctx->calibrate();

    usingSetVoltage(ain, aout);
    usingSetVoltageRaw(ain, aout);

    contextClose(ctx);
    return 0;
}

void usingSetVoltage(M2kAnalogIn *ain, M2kAnalogOut *aout)
{
    auto out_voltage_0 = 2.5;
    auto out_voltage_1 = -2.5;
    auto out_raw_0 = aout->setVoltage(0, out_voltage_0);
    auto out_raw_1 = aout->setVoltage(1, out_voltage_1);
    std::cout << "Example using setVoltage:" << std::endl;
    std::cout << "\tSetting VOLTAGE at channel 0: " << out_voltage_0 << " [V] = " << out_raw_0 << " [raw] " << std::endl;
    std::cout << "\tSetting VOLTAGE at channel 1: " << out_voltage_1 << " [V] = " << out_raw_1 << " [raw] " << std::endl;

    auto in_voltage_0 = ain->getVoltage(0);
    auto in_voltage_1 = ain->getVoltage(1);
    std::cout << "\tRedback value at channel 0: " << in_voltage_0 << " [V]" << std::endl;
    std::cout << "\tRedback value at channel 1: " << in_voltage_1 << " [V]" << std::endl;
}

void usingSetVoltageRaw(M2kAnalogIn *ain, M2kAnalogOut *aout)
{
    auto out_raw_0 = aout->setVoltageRaw(0, 49000);
    auto out_raw_1 = aout->setVoltageRaw(1, 16000);
    std::cout << "Example using setVoltageRaw:" << std::endl;
    std::cout << "\tSetting RAW at channel 0: " << out_raw_0 << " [raw]" << std::endl;
    std::cout << "\tSetting RAW at channel 1: " << out_raw_1 << " [raw]" << std::endl;

    auto in_voltage_0 = ain->getVoltage(0);
    auto in_voltage_1 = ain->getVoltage(1);
    std::cout << "\tRedback value at channel 0: " << in_voltage_0 << " [V]" << std::endl;
    std::cout << "\tRedback value at channel 1: " << in_voltage_1 << " [V]" << std::endl;
}
