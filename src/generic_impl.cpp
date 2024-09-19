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

#include "generic_impl.hpp"
#include <vector>
#include <libm2k/utils/utils.hpp>
#include "utils/channel.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "analog/generic/genericanalogin_impl.hpp"
#include "analog/generic/genericanalogout_impl.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::context;
using namespace libm2k::utils;

GenericImpl::GenericImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync)
{
	/* Initialize the AnalogIn list */
	auto aIn_lst = scanAllAnalogIn();
	for (auto aIn : aIn_lst) {
		m_instancesAnalogIn.push_back(new GenericAnalogInImpl(ctx, aIn));
	}

	/* Initialize the AnalogIn list */
	auto aOut_lst = scanAllAnalogOut();
	for (auto aOut : aOut_lst) {
		m_instancesAnalogOut.push_back(new GenericAnalogOutImpl(ctx, aOut));
	}
}

GenericImpl::~GenericImpl()
{
	for (auto ain : m_instancesAnalogIn) {
		delete ain;
	}
	for (auto aout : m_instancesAnalogOut) {
		delete aout;
	}
	m_instancesAnalogIn.clear();
	m_instancesAnalogOut.clear();
}

std::vector<std::string> GenericImpl::scanAllAnalogIn()
{
	auto dev_list = getAllDevices();
	std::vector<std::string> aIn_lst = {};
	std::exception e;
	for (auto dev : dev_list) {
		if (isIioDeviceBufferCapable(dev) &&
				(getIioDeviceType(dev) == ANALOG_DEV) &&
				getIioDeviceDirection(dev) == INPUT) {
			aIn_lst.push_back(dev);
		}
	}
	std::sort(aIn_lst.begin(), aIn_lst.end(), [](const std::string& ldev, std::string& rdev)
	{
		return Utils::compareNatural(ldev, rdev);
	});

	return aIn_lst;
}

std::vector<std::string> GenericImpl::scanAllAnalogOut()
{
	auto dev_list = getAllDevices();
	std::vector<std::string> aOut_lst = {};
	for (auto dev : dev_list) {
		if (isIioDeviceBufferCapable(dev) &&
				(getIioDeviceType(dev) == ANALOG_DEV) &&
				getIioDeviceDirection(dev) == OUTPUT) {
			aOut_lst.push_back(dev);
		}
	}
	std::sort(aOut_lst.begin(), aOut_lst.end(), [](const std::string& ldev, std::string& rdev)
	{
		return Utils::compareNatural(ldev, rdev);
	});

	return aOut_lst;
}

unsigned int GenericImpl::getAnalogInCount()
{
	return m_instancesAnalogIn.size();
}

GenericAnalogIn* GenericImpl::getAnalogIn(unsigned int index)
{
	if (index < m_instancesAnalogIn.size()) {
		return m_instancesAnalogIn.at(index);
	} else {
		return nullptr;
	}
}

GenericAnalogIn* GenericImpl::getAnalogIn(std::string dev_name)
{
	for (GenericAnalogIn* d : m_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

unsigned int GenericImpl::getAnalogOutCount()
{
	return m_instancesAnalogOut.size();
}

GenericAnalogOut* GenericImpl::getAnalogOut(unsigned int index)
{
	if (index < m_instancesAnalogOut.size()) {
		return m_instancesAnalogOut.at(index);
	} else {
		return nullptr;
	}
}

GenericAnalogOut* GenericImpl::getAnalogOut(std::string dev_name)
{
	for (GenericAnalogOut* d : m_instancesAnalogOut) {
		if (d->getName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

