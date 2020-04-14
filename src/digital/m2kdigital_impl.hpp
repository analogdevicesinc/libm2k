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

#ifndef M2KDIGITAL_IMPL_HPP
#define M2KDIGITAL_IMPL_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include "utils/deviceout.hpp"
#include "utils/devicein.hpp"
#include <libm2k/digital/m2kdigital.hpp>
#include <string>
#include <vector>

namespace libm2k {

namespace digital {
class M2kDigitalImpl : public M2kDigital
{
public:
	M2kDigitalImpl(struct iio_context* ctx, std::string logic_dev, bool sync, M2kHardwareTrigger *trigger);
	virtual ~M2kDigitalImpl();

	void init();

	void setDirection(unsigned short mask);
	void setDirection(unsigned int index, DIO_DIRECTION dir);
	void setDirection(unsigned int index, bool dir);
	void setDirection(DIO_CHANNEL index, bool dir);
	void setDirection(DIO_CHANNEL index, DIO_DIRECTION dir);
	DIO_DIRECTION getDirection(DIO_CHANNEL index);

	void push(std::vector<unsigned short> const &data);
	void push(unsigned short *data, unsigned int nb_samples);

	void setValueRaw(DIO_CHANNEL index, DIO_LEVEL level);
	void setValueRaw(unsigned int index, DIO_LEVEL level);
	void setValueRaw(DIO_CHANNEL index, bool level);
	DIO_LEVEL getValueRaw(DIO_CHANNEL index);
	DIO_LEVEL getValueRaw(unsigned int index);

	void stopBufferOut();
	void startAcquisition(unsigned int nb_samples) override;
	void stopAcquisition();

	std::vector<unsigned short> getSamples(unsigned int nb_samples);
	const unsigned short *getSamplesP(unsigned int nb_samples);

	void enableChannel(unsigned int index, bool enable);
	void enableChannel(DIO_CHANNEL index, bool enable);
	void enableAllOut(bool enable);
	bool anyChannelEnabled(DIO_DIRECTION dir);

	void setOutputMode(DIO_CHANNEL chn, DIO_MODE mode);
	void setOutputMode(unsigned int chn, DIO_MODE mode);
	DIO_MODE getOutputMode(DIO_CHANNEL chn);
	DIO_MODE getOutputMode(unsigned int chn);

	double setSampleRateIn(double samplerate);
	double setSampleRateOut(double samplerate);
	double getSampleRateIn();
	double getSampleRateOut();

	bool getCyclic();
	void setCyclic(bool cyclic);

	libm2k::M2kHardwareTrigger* getTrigger();

	void setKernelBuffersCountIn(unsigned int count);
	void setKernelBuffersCountOut(unsigned int count);

	struct IIO_OBJECTS getIioObjects();

	void cancelBufferIn();
	void cancelBufferOut();

	unsigned int getNbChannelsIn();
	unsigned int getNbChannelsOut();

	void getSamples(std::vector<unsigned short> &data, unsigned int nb_samples);
private:
	bool m_cyclic;
	std::shared_ptr<libm2k::utils::DeviceIn> m_dev_read;
	std::shared_ptr<libm2k::utils::DeviceOut> m_dev_write;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_dev_generic;
	std::string m_dev_name_read;
	std::string m_dev_name_write;
	std::vector<bool> m_tx_channels_enabled;
	std::vector<bool> m_rx_channels_enabled;
	libm2k::M2kHardwareTrigger *m_trigger;
	static std::vector<std::string> m_output_mode;

	void syncDevice();
};
}
}
#endif //M2KDIGITAL_IMPL_HPP
