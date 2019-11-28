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

#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/digital/genericdigital.hpp>
#include <libm2k/m2kexceptions.hpp>

#include <libm2k/utils/utils.hpp>
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;

class GenericDigital::GenericDigitalImpl : public DeviceGeneric {
public:
	GenericDigitalImpl(iio_context *ctx, string logic_dev) :
		DeviceGeneric(ctx, logic_dev)
	{
		m_dev_name = logic_dev;

		for (unsigned int i = 0; i < getNbChannels(false); i++) {
			std::string name = "voltage" + std::to_string(i);
			Channel* channel = getChannel(name, false);
			if (channel) {
				m_channel_list.push_back(channel);
			}
		}
	}

	~GenericDigitalImpl()
	{
		m_channel_list.clear();
	}

	double getSampleRate()
	{
		return getDoubleValue("sampling_frequency");
	}

	double setSampleRate(double sampleRate)
	{
		return setDoubleValue(sampleRate,
					     "sampling_frequency");
	}

	void setCyclic(bool cyclic)
	{
		m_cyclic = cyclic;
		DeviceGeneric::setCyclic(cyclic);
	}

	bool getCyclic()
	{
		return m_cyclic;
	}

	void enableChannel(unsigned int index, bool enable)
	{
		if (index < getNbChannels(false)) {
			DeviceGeneric::enableChannel(index, enable, false);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "Cannot enable digital channel.");
		}
	}

private:
	std::string m_dev_name;
	std::vector<Channel*> m_channel_list;
	bool m_cyclic;

};
