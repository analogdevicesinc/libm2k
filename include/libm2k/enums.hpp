/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
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

#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>
#include <vector>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
	struct iio_buffer;
}
/**
 * @file libm2k/enums.hpp
 * @brief Generic M2K enumerations
 */

namespace libm2k {

	/**
	 * @private
	 */
	enum CALIBRATION_MODE {
		ADC_REF1,
		ADC_REF2,
		ADC_GND,
		DAC,
		NONE
	};


	/**
	* @struct CALIBRATION_PARAMETERS enums.hpp libm2k/enums.hpp
	* @brief Calibration parameters of m2k
	*/
	struct CALIBRATION_PARAMETERS {
		int adc_offset_ch_1; ///< ADC calibration offset - channel 1
		int adc_offset_ch_2; ///< ADC calibration offset - channel 2
		double adc_gain_ch_1; ///< ADC calibration gain - channel 1
		double adc_gain_ch_2; ///< ADC calibration gain - channel 2
		int dac_a_offset; ///< DAC calibration offset - channel 1
		int dac_b_offset; ///< DAC calibration offset - channel 2
		double dac_a_gain; ///< DAC calibration gain - channel 1
		double dac_b_gain; ///< DAC calibration gain - channel 2
	};


	/**
	 * @private
	 */
	enum GAIN_MODE {
		LOW_GAIN,
		HIGH_GAIN
	};

	/**
	* @enum M2K_EXCEPTION_TYPE
	* @brief M2k exception types
	*
	*/
	enum M2K_EXCEPTION_TYPE {
		EXC_OUT_OF_RANGE = 0,
		EXC_RUNTIME_ERROR = 1,
		EXC_INVALID_PARAMETER = 2,
		EXC_TIMEOUT = 3,
		EXC_INVALID_FIRMWARE_VERSION = 4
	};


	/**
	* @enum M2K_TRIGGER_CONDITION_ANALOG for the analog side
	* @brief Condition of triggering
	*
	*/
	enum M2K_TRIGGER_CONDITION_ANALOG {
		RISING_EDGE_ANALOG = 0,
		FALLING_EDGE_ANALOG = 1,
		LOW_LEVEL_ANALOG = 2,
		HIGH_LEVEL_ANALOG = 3
	};


	/**
	* @enum M2K_TRIGGER_CONDITION_DIGITAL for the digital side
	* @brief Condition of triggering
	*
	*/
	enum M2K_TRIGGER_CONDITION_DIGITAL {
		RISING_EDGE_DIGITAL = 0,
		FALLING_EDGE_DIGITAL = 1,
		LOW_LEVEL_DIGITAL = 2,
		HIGH_LEVEL_DIGITAL = 3,
		ANY_EDGE_DIGITAL = 4,
		NO_TRIGGER_DIGITAL = 5,
	};


	/**
	* @enum M2K_TRIGGER_MODE
	* @brief Select the mode for the analog trigger
	*/
	enum M2K_TRIGGER_MODE {
		ALWAYS = 0, ///< ALWAYS - Disable analog trigger;
		ANALOG = 1, ///< ANALOG - Trigger condition specified only by analog trigger (CH1 and CH2)
		EXTERNAL = 2, ///< EXTERNAL - Trigger condition specified only by external trigger (TI)
		DIGITAL_OR_ANALOG = 3,
		DIGITAL_AND_ANALOG = 4,
		DIGITAL_XOR_ANALOG = 5,
		N_DIGITAL_OR_ANALOG = 6,
		N_DIGITAL_AND_ANALOG = 7,
		N_DIGITAL_XOR_ANALOG = 8,
	};


	/**
	* @enum M2K_TRIGGER_SOURCE_ANALOG
	* @brief Select the source for the analog trigger
	*/
	enum M2K_TRIGGER_SOURCE_ANALOG {
		CHANNEL_1 = 0, ///< CHANNEL_1 - trigger events on analog CHANNEL_1 trigger the AnalogIn interface
		CHANNEL_2 = 1, ///< CHANNEL_2 - trigger events on analog CHANNEL_2 trigger the AnalogIn interface
		CHANNEL_1_OR_CHANNEL_2 = 2,
		CHANNEL_1_AND_CHANNEL_2 = 3,
		CHANNEL_1_XOR_CHANNEL_2 = 4,
		SRC_DIGITAL_IN = 5, ///< SRC_DIGITAL_IN - trigger events on the DigitalIn interface trigger the AnalogIn interface
		CHANNEL_1_OR_SRC_LOGIC_ANALYZER = 6,
		CHANNEL_2_OR_SRC_LOGIC_ANALYZER = 7,
		CHANNEL_1_OR_CHANNEL_2_OR_SRC_LOGIC_ANALYZER = 8,
		NO_SOURCE = 9, ///< NO_SOURCE - block the AnalogIn interface
	};


	/**		
	* @enum M2K_TRIGGER_SOURCE_OUT
	* @brief Selects the source trigger for the output interfaces
	*/
	enum M2K_TRIGGER_SOURCE_OUT {
		TRIGGER_NONE = 0,
		TRIGGER_TI = 1, 
		TRIGGER_ADC = 2,
		TRIGGER_LA = 3,
	};

	/**
	* @enum M2K_TRIGGER_CONDITION_OUT
	* @brief Trigger condition when the source for M2K_TRIGGER_SOURCE_OUT is TRIGGER_TI
	*/
	enum M2K_TRIGGER_CONDITION_OUT {
		NONE_OUT = 0,
		LOW_LEVEL_OUT = 1,
		HIGH_LEVEL_OUT = 2,
		ANY_EDGE_OUT = 3,
		RISING_EDGE_OUT = 4,
		FALLING_EDGE_OUT = 5,
	};

	/**
	* @enum M2K_TRIGGER_STATUS_ANALOG_OUT
	* @brief The status value determines the action that the output interface will take when the trigger condition is met.
	*/
	enum M2K_TRIGGER_STATUS_ANALOG_OUT {
		DISABLED = 0,
		START = 1,
		STOP = 2,
	};	


	/**
	 * @enum M2K_TRIGGER_SOURCE_DIGITAL
	 * @brief Select the source for the digital trigger
	 */
	enum M2K_TRIGGER_SOURCE_DIGITAL {
		SRC_TRIGGER_IN = 0, ///< SRC_TRIGGER_IN - trigger events on the TI(trigger in) pin trigger the DigitalIn interface
		SRC_ANALOG_IN = 1, ///< SRC_ANALOG_IN - trigger events on the AnalogIn interface trigger the DigitalIn interface
		SRC_NONE = 2, ///< SRC_NONE - trigger events on the DigitalIn are conditioned by the internal digital trigger structure
		SRC_DISABLED = 3, ///< SRC_DISABLED - block the DigitalIn interface
	};


	/**
	 * @enum M2K_TRIGGER_OUT_SELECT
	 * @brief Select which trigger event will be forwarded on TO pin (trigger out)
	 */
	enum M2K_TRIGGER_OUT_SELECT {
		SELECT_NONE = 0, ///< SELECT_NONE - no trigger event is forwarded
		SELECT_TRIGGER_I_SAME_CHAN = 1,
		SELECT_TRIGGER_IN = 2, ///< SELECT_TRIGGER_IN - forwards trigger events from TI pin(trigger in)
		SELECT_ANALOG_IN = 3, ///< SELECT_ANALOG_IN  - forwards trigger events from AnalogIn interface
		SELECT_DIGITAL_IN = 4 ///< SELECT_DIGITAL_IN - forwards trigger events from DigitalIn interface
	};


	/**
	* @struct SETTINGS enums.hpp libm2k/enums.hpp
	* @brief Triggering system
	*
	*/
	struct SETTINGS {
		std::vector<M2K_TRIGGER_CONDITION_ANALOG> analog_condition; ///< Analogical trigger's condition
		std::vector<M2K_TRIGGER_CONDITION_DIGITAL> digital_condition; ///< Digital trigger's condition
		std::vector<int> raw_level; ///< Trigger's raw level
		std::vector<double> level; ///< Trigger's level
		std::vector<double> hysteresis; ///< Trigger's hysteresis
		std::vector<M2K_TRIGGER_MODE> mode; ///<Triggering mode
		M2K_TRIGGER_SOURCE_ANALOG trigger_source; ///< Triggering source
		int delay; ///< Trigger's delay
	};


	/**
	 * @struct CONTEXT_INFO enums.hpp libm2k/enums.hpp
	 * @brief Additional information about the context
	 */
	struct CONTEXT_INFO {
		std::string id_vendor; ///< Vendor ID extracted from IIO context
		std::string id_product; ///< Product ID extracted from IIO context
		std::string manufacturer; ///< Manufacturer extracted from IIO context
		std::string product; ///< Product name extracted from IIO context
		std::string serial; ///< Serial number
		std::string uri; ///< IIO context URI
	};


	/**
	 * @private
	 */
	struct IIO_OBJECTS {
		std::vector<iio_channel*> channels_in;
		std::vector<iio_channel*> channels_out;
		std::vector<iio_device*> devices;
		std::vector<iio_buffer*> buffers_rx;
		std::vector<iio_buffer*> buffers_tx;
		iio_context* context;
	};

	/**
	 * @struct IIO_CONTEXT_VERSION enums.hpp libm2k/enums.hpp
	 * @brief The version of the backend
	 */
	struct IIO_CONTEXT_VERSION {
	        unsigned int major; ///< major version
	        unsigned int minor; ///< minor version
	        char git_tag[8]; ///< git tag
	};
}


#endif
