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

#include <libm2k/m2kexceptions.hpp>
#include <libm2k/context.hpp>
#include <libm2k/analog/genericanalogin.hpp>
#include <libm2k/analog/genericanalogout.hpp>
#include <libm2k/analog/powersupply.hpp>
#include <libm2k/analog/dmm.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2k.hpp>
#include <iio.h>
#include <iostream>
#include <memory>

using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::contexts;
using namespace libm2k::utils;

class Context::ContextImpl {
public:
	ContextImpl(std::string uri, struct iio_context *ctx, std::string name, bool sync)
	{
		m_context = ctx;
		m_uri = uri;
		m_sync = sync;

		/* Initialize the AnalogIn list */
		auto aIn_lst = scanAllAnalogIn();
		for (auto aIn : aIn_lst) {
			m_instancesAnalogIn.push_back(new GenericAnalogIn(ctx, aIn));
		}

		/* Initialize the AnalogIn list */
		auto aOut_lst = scanAllAnalogOut();
		for (auto aOut : aOut_lst) {
			m_instancesAnalogOut.push_back(new GenericAnalogOut(ctx, aOut));
		}

		/* Initialize the DMM list */
		scanAllDMM();


		/* Initialize the power supply list */
		scanAllPowerSupply();

		initializeContextAttributes();
	}

	virtual ~ContextImpl()
	{
		for (auto d : m_instancesDMM) {
			delete d;
		}
		for (auto ain : m_instancesAnalogIn) {
			delete ain;
		}
		for (auto aout : m_instancesAnalogOut) {
			delete aout;
		}
		m_instancesDMM.clear();
		m_instancesAnalogIn.clear();
		m_instancesAnalogOut.clear();

		if (m_context) {
			iio_context_destroy(m_context);
		}
	}

	virtual void init()
	{
	}

	virtual void deinitialize()
	{

	}

	unsigned int getAnalogInCount()
	{
		return m_instancesAnalogIn.size();
	}

	GenericAnalogIn* getAnalogIn(unsigned int index)
	{
		if (index < m_instancesAnalogIn.size()) {
			return m_instancesAnalogIn.at(index);
		} else {
			return nullptr;
		}
	}

	GenericAnalogIn* getAnalogIn(std::string dev_name)
	{
		for (GenericAnalogIn* d : m_instancesAnalogIn) {
			if (d->getDeviceName() == dev_name) {
				return d;
			}
		}
		return nullptr;
	}

	unsigned int getAnalogOutCount()
	{
		return m_instancesAnalogOut.size();
	}

	GenericAnalogOut* getAnalogOut(unsigned int index)
	{
		if (index < m_instancesAnalogOut.size()) {
			return m_instancesAnalogOut.at(index);
		} else {
			return nullptr;
		}
	}

	GenericAnalogOut* getAnalogOut(std::string dev_name)
	{
		for (GenericAnalogOut* d : m_instancesAnalogOut) {
			if (d->getName() == dev_name) {
				return d;
			}
		}
		return nullptr;
	}

	static bool iioDevHasAttribute(iio_device* dev, std::string const& attr)
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

	static bool iioChannelHasAttribute(iio_channel* chn, std::string const& attr)
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

	DEVICE_DIRECTION getIioDeviceDirection(std::string dev_name)
	{
		DEVICE_DIRECTION dir = NO_DIRECTION;
		auto dev = iio_context_find_device(m_context, dev_name.c_str());
		if (!dev) {
			throw_exception(EXC_RUNTIME_ERROR, "No device found with name: " + dev_name);
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


	DEVICE_TYPE getIioDeviceType(std::string dev_name)
	{
		auto dev = iio_context_find_device(m_context, dev_name.c_str());
		if (!dev) {
			throw_exception(EXC_INVALID_PARAMETER, "No device found with name: " + dev_name);
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

	std::vector<std::pair<std::string, std::string>> getIioDevByChannelAttrs(std::vector<std::string> attr_list)
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

	bool isIioDeviceBufferCapable(std::string dev_name)
	{
		unsigned int dev_count = iio_device_get_buffer_attrs_count(
					iio_context_find_device(m_context, dev_name.c_str()));
		if (dev_count > 0) {
			return true;
		} else {
			return false;
		}
	}

	std::unordered_set<std::string> getAllDevices()
	{

		return Utils::getAllDevices(m_context);
	}

	std::vector<std::string> scanAllAnalogIn()
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
		return aIn_lst;
	}

	std::vector<std::string> scanAllAnalogOut()
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
		return aOut_lst;
	}

	void scanAllDMM()
	{
		auto dev_list = getIioDevByChannelAttrs({"raw", "scale"});
		for (auto dev : dev_list) {
			if (getIioDeviceDirection(dev.first) != OUTPUT) {
				if (!getDMM(dev.first)) {
					m_instancesDMM.push_back(new DMM(m_context, dev.first, m_sync));
				}
			}
		}
	}

	DMM* getDMM(std::string dev_name)
	{
		for (DMM* d : m_instancesDMM) {
			if (d->getName() == dev_name) {
				return d;
			}
		}
		return nullptr;
	}


	DMM* getDMM(unsigned int index)
	{
		if (index < m_instancesDMM.size()) {
			return m_instancesDMM.at(index);
		} else {
			return nullptr;
		}
	}

	std::vector<DMM*> getAllDmm()
	{
		return m_instancesDMM;
	}

	unsigned int getDmmCount()
	{
		return m_instancesDMM.size();
	}


	std::vector<std::string> getAvailableContextAttributes()
	{
		std::vector<std::string> available_attrs = {};
		for (auto a : m_context_attributes) {
			available_attrs.push_back(a.first);
		}
		return available_attrs;
	}

	std::string getContextAttributeValue(std::string attr)
	{
		std::string val;
		__try {
			val = m_context_attributes.at(attr);
		} __catch (std::exception &) {
			throw_exception(EXC_INVALID_PARAMETER, "No such context attribute" + attr);
		}
		return val;
	}

	std::string getContextDescription()
	{
		if (!m_context) {
			return "";
		}
		std::string descr = std::string(iio_context_get_description(m_context));
		return descr;
	}

	std::string getSerialNumber()
	{
		return getContextAttributeValue("hw_serial");
	}

	M2k* toM2k(Context *parent)
	{
		libm2k::contexts::M2k* dev = dynamic_cast<M2k*>(parent);
		if(dev) {
			return dev;
		} else {
			return nullptr;
		}
	}

	std::string getUri()
	{
		return m_uri;
	}

	void scanAllPowerSupply()
	{

	}

	void scanAllDigital()
	{

	}

	std::string getFirmwareVersion()
	{
		return Utils::getFirmwareVersion(m_context);
	}

protected:
	struct iio_context* m_context;
	std::vector<libm2k::analog::DMM*> m_instancesDMM;
private:
	void initializeContextAttributes()
	{
		const char *name;
		const char *value;
		char ctx_git_tag[8];
		unsigned int ctx_major, ctx_minor;
		iio_context_get_version(m_context, &ctx_major, &ctx_minor, ctx_git_tag);
		unsigned int attr_no = iio_context_get_attrs_count(m_context);
		for (unsigned int i = 0; i < attr_no; i++) {
			std::pair<std::string, std::string> pair;
			int ret = iio_context_get_attr(m_context, i, &name, &value);
			if (ret < 0) {
				throw_exception(EXC_RUNTIME_ERROR, "Device: Can't get context attribute " +
						std::to_string(i));
			}
			pair.first = std::string(name);
			pair.second = std::string(value);
			m_context_attributes.insert(pair);
		}
	}

	std::vector<libm2k::analog::GenericAnalogIn*> m_instancesAnalogIn;
	std::vector<libm2k::analog::GenericAnalogOut*> m_instancesAnalogOut;
	std::vector<libm2k::analog::PowerSupply*> m_instancesPowerSupply;
	std::vector<libm2k::digital::GenericDigital*> m_instancesDigital;
	std::map<std::string, std::string> m_context_attributes;

	std::string m_uri;
	std::string m_name;
	bool m_sync;
};
