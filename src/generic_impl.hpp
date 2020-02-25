/*
 * Copyright (c) 2019 Analog Devices Inc.
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

#ifndef GENERIC_IMPL_H
#define GENERIC_IMPL_H

#include <map>
#include <vector>
#include <list>
#include <libm2k/generic.hpp>
#include "utils/devicein.hpp"
#include "utils/deviceout.hpp"
#include "context_impl.hpp"

using namespace std;

namespace libm2k {
namespace contexts {
class GenericImpl : public Generic, public ContextImpl {
public:
	GenericImpl(std::string uri, iio_context* ctx, std::string name, bool sync);
	virtual ~GenericImpl();

	libm2k::analog::GenericAnalogIn* getAnalogIn(unsigned int);
	libm2k::analog::GenericAnalogIn* getAnalogIn(std::string);
	libm2k::analog::GenericAnalogOut* getAnalogOut(unsigned int);
	libm2k::analog::GenericAnalogOut* getAnalogOut(std::string);

	unsigned int getAnalogInCount();
	unsigned int getAnalogOutCount();

private:
	std::vector<analog::GenericAnalogOut*> m_instancesAnalogOut;
	std::vector<analog::GenericAnalogIn*> m_instancesAnalogIn;

	std::vector<std::string> scanAllAnalogIn();
	std::vector<std::string> scanAllAnalogOut();
};
}
}

#endif
