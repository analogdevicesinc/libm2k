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

#ifndef DEVICEBUILDER_HPP
#define DEVICEBUILDER_HPP

#include <vector>
#include <string>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>
#include <libm2k/enums.hpp>
#include <unordered_set>
#include <map>

enum ContextTypes {
	CtxFMCOMMS,
	CtxM2K,
	Other
};

namespace libm2k {


namespace context {

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
	 static std::vector<struct libm2k::CONTEXT_INFO*> getContextsInfo();
	/**
	* @private
	*/
	static std::vector<std::string> getAllContexts();
	/**
	 * @private
	 */
	static Context* contextOpen(const char*);
	/**
	* @private
	*/
	static Context* contextOpen(struct iio_context*, const char*);
	/**
	* @private
	*/
	static Context* contextOpen();
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
	static void contextClose(Context*, bool deinit = true);

	static void contextCloseAll();

	static std::string getVersion();

	/**
	* @private
	*/
	static void enableLogging(bool enable);

private:
	static std::map<ContextTypes, std::vector<std::string>> m_dev_map;
	static std::map<ContextTypes, std::string> m_dev_name_map;
	//                std::shared_ptr<M2KImpl> m_pimpl;
	static ContextTypes identifyContext(iio_context *ctx);
	static Context* buildContext(ContextTypes type,
		std::string uri,
		struct iio_context *ctx,
		bool sync,
		bool ownsContext = false);
	static bool m_disable_logging;

	static std::map<std::string, int> reference_count;
	static void incrementReferenceCount(std::string uri);
	static void decrementReferenceCount(std::string uri);
	static bool checkLastReference(std::string uri);
	static Context* searchInConnectedDevices(std::string uri);
};

/**
 * @addtogroup context
 * @{
 * @defgroup contextbuilder ContextBuilder
 * @brief Creates/destroys the context
 * @{
 */


/**
* @private
*/
LIBM2K_API Context* contextOpen();


/**
 * @private
 */
LIBM2K_API Context* contextOpen(const char* uri);


/**
 * @private
 */
LIBM2K_API Context* contextOpen(struct iio_context* ctx, const char* uri);


/**
 * @brief Open an ADALM2000 based on a given uri
 * @param uri Describe the location of the context
 * @return M2k object or nullptr if no ADALM2000 was found/available
 *
 * @note the uri can be something similar to:
 * "ip:192.168.2.1" or "usb:1.6.5"
 */
LIBM2K_API M2k* m2kOpen(const char* uri);


/**
 * @brief Open an ADALM2000 based on an existing iio_context and uri
 * @param ctx A pointer to an iio_context structure
 * @param uri Describe the location of the context
 * @return M2k object or nullptr if no ADALM2000 was found/available
 *
 * @note the uri can be something similar to:
 * "ip:192.168.2.1" or "usb:1.6.5"
 */
LIBM2K_API M2k* m2kOpen(struct iio_context* ctx, const char* uri);


/**
 * @brief Open an ADALM2000
 * @return M2k object or nullptr if no ADALM2000 was found/available
 *
 * @note this will try to open USB connected devices
 */
LIBM2K_API M2k* m2kOpen();


/**
 * @brief List the information about all available contexts
 * @return A list containing the info about available contexts
 */
LIBM2K_API std::vector<struct libm2k::CONTEXT_INFO*> getContextsInfo();


/**
 * @brief List all available contexts
 * @return A list containing the available contexts
 */
LIBM2K_API std::vector<std::string> getAllContexts();


/**
 * @brief Destroy the given context
 * @param ctx The context to be destroyed
 * @param deinit If deinit is set to false, running contexts won't be affected
 */
LIBM2K_API void contextClose(Context* ctx, bool deinit = true);


/**
 * @brief Close all the devices
 */
LIBM2K_API void contextCloseAll();


/**
 * @brief Get library version
 * @return a string containing the libm2k version
 */
LIBM2K_API std::string getVersion();


/**
 * @brief Enable/disable the logging system
 * @param enable If true enable, otherwise disable
 */
LIBM2K_API void enableLogging(bool enable);


/**
 * @}
 * @}
 */
}
}
#endif //M2K_H
