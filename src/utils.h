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

#include <string>
#include <vector>

class iio_context;
class iio_device;
class iio_channel;
namespace m2k {
	class Utils {
	public:
		enum MEASURE_UNIT {
			NO_UNIT = 0,
			VOLTS = 1,
			DEGREES = 2,
		};

		static std::vector<std::string> getIioDevByGlobalAttrs(iio_context*, std::vector<std::string> attr_list);

		/**
		 * @brief Utils::getIioDevByChannelAttrs
		 * @param ctx : iio_context*
		 * @param attr_list : list containing the required attributes
		 * @return A list of pairs of type <device_name, channel_name> which have
		 * all the required attributes as channel attributes for the <channel_name>.
		 * The list can also contain pairs of type <device_name, "">, which means
		 * all the required attributes were found as global attributes for the device <device_name>.
		 */
		static std::vector<std::pair<std::string, std::string>>
			getIioDevByChannelAttrs(iio_context*, std::vector<std::string> attr_list);

		static bool iioDevHasAttribute(iio_device* dev, std::string const& attr);

		static bool iioChannelHasAttribute(iio_channel* chn, std::string const& attr);

		static bool isInputChannel(iio_context*, std::string const& dev, std::string const& chn);

		static int getChannelUnit(iio_context*, std::string const& dev, std::string const& chn);

		static double getIioDevTemp(iio_context* ctx, const std::string& d_name);

	};
}


#endif //UTILS_H
