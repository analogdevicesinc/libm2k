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

#ifndef CONTEXT_IMPL_HPP
#define CONTEXT_IMPL_HPP

#include <libm2k/context.hpp>
#include <libm2k/lidar.hpp>
#include <libm2k/generic.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/enums.hpp>
#include <libm2k/utils/utils.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
	class GenericAnalogIn;
	class GenericAnalogOut;
	class DMM;
	class PowerSupply;
}

namespace digital {
	class GenericDigital;
}

namespace context {
class Context;
class Lidar;
class M2k;

class ContextImpl : public virtual Context {
public:
	ContextImpl(std::string uri, struct iio_context*, std::string name, bool sync);
	~ContextImpl();

	void init();
	void deinitialize();

	void scanAllDMM();

	std::string getUri();

	libm2k::analog::DMM* getDMM(unsigned int);
	libm2k::analog::DMM* getDMM(std::string);
	std::vector<libm2k::analog::DMM*> getAllDmm();


	std::vector<std::string> getAvailableContextAttributes();
	std::string getContextAttributeValue(std::string attr);
	std::string getContextDescription();
	std::string getSerialNumber();
	std::unordered_set<std::string> getAllDevices();

	libm2k::context::M2k* toM2k();
	libm2k::context::Generic* toGeneric();
	libm2k::context::Lidar* toLidar();

	static bool iioChannelHasAttribute(iio_channel *chn, const std::string &attr);
	static bool iioDevHasAttribute(iio_device *dev, const std::string &attr);
	static bool iioDevBufferHasAttribute(iio_device *dev, const std::string &attr);

	unsigned int getDmmCount();
	std::string getFirmwareVersion();
	void setTimeout(unsigned int timeout);

protected:
	struct iio_context* m_context;
	std::vector<libm2k::analog::DMM*> m_instancesDMM;
	bool isIioDeviceBufferCapable(std::string dev_name);
	std::vector<std::pair<std::string, std::string> > getIioDevByChannelAttrs(std::vector<std::string> attr_list);
	libm2k::utils::DEVICE_TYPE getIioDeviceType(std::string dev_name);
	libm2k::utils::DEVICE_DIRECTION getIioDeviceDirection(std::string dev_name);
private:
	void initializeContextAttributes();
	std::map<std::string, std::string> m_context_attributes;

	std::string m_uri;
	std::string m_name;
	bool m_sync;
};
}
}

#endif // CONTEXT_IMPL_HPP
