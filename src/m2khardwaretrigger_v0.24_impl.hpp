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

#ifndef M2KHARDWARETRIGGER_IMPL_v024_HPP
#define M2KHARDWARETRIGGER_IMPL_v024_HPP

#include "m2khardwaretrigger_impl.hpp"
#include <libm2k/enums.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include "utils/channel.hpp"
#include "utils/devicein.hpp"
#include <vector>
#include <memory>

using namespace libm2k::utils;
using namespace libm2k::digital;


namespace libm2k {
class M2kHardwareTriggerV024Impl : public M2kHardwareTriggerImpl
{
public:
	M2kHardwareTriggerV024Impl(struct iio_context *ctx, bool init = true);
	virtual ~M2kHardwareTriggerV024Impl();

	void reset();
	void deinitialize() override;

	void setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT output_select);
	M2K_TRIGGER_OUT_SELECT getAnalogExternalOutSelect();

	bool hasExternalTriggerIn() const;
	bool hasExternalTriggerOut() const;
	bool hasCrossInstrumentTrigger() const;

	void setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src);
	M2K_TRIGGER_SOURCE_DIGITAL getDigitalSource() const;

	M2K_TRIGGER_SOURCE_ANALOG getAnalogSource();
	void setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src);

	struct SETTINGS *getCurrentHwSettings();
	void setHwTriggerSettings(struct SETTINGS *settings);

private:
	static std::vector<std::string> m_digital_out_select;
	static std::vector<std::string> m_digital_out_direction;
	static std::vector<std::string> m_trigger_ext_digital_source;
	static std::vector<std::string> m_trigger_source;
};
}

#endif //M2KHARDWARETRIGGER_IMPL_v024_HPP
