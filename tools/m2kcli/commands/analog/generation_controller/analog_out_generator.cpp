/*
 * Copyright (c) 2019 Analog Devices Inc.
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

#include "analog_out_generator.h"

AnalogOutGenerator::AnalogOutGenerator(libm2k::analog::M2kAnalogOut *analogOut, unsigned int bufferSize,
				       std::vector<unsigned int> &channels, bool cyclic)
{
	this->analogOut = analogOut;
	this->bufferSize = bufferSize;
	this->channels = channels;
	this->cyclic = cyclic;
}
