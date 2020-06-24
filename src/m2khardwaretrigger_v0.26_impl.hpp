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

protected:
	void setTriggerOutSource(M2K_TRIGGER_OUT_SOURCE src, const std::shared_ptr<libm2k::utils::DeviceOut>& device);
	M2K_TRIGGER_OUT_SOURCE getTriggerOutSource(const std::shared_ptr<libm2k::utils::DeviceOut>& device) const;
	void setTriggerOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond, const std::shared_ptr<libm2k::utils::DeviceOut>& device);
	M2K_TRIGGER_CONDITION_DIGITAL getTriggerOutCondition(const std::shared_ptr<libm2k::utils::DeviceOut>& device) const;

	static std::vector<std::string> m_trigger_source;
	static std::vector<std::string> m_trigger_cond;

	static const std::string m_source_attr;
	static const std::string m_condition_attr;
};
}

#endif //M2KOUTHARDWARETRIGGER_IMPL_HPP
