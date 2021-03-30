/*
 * Copyright (c) 2020 Analog Devices Inc.
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

#ifndef M2KOUTHARDWARETRIGGER_IMPL_HPP
#define M2KOUTHARDWARETRIGGER_IMPL_HPP

#include "m2khardwaretrigger_v0.24_impl.hpp"
#include "utils/deviceout.hpp"


namespace libm2k {

class M2kHardwareTriggerV026Impl : public M2kHardwareTriggerV024Impl {
public:
	explicit M2kHardwareTriggerV026Impl(struct iio_context *ctx, bool init = true);

	~M2kHardwareTriggerV026Impl() override = default;

	void reset() override;

	void setDigitalOutSource(M2K_TRIGGER_OUT_SOURCE src) override;

	M2K_TRIGGER_OUT_SOURCE getDigitalOutSource() const override;

	void setDigitalOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond) override;

	M2K_TRIGGER_CONDITION_DIGITAL getDigitalOutCondition() const override;

	void setAnalogOutSource(M2K_TRIGGER_OUT_SOURCE src) override;

	M2K_TRIGGER_OUT_SOURCE getAnalogOutSource() const override;

	void setAnalogOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond) override;

	M2K_TRIGGER_CONDITION_DIGITAL getAnalogOutCondition() const override;

	void setAnalogOutStartOnTrigger(bool value) override;

	void setAnalogOutStopOnTrigger(bool value) override;

	void resetAnalogOutTriggerAfterBuffer(bool value) override;

protected:
	void setTriggerOutSource(M2K_TRIGGER_OUT_SOURCE src, const std::shared_ptr<libm2k::utils::DeviceOut>& device);
	M2K_TRIGGER_OUT_SOURCE getTriggerOutSource(const std::shared_ptr<libm2k::utils::DeviceOut>& device) const;
	void setTriggerOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond, const std::shared_ptr<libm2k::utils::DeviceOut>& device);
	M2K_TRIGGER_CONDITION_DIGITAL getTriggerOutCondition(const std::shared_ptr<libm2k::utils::DeviceOut>& device) const;

	std::shared_ptr<libm2k::utils::DeviceOut> m_digital_trigger_device;
	std::shared_ptr<libm2k::utils::DeviceOut> m_analog_trigger_device;

	static std::vector<std::string> m_trigger_source;
	static std::vector<std::string> m_trigger_cond;

	static const std::string m_source_attr;
	static const std::string m_condition_attr;
};
}

#endif //M2KOUTHARDWARETRIGGER_IMPL_HPP
