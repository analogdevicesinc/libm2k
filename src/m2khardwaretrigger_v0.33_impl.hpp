/*
 * Copyright (c) 2024 Analog Devices Inc.
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

#ifndef M2KHARDWARETRIGGER_IMPL_v033_HPP
#define M2KHARDWARETRIGGER_IMPL_v033_HPP

#include "m2khardwaretrigger_impl.hpp"
#include "m2khardwaretrigger_v0.24_impl.hpp"
#include <libm2k/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include "utils/channel.hpp"
#include "utils/devicein.hpp"
#include <vector>
#include <memory>

using namespace libm2k::utils;


namespace libm2k {
class M2kHardwareTriggerV033Impl : public M2kHardwareTriggerV024Impl
{
public:
	M2kHardwareTriggerV033Impl(struct iio_context *ctx, bool init = true);
	virtual ~M2kHardwareTriggerV033Impl();

	void reset();

	void setAnalogOutTriggerSource(M2K_TRIGGER_SOURCE_OUT src);
	M2K_TRIGGER_SOURCE_OUT getAnalogOutTriggerSource() const;

	void setAnalogOutTriggerCondition(M2K_TRIGGER_CONDITION_OUT condition);
	M2K_TRIGGER_CONDITION_OUT getAnalogOutTriggerCondition() const;

	void setAnalogOutTriggerStatus(M2K_TRIGGER_STATUS_ANALOG_OUT status);
	M2K_TRIGGER_STATUS_ANALOG_OUT getAnalogOutTriggerStatus() const;

protected:
	std::shared_ptr<DeviceIn> m_analog_out_trigger_device;
private:
	static std::vector<std::string> m_trigger_source_out;
	static std::vector<std::string> m_trigger_condition_out;
	static std::vector<std::string> m_trigger_status_out;
};
}

#endif //M2KHARDWARETRIGGER_IMPL_v033_HPP
