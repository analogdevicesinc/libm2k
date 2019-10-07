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

#ifndef DEVICEBUILDER_HPP
#define DEVICEBUILDER_HPP

#include <vector>
#include <string>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>
#include <unordered_set>
#include <map>

enum DeviceTypes {
	DevFMCOMMS,
	DevM2K,
	Other
};

namespace libm2k {


namespace devices {

    /**
     * @private
     */
class LIBM2K_API ContextBuilder {
	static std::vector<Context*> s_connectedDevices;
public:
	/**
	* @private
	*/
	explicit ContextBuilder();
	/**
	* @private
	*/
	~ContextBuilder();
	/**
	* @private
	*/
	static std::vector<std::string> listDevices();
	/**
	 * @private
	 */
	static Context* deviceOpen(const char*);
	/**
	* @private
	*/
	static Context* deviceOpen(struct iio_context*, const char*);
	/**
	* @private
	*/
	static Context* deviceOpen();
	/**
	* @private
	*/
	static M2k* m2kOpen(const char*);
	/**
	* @private
	*/
	static M2k* m2kOpen(struct iio_context*, const char*);
	/**
	* @private
	*/
	static M2k* m2kOpen();
	/**
	* @private
	*/
	static void deviceClose(Context*, bool deinit = true);

	static void deviceCloseAll();
private:
	static std::map<DeviceTypes, std::vector<std::string>> m_dev_map;
	static std::map<DeviceTypes, std::string> m_dev_name_map;
	//                std::shared_ptr<M2KImpl> m_pimpl;
	static DeviceTypes identifyDevice(iio_context *ctx);
	static Context* buildDevice(DeviceTypes type,
		std::string uri,
		struct iio_context *ctx,
		bool sync);

};

/**
 * @addtogroup devices
 * @{
 * @defgroup context ContextBuilder
 * @brief Creates/destroys the context
 * @{
 */


/**
* @private
*/
LIBM2K_API Context* deviceOpen();


/**
 * @private
 */
LIBM2K_API Context* deviceOpen(const char* uri);


/**
 * @private
 */
LIBM2K_API Context* deviceOpen(struct iio_context* ctx, const char* uri);


/**
 * @brief Open a device based on its uri
 * @param uri Describe the location of the context
 * @return M2k object
 */
LIBM2K_API M2k* m2kOpen(const char* uri);


/**
 * @brief Open a device based on its uri
 * @param ctx A pointer to an iio_context structure
 * @param uri Describe the location of the context
 * @return M2k object
 */
LIBM2K_API M2k* m2kOpen(struct iio_context* ctx, const char* uri);


/**
 *@brief Open a device based on its USB uri
 * @return  M2k object
 */
LIBM2K_API M2k* m2kOpen();


/**
 * @brief List all available devices
 * @return A list containing the available devices
 */
LIBM2K_API std::vector<std::string> listDevices();


/**
 * @brief Destroy the given context
 * @param ctx The context to be destroyed
 * @param deinit If deinit is set to false, running contexts won't be affected
 */
LIBM2K_API void deviceClose(Context* ctx, bool deinit = true);


/**
 * @brief Close all the devices
 */
LIBM2K_API void deviceCloseAll();

/**
 * @}
 * @}
 */
}
}
#endif //M2K_H
