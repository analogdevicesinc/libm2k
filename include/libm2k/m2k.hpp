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

/** @defgroup contexts Contexts
 * @brief Contains the representation of the M2K
 * @{
 */
namespace contexts {

/**
* @defgroup m2k M2k
* @brief Contains the representation of the M2K
* @{
* @class M2k
* @brief Controls the ADALM2000
*/
class LIBM2K_API M2k : public Context
{
public:
	/**
	* @private
	*/
	M2k(std::string uri, iio_context* ctx, std::string name, bool sync);


	/**
	* @private
	*/
	virtual ~M2k();


	/**
	 * @brief Initialize the M2K device to default settings:
	 *	- maximum samplerates
	 *	- set oversampling ratio to 1
	 *	- set AnalogIn range to low range
	 *	- set kernel buffers count to 1 (both analog and digital)
	 *	- reset calibration coefficients - a calibration is needed afterwards
	 *	- disable all TX digital channels and enable all RX digital channels
	 */
	void init();


	/**
	* @private
	*/
	void deinitialize();


	/**
	* @private
	*/
	void scanAllAnalogIn();


	/**
	* @private
	*/
	void scanAllAnalogOut();


	/**
	* @private
	*/
	void scanAllPowerSupply();


	/**
	* @private
	*/
	void scanAllDigital();


	/**
	* @private
	*/
	void calibrate();


	/**
	* @brief Calibrate the ADC
	*
	* @return On succces, true
	* @return Otherwise, false
	*/
	bool calibrateADC();


	/**
	* @brief Calibrate the DAC
	*
	* @return On succces, true
	* @return Otherwise, false
	*/
	bool calibrateDAC();


	/**
	* @private
	*/
	bool resetCalibration();


	/**
	* @brief Retrieve the Digital object
	*
	* @return On success, a pointer to a Digital object
	* @throw EXC_INVALID_PARAMETER No M2K digital device found
	*/
	libm2k::digital::M2kDigital* getDigital();


	/**
	* @brief Retrieve the PowerSupply object
	*
	* @return On success, a pointer to a PowerSupply object
	* @throw EXC_INVALID_PARAMETER No M2K power supply
	*/
	libm2k::analog::M2kPowerSupply* getPowerSupply();


	/**
	* @brief Retrieve the AnalogIn object
	*
	* @return On success, a pointer to an AnalogIn object
	* @return On error, null is returned
	*/
	libm2k::analog::M2kAnalogIn* getAnalogIn();


	/**
	* @brief Retrieve the AnalogIn object of a device
	*
	* @param dev_name The name corresponding to a device
	* @return On success, a pointer to an AnalogIn object
	* @return If the name does not correspond to any device, null is returned
	*/
	libm2k::analog::M2kAnalogIn* getAnalogIn(std::string dev_name);



	/**
	* @brief Retrieve the AnalogOut object
	*
	* @return On success, a pointer to an AnalogOut object
	* @return On error, null is returned
	*/
	libm2k::analog::M2kAnalogOut* getAnalogOut();


	/**
	* @private
	*/
	std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn();
	/**
	* @private
	*/
	std::vector<libm2k::analog::M2kAnalogOut*> getAllAnalogOut();


	/**
	* @brief Get the calibration offset of the DAC-B
	*
	* @return The value of the calibration offset
	*/
	int getDacBCalibrationOffset();


	/**
	* @brief Get the calibration offset of the DAC-A
	*
	* @return The value of the calibration offset
	*/
	int getDacACalibrationOffset();


	/**
	* @brief Get the calibration gain of the DAC-B
	*
	* @return The value of the calibration gain
	*/
	double getDacBCalibrationGain();


	/**
	* @brief Get the calibration gain of the DAC-A
	*
	* @return The value of the calibration gain
	*/
	double getDacACalibrationGain();


	/**
	* @brief Get the calibration offset of the ADC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration offset
	*/
	int getAdcCalibrationOffset(unsigned int chn);


	/**
	* @brief Get the calibration gain of the ADC
	*
	* @param chn The index corresponding to a channel
	* @return The value of the calibration gain
	*/
	double getAdcCalibrationGain(unsigned int chn);

	/**
	* @brief Set a timeout for I/O operations
	*
	* @param timeout A positive integer representing the time in milliseconds after which a timeout occurs. A value of 0 is used to specify that no timeout should occur.
	*/
	void setTimeout(unsigned int timeout);


	/**
	* @brief Turn on or off the board's led
	*
	* @param on A boolean value corresponding to the state of the led
	*
	* @note For turning on the led, set the parameter true.
	*/
	void setLed(bool on);


	/**
	* @brief Retrieve the state of the led
	*
	* @return If the led is turned on, true
	* @return Otherwise, false
	*/
	bool getLed();
private:
	class M2kImpl;
	std::shared_ptr<M2kImpl> m_pimpl;
};
}
}
#endif // M2K_H
