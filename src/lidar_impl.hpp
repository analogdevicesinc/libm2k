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

#ifndef LIDAR_IMPL_H
#define LIDAR_IMPL_H

#include <map>
#include <vector>
#include <list>
#include <libm2k/lidar.hpp>
#include "utils/devicein.hpp"
#include "utils/deviceout.hpp"
#include "context_impl.hpp"

using namespace std;

namespace libm2k {
namespace contexts {
class LidarImpl : public Lidar, public ContextImpl {
public:
	LidarImpl(std::string uri, iio_context* ctx, std::string name, bool sync);
	virtual ~LidarImpl();

	void channelEnableDisable(string channel, bool enable);
	map<string, vector<int8_t>> readChannels(unsigned int nb_samples);

	void setApdBias(unsigned int bias);
	void setTiltVoltage(unsigned int voltage);
	void laserEnable();
	void laserDisable();
	void setLaserPulseWidth(long long pulse_width);
	void setLaserFrequency(long long frequency);

	void enableChannelSequencer();
	void disableChannelSequencer();
	void setSequencerPulseDelay(long long ns);

	void setChannelSequencerOpModeManual();
	void setChannelSequencerOpModeAuto();
	void setChannelSequencerOrderAutoMode(const char* order);
	void setChannelSequencerOrderManualMode(const char* order);

	void initialize();
private:
	libm2k::utils::DeviceGeneric* pulse_dev;
	libm2k::utils::DeviceGeneric* afe_dev;
	libm2k::utils::DeviceIn* adc_dev;
	void setAllIIoAttributesToDefaultValues();
	void setChannelSequencerEnableDisable(bool status);
	vector<string> enabledChannelsList();
	double adp_bias_volts_to_raw_convert(double value, bool inverse);
	void setChannelSequencerOpMode(const char *mode);
	double tilt_volts_to_raw_convert(double value, bool inverse);
};
}
}

#endif
