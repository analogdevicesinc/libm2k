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
namespace devices {

/** @defgroup Devices
 * @brief Contains the representation of the M2K
 * @{
 * @class M2k
 */
class LIBM2K_API M2k : public Context
{
public:
	M2k(std::string uri, iio_context* ctx, std::string name, bool sync);
	virtual ~M2k();
	void init();
	void deinitialize();

	void scanAllAnalogIn();
	void scanAllAnalogOut();
	void scanAllPowerSupply();
	void scanAllDigital();

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
     * @param std::string The name corresponding to a device
     * @return On success, a pointer to an AnalogIn object
     * @return If the name does not correspond to any device, null is returned
     */
	libm2k::analog::M2kAnalogIn* getAnalogIn(std::string);



    /**
     * @brief Retrieve the AnalogOut object
     *
     * @return On success, a pointer to an AnalogOut object
     * @return On error, null is returned
     */
    libm2k::analog::M2kAnalogOut* getAnalogOut();


	std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn();
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
