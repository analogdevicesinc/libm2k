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

#include "private/m2kdigital_impl.cpp"

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace libm2k::analog;
using namespace std;


M2kDigital::M2kDigital(struct iio_context *ctx, std::string logic_dev) :
	Device(new M2kDigitalImpl(ctx, logic_dev))
{
	m_pimpl = dynamic_pointer_cast<M2kDigitalImpl>(Device::m_pimpl);
}

M2kDigital::~M2kDigital()
{
}

void M2kDigital::setDirection(unsigned short mask)
{
	m_pimpl->setDirection(mask);
}

void M2kDigital::setDirection(DIO_CHANNEL index, bool dir)
{
	m_pimpl->setDirection(index, dir);
}

void M2kDigital::setDirection(unsigned int index, bool dir)
{
	m_pimpl->setDirection(index, dir);
}

void M2kDigital::setDirection(unsigned int index, DIO_DIRECTION dir)
{
	m_pimpl->setDirection(index, dir);
}

void M2kDigital::setDirection(DIO_CHANNEL index, DIO_DIRECTION dir)
{
	m_pimpl->setDirection(index, dir);
}

DIO_DIRECTION M2kDigital::getDirection(DIO_CHANNEL index)
{
	return m_pimpl->getDirection(index);
}

void M2kDigital::setValueRaw(DIO_CHANNEL index, DIO_LEVEL level)
{
	m_pimpl->setValueRaw(index, level);
}

void M2kDigital::setValueRaw(DIO_CHANNEL index, bool level)
{
	DIO_LEVEL lvl = static_cast<DIO_LEVEL>(level);
	m_pimpl->setValueRaw(index, lvl);
}

DIO_LEVEL M2kDigital::getValueRaw(DIO_CHANNEL index)
{
	return m_pimpl->getValueRaw(index);
}

void M2kDigital::push(std::vector<short> &data)
{
	m_pimpl->push(data);
}

void M2kDigital::stop()
{
	m_pimpl->stop();
}

std::vector<unsigned short> M2kDigital::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

void M2kDigital::enableChannel(unsigned int index, bool enable)
{
	m_pimpl->enableChannel(index, enable);
}

void M2kDigital::enableChannel(DIO_CHANNEL index, bool enable)
{
	m_pimpl->enableChannel(index, enable);
}

void M2kDigital::enableAllOut(bool enable)
{
	m_pimpl->enableAllOut(enable);
}

bool M2kDigital::anyChannelEnabled(DIO_DIRECTION dir)
{
	return m_pimpl->anyChannelEnabled(dir);
}

void M2kDigital::setTrigger(DIO_CHANNEL chn, M2K_TRIGGER_CONDITION cond)
{
	m_pimpl->setTrigger(chn, cond);
}

M2K_TRIGGER_CONDITION M2kDigital::getTrigger(DIO_CHANNEL chn)
{
	return m_pimpl->getTrigger(chn);
}

void M2kDigital::setTriggerDelay(int delay)
{
	m_pimpl->setTriggerDelay(delay);
}

int M2kDigital::getTriggerDelay()
{
	return m_pimpl->getTriggerDelay();
}

void M2kDigital::setTriggerMode(DIO_TRIGGER_MODE trig_mode)
{
	m_pimpl->setTriggerMode(trig_mode);
}

DIO_TRIGGER_MODE M2kDigital::getTriggerMode()
{
	return m_pimpl->getTriggerMode();
}

void M2kDigital::setOutputMode(DIO_CHANNEL chn, DIO_MODE mode)
{
	m_pimpl->setOutputMode(chn, mode);
}

DIO_MODE M2kDigital::getOutputMode(DIO_CHANNEL chn)
{
	return m_pimpl->getOutputMode(chn);
}

double M2kDigital::setSamplerateIn(double samplerate)
{
	return m_pimpl->setSamplerateIn(samplerate);
}

double M2kDigital::setSamplerateOut(double samplerate)
{
	return m_pimpl->setSamplerateOut(samplerate);
}

double M2kDigital::getSamplerateIn()
{
	return m_pimpl->getSamplerateIn();
}

double M2kDigital::getSamplerateOut()
{
	return m_pimpl->getSamplerateOut();
}

bool M2kDigital::getCyclic()
{
	return m_pimpl->getCyclic();
}

void M2kDigital::setCyclic(bool cyclic)
{
	m_pimpl->setCyclic(cyclic);
}

