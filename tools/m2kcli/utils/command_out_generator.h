/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * */

#ifndef LIBM2K_COMMAND_OUT_GENERATOR_H
#define LIBM2K_COMMAND_OUT_GENERATOR_H


class CommandOutGenerator {
public:
	virtual void generate(bool &keepReading) = 0;

	virtual ~CommandOutGenerator() = default;

protected:
	virtual void getSamples(bool &keepReading) = 0;
};

#endif //LIBM2K_COMMAND_OUT_GENERATOR_H
