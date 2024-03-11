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

#include "context_impl.hpp"
#include "analog/dmm_impl.hpp"
#include "utils/channel.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2k.hpp>
#include <iio.h>
#include <iostream>
#include <memory>

using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::context;
using namespace libm2k::utils;

ContextImpl::ContextImpl(ContextImplPrivate *p)
{
	m_contextPrivate = p;
	m_contextPrivate->m_refCount++;
	std::cout << "refcount++ " << m_contextPrivate->getRefCount() << " " << m_contextPrivate->getUri() << "\n";
}

ContextImpl::~ContextImpl()
{
	m_contextPrivate->m_refCount--;
	std::cout << "refcount-- " << m_contextPrivate->getRefCount() << " " << m_contextPrivate->getUri() << "\n";
}

void ContextImpl::reset()
{
	m_contextPrivate->reset();
}

void ContextImpl::deinitialize()
{
	m_contextPrivate->deinitialize();
}

bool ContextImpl::iioDevHasAttribute(iio_device* dev, std::string const& attr)
{
	return ContextImplPrivate::iioDevHasAttribute(dev, attr);
}

bool ContextImpl::iioDevBufferHasAttribute(iio_device *dev, const std::string &attr)
{
	return ContextImplPrivate::iioDevBufferHasAttribute(dev, attr);
}

bool ContextImpl::iioChannelHasAttribute(iio_channel* chn, std::string const& attr)
{
	return ContextImplPrivate::iioChannelHasAttribute(chn, attr);
}

std::unordered_set<std::string> ContextImpl::getAllDevices() const
{
	return m_contextPrivate->getAllDevices();
}

void ContextImpl::scanAllDMM()
{
	m_contextPrivate->scanAllDMM();
}

void ContextImpl::logAllAttributes() const
{
	m_contextPrivate->logAllAttributes();
}

DMM* ContextImpl::getDMM(std::string dev_name)
{
	return m_contextPrivate->getDMM(dev_name);
}


DMM* ContextImpl::getDMM(unsigned int index)
{
	return m_contextPrivate->getDMM(index);
}

std::vector<DMM*> ContextImpl::getAllDmm()
{
	return m_contextPrivate->getAllDmm();
}

unsigned int ContextImpl::getDmmCount()
{
	return m_contextPrivate->getDmmCount();
}


std::vector<std::string> ContextImpl::getAvailableContextAttributes()
{
	return m_contextPrivate->getAvailableContextAttributes();
}

std::string ContextImpl::getContextAttributeValue(std::string attr)
{
	return m_contextPrivate->getContextAttributeValue(attr);
}

std::string ContextImpl::getContextDescription()
{
	return m_contextPrivate->getContextDescription();
}

std::string ContextImpl::getSerialNumber()
{
	return m_contextPrivate->getSerialNumber();
}

M2k* ContextImpl::toM2k()
{
	libm2k::context::M2k* dev = dynamic_cast<M2k*>(this);
	if(dev) {
		dev->calibrateADC();
		return dev;
	} else {
		return nullptr;
	}
}

Generic *ContextImpl::toGeneric()
{
	libm2k::context::Generic* dev = dynamic_cast<Generic*>(this);
	if(dev) {
		return dev;
	} else {
		return nullptr;
	}
}

std::string ContextImpl::getUri()
{
	return m_contextPrivate->getUri();
}

std::string ContextImpl::getFirmwareVersion()
{
	return m_contextPrivate->getFirmwareVersion();
}

const struct libm2k::IIO_CONTEXT_VERSION ContextImpl::getIioContextVersion()
{
	return m_contextPrivate->getIioContextVersion();
}

struct iio_context *ContextImpl::getIioContext()
{
	return m_contextPrivate->getIioContext();
}

void ContextImpl::setTimeout(unsigned int timeout)
{
	m_contextPrivate->setTimeout(timeout);
}

void ContextImpl::setContextOwnership(bool ownsContext)
{
	m_contextPrivate->setContextOwnership(ownsContext);
}
