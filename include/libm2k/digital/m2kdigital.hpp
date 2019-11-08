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
 * @class M2kDigital
 * @brief Controls the digital input and output segment
 */
class LIBM2K_API M2kDigital
{
public:
	/**
	* @private
	*/
	M2kDigital(struct iio_context* ctx, std::string logic_dev, bool sync, M2kHardwareTrigger *trigger);


	/**
	 * @private
	 */
	virtual ~M2kDigital();


	/**
	* @private
	*/
	void init();


	/**
	* @brief Set the direction for all digital channels
	*
	* @param mask A bitmask
	*
	* @note Each bit of the mask corresponds to the channel with the same index. The value of the bit represents the channel's direction. O - input, 1 - output
	*/
	void setDirection(unsigned short mask);


	/**
	* @brief Set the direction of the given digital channel
	*
	* @param index The index corresponding to the channel
	* @param dir An enumerator that indicates the direction of a channel
	*/
	void setDirection(unsigned int index, DIO_DIRECTION dir);


	/**
	* @brief Set the direction of the given digital channel
	*
	* @param index The index corresponding to the channel
	* @param dir A boolean value that corresponds to one direction
	*
	* @note The boolean value for input direction is false and for output direction is true
	*/
	void setDirection(unsigned int index, bool dir);


	/**
	* @brief Set the direction of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param dir A boolean value that corresponds to one direction
	* @note The boolean value for input direction is false and for output direction is true
	*/
	void setDirection(DIO_CHANNEL index, bool dir);


	/**
	* @brief Set the direction of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param dir An enumerator that indicates the direction of a channel
	*/
	void setDirection(DIO_CHANNEL index, DIO_DIRECTION dir);


	/**
	* @brief Retrieve the direction of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @return DIO_DIRECTION An enumerator that indicates the direction of a channel
	*/
	DIO_DIRECTION getDirection(DIO_CHANNEL index);


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param level An enumerator corresponding to the raw value
	*
	*/
	void setValueRaw(DIO_CHANNEL index, DIO_LEVEL level);


	/**
	* @brief Send the samples to all digital channels
	*
	* @param data The std vector that contains the samples
	*/
	void push(std::vector<unsigned short> const &data);


	/**
	* @brief Send the samples to all digital channels
	*
	* @param data a pointer to the samples
	* @param nb_samples the number of samples
	*/
	void push(unsigned short *data, unsigned int nb_samples);


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index The index corresponding to the channel
	* @param level An enumerator corresponding to the raw value
	*
	*/
	void setValueRaw(unsigned int index, DIO_LEVEL level);


	/**
	* @brief Set the raw value of a given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param level A boolean value corresponding to the raw value
	*
	*/
	void setValueRaw(DIO_CHANNEL index, bool level);


	/**
	* @brief Retrieve the raw value of the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @return DIO_LEVEL An enumerator corresponding to the raw value
	*/
	DIO_LEVEL getValueRaw(DIO_CHANNEL index);


	/**
	* @brief Retrieve the raw value of the given digital channel
	*
	* @param index The index corresponding to the channel
	* @return DIO_LEVEL An enumerator corresponding to the raw value
	*/
	DIO_LEVEL getValueRaw(unsigned int index);


	/**
	* @brief Stop all digital channels from sending the signals
	*/
	void stopBufferOut();


	/**
	 * @brief Destroy the buffer
	 */
	void flushBufferIn();


	/**
	* @brief Retrieve a specific number of samples
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list that contains the samples
	*/
	std::vector<unsigned short> getSamples(unsigned int nb_samples);

	/**
	 * @brief Retrieve a specific number of samples
	 * @param nb_samples The number of samples that will be retrieved
	 * @return A pointer to the data
	 */
	const unsigned short *getSamplesP(unsigned int nb_samples);

	/* Enable/disable TX channels only*/


	/**
	* @brief Enable or disable the given digital channel
	*
	* @param index The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	void enableChannel(unsigned int index, bool enable);


	/**
	* @brief Enable or disable the given digital channel
	*
	* @param index An enumerator corresponding to the channel's index
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	void enableChannel(DIO_CHANNEL index, bool enable);


	/**
	* @brief Enable or disable all output channels
	*
	* @param enable A boolean value corresponding to the channel's state
	*
	* @note This function affects only the TX channels
	*/
	void enableAllOut(bool enable);


	/**
	* @brief Check if there is at least one channel enabled
	*
	* @param dir An enumerator corresponding to the direction of a channel
	* @return On succces, true
	* @return Otherwise, false
	*/
	bool anyChannelEnabled(DIO_DIRECTION dir);


	/**
	* @brief Set the output mode for the given digital channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @param mode An enumerator corresponding to the output mode
	*/
	void setOutputMode(DIO_CHANNEL chn, DIO_MODE mode);


	/**
	* @brief Set the output mode for the given digital channel
	*
	* @param chn The index corresponding to the channel
	* @param mode An enumerator corresponding to the output mode
	*/
	void setOutputMode(unsigned int chn, DIO_MODE mode);


	/**
	* @brief Retrieve the output mode for the given digital channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @return An enumerator corresponding to the output mode
	*/
	DIO_MODE getOutputMode(DIO_CHANNEL chn);


	/**
	* @brief Retrieve the output mode for the given digital channel
	*
	* @param chn The index corresponding to the channel
	* @return An enumerator corresponding to the output mode
	*/
	DIO_MODE getOutputMode(unsigned int chn);


	/**
	* @brief Set the sample rate for all digital input channels
	*
	* @param samplerate A double value to set the sample rate to
	* @return The current sample rate for all digital input channels
	*/
	double setSampleRateIn(double samplerate);


	/**
	* @brief Set the sample rate for all digital output channels
	*
	* @param samplerate A double value to set the sample rate to
	* @return The current sample rate for all digital output channels
	*/
	double setSampleRateOut(double samplerate);


	/**
	* @brief Retrieve the sample rate of all digital input channels
	* @return The value of the sample rate
	*/
	double getSampleRateIn();


	/**
	* @brief Retrieve the sample rate of all digital output channels

	* @return The value of the sample rate
	*/
	double getSampleRateOut();


	/**
	* @brief Retrieve the value of the cyclic mode
	*
	* @return A boolean value corresponding to the state of the cyclic mode
	*/
	bool getCyclic();


	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param cyclic If true, enable cyclic mode
	*/
	void setCyclic(bool cyclic);


	/**
	* @brief Get the hardware trigger handler
	* @return the trigger object
	*/
	libm2k::M2kHardwareTrigger* getTrigger();


	/**
	 * @brief Set the kernel buffers to a specific value
	 * @param count the number of kernel buffers
	 */
	void setKernelBuffersCountIn(unsigned int count);

	/** @} */

private:
	class M2kDigitalImpl;
	std::unique_ptr<M2kDigitalImpl> m_pimpl;
};
}
}
#endif //M2KDIGITAL_HPP
