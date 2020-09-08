/*
 * Copyright (c) 2020 Analog Devices Inc.
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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <libm2k/m2kglobal.hpp>
#include <vector>
#include <string>
#include <sstream>

// operations
#define LIBM2K_ATTRIBUTE_WRITE "write"
#define LIBM2K_ATTRIBUTE_READ "read"

namespace libm2k {
    static std::string buildLoggingMessage(const std::vector<const char *> &info, const std::string &message)
    {
        std::stringstream ss;
        for (auto val : info) {
            ss << "[" << val << "]";
        }
        ss << " " << message;
        return ss.str();
    }
}

#ifdef LIBM2K_ENABLE_LOG
#include <glog/logging.h>

#define LIBM2K_LOG(S, M) LOG(S) << (M)
#define LIBM2K_LOG_IF(S, COND,M) LOG_IF(S, COND) << (M)

#else

#define INFO 0
#define WARNING 1
#define ERROR 2
#define FATAL 3

#define LIBM2K_LOG(S, M) do { (void)(S); (void)(M); } while(false)

#define LIBM2K_LOG_IF(S, COND, M) do { (void)(S); (void)(COND); (void)(M); } while(false)

#endif

#endif // LOGGER_HPP
