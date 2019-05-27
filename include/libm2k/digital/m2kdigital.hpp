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

#ifndef M2KDIGITAL_HPP
#define M2KDIGITAL_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/analog/m2khardwaretrigger.hpp>
#include <string>
#include <vector>

namespace libm2k {
namespace digital {
class LIBM2K_API M2kDigital
{
public:
	M2kDigital(struct iio_context* ctx, std::string logic_dev, bool sync);
	virtual ~M2kDigital();

	void init();

	void setDirection(unsigned short mask);
	void setDirection(unsigned int index, DIO_DIRECTION dir);
	void setDirection(unsigned int index, bool dir);
	void setDirection(DIO_CHANNEL index, bool dir);
	void setDirection(DIO_CHANNEL index, DIO_DIRECTION dir);
	DIO_DIRECTION getDirection(DIO_CHANNEL index);

	void setValueRaw(DIO_CHANNEL index, DIO_LEVEL);
	void setValueRaw(unsigned int index, DIO_LEVEL);
	void setValueRaw(DIO_CHANNEL index, bool level);
	DIO_LEVEL getValueRaw(DIO_CHANNEL index);
	DIO_LEVEL getValueRaw(unsigned int index);

	void push(std::vector<short> const &data);
	void stop();

	std::vector<unsigned short> getSamples(unsigned int nb_samples);

	/* Enable/disable TX channels only*/
	void enableChannel(unsigned int index, bool enable);
	void enableChannel(DIO_CHANNEL index, bool enable);
	void enableAllOut(bool enable);

	bool anyChannelEnabled(DIO_DIRECTION dir);

	void setTrigger(DIO_CHANNEL, libm2k::analog::M2K_TRIGGER_CONDITION);
	void setTrigger(unsigned int, libm2k::analog::M2K_TRIGGER_CONDITION);
	libm2k::analog::M2K_TRIGGER_CONDITION getTrigger(DIO_CHANNEL);
	libm2k::analog::M2K_TRIGGER_CONDITION getTrigger(unsigned int);

	void setTriggerDelay(int delay);
	int getTriggerDelay();

	void setTriggerMode(DIO_TRIGGER_MODE);
	DIO_TRIGGER_MODE getTriggerMode();

	void setOutputMode(DIO_CHANNEL, DIO_MODE);
	void setOutputMode(unsigned int, DIO_MODE);
	DIO_MODE getOutputMode(DIO_CHANNEL);
	DIO_MODE getOutputMode(unsigned int);

	double setSampleRateIn(double samplerate);
	double setSampleRateOut(double samplerate);
	double getSampleRateIn();
	double getSampleRateOut();

	bool getCyclic();
	void setCyclic(bool cyclic);

private:
	class M2kDigitalImpl;
	std::unique_ptr<M2kDigitalImpl> m_pimpl;
};
}
}

#endif //M2KDIGITAL_HPP
