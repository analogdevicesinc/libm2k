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

#ifndef UTILS_H
#define UTILS_H

#include <iio.h>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/enums.hpp>
#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

namespace libm2k {
namespace utils {
	class Utils {
	public:
		Utils() {

		}

		~Utils() {

		}
		/**
		 * @brief Utils::getIioDevByChannelAttrs
		 * @param ctx : iio_context*
		 * @param attr_list : list containing the required attributes
		 * @return A list of pairs of type <device_name, channel_name> which have
		 * all the required attributes as channel attributes for the <channel_name>.
		 * The list can also contain pairs of type <device_name, "">, which means
		 * all the required attributes were found as global attributes for the device <device_name>.
		 */

		static std::vector<ini_device_struct> parseIniFile(std::string path);

		static std::unordered_set<std::string> getAllDevices(iio_context *ctx);

		static std::vector<std::string> valuesForIniConfigKey(const ini_device_struct &iniconf,
								const std::string &key);
		static bool devicesFoundInContext(iio_context *ctx, std::vector<std::string> device_list);

		static std::string getHardwareRevision(struct iio_context *ctx);

		static double average(double *data, size_t numElements);

		static std::vector<double> getAvailableSamplerates(struct iio_device*);

		static DEVICE_DIRECTION getIioDeviceDirection(iio_device *dev);
		static std::vector<std::string> split(std::string, std::string);
	private:
		static std::string parseIniSection(std::string line);
		static std::pair<std::string, std::vector<std::string>>
			parseIniPair(std::string line);
	};
}
}


#endif //UTILS_H
