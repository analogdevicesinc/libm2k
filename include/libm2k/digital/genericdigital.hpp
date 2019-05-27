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

#ifndef GENERICDIGITALIN_HPP
#define GENERICDIGITALIN_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/device.hpp>
#include <libm2k/digital/enums.hpp>
#include <string>
#include <vector>
#include <memory>

namespace libm2k {
namespace digital {
class LIBM2K_API GenericDigital : public libm2k::utils::Device {
public:
	GenericDigital(struct iio_context* ctx, std::string logic_dev);
	virtual ~GenericDigital();

	double getSampleRate();
	double setSampleRate(double sampleRate);

	void setCyclic(bool cyclic);
	bool getCyclic();

	void enableChannel(unsigned int index, bool enable);

private:
	class GenericDigitalImpl;
	std::shared_ptr<GenericDigitalImpl> m_pimpl;
};
}
}

#endif //GENERICDIGITALIN_HPP
