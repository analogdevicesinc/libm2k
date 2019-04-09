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

#include "private/dmm_impl.cpp"

using namespace libm2k::analog;

DMM::DMM(struct iio_context *ctx, std::string dev) :
	Device(new DMMImpl(ctx, dev))
{
	m_pimpl = dynamic_pointer_cast<DMMImpl>(Device::m_pimpl);
}

DMM::~DMM()
{

}

DMM_READING DMM::readChannel(unsigned int index)
{
	return m_pimpl->readChannel(index);
}

std::vector<std::string> DMM::getAllChannels()
{
	return m_pimpl->getAllChannels();
}

DMM_READING DMM::readChannel(std::string chn_name)
{
	return m_pimpl->readChannel(chn_name);
}

std::vector<DMM_READING> DMM::readAll()
{
	return m_pimpl->readAll();
}

string DMM::getName()
{
	return m_pimpl->getName();
}

