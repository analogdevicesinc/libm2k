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
#ifndef LOGGER_H
#define LOGGER_H

#ifdef LIBM2K_LOG4CPP

#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>

#endif

#include <string>
#include <iostream>
#include <libm2k/m2kglobal.hpp>

#ifdef LIBM2K_ENABLE_LOG
	#define LOG(x) libm2k::Logger::getInstance().warn(x)
#else
	#define LOG(x)
#endif

namespace libm2k {
class LIBM2K_API Logger
{
public:
    static Logger & getInstance();
    void warn(std::string message);


private:
	Logger();

#ifdef LIBM2K_LOG4CPP

        log4cpp::Appender *appender;
        log4cpp::BasicLayout *basicLayout;

#endif
};
}

#endif // LOGGER_H
