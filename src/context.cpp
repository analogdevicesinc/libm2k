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

#include "private/context_impl.cpp"

using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::devices;
using namespace libm2k::utils;

Context::Context(std::string uri, struct iio_context *ctx, std::string name) :
	m_pimpl(std::shared_ptr<ContextImpl>(new ContextImpl(uri, ctx, name)))
{
}

Context::Context(ContextImpl* impl)
{
	m_pimpl = std::shared_ptr<ContextImpl>(impl);
}

Context::~Context()
{
}

GenericAnalogIn* Context::getAnalogIn(unsigned int index)
{
	return m_pimpl->getAnalogIn(index);
}

GenericAnalogIn* Context::getAnalogIn(std::string dev_name)
{
	return m_pimpl->getAnalogIn(dev_name);
}

GenericAnalogOut* Context::getAnalogOut(unsigned int index)
{
	return m_pimpl->getAnalogOut(index);
}

GenericAnalogOut* Context::getAnalogOut(std::string dev_name)
{
	return m_pimpl->getAnalogOut(dev_name);
}

bool Context::iioDevHasAttribute(iio_device* dev, std::string const& attr)
{
	return ContextImpl::iioDevHasAttribute(dev, attr);
}

bool Context::iioChannelHasAttribute(iio_channel* chn, std::string const& attr)
{
	return ContextImpl::iioChannelHasAttribute(chn, attr);
}

std::unordered_set<std::string> Context::getAllDevices(iio_context *ctx)
{
	return m_pimpl->getAllDevices(ctx);
}

std::vector<std::string> Context::scanAllAnalogIn()
{
	return m_pimpl->scanAllAnalogIn();
}

std::vector<std::string> Context::scanAllAnalogOut()
{
	return m_pimpl->scanAllAnalogOut();
}

void Context::scanAllDMM()
{
	m_pimpl->scanAllDMM();
}

DMM* Context::getDMM(std::string dev_name)
{
	return m_pimpl->getDMM(dev_name);
}


DMM* Context::getDMM(unsigned int index)
{
	return m_pimpl->getDMM(index);
}

std::vector<DMM*> Context::getAllDmm()
{
	return m_pimpl->getAllDmm();
}

std::vector<string> Context::getAvailableContextAttributes()
{
	return m_pimpl->getAvailableContextAttributes();
}

std::string Context::getContextAttributeValue(std::string attr)
{
	return m_pimpl->getContextAttributeValue(attr);
}

std::string Context::getContextDescription()
{
	return m_pimpl->getContextDescription();
}

std::string Context::getSerialNumber()
{
	return m_pimpl->getSerialNumber();
}

M2K* Context::toM2k()
{
	return m_pimpl->toM2k(this);
}

string Context::getUri()
{
	return m_pimpl->getUri();
}

string Context::getName()
{
	return m_pimpl->getName();
}

void devices::Context::scanAllPowerSupply()
{
	m_pimpl->scanAllPowerSupply();
}

void devices::Context::scanAllDigital()
{
	m_pimpl->scanAllDigital();
}
