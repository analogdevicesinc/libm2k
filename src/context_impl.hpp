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
#include <libm2k/context_private.hpp>
#include "context_impl_private.hpp"
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
class M2k;

class ContextImpl : public virtual Context {
public:
	ContextImpl(ContextImplPrivate *ctx);
	~ContextImpl() override;

	void reset() override;
	void deinitialize() override;

	void scanAllDMM();

	std::string getUri() override;

	libm2k::analog::DMM* getDMM(unsigned int) override;
	libm2k::analog::DMM* getDMM(std::string) override;
	std::vector<libm2k::analog::DMM*> getAllDmm() override;

	std::vector<std::string> getAvailableContextAttributes() override;
	std::string getContextAttributeValue(std::string attr) override;
	std::string getContextDescription() override;
	std::string getSerialNumber() override;
	std::unordered_set<std::string> getAllDevices() const override;
	void logAllAttributes() const override;

	libm2k::context::M2k* toM2k() override;
	libm2k::context::Generic* toGeneric() override;

	static bool iioChannelHasAttribute(iio_channel *chn, const std::string &attr);
	static bool iioDevHasAttribute(iio_device *dev, const std::string &attr);
	static bool iioDevBufferHasAttribute(iio_device *dev, const std::string &attr);

	unsigned int getDmmCount() override;
	std::string getFirmwareVersion() override;
	const struct libm2k::IIO_CONTEXT_VERSION getIioContextVersion() override;
	struct iio_context *getIioContext() override;
	void setTimeout(unsigned int timeout) override;
	void setContextOwnership(bool ownsContext);

private:
	ContextImplPrivate *m_contextPrivate;
};
}
}

#endif // CONTEXT_IMPL_HPP
