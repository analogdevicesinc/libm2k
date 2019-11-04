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

#ifndef M2KHARDWARETRIGGER_HPP
#define M2KHARDWARETRIGGER_HPP

#include <libm2k/analog/enums.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include <vector>
#include <memory>

namespace libm2k {

/**
 * @addtogroup analog
 * @{
 */
namespace analog {
class M2kAnalogIn;

/**
 * @defgroup m2ktrigger HardwareTrigger
 * @brief Contains the representation of the ADALM2000 trigger system
 * @{
 * @class M2kHardwareTrigger
 * @brief Controls the hardware trigger for ADALM2000
 */
class LIBM2K_API M2kHardwareTrigger
{
public:
	/**
	 * @private
	 */
	M2kHardwareTrigger(struct iio_context *ctx, std::string fw_version);

	/**
	 * @private
	 */
	virtual ~M2kHardwareTrigger();


	/**
	 * @brief Get the raw analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @return the raw analog trigger level
	 */
	int getAnalogLevelRaw(unsigned int chnIdx);


	/**
	 * @brief Set the raw analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @param level the raw analog trigger level
	 */
	void setAnalogLevelRaw(unsigned int chnIdx, int level);


	/**
	 * @brief Set the analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @param v_level the analog trigger level in volts
	 */
	void setAnalogLevel(unsigned int chnIdx, double v_level);


	/**
	 * @brief Get the analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @return the analog trigger level in volts
	 */
	double getAnalogLevel(unsigned int chnIdx);


	/**
	 * @brief Get the analog hysteresis value for one channel
	 * @param chnIdx the required channel
	 * @return the value of the hysteresis
	 */
	int getAnalogHysteresis(unsigned int chnIdx);


	/**
	 * @brief Set the analog hysteresis value for one channel
	 * @param chnIdx the required channel
	 * @param histeresis
	 */
	void setAnalogHysteresis(unsigned int chnIdx, int histeresis);


	/**
	 * @brief Get the trigger condition for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the trigger condition
	 */
	M2K_TRIGGER_CONDITION getAnalogCondition(unsigned int chnIdx);


	/**
	 * @brief Set the trigger condition for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param cond the specific trigger condition
	 */
	void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);


	/**
	 * @brief Get the trigger condition for one of the external trigger pins,
	 *  on a specific channel
	 * @param chnIdx the required channel
	 * @return the trigger condition
	 */
	M2K_TRIGGER_CONDITION getExternalCondition(unsigned int chnIdx);


	/**
	 * @brief Set the trigger condition for one of the external trigger pins,
	 *  on a specific channel
	 * @param chnIdx the required channel
	 * @param cond the trigger condition
	 */
	void setExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);



	/**
	 * @brief Get the trigger condition for the digital trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the trigger condition
	 */
	M2K_TRIGGER_CONDITION getDigitalCondition(unsigned int chnIdx);


	/**
	 * @brief Set the trigger condition for the digital trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param cond the specific trigger condition
	 */
	void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);



	/**
	 * @brief Get the mode for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the analog trigger mode
	 */
	M2K_TRIGGER_MODE getAnalogMode(unsigned int chnIdx);


	/**
	 * @brief Set the mode for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param mode the analog trigger mode
	 */
	void setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode);


	/**
	 * @brief Get the mode for the digital trigger
	 * @return the digital trigger mode (DIO_OR/DIO_AND)
	 */
	libm2k::digital::DIO_TRIGGER_MODE getDigitalMode();


	/**
	 * @brief Set the mode for the digital trigger
	 * @param mode the digital trigger mode (DIO_OR/DIO_AND)
	 */
	void setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode);


	/**
	 * @brief Get the source of the analog trigger
	 * @return analog trigger source (channel1, channel2, etc)
	 */
	M2K_TRIGGER_SOURCE getAnalogSource();


	/**
	 * @brief Set the source of the analog trigger
	 * @param src analog trigger source (channel1, channel2, etc)
	 */
	void setAnalogSource(M2K_TRIGGER_SOURCE src);


	/**
	 * @brief Get the channel source of the analog trigger
	 * @return analog trigger channel source (channel1, channel2)
	 */
	int getAnalogSourceChannel();


	/**
	 * @brief Set the channel source of the analog trigger
	 * @param chnIdx the index of the channel that should be used
	 */
	void setAnalogSourceChannel(unsigned int chnIdx);


	/**
	 * @brief Get the analog trigger delay
	 * @return the value of the delay
	 *
	 * @note the delay can have negative values
	 * The negative values are used to place the trigger inside the buffer
	 * The value of the delay represents the number of samples in buffer before the triggered sample
	 * If the value is 0, the triggered sample will be the first one in the buffer
	 * For a buffer of 1000 samples, in order to have the trigger at the middle, you need to set it to -500
	 * The minimum negative value is -8192
	 *
	 * @note the delay can have positive values
	 * TBD
	 */
	int getAnalogDelay() const;


	/**
	 * @brief Set the analog trigger delay
	 * @param delay the value of the analog delay
	 *
	 * @note the delay can have negative values
	 * The negative values are used to place the trigger inside the buffer
	 * The value of the delay represents the number of samples in buffer before the triggered sample
	 * If the value is 0, the triggered sample will be the first one in the buffer
	 * For a buffer of 1000 samples, in order to have the trigger at the middle, you need to set it to -500
	 * The minimum negative value is -8192
	 *
	 * @note the delay can have positive values
	 * TBD
	 */
	void setAnalogDelay(int delay);


	/**
	 * @brief Get the digital trigger delay
	 * @return the value of the digital delay
	 *
	 * @note the delay can have negative values
	 * The negative values are used to place the trigger inside the buffer
	 * The value of the delay represents the number of samples in buffer before the triggered sample
	 * If the value is 0, the triggered sample will be the first one in the buffer
	 * For a buffer of 1000 samples, in order to have the trigger at the middle, you need to set it to -500
	 * The minimum negative value is -8192
	 *
	 * @note the delay can have positive values
	 * TBD
	 */
	int getDigitalDelay() const;


	/**
	 * @brief Set the digital trigger delay
	 * @param delay the value of the digital delay
	 *
	 * @note the delay can have negative values
	 * The negative values are used to place the trigger inside the buffer
	 * The value of the delay represents the number of samples in buffer before the triggered sample
	 * If the value is 0, the triggered sample will be the first one in the buffer
	 * For a buffer of 1000 samples, in order to have the trigger at the middle, you need to set it to -500
	 * The minimum negative value is -8192
	 *
	 * @note the delay can have positive values
	 * TBD
	 */
	void setDigitalDelay(int delay);


	/**
	* @private
	*/
	struct SETTINGS *getCurrentHwSettings();


	/**
	* @private
	*/
	void setHwTriggerSettings(struct SETTINGS *settings);


	/**
	 * @brief Set the streaming flag for the analog part
	 * @param enable the streaming
	 */
	void setAnalogStreamingFlag(bool enable);


	/**
	 * @brief Get the streaming flag for the analog part
	 * @return whether the streaming is enabled (true/false)
	 */
	bool getAnalogStreamingFlag();


	/**
	 * @brief Set the streaming flag for the digital part
	 * @param enable the streaming
	 */
	void setDigitalStreamingFlag(bool enable);


	/**
	 * @brief Get the streaming flag for the digital part
	 * @return whether the streaming is enabled (true/false)
	 */
	bool getDigitalStreamingFlag();


	/**
	* @private
	*/
	static std::vector<std::string> getAvailableDigitalConditions();


	/**
	* @private
	*/
	void setCalibParameters(unsigned int chnIdx, double scaling, double vert_offset);
private:
	class M2kHardwareTriggerImpl;
	std::unique_ptr<M2kHardwareTriggerImpl> m_pimpl;
};
}
}

#endif //M2KHARDWARETRIGGER_HPP
