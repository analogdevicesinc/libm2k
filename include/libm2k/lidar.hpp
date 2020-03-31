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

#ifndef LIDAR_H
#define LIDAR_H

#include <map>
#include <vector>
#include <list>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>

using namespace std;

namespace libm2k {
namespace context {
class LIBM2K_API Lidar : public virtual Context {
public:
	virtual ~Lidar() {}

	virtual void channelEnableDisable(string channel, bool enable) = 0;
	virtual map<string, vector<int8_t>> readChannels(unsigned int nb_samples) = 0;

	virtual void setApdBias(unsigned int bias) = 0;
	virtual void setTiltVoltage(unsigned int voltage) = 0;
	virtual void laserEnable() = 0;
	virtual void laserDisable() = 0;
	virtual void setLaserPulseWidth(long long pulse_width) = 0;
	virtual void setLaserFrequency(long long frequency) = 0;

	virtual void enableChannelSequencer() = 0;
	virtual void disableChannelSequencer() = 0;
	virtual void setSequencerPulseDelay(long long ns) = 0;

	virtual void setChannelSequencerOpModeManual() = 0;
	virtual void setChannelSequencerOpModeAuto() = 0;
	virtual void setChannelSequencerOrderAutoMode(const char* order) = 0;
	virtual void setChannelSequencerOrderManualMode(const char* order) = 0;
};
}
}

#endif
