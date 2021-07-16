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

#ifndef GENERICANALOGIN_IMPL_HPP
#define GENERICANALOGIN_IMPL_HPP

#include <libm2k/analog/genericanalogin.hpp>
#include "utils/devicein.hpp"
#include <string>
#include <vector>
#include <memory>

namespace libm2k {
namespace analog {
class GenericAnalogInImpl : public GenericAnalogIn {
public:
	GenericAnalogInImpl(struct iio_context* ctx, std::string adc_dev);
	virtual ~GenericAnalogInImpl();

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double sampleRate);
	double setSampleRate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSampleRates();
	std::vector<double> getAvailableSampleRates(unsigned int chn_idx);

	const double* getSamplesInterleaved(unsigned int nb_samples);
	const short* getSamplesRawInterleaved(unsigned int nb_samples);

	void enableChannel(unsigned int index, bool enable);
	bool isChannelEnabled(unsigned int chnIdx);
	void setKernelBuffersCount(unsigned int count);
	std::string getName();

	struct IIO_OBJECTS getIioObjects();
	unsigned int getNbChannels();
	double getMaximumSamplerate();
	double getMaximumSamplerate(unsigned int chn_idx);
private:
	std::vector<std::shared_ptr<libm2k::utils::DeviceIn>> m_devices_in;
	std::shared_ptr<libm2k::utils::DeviceIn> getAdcDevice(unsigned int index);
	static double processSample(int16_t sample, unsigned int channel);
};
}
}

#endif // GENERICANALOGIN_IMPL_HPP
