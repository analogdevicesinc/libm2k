/*
 * Copyright 2016 Analog Devices, Inc.
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

#include "private/device_impl.cpp"

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::devices;

/*
 * Represents an iio_device
 */
Device::Device(struct iio_context* context, std::string dev_name, bool input) :
	m_pimpl(std::unique_ptr<DeviceImpl>(new DeviceImpl(context, dev_name, input)))
{
}

Device::Device(Device::DeviceImpl *impl)
{
	m_pimpl = std::unique_ptr<DeviceImpl>(impl);
}

Device::~Device()
{
	m_pimpl.reset();
}

Channel* Device::getChannel(unsigned int chnIdx)
{
	return m_pimpl->getChannel(chnIdx);
}

Channel* Device::getChannel(std::string id)
{
	return m_pimpl->getChannel(id);
}

bool Device::isChannel(unsigned int chnIdx, bool output)
{
	return m_pimpl->isChannel(chnIdx, output);
}

void Device::enableChannel(unsigned int chnIdx, bool enable)
{
	m_pimpl->enableChannel(chnIdx, enable);
}

bool Device::isChannelEnabled(unsigned int chnIdx)
{
	return m_pimpl->isChannelEnabled(chnIdx);
}


void Device::push(std::vector<short> &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, cyclic, multiplex);
}

void Device::push(std::vector<double> &data, unsigned int channel, bool cyclic)
{
	m_pimpl->push(data, channel, cyclic);
}

void Device::stop()
{
	m_pimpl->stop();
}

std::vector<unsigned short> Device::getSamples(int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

std::vector<std::vector<double> > Device::getSamples(int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamples(nb_samples, process);
}

string Device::getName()
{
	return m_pimpl->getName();
}

double Device::getDoubleValue(std::string attr)
{
	return m_pimpl->getDoubleValue(attr);
}

double Device::getDoubleValue(unsigned int chn_idx, std::string attr, bool output)
{
	return m_pimpl->getDoubleValue(chn_idx, attr, output);
}

double Device::setDoubleValue(double value, std::string attr)
{
	return m_pimpl->setDoubleValue(value, attr);
}

double Device::setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output)
{
	return m_pimpl->setDoubleValue(chn_idx, value, attr, output);
}

bool Device::getBoolValue(string attr)
{
	return m_pimpl->getBoolValue(attr);
}

bool Device::getBoolValue(unsigned int chn_idx, string attr, bool output)
{
	return m_pimpl->getBoolValue(chn_idx, attr, output);
}

bool Device::setBoolValue(bool value, string attr)
{
	return m_pimpl->setBoolValue(value, attr);
}

bool Device::setBoolValue(unsigned int chn_idx, bool value, string attr, bool output)
{
	return m_pimpl->setBoolValue(chn_idx, value, attr, output);
}

string Device::setStringValue(string attr, string value)
{
	return m_pimpl->setStringValue(attr, value);
}

string Device::setStringValue(unsigned int chn_idx, string attr, string value, bool output)
{
	return m_pimpl->setStringValue(chn_idx, attr, value, output);
}

string Device::getStringValue(string attr)
{
	return m_pimpl->getStringValue(attr);
}

string Device::getStringValue(unsigned int chn_idx, string attr, bool output)
{
	return m_pimpl->getStringValue(chn_idx, attr, output);
}

std::vector<double> Device::getAvailableSamplerates()
{
	return m_pimpl->getAvailableSamplerates();
}

void Device::writeRegister(uint32_t address, uint32_t value)
{
	m_pimpl->writeRegister(address, value);
}

std::string Device::getHardwareRevision()
{
	return m_pimpl->getHardwareRevision();
}

unsigned int Device::getNbChannels()
{
	return m_pimpl->getNbChannels();
}

void Device::convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src)
{
	m_pimpl->convertChannelHostFormat(chn_idx, avg, src);
}

void Device::convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
{
	m_pimpl->convertChannelHostFormat(chn_idx, avg, src);
}

void Device::setKernelBuffersCount(unsigned int count)
{
	m_pimpl->setKernelBuffersCount(count);
}

bool Device::isValidDmmChannel(unsigned int chnIdx)
{
	return m_pimpl->isValidDmmChannel(chnIdx);
}

std::pair<string, string> Device::getContextAttr(unsigned int attrIdx)
{
	return m_pimpl->getContextAttr(attrIdx);
}
