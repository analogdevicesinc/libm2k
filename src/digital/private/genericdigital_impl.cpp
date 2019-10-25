/*
 * Copyright 2018 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			std::string name = "voltage" + std::to_string(i);
			Channel* channel = getChannel(name);
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
		if (index < getNbChannels()) {
			enableChannel(index, enable);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "Cannot enable digital channel.", __FILE__, __LINE__);
		}
	}

private:
	std::string m_dev_name;
	std::vector<Channel*> m_channel_list;
	bool m_cyclic;

};
