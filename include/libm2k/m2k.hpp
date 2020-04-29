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

#ifndef M2K_H
#define M2K_H

#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>
#include <iostream>
#include <vector>

namespace libm2k {
namespace analog {
class M2kAnalogIn;
class M2kAnalogOut;
class M2kPowerSupply;
}

namespace digital {
class M2kDigital;
}

class M2kCalibration;

namespace context {

/**
* @defgroup m2k M2k
* @brief Contains the representation of the M2K
* @{
* @class M2k
* @brief Controls the ADALM2000
*/
class LIBM2K_API M2k : public virtual Context
{
public:
	/**
	* @private
	*/
	virtual ~M2k() {}


	/**
	 * @brief Reset the M2K device to default settings:
	 *	- Reset calibration coefficients
	 *	- Maximum samplerates
	 *	- Set oversampling ratio to 1
	 *	- Set AnalogIn range to low range
	 *	- Set kernel buffers count to 4 (analog/digital and in/out devices)
	 *	- Disable all TX digital channels and enable all RX digital channels
	 * @note A calibration is needed afterwards
	 */
	virtual void reset() = 0;


	/**
	* @private
	*/
	virtual void deinitialize() = 0;

	/**
	* @private
	*/
	virtual bool calibrate() = 0;


	/**
	* @brief Calibrate the ADC
	*
	* @return On succces, true
	* @return Otherwise, false
	*/
	virtual bool calibrateADC() = 0;


	/**
	* @brief Calibrate the DAC
	*
	* @return On succces, true
	* @return Otherwise, false
	*/
	virtual bool calibrateDAC() = 0;


	/**
	* @private
	*/
	virtual bool resetCalibration() = 0;


	/**
	* @brief Retrieve the Digital object
	*
	* @return On success, a pointer to a Digital object
	* @throw EXC_INVALID_PARAMETER No M2K digital device found
	*/
	virtual libm2k::digital::M2kDigital* getDigital() = 0;


	/**
	* @brief Retrieve the PowerSupply object
	*
	* @return On success, a pointer to a PowerSupply object
	* @throw EXC_INVALID_PARAMETER No M2K power supply
	*/
	virtual libm2k::analog::M2kPowerSupply* getPowerSupply() = 0;


	/**
	* @brief Retrieve the AnalogIn object
	*
	* @return On success, a pointer to an AnalogIn object
	* @return On error, null is returned
	*/
	virtual libm2k::analog::M2kAnalogIn* getAnalogIn() = 0;


	/**
	* @brief Retrieve the AnalogIn object of a device
	*
	* @param dev_name The name corresponding to a device
	* @return On success, a pointer to an AnalogIn object
	* @return If the name does not correspond to any device, null is returned
	*/
	virtual libm2k::analog::M2kAnalogIn* getAnalogIn(std::string dev_name) = 0;



	/**
	* @brief Retrieve the AnalogOut object
	*
	* @return On success, a pointer to an AnalogOut object
	* @return On error, null is returned
	*/
	virtual libm2k::analog::M2kAnalogOut* getAnalogOut() = 0;


	/**
	* @private
	*/
	virtual std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn() = 0;
	/**
	* @private
	*/
	virtual std::vector<libm2k::analog::M2kAnalogOut*> getAllAnalogOut() = 0;


	/**
	 * @brief Initiate the mixed acquisition
	 * @param nb_samples The number of samples that will be retrieved
	 *
	 * @note Only available from firmware v0.26.
	 */
	virtual void startMixedSignalAcquisition(unsigned int nb_samples) = 0;


	/**
	* @brief Get the calibration offset of the DAC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration offset
	*/
	virtual int getDacCalibrationOffset(unsigned int chn) = 0;


	/**
	* @brief Get the calibration gain of the DAC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration gain
	*/
	virtual double getDacCalibrationGain(unsigned int chn) = 0;


	/**
	* @brief Get the calibration offset of the ADC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration offset
	*/
	virtual int getAdcCalibrationOffset(unsigned int chn) = 0;


	/**
	* @brief Get the calibration gain of the ADC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration gain
	*/
	virtual double getAdcCalibrationGain(unsigned int chn) = 0;


	/**
	* @brief Set the calibration offset of the DAC
	* @param chn The index corresponding to a channel
	* @param offset The calibration offset value
	* @note Overrides the calibration coefficients.\n
	* Can be reset by running a calibration.
	*/
	virtual void setDacCalibrationOffset(unsigned int chn, int offset) = 0;


	/**
	* @brief Set the calibration gain of the DAC
	* @param chn The index corresponding to a channel
	* @param gain The calibration gain value
	* @note Overrides the calibration coefficients.\n
	* Can be reset by running a calibration.\n
	* The gain value is currently limited at the (-2,2) range  by the hardware.
	*/
	virtual void setDacCalibrationGain(unsigned int chn, double gain) = 0;


	/**
	* @brief Set the calibration offset of the ADC
	* @param chn The index corresponding to a channel
	* @param offset The calibration offset value
	* @note Overrides the calibration coefficients.\n
	* Can be reset by running a calibration.
	*/
	virtual void setAdcCalibrationOffset(unsigned int chn, int offset) = 0;


	/**
	* @brief Set the calibration gain of the ADC
	* @param chn The index corresponding to a channel
	* @param gain The calibration gain value
	* @note Overrides the calibration coefficients.\n
	* Can be reset by running a calibration.\n
	* The gain value is currently limited at the (-2,2) range  by the hardware.
	*/
	virtual void setAdcCalibrationGain(unsigned int chn, double gain) = 0;


	/**
	* @brief Turn on or off the board's led
	*
	* @param on A boolean value corresponding to the state of the led
	*
	* @note For turning on the led, set the parameter true.
	*/
	virtual void setLed(bool on) = 0;


	/**
	* @brief Retrieve the state of the led
	*
	* @return If the led is turned on, true
	* @return Otherwise, false
	*/
	virtual bool getLed() = 0;
};
}
}
#endif // M2K_H
