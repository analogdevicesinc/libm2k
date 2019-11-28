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
#include <libm2k/logger.hpp>

using namespace libm2k;

Logger &Logger::getInstance()
{
    static Logger instance;

    return instance;
}

void Logger::warn(std::string message)
{
#ifdef LIBM2K_LOG4CPP
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.addAppender(appender);

	root << log4cpp::Priority::WARN << message;
#else
	std::cerr << message << std::endl;
#endif
}

Logger::Logger()
{
#ifdef LIBM2K_LOG4CPP
	appender = new log4cpp::OstreamAppender("console", &std::cerr);
	basicLayout = new log4cpp::BasicLayout();
	appender->setLayout(basicLayout);
#endif
}
