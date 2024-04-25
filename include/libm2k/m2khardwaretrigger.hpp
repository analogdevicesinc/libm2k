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

#ifndef M2KHARDWARETRIGGER_HPP
#define M2KHARDWARETRIGGER_HPP

#include <libm2k/enums.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include <vector>
#include <memory>

namespace libm2k {

/**
 * @defgroup m2ktrigger HardwareTrigger
 * @brief Contains the representation of the ADALM2000 trigger system
 * @{
 * @class M2kHardwareTrigger m2khardwaretrigger.hpp libm2k/m2khardwaretrigger.hpp
 * @brief Controls the hardware trigger for ADALM2000
 */
class LIBM2K_API M2kHardwareTrigger
{
public:
	/**
	 * @private
	 */
	virtual ~M2kHardwareTrigger() {}


	/**
	 * @private
	 */
	virtual void reset() = 0;

	/**
	 * @brief Get the raw analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @return the raw analog trigger level
	 */
	virtual int getAnalogLevelRaw(unsigned int chnIdx) = 0;


	/**
	 * @brief Set the raw analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @param level the raw analog trigger level
	 */
	virtual void setAnalogLevelRaw(unsigned int chnIdx, int level) = 0;


	/**
	 * @brief Set the analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @param v_level the analog trigger level in volts
	 */
	virtual void setAnalogLevel(unsigned int chnIdx, double v_level) = 0;


	/**
	 * @brief Get the analog trigger level for one channel
	 * @param chnIdx the required channel
	 * @return the analog trigger level in volts
	 */
	virtual double getAnalogLevel(unsigned int chnIdx) = 0;


	/**
	 * @brief Get the analog hysteresis value for one channel
	 * @param chnIdx the required channel
	 * @return the value of the hysteresis in Volts
	 */
	virtual double getAnalogHysteresis(unsigned int chnIdx) = 0;


	/**
	 * @brief Set the analog hysteresis value for one channel
	 * @param chnIdx the required channel
	 * @param hysteresis in Volts
	 */
	virtual void setAnalogHysteresis(unsigned int chnIdx, double hysteresis) = 0;


	/**
	 * @brief Get the trigger condition for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the trigger condition
	 */
	virtual M2K_TRIGGER_CONDITION_ANALOG getAnalogCondition(unsigned int chnIdx) = 0;


	/**
	 * @brief Set the trigger condition for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param cond the specific trigger condition
	 */
	virtual void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_ANALOG cond) = 0;


	/**
	 * @brief Get the trigger condition for the digital trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the trigger condition
	 */
	virtual M2K_TRIGGER_CONDITION_DIGITAL getDigitalCondition(unsigned int chnIdx) = 0;


	/**
	 * @brief Set the trigger condition for the digital trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param cond the specific trigger condition
	 */
	virtual void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond) = 0;



	/**
	 * @brief Get the mode for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @return the analog trigger mode
	 */
	virtual M2K_TRIGGER_MODE getAnalogMode(unsigned int chnIdx) = 0;


	/**
	 * @brief Set the mode for the analog trigger on a specific channel
	 * @param chnIdx the required channel
	 * @param mode the analog trigger mode
	 */
	virtual void setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode) = 0;


	/**
	 * @brief Get the mode for the digital trigger
	 * @return the digital trigger mode (DIO_OR/DIO_AND)
	 */
	virtual libm2k::digital::DIO_TRIGGER_MODE getDigitalMode() = 0;


	/**
	 * @brief Set the mode for the digital trigger
	 * @param mode the digital trigger mode (DIO_OR/DIO_AND)
	 */
	virtual void setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode) = 0;


	/**
	 * @brief Get the source of the analog trigger
	 * @return analog trigger source (channel1, channel2, etc)
	 */
	virtual M2K_TRIGGER_SOURCE_ANALOG getAnalogSource() = 0;


	/**
	 * @brief Set the source of the analog trigger
	 * @param src analog trigger source (channel1, channel2, etc)
	 */
	virtual void setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src) = 0;


	/**
	 * @brief Get the channel source of the analog trigger
	 * @return analog trigger channel source (channel1, channel2)
	 */
	virtual int getAnalogSourceChannel() = 0;


	/**
	 * @brief Set the channel source of the analog trigger
	 * @param chnIdx the index of the channel that should be used
	 */
	virtual void setAnalogSourceChannel(unsigned int chnIdx) = 0;


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
	virtual int getAnalogDelay() const = 0;


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
	virtual void setAnalogDelay(int delay) = 0;


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
	virtual int getDigitalDelay() const = 0;


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
	virtual void setDigitalDelay(int delay) = 0;


	/**
	* @private
	*/
	virtual struct SETTINGS *getCurrentHwSettings() = 0;


	/**
	* @private
	*/
	virtual void setHwTriggerSettings(struct SETTINGS *settings) = 0;


	/**
	 * @brief Set the streaming flag for the analog part
	 * @param enable the streaming
	 */
	virtual void setAnalogStreamingFlag(bool enable) = 0;


	/**
	 * @brief Get the streaming flag for the analog part
	 * @return whether the streaming is enabled (true/false)
	 */
	virtual bool getAnalogStreamingFlag() = 0;


	/**
	 * @brief Set the streaming flag for the digital part
	 * @param enable the streaming
	 *
	 * @note The digital condition needs to be set before setting the
	 * digital streaming flag to true, otherwise after setting the streaming
	 * flag to true, a trigger might occur on the old condition.
	 *
	 */
	virtual void setDigitalStreamingFlag(bool enable) = 0;


	/**
	 * @brief Get the streaming flag for the digital part
	 * @return whether the streaming is enabled (true/false)
	 */
	virtual bool getDigitalStreamingFlag() = 0;


	/**
	* @private
	*/
	virtual void setCalibParameters(unsigned int chnIdx, double scaling, double vert_offset) = 0;


	/**
	 * @brief getAnalogExternalCondition
	 * @param chnIdx : integer - the index of the required channel
	 * @return M2K_TRIGGER_CONDITION_DIGITAL
	 */
	virtual M2K_TRIGGER_CONDITION_DIGITAL getAnalogExternalCondition(unsigned int chnIdx) = 0;


	/**
	 * @brief setAnalogExternalCondition
	 * @param chnIdx : integer - the index of the required channel
	 * @param cond : M2K_TRIGGER_CONDITION_DIGITAL
	 */
	virtual void setAnalogExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond) = 0;


	/**
	 * @brief getDigitalExternalCondition
	 * @return M2K_TRIGGER_CONDITION_DIGITAL
	 */
	virtual M2K_TRIGGER_CONDITION_DIGITAL getDigitalExternalCondition() const = 0;


	/**
	 * @brief setDigitalExternalCondition
	 * @param cond : M2K_TRIGGER_CONDITION_DIGITAL
	 */
	virtual void setDigitalExternalCondition(M2K_TRIGGER_CONDITION_DIGITAL cond) = 0;


	/**
	 * @brief Select what should be redirected on the TO (trigger out) pin.\n
	 *	The options are (none, trigger in, digital in, analog in).\n
	 * @param output_select : of type M2K_TRIGGER_OUT_SELECT:\n
	 *	SELECT_TRIGGER_IN - forwards trigger events from TI pin(trigger in);\n
	 *	SELECT_DIGITAL_IN - forwards trigger events from DigitalIn interface;\n
	 *	SELECT_ANALOG_IN  - forwards trigger events from AnalogIn interface;\n
	 *	SELECT_NONE - no trigger event is forwarded;\n
	 *
	 * @note Only available from firmware v0.24.
	 */
	virtual void setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT output_select);


	/**
	 * @brief Check which trigger events are forwarded on the TO (trigger out) pin.
	 * @return M2K_TRIGGER_OUT_SELECT :\n
	 *	SELECT_NONE;\n
	 *	SELECT_TRIGGER_IN;\n
	 *	SELECT_DIGITAL_IN;\n
	 *	SELECT_ANALOG_IN;\n
	 * @note Only available from firmware v0.24.
	 */
	virtual M2K_TRIGGER_OUT_SELECT getAnalogExternalOutSelect();


	/**
	 * @private
	 */
	virtual bool hasExternalTriggerIn() const = 0;


	/**
	 * @private
	 */
	virtual bool hasExternalTriggerOut() const = 0;


	/**
	 * @private
	 */
	virtual bool hasCrossInstrumentTrigger() const = 0;


	/**
	 * @brief Select which interface triggers the DigitalIn.
	 * @param external_src: of type M2K_TRIGGER_SOURCE_DIGITAL:\n
	 *	SRC_TRIGGER_IN - trigger events on the TI(trigger in) pin trigger the DigitalIn interface;\n
	 *	SRC_ANALOG_IN - trigger events on the AnalogIn interface trigger the DigitalIn interface;\n
	 *	SRC_NONE - trigger events on the DigitalIn are conditioned by the internal digital trigger structure;\n
	 * @note Only available from firmware v0.24.
	 */
	virtual void setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src);


	/**
	 * @brief Check which is the source of the DigitalIn interface trigger event.
	 * @return M2K_TRIGGER_SOURCE_DIGITAL :\n
	 *	SRC_TRIGGER_IN;\n
	 *	SRC_ANALOG_IN;\n
	 *	SRC_NONE;\n
	 * @note Only available from firmware v0.24.
	 */
	virtual M2K_TRIGGER_SOURCE_DIGITAL getDigitalSource() const;


	/**
	 * @brief Select which interface triggers the AnalogOut.
	 * @param src: of type M2K_TRIGGER_SOURCE_OUT:\n
	 *	TRIGGER_NONE - trigger events are disabled;\n
	 *	TRIGGER_TI - trigger events on the TI pin will trigger the AnalogOut interface;\n
	 *	TRIGGER_ADC - trigger events on the AnalogIn interface will trigger the AnalogOut interface;\n
	 *	TRIGGER_LA - trigger events on the DigitalIn interface will trigger the AnalogOut interface;\n
	 * @note Only available from firmware v0.33. 
	 */	
	virtual void setAnalogOutTriggerSource(M2K_TRIGGER_SOURCE_OUT src) = 0;


	/**
	 * @brief Returns the source of the AnalogOut trigger event.
	 * @return M2K_TRIGGER_SOURCE_OUT :\n
	 *	TRIGGER_NONE;\n
	 *	TRIGGER_TI;\n
	 *	TRIGGER_ADC;\n
	 *	TRIGGER_LA;\n
	 * @note Only available from firmware v0.33.
	 */
	virtual M2K_TRIGGER_SOURCE_OUT getAnalogOutTriggerSource() const = 0;


	/**
	 * @brief Configures the triggering condition when the source of the AnalogOut trigger event is set to TRIGGER_TI_0. 
	 * @param condition: of type M2K_TRIGGER_CONDITION_OUT:\n
	 *	NONE_OUT - disabled;\n
	 *	LOW_LEVEL_OUT;\n
	 *	HIGH_LEVEL_OUT;\n
	 *	ANY_EDGE_OUT;\n
	 *	RISING_EDGE_OUT;\n
	 *	FALLING_EDGE_OUT;\n
	 * @note For the other sources this condition is ignored, the configuration is forwarded by the corresponding interface.
	 * @note Only available from firmware v0.33.
	 */
	virtual void setAnalogOutTriggerCondition(M2K_TRIGGER_CONDITION_OUT condition) = 0;


	/**
	 * @brief Returns the condition of the AnalogOut trigger event when the source is TI.
	 * @return M2K_TRIGGER_CONDITION_OUT :\n
	 *	NONE_OUT - disabled;\n
	 *	LOW_LEVEL_OUT;\n
	 *	HIGH_LEVEL_OUT;\n
	 *	ANY_EDGE_OUT;\n
	 *	RISING_EDGE_OUT;\n
	 *	FALLING_EDGE_OUT;\n
	 * @note Only available from firmware v0.33.
	 */
	virtual M2K_TRIGGER_CONDITION_OUT getAnalogOutTriggerCondition() const = 0;

	/**
	 * @brief Defines the action to be performed when a trigger event occurs.
	 * @param status: of type M2K_TRIGGER_STATUS_ANALOG_OUT:\n
	 *	DISABLED - no action is performed when a trigger event occurs;\n
	 *	START - the AnalogOut interface starts outputting samples;\n
	 *	STOP - the AnalogOut interface stops outputting samples;\n
	 * @note Only available from firmware v0.33.
	 */
	virtual void setAnalogOutTriggerStatus(M2K_TRIGGER_STATUS_ANALOG_OUT status) = 0;


	/**
	 * @brief Returns the action that is performed when a trigger event occurs.
	 * @return M2K_TRIGGER_STATUS_ANALOG_OUT :\n
	 *	DISABLED;\n
	 *	START;\n
	 *	STOP;\n
	 * @note Only available from firmware v0.33.
	 */
	virtual M2K_TRIGGER_STATUS_ANALOG_OUT getAnalogOutTriggerStatus() const = 0;
};
}

#endif //M2KHARDWARETRIGGER_HPP
