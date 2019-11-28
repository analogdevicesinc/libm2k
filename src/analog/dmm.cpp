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

#include "private/dmm_impl.cpp"

using namespace libm2k::analog;

DMM::DMM(struct iio_context *ctx, std::string dev, bool sync) :
	m_pimpl(std::unique_ptr<DMMImpl>(new DMMImpl(ctx, dev, sync)))
{
}

DMM::~DMM()
{

}

void DMM::init()
{
	m_pimpl->init();
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

