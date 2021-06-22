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

#ifndef M2KANALOGIN_HPP
#define M2KANALOGIN_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <vector>
#include <map>
#include <memory>

namespace libm2k {
/**
 * @defgroup analog Analog
 * @brief Contains all analogical components
 * @{
 */
namespace analog {
/**
 * @defgroup analogin AnalogIn
 * @brief Contains the representation of the analogical input segment
 * @{
 * @class M2kAnalogIn
 * @brief Controls the analogical input compound
 */
class LIBM2K_API M2kAnalogIn
{
public:
	/**
	* @private
	*/
	virtual ~M2kAnalogIn() {}

	/**
	* @private
	*/
	virtual void reset() = 0;

	/**
	 * @brief Create the buffer for both channels and start the acquisition
	 * @param nb_samples size of the buffer in samples for each channel
	 */
	virtual void startAcquisition(unsigned int nb_samples) = 0;


	/**
	 * @brief Destroy the buffer and stop the acquisition
	 */
	virtual void stopAcquisition() = 0;


	/**
	* @brief Retrieve a specific number of samples from each channel
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list containing lists of samples for each channel
	*
	* @note The index of the list corresponds to the index of the channel
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual std::vector<std::vector<double>> getSamples(unsigned int nb_samples) = 0;


	/**
	* @brief Retrieve a specific number of raw samples from each channel
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list containing lists of raw samples for each channel
	*
	* @note The index of the list corresponds to the index of the channel
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples) = 0;


	/**
	* @brief Retrieve a specific number of samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved samples
	*
	* @note Before the acquisition, both channels will be automatically enabled
	* @note The data array will contain samples from both channels
	* @note After the acquisition is finished, the channels will return to their initial state
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual const double* getSamplesInterleaved(unsigned int nb_samples_per_channel) = 0;


	/**
	* @brief Retrieve a specific number of raw samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved raw samples
	*
	* @note Before the acquisition, both channels will be automatically enabled
	* @note The data array will contain samples from both channels
	* @note After the acquisition is finished, the channels will return to their initial state
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual const short* getSamplesRawInterleaved(unsigned int nb_samples_per_channel) = 0;


	/**
	* @brief Retrieve a specific number of samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved samples
	*
	* @note MATLAB specific API wrapper
	* @note Before the acquisition, both channels will be automatically enabled
	* @note The data array will contain samples from both channels
	* @note The data array will contain nb_samples/2 for each channel
	* @note After the acquisition is finished, the channels will return to their initial state
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual const double* getSamplesInterleaved_matlab(unsigned int nb_samples) = 0;


	/**
	* @brief Retrieve a specific number of raw samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved raw samples
	*
	* @note MATLAB specific API wrapper
	* @note Before the acquisition, both channels will be automatically enabled
	* @note The data array will contain samples from both channels
	* @note The data array will contain nb_samples/2 for each channel
	* @note After the acquisition is finished, the channels will return to their initial state
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual const short* getSamplesRawInterleaved_matlab(unsigned int nb_samples) = 0;


	/**
	* @brief Retrieve the average raw value of the given channel
	*
	* @param ch The index corresponding to the channel
	* @return The average raw value
	*/
	virtual short getVoltageRaw(unsigned int ch) = 0;


	/**
	* @brief Retrieve the average voltage of the given channel
	*
	* @param ch The index corresponding to the channel
	* @return The average voltage
	*/
	virtual double getVoltage(unsigned int ch) = 0;


	/**
	* @brief Retrieve the average raw value of the given channel
	*
	* @param ch  An enumerator corresponding to the channel's index
	* @return The average raw value
	*/
	virtual short getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch) = 0;


	/**
	* @brief Retrieve the average voltage of the given channel
	*
	* @param ch An enumerator corresponding to the channel's index
	* @return The average voltage
	*/
	virtual double getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch) = 0;


	/**
	* @brief Retrieve the average raw value for each channel
	*
	* @return A list containing the average raw value of each channel
	*
	* @note The index of the value corresponds to the channel's index
	*/
	virtual std::vector<short> getVoltageRaw() = 0;


	/**
	* @brief Retrieve the average voltage for each channel
	*
	* @return A list containing the average voltage of each channel
	*
	* @note The index of the voltage corresponds to the channel's index
	*/
	virtual std::vector<double> getVoltage() = 0;


	/**
	* @brief Retrieve the average raw value for both channels
	*
	* @return A pointer to the average raw value of both channels
	*/
	virtual const short *getVoltageRawP() = 0;


	/**
	* @brief Retrieve the average voltage for both channels
	*
	* @return A pointer to the average voltage of both channels
	*/
	virtual const double *getVoltageP() = 0;

	/**
	 * @brief Set the vertical offset, in Volts, of a specific channel
	 * @param channel the index of the channel
	 * @param vertOffset the value of the offset in Volts
	 */
	virtual void setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset) = 0;

	/**
	 * @brief getVerticalOffset
	 * @param channel the index of the channel
	 * @return the value of the offset in Volts
	 */
	virtual double getVerticalOffset(ANALOG_IN_CHANNEL channel) = 0;


	/**
	* @brief Retrieve the scaling factor
	*
	* @param ch An enumerator corresponding to the channel's index
	* @return The value of the scaling factor
	*/
	virtual double getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch) = 0;


	/**
	* @brief Set the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param range  An enumerator corresponding to a range
	*/
	virtual void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range) = 0;


	/**
	* @brief Set the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param min Upper bound
	* @param max Lower bound
	*/
	virtual void setRange(ANALOG_IN_CHANNEL channel, double min, double max) = 0;


	/**
	* @brief Retrieve the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @return An enumerator corresponding to the range
	*/
	virtual libm2k::analog::M2K_RANGE getRange(libm2k::analog::ANALOG_IN_CHANNEL channel) = 0;


	/**
	* @brief Retrieve the bounds for the given range
	*
	* @param range An enumerator corresponding to the range
	* @return A pair containing the lower and upper bound
	*/
	virtual std::pair<double, double> getRangeLimits(libm2k::analog::M2K_RANGE range) = 0;


	/**
	* @brief Retrieve all ranges that are available
	*
	* @return A list of pairs containing all available ranges and their name
	*/
	virtual std::vector<std::pair<std::string, std::pair<double, double>>> getAvailableRanges() = 0;


	/**
	* @brief Retrieve the global oversampling ratio
	*
	* @return The value of the global ratio
	*/
	virtual int getOversamplingRatio() = 0;


	/**
	* @brief Retrieve the oversampling ratio for the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @return The ratio value
	*/
	virtual int getOversamplingRatio(unsigned int chn_idx) = 0;


	/**
	* @brief Set the global oversampling ratio
	*
	* @param oversampling Integer value to set the oversampling ratio to
	* @return The current value of the global ratio
	*/
	virtual int setOversamplingRatio(int oversampling) = 0;


	/**
	* @brief Set the oversampling ratio for the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @param oversampling Integer value to set the oversampling ratio to
	* @return The current ratio value
	*/
	virtual int setOversamplingRatio(unsigned int chn_idx, int oversampling) = 0;


	/**
	* @brief Retrieve the global sample rate
	*
	* @return The value of the sample rate
	*/
	virtual double getSampleRate() = 0;


	/**
	 * @brief getAvailableSampleRates
	 * @return The list of available samplerates for this device
	 */
	virtual std::vector<double> getAvailableSampleRates() = 0;

	/**
	* @brief Set the global sample rate
	*
	* @param samplerate A double value to set the sample rate to
	* @return The value of the global sample rate
	*/
	virtual double setSampleRate(double samplerate) = 0;


	/**
	* @brief Retrieve the bounds of the analogical trigger's hysteresis for the given channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @return A pair containing the lower and upper bound
	*/
	virtual std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL chn) = 0;


	/**
	* @brief Retrieve the filter compensation for the given sample rate
	*
	* @param samplerate A double value representing the sample rate
	* @return The value of the filter compensation
	*/
	virtual double getFilterCompensation(double samplerate) = 0;


	/**
	* @brief Retrieve the numeric value corresponding to the given range
	*
	* @param range
	* @return double
	*/
	virtual double getValueForRange(M2K_RANGE range) = 0;


	/**
	 * @brief Convert the raw value of a sample into volts
	 * @param raw: the raw value of a sample;
	 * @param channel: The index corresponding to the channel;
	 * @return The value of a sample converted into volts;
	 */
	virtual double convertRawToVolts(unsigned int channel, short raw) = 0;


	/**
	* @brief Convert the voltage value of a sample into raw
	*
	* @param voltage The voltage value of a sample;
	* @param channel The index corresponding to the channel;
	* @return The value of a sample converted into raw;
	*/
	virtual short convertVoltsToRaw(unsigned int channel, double voltage) = 0;


	/**
	* @brief Retrieve the number of analogical channels
	*
	* @return The number of channels
	*/
	virtual unsigned int getNbChannels() = 0;


	/**
	* @brief Retrieve the name of the device
	*
	* @return The name of the device
	*/
	virtual std::string getName() = 0;


	/**
	* @brief Enable or disable the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*/
	virtual void enableChannel(unsigned int chnIdx, bool enable) = 0;


	/**
	* @brief Retrieve the state of the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @return True if the channel is enabled
	* @return False if the channel is disabled
	*/
	virtual bool isChannelEnabled(unsigned int chnIdx) = 0;


	/**
	* @brief Cancel all buffer operations
	* @note Should be used to cancel an ongoing acquisition
	*/
	virtual void cancelAcquisition() = 0;


	/**
	 * @brief Set the kernel buffers to a specific value
	 * @param count the number of kernel buffers
	 */
	virtual void setKernelBuffersCount(unsigned int count) = 0;

	/**
	 * @brief Get the number of kernel buffers
	 * @return the number of previously set kernel buffers (saved in this session)
	 */
	virtual unsigned int getKernelBuffersCount() const = 0;


	/**
	* @brief Get the hardware trigger handler
	*
	* @return A pointer to the hardware trigger trigger
	*/
	virtual libm2k::M2kHardwareTrigger* getTrigger() = 0;

	/**
	 * @brief Get access to IIO channels, buffers, devices and context.
	 * Can be used when debugging directly with libiio.
	 * @return IIO_OBJECTS structure.
	 */
	virtual struct IIO_OBJECTS getIioObjects() = 0;


	/**
	* @brief Retrieve a specific number of samples from each channel
	*
	* @param data - a reference to a vector owned/created by the client
	* @param nb_samples The number of samples that will be retrieved
	*
	* @note The vector will be cleaned and then filled with samples
	* @note The index of the list corresponds to the index of the channel
	* @note Due to a hardware limitation, the number of samples must
	* be a multiple of 4 and greater than 16.
	*/
	virtual void getSamples(std::vector<std::vector<double>> &data, unsigned int nb_samples) = 0;

	/**
	 * @brief Get the channel name for each ADC channel
	 * @param channel - unsigned int representing the index of the channel
	 * @return std::string - name of the channel
	 */
	virtual std::string getChannelName(unsigned int channel) = 0;

	/**
	 * @brief Get the maximum samplerate for the ADC
	 * @return double - the value of the maximum samplerate
	 */
	virtual double getMaximumSamplerate() = 0;

};
}
}

#endif //M2KANALOGIN_HPP
