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

ContextImpl::ContextImpl(std::string uri, struct iio_context *ctx, std::string name, bool sync)
{
	UNUSED(name);
	m_context = ctx;
	m_uri = uri;
	m_sync = sync;
	m_ownsContext = false;

	/* Initialize the DMM list */
	scanAllDMM();

	initializeContextAttributes();
}

ContextImpl::~ContextImpl()
{
	for (auto d : m_instancesDMM) {
		delete d;
	}
	m_instancesDMM.clear();

	if (m_context && m_ownsContext) {
		iio_context_destroy(m_context);
	}
}

void ContextImpl::reset()
{
}

void ContextImpl::deinitialize()
{
}


bool ContextImpl::iioDevHasAttribute(iio_device* dev, std::string const& attr)
{
	unsigned int nb_attr = iio_device_get_attrs_count(dev);
	const char* attr_name;
	for (unsigned int i = 0; i < nb_attr; i++) {
		attr_name = iio_device_get_attr(dev, i);
		std::size_t found = std::string(attr_name).find(attr);
		if (found != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool ContextImpl::iioDevBufferHasAttribute(iio_device *dev, const std::string &attr)
{
	const char *attribute = iio_device_find_buffer_attr(dev, attr.c_str());
	return attribute != nullptr;
}

bool ContextImpl::iioChannelHasAttribute(iio_channel* chn, std::string const& attr)
{
	unsigned int nb_attr = iio_channel_get_attrs_count(chn);
	const char* attr_name;
	for (unsigned int i = 0; i < nb_attr; i++) {
		attr_name = iio_channel_get_attr(chn, i);
		std::size_t found = std::string(attr_name).find(attr);
		if (found != std::string::npos) {
			return true;
		}
	}
	return false;
}

DEVICE_DIRECTION ContextImpl::getIioDeviceDirection(std::string dev_name)
{
	DEVICE_DIRECTION dir = NO_DIRECTION;
	auto dev = iio_context_find_device(m_context, dev_name.c_str());
	if (!dev) {
		THROW_M2K_EXCEPTION("No device found with name: " + dev_name, libm2k::EXC_INVALID_PARAMETER);
	}

	auto chn_count = iio_device_get_channels_count(dev);
	for (unsigned int i = 0; i < chn_count; i++) {
		auto chn = iio_device_get_channel(dev, i);
		if (iio_channel_is_output(chn)) {
			if (dir == INPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = OUTPUT;
			}
		} else {
			if (dir == OUTPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = INPUT;
			}
		}
	}
	return dir;
}


DEVICE_TYPE ContextImpl::getIioDeviceType(std::string dev_name)
{
	auto dev = iio_context_find_device(m_context, dev_name.c_str());
	if (!dev) {
		THROW_M2K_EXCEPTION("No device found with name: " + dev_name, libm2k::EXC_INVALID_PARAMETER);
	}

	auto chn = iio_device_get_channel(dev, 0);
	if (!chn) {
		return NO_DEV;
	}

	const struct iio_data_format* data_format = iio_channel_get_data_format(chn);
	if (data_format->bits == 1) {
		return DIGITAL_DEV;
	} else {
		return ANALOG_DEV;
	}
}

std::vector<std::pair<std::string, std::string>> ContextImpl::getIioDevByChannelAttrs(std::vector<std::string> attr_list)
{
	iio_device* dev = nullptr;
	iio_channel* chn = nullptr;
	std::vector<std::pair<std::string, std::string>> dev_chn_list;
	unsigned int nb_chn = 0;
	unsigned int nb_dev = iio_context_get_devices_count(m_context);

	for (unsigned int i_dev = 0; i_dev < nb_dev; i_dev++) {
		bool dev_match = true;
		dev = iio_context_get_device(m_context, i_dev);
		nb_chn = iio_device_get_channels_count(dev);

		for (unsigned int i_chn = 0; i_chn < nb_chn; i_chn++) {
			bool chn_match = true;
			chn = iio_device_get_channel(dev, i_chn);

			/* Check if the current channel has all the required attributes */
			for (unsigned int i_attr = 0; i_attr < attr_list.size(); i_attr++) {
				if (!iioChannelHasAttribute(chn, attr_list.at(i_attr))) {
					chn_match = false;
					break;
				}
			}

			if (chn_match) {
				auto d_name = iio_device_get_name(dev);
				auto c_name = iio_channel_get_id(chn);
				if (c_name && d_name) {
					dev_chn_list.push_back(make_pair(std::string(d_name),
									 std::string(c_name)));
				}
			}
		}

		/* Check if the device has all the required attributes as global attributes */
		for(unsigned int i_attr = 0; i_attr < attr_list.size(); i_attr++) {
			if (!iioDevHasAttribute(dev, attr_list.at(i_attr))) {
				dev_match = false;
				break;
			}
		}

		if (dev_match) {
			dev_chn_list.push_back(make_pair(std::string(iio_device_get_name(dev)), ""));
		}
	}
	return dev_chn_list;
}

std::vector<std::pair<std::string, std::string>> ContextImpl::getHwmonDevices(){
	iio_device* dev = nullptr;
	iio_channel* chn = nullptr;
	std::vector<std::pair<std::string, std::string>> dev_chn_list;
	unsigned int nb_chn = 0;
	unsigned int nb_dev = iio_context_get_devices_count(m_context);

	for (unsigned int i_dev = 0; i_dev < nb_dev; i_dev++) {
		dev = iio_context_get_device(m_context, i_dev);
		nb_chn = iio_device_get_channels_count(dev);
		for (unsigned int i_chn = 0; i_chn < nb_chn; i_chn++) {
			chn = iio_device_get_channel(dev, i_chn);

			/* Check if the current device has any valid channels*/
			if (iio_device_is_hwmon(dev) &&  iioChannelHasAttribute(chn, "input")){
				auto d_name = iio_device_get_name(dev);
				auto c_name = std::string(iio_channel_get_id(chn));
				if (!c_name.empty() && d_name) {
					dev_chn_list.push_back(make_pair(std::string(d_name),c_name));
				}
			}
		}
	}
	return dev_chn_list;
}

bool ContextImpl::isIioDeviceBufferCapable(std::string dev_name)
{
	unsigned int dev_count = iio_device_get_buffer_attrs_count(
				iio_context_find_device(m_context, dev_name.c_str()));
	if (dev_count > 0) {
		return true;
	} else {
		return false;
	}
}

std::unordered_set<std::string> ContextImpl::getAllDevices() const
{
	return Utils::getAllDevices(m_context);
}

void ContextImpl::scanAllDMM()
{
	auto dev_list = getIioDevByChannelAttrs({"raw", "scale"});
	// check if there are any dmm hwmon devices
	auto dev_list_by_ch = getHwmonDevices();
	dev_list.insert(dev_list.end(), dev_list_by_ch.begin(), dev_list_by_ch.end());

	for (auto dev : dev_list) {
		if (getIioDeviceDirection(dev.first) != OUTPUT) {
			if (!getDMM(dev.first)) {
				m_instancesDMM.push_back(new DMMImpl(m_context, dev.first, m_sync));
			}
		}
	}
}

void ContextImpl::logAllAttributes() const
{
#ifdef LIBM2K_ENABLE_LOG
        LIBM2K_LOG(INFO, "[BEGIN] LOG ALL");
        const char *name, *value;
	unsigned int attr_no = iio_context_get_attrs_count(m_context);
	for (unsigned int i = 0; i < attr_no; i++) {
		std::pair<std::string, std::string> pair;
		int ret = iio_context_get_attr(m_context, i, &name, &value);
		if (ret < 0) {
			THROW_M2K_EXCEPTION("Device: Can't get context attribute " + std::to_string(i),
					    libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({"context", name, LIBM2K_ATTRIBUTE_READ}, value));
	}

        auto devices = getAllDevices();
        for (auto &dev_name : devices) {
                DeviceGeneric dev(m_context, dev_name);
                auto nb_out_channels = dev.getNbChannels(true);
                auto nb_in_channels = dev.getNbChannels(false);
                for (unsigned int i = 0; i < nb_out_channels; i++) {
                        auto channel = dev.getChannel(i, true);
                        for (unsigned int j = 0; j < channel->getNbAttributes(); j++) {
                                auto attr_name =  channel->getAttributeName(j);
                                auto attr_val = channel->getStringValue(attr_name);
                        }
                }
                for (unsigned int i = 0; i < nb_in_channels; i++) {
                        auto channel = dev.getChannel(i, false);
                        for (unsigned int j = 0; j < channel->getNbAttributes(); j++) {
                                auto attr_name =  channel->getAttributeName(j);
                                auto attr_val = channel->getStringValue(attr_name);
                        }
                }
                auto nb_dev_attrs = dev.getNbAttributes();
                for(unsigned int i = 0; i < nb_dev_attrs; i++) {
                        auto attr_name = dev.getAttributeName(i);
                        auto attr_val = dev.getStringValue(attr_name);
                }
                auto nb_dev_buffer_attrs = dev.getNbBufferAttributes();
                for(unsigned int i = 0; i < nb_dev_buffer_attrs; i++) {
                        auto attr_name = dev.getBufferAttributeName(i);
                        auto attr_val = dev.getBufferStringValue(attr_name);
                }
        }
        LIBM2K_LOG(INFO, "[END] LOG ALL");
#endif
}

DMM* ContextImpl::getDMM(std::string dev_name)
{
	for (DMM* d : m_instancesDMM) {
		if (d->getName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}


DMM* ContextImpl::getDMM(unsigned int index)
{
	if (index < m_instancesDMM.size()) {
		return m_instancesDMM.at(index);
	} else {
		return nullptr;
	}
}

std::vector<DMM*> ContextImpl::getAllDmm()
{
	return m_instancesDMM;
}

unsigned int ContextImpl::getDmmCount()
{
	return m_instancesDMM.size();
}


std::vector<std::string> ContextImpl::getAvailableContextAttributes()
{
	std::vector<std::string> available_attrs = {};
	for (auto a : m_context_attributes) {
		available_attrs.push_back(a.first);
	}
	return available_attrs;
}

std::string ContextImpl::getContextAttributeValue(std::string attr)
{
	std::string val;
	val = m_context_attributes.at(attr);
	return val;
}

std::string ContextImpl::getContextDescription()
{
	if (!m_context) {
		return "";
	}
	std::string descr = std::string(iio_context_get_description(m_context));
	return descr;
}

std::string ContextImpl::getSerialNumber()
{
	return getContextAttributeValue("hw_serial");
}

M2k* ContextImpl::toM2k()
{
	libm2k::context::M2k* dev = dynamic_cast<M2k*>(this);
	if(dev) {
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
	return m_uri;
}

std::string ContextImpl::getFirmwareVersion()
{
	return Utils::getFirmwareVersion(m_context);
}

void ContextImpl::initializeContextAttributes()
{
	const char *name;
	const char *value;
	char ctx_git_tag[8];
	unsigned int ctx_major, ctx_minor;
	int ret;
	ret = iio_context_get_version(m_context, &ctx_major, &ctx_minor, ctx_git_tag);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Context: Can't get context version", libm2k::EXC_RUNTIME_ERROR, ret);
	}
	unsigned int attr_no = iio_context_get_attrs_count(m_context);
	for (unsigned int i = 0; i < attr_no; i++) {
		std::pair<std::string, std::string> pair;
		int ret = iio_context_get_attr(m_context, i, &name, &value);
		if (ret < 0) {
			THROW_M2K_EXCEPTION("Device: Can't get context attribute " + std::to_string(i), libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({"context", name, LIBM2K_ATTRIBUTE_READ}, value));
		pair.first = std::string(name);
		pair.second = std::string(value);
		m_context_attributes.insert(pair);
	}
}

const struct libm2k::IIO_CONTEXT_VERSION ContextImpl::getIioContextVersion()
{
        libm2k::IIO_CONTEXT_VERSION iioContextVersion = {};
        int ret = iio_context_get_version(m_context, &iioContextVersion.major, &iioContextVersion.minor, iioContextVersion.git_tag);
        if (ret < 0) {
                THROW_M2K_EXCEPTION("Context: Cannot get context version ", libm2k::EXC_RUNTIME_ERROR, ret);
        }
        return iioContextVersion;
}

struct iio_context *ContextImpl::getIioContext()
{
        return m_context;
}

void ContextImpl::setTimeout(unsigned int timeout)
{
	iio_context_set_timeout(m_context, timeout);
}

void ContextImpl::setContextOwnership(bool ownsContext)
{
        m_ownsContext = ownsContext;
}
