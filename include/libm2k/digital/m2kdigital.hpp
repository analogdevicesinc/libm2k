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

#ifndef M2KDIGITAL_HPP
#define M2KDIGITAL_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <string>
#include <vector>

namespace libm2k {

namespace digital {


/**
 * @defgroup digital Digital
 * @brief Contains the representation of the digital segment
 * @{
 * @class M2kDigital m2kdigital.hpp libm2k/digital/m2kdigital.hpp
 * @brief Controls the digital input and output segment
 */
class LIBM2K_API M2kDigital
{
public:
	virtual ~M2kDigital() {}


	/**
	* @private
	*/
	virtual void reset() = 0;


	/**
	* @brief Set the direction for all digital channels
	* @param mask A bitmask
	* @note Each bit of the mask corresponds to the channel with the same index. The value of the bit represents the channel's direction. O - input, 1 - output
	*/
	virtual void setDirection(unsigned short mask) = 0;


	/**
	* @brief Set the direction of the given digital channel
	* @param index The index corresponding to the channel
	* @param dir An enumerator that indicates the direction of a channel
	*/
	virtual void setDirection(unsigned int index, DIO_DIRECTION dir) = 0;


	/**
	* @brief Set the direction of the given digital channel
	* @param index The index corresponding to the channel
	* @param dir A boolean value that corresponds to one direction
	* @note The boolean value for input direction is false and for output direction is true
	*/
	virtual void setDirection(unsigned int index, bool dir) = 0;


	/**
	* @brief Set the direction of the given digital channel
	* @param index An enumerator corresponding to the channel's index
	* @param dir A boolean value that corresponds to one direction
	* @note The boolean value for input direction is false and for output direction is true
	*/
	virtual void setDirection(DIO_CHANNEL index, bool dir) = 0;


	/**
	* @brief Set the direction of the given digital channel
	* @param index An enumerator corresponding to the channel's index
	* @param dir An enumerator that indicates the direction of a channel
	*/
	virtual void setDirection(DIO_CHANNEL index, DIO_DIRECTION dir) = 0;


	/**
	* @brief Retrieve the direction of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @return DIO_DIRECTION An enumerator that indicates the direction of a channel
	*/
	virtual DIO_DIRECTION getDirection(DIO_CHANNEL index) = 0;


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param level An enumerator corresponding to the raw value
	*
	*/
	virtual void setValueRaw(DIO_CHANNEL index, DIO_LEVEL level) = 0;


	/**
	* @brief Send the samples to all digital channels
	*
	* @param data The std vector that contains the samples
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual void push(std::vector<unsigned short> const &data) = 0;


	/**
	* @brief Send the samples to all digital channels
	*
	* @param data a pointer to the samples
	* @param nb_samples the number of samples
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual void push(unsigned short *data, unsigned int nb_samples) = 0;


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index The index corresponding to the channel
	* @param level An enumerator corresponding to the raw value
	*
	*/
	virtual void setValueRaw(unsigned int index, DIO_LEVEL level) = 0;


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param level A boolean value corresponding to the raw value
	*
	*/
	virtual void setValueRaw(DIO_CHANNEL index, bool level) = 0;


	/**
	* @brief Retrieve the raw value of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @return DIO_LEVEL An enumerator corresponding to the raw value
	*/
	virtual DIO_LEVEL getValueRaw(DIO_CHANNEL index) = 0;


	/**
	* @brief Retrieve the raw value of the given digital channel
	*
	* @param index The index corresponding to the channel
	* @return DIO_LEVEL An enumerator corresponding to the raw value
	*/
	virtual DIO_LEVEL getValueRaw(unsigned int index) = 0;


	/**
	* @brief Stop all digital channels from sending the signals
	*/
	virtual void stopBufferOut() = 0;

	/**
	 * @brief Create the buffer for all channels and start the acquisition
	 * @param nb_samples size of the buffer in samples
	 */
	virtual void startAcquisition(unsigned int nb_samples) = 0;


	/**
	 * @brief Destroy the buffer and stop the acquisition
	 */
	virtual void stopAcquisition() = 0;


	/**
	* @brief Cancel all rx-buffer operations
	* @note Should be used to cancel an ongoing acquisition
	*/
	virtual void cancelAcquisition() = 0;

	/**
	* @brief Cancel all tx-buffer operations
	* @note Should be used to cancel an ongoing data write.
	*/
	virtual void cancelBufferOut() = 0;


	/**
	* @brief Retrieve a specific number of samples
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list that contains the samples
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual std::vector<unsigned short> getSamples(unsigned int nb_samples) = 0;

	/**
	 * @brief Retrieve a specific number of samples
	 * @param nb_samples The number of samples that will be retrieved
	 * @return A pointer to the data
	 * @note Due to a hardware limitation, the number of samples must
	 * be a multiple of 4 and greater than 16.
	 */
	virtual const unsigned short *getSamplesP(unsigned int nb_samples) = 0;

	/* Enable/disable TX channels only*/


	/**
	* @brief Enable or disable the given digital channel
	*
	* @param index The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	virtual void enableChannel(unsigned int index, bool enable) = 0;


	/**
	* @brief Enable or disable the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	virtual void enableChannel(DIO_CHANNEL index, bool enable) = 0;


	/**
	* @brief Enable or disable all output channels
	*
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	virtual void enableAllOut(bool enable) = 0;


	/**
	* @brief Check if there is at least one channel enabled
	*
	* @param dir An enumerator corresponding to the direction of a channel
	* @return On succces, true
	* @return Otherwise, false
	*/
	virtual bool anyChannelEnabled(DIO_DIRECTION dir) = 0;


	/**
	* @brief Set the output mode for the given digital channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @param mode An enumerator corresponding to the output mode
	*/
	virtual void setOutputMode(DIO_CHANNEL chn, DIO_MODE mode) = 0;


	/**
	* @brief Set the output mode for the given digital channel
	*
	* @param chn The index corresponding to the channel
	* @param mode An enumerator corresponding to the output mode
	*/
	virtual void setOutputMode(unsigned int chn, DIO_MODE mode) = 0;


	/**
	* @brief Retrieve the output mode for the given digital channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @return An enumerator corresponding to the output mode
	*/
	virtual DIO_MODE getOutputMode(DIO_CHANNEL chn) = 0;


	/**
	* @brief Retrieve the output mode for the given digital channel
	*
	* @param chn The index corresponding to the channel
	* @return An enumerator corresponding to the output mode
	*/
	virtual DIO_MODE getOutputMode(unsigned int chn) = 0;


	/**
	* @brief Set the sample rate for all digital input channels
	*
	* @param samplerate A double value to set the sample rate to
	* @return The current sample rate for all digital input channels
	*/
	virtual double setSampleRateIn(double samplerate) = 0;


	/**
	* @brief Set the sample rate for all digital output channels
	*
	* @param samplerate A double value to set the sample rate to
	* @return The current sample rate for all digital output channels
	*/
	virtual double setSampleRateOut(double samplerate) = 0;


	/**
	* @brief Retrieve the sample rate of all digital input channels
	* @return The value of the sample rate
	*/
	virtual double getSampleRateIn() = 0;


	/**
	* @brief Retrieve the sample rate of all digital output channels

	* @return The value of the sample rate
	*/
	virtual double getSampleRateOut() = 0;


	/**
	* @brief Retrieve the value of the cyclic mode
	*
	* @return A boolean value corresponding to the state of the cyclic mode
	*/
	virtual bool getCyclic() = 0;


	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param cyclic If true, enable cyclic mode
	*/
	virtual void setCyclic(bool cyclic) = 0;


	/**
	* @brief Get the hardware trigger handler
	* @return the trigger object
	*/
	virtual libm2k::M2kHardwareTrigger* getTrigger() = 0;


	/**
	 * @brief Set the kernel buffers for input to a specific value
	 * @param count the number of kernel buffers
	 */
	virtual void setKernelBuffersCountIn(unsigned int count) = 0;


	/**
	 * @brief Set the kernel buffers for output to a specific value
	 * @param count the number of kernel buffers
	 */
	virtual void setKernelBuffersCountOut(unsigned int count) = 0;


	/**
	 * @brief Get access to IIO channels, buffers, devices and context.
	 * Can be used when debugging directly with libiio.
	 * @return IIO_OBJECTS structure.
	 */
	virtual struct IIO_OBJECTS getIioObjects() = 0;


	/**
	* @brief Retrieve the number of digital input channels
	*
	* @return an unsigned int representing the number of digital IN channels
	*/
	virtual unsigned int getNbChannelsIn() = 0;


	/**
	* @brief Retrieve the number of digital output channels
	*
	* @return an unsigned int representing the number of digital OUT channels
	*/
	virtual unsigned int getNbChannelsOut() = 0;


	/**
	 * @brief Retrieve a specific number of samples
	 * @param data - a reference to a vector owned/created by the client
	 * @param nb_samples The number of samples that will be retrieved. The vector will be cleaned and then filled with samples.
	 * @note Due to a hardware limitation, the number of samples must
	 * be a multiple of 4 and greater than 16.
	 */
	virtual void getSamples(std::vector<unsigned short> &data, unsigned int nb_samples) = 0;


	/**
	 * @brief Force the digital interface to use the analogical rate
	 *
	 * @note Only available from firmware v0.26.
	 */
	virtual void setRateMux() = 0;


	/**
	 * @brief Reset the digital rate to default
	 *
	 * @note Only available from firmware v0.26.
	 */
	virtual void resetRateMux() = 0;

	/**
	 * @brief Set the clocksource
	 * @param external - True to set source to external
	 *		   - False to set source to internal
	 *
	 * @note Only available from firmware v0.26.
	 */
	virtual void setExternalClocksource(bool external) = 0;

	/**
	 * @brief Get the current clocksource
	 * @return True if clocksource is set to external
	 *
	 *
	 * @note Only available from firmware v0.26.
	 */
	virtual bool isClocksourceExternal() = 0;
};
}
}
#endif //M2KDIGITAL_HPP
