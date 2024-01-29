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

#ifndef M2KANALOGOUT_HPP
#define M2KANALOGOUT_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/enums.hpp>
#include <vector>
#include <memory>
#include <map>

extern "C" {
	struct iio_context;
}

namespace libm2k {

/**
 * @addtogroup analog
 * @{
 */
namespace analog {


/**
 * @defgroup analogout AnalogOut
 * @brief Contains the representation of the analogical output segment
 * @{
 * @class M2kAnalogOut
 * @brief Controls the analogical output compound
 */
class LIBM2K_API M2kAnalogOut
{
public:
	/**
	 * @private
	*/
	virtual ~M2kAnalogOut() {}


	/**
	* @private
	*/
	virtual void reset() = 0;



	/**
	* @brief Retrieve the global oversampling ratio
	* @return The value of the global oversampling ratio
	*/
	virtual std::vector<int> getOversamplingRatio() = 0;


	/**
	* @brief Retrieve the oversampling ratio for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The oversampling ratio value
	*/
	virtual int getOversamplingRatio(unsigned int chn) = 0;

	/**
	* @brief Set the value of the oversampling ratio for each channel
	*
	* @param oversampling_ratio A list containing the ratios for each channel (as integers)
	* @return A list containing the oversampling ratio value for each channel
	*/
	virtual std::vector<int> setOversamplingRatio(std::vector<int> oversampling_ratio) = 0;


	/**
	* @brief Set the oversampling ratio for the given channel
	* @param chn The index corresponding to the channel
	* @param oversampling_ratio Integer value to set the oversampling ratio to
	* @return The current oversampling ratio value
	*/
	virtual int setOversamplingRatio(unsigned int chn, int oversampling_ratio) = 0;

	/**
	* @brief Retrieve the sample rate of both DACs
	* @return A list containing the sample rates
	*/
	virtual std::vector<double> getSampleRate() = 0;


	/**
	* @brief Retrieve the sample rate for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The value of the sample rate
	*/
	virtual double getSampleRate(unsigned int chn) = 0;


	/**
	 * @brief getAvailableSampleRates
	 * @param chn The index corresponding to the required channel
	 * @return The list of available samplerates for this device
	 */
	virtual std::vector<double> getAvailableSampleRates(unsigned int chn) = 0;


	/**
	* @brief Set the sample rate for both channels
	*
	* @param samplerates A list containing the sample rates of each channel
	* @return A list containing the previously setted sample rates
	*/
	virtual std::vector<double> setSampleRate(std::vector<double> samplerates) = 0;


	/**
	* @brief Set the sample rate for the given channel
	*
	* @param chn The index corresponding to the channel
	* @param samplerate A double value to set the sample rate to
	* @return The value of the sample rate
	*/
	virtual double setSampleRate(unsigned int chn, double samplerate) = 0;


	/**
	* @private
	*/
	virtual void setSyncedDma(bool en, int chn = -1) = 0;

	/**
	* @private
	*/
	virtual bool getSyncedDma(int chn = -1) = 0;

	/**
	* @private
	*/
	virtual void setSyncedStartDma(bool en, int chn = -1) = 0;

	/**
	* @private
	*/
	virtual bool getSyncedStartDma(int chn = -1) = 0;

	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param en If true, enable cyclic mode
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void setCyclic(bool en) = 0;


	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param chn The index corresponding to the channel
	* @param en If true, enable cyclic mode
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void setCyclic(unsigned int chn, bool en) = 0;


	/**
	* @brief Retrieve the value of the cyclic mode
	*
	* @param chn The index corresponding to the channel
	* @return A boolean value corresponding to the state of the cyclic mode
	*/
	virtual bool getCyclic(unsigned int chn) = 0;


	/**
	* @brief Retrieve the scaling factor for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The value of the scaling factor
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual double getScalingFactor(unsigned int chn) = 0;


	/**
	* @brief Retrieve the filter compensation for the given sample rate
	*
	* @param samplerate A double value representing the sample rate
	* @return The value of the filter compensation
	*/
	virtual double getFilterCompensation(double samplerate) = 0;


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A pointer to the samples
	* @param nb_samples the number of samples
	*
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void pushBytes(unsigned int chnIdx, double *data, unsigned int nb_samples) = 0;


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A pointer to the raw samples
	* @param nb_samples the number of samples
	*
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void pushRawBytes(unsigned int chnIdx, short *data, unsigned int nb_samples) = 0;


	/**
	* @brief Send samples to all the channels
	*
	* @param data A pointer to the interleaved data
	* @param nb_channels the number of channels on which we want to push
	* @param nb_samples the number of samples total (samples_per_channel * channels)
	* @note Make sure the samples are interleaved
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel will be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	*/
	virtual void pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples) = 0;


	/**
	* @brief Send samples to all the channels
	*
	* @param data A pointer to the interleaved raw data
	* @param nb_channels the number of channels on which we want to push
	* @param nb_samples the number of samples total (samples_per_channel * channels)
	* @note Make sure the raw samples are interleaved
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel will be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	*/
	virtual void pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples) = 0;


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A list of doubles containing all samples
	*
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void push(unsigned int chnIdx, std::vector<double> const &data) = 0;


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A list of shorts containing all samples
	*
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void pushRaw(unsigned int chnIdx, std::vector<short> const &data) = 0;


	/**
	* @brief Send samples to channels.
	*
	* @param data A list containing lists of samples
	*
	* @note The index of each list of samples represents the channel's index
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	*/
	virtual void push(std::vector<std::vector<double>> const &data) = 0;


	/**
	* @brief Send samples to channels
	*
	* @param data A list containing lists of samples
	*
	* @note The index of each list of samples represents the channel's index
	* @note Streaming data is possible - required multiple kernel buffers
	* @note The given channel won't be synchronized with the other channel
	* @note Due to a hardware limitation, the number of samples per channel must
	* be a multiple of 4 and greater than 16 (non-cyclic buffers) or 1024 (cyclic buffers)
	* @note The samples in the buffer can be repeated until the buffer reaches the size requirements
	*/
	virtual void pushRaw(std::vector<std::vector<short>> const &data) = 0;


	/**
	* @brief Stop all channels from sending the signals.
	*
	* @note Both DACs will be powered down
	*
	*/
	virtual void stop() = 0;


	/**
	* @brief Stop the given channels from sending the signals
	*
	* @param chn The index corresponding to the channel
	*
	* @note The corresponding DAC will be powered down
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void stop(unsigned int chn) = 0;


	/**
	 * @brief Cancel all buffer operations of enabled channels
	 * @note Should be used to cancel an ongoing data write.
	 */
	virtual void cancelBuffer() = 0;


	/**
	 * @brief Cancel all buffer operations of the given channel
	 * @param chn The index corresponding to the channel
	 * @note Should be used to cancel an ongoing data write.
	 */
	virtual void cancelBuffer(unsigned int chn) = 0;

	/**
	* @brief Enable or disable the given digital channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual void enableChannel(unsigned int chnIdx, bool enable) = 0;


	/**
	* @brief Check if the given channel is enabled
	*
	* @param chnIdx The index corresponding to the channel
	* @return A boolean value corresponding to the state of the channel
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	virtual bool isChannelEnabled(unsigned int chnIdx) = 0;


	/**
	 * @brief Check if the generation of the signal (only for non-cyclic buffer) is done
	 * @param chnIdx The index corresponding to the channel
	 * @return True if the push process is done, false otherwise
	 *
	 * @note This function takes the number of kernel buffers into consideration.
	 * @note If a new session is started without unplugging the board and the number of kernel buffers was modified in the previous session
	 * @note (default value = 4) a DAC calibration must be performed before calling isPushDone in order to compute the
	 * @note current number of kernel buffers or call again the function setKernelBuffersCount.
	 *
	 * @note Available only in firmware versions newer than 0.23.
	 */
	virtual bool isPushDone(unsigned int chnIdx) const = 0;


	/**
	 * @brief Set the kernel buffers to a specific value
	 * @param chnIdx The index corresponding to the channel
	 * @param count the number of kernel buffers
	 */
	virtual void setKernelBuffersCount(unsigned int chnIdx, unsigned int count) = 0;

	/**
	 * @brief Get the number of kernel buffers
	 * @param chnIdx The index corresponding to the channel
	 * @return the number of previously set kernel buffers (saved in this session)
	 */
	virtual unsigned int getKernelBuffersCount(unsigned int chnIdx) const = 0;


	/**
	 * @brief Convert the volts value of a sample into raw
	 * @param channel The index corresponding to the channel
	 * @param voltage The volts value of a sample
	 * @return The value of a sample converted into raw
	 */
	virtual short convertVoltsToRaw(unsigned int channel, double voltage) = 0;


	/**
	 * @brief Convert the raw value of a sample into volts
	 * @param channel The index corresponding to the channel
	 * @param raw The raw value of a sample
	 * @return The value of a sample converted into volts
	 */
	virtual double convertRawToVolts(unsigned int channel, short raw) = 0;


	/**
	 * @brief Get access to IIO channels, buffers, devices and context.
	 * @note Can be used when debugging directly with libiio.
	 * @return IIO_OBJECTS structure.
	 */
	virtual struct IIO_OBJECTS getIioObjects() = 0;


	/**
	* @brief Retrieve the number of analogical channels
	* @return The number of channels
	*/
	virtual unsigned int getNbChannels() = 0;


	/**
	 * @brief Get the channel name for each DAC channel
	 * @param channel - unsigned int representing the index of the channel
	 * @return std::string - name of the channel
	 */
	virtual std::string getChannelName(unsigned int channel) = 0;

	/**
	 * @brief Get the maximum samplerate for the DAC
	 * @param chn_idx - unsigned int representing the index of the channel
	 * @return double - the value of the maximum samplerate
	 */
	virtual double getMaximumSamplerate(unsigned int chn_idx) = 0;


	/**
	 * @brief Sets the voltage output of the DAC channel
	 * @param chn_idx - unsigned int representing the index of the channel
	 * @param volts - actual value to be set
	 * @return unsigned short - the corresponding raw value for the given voltage
	 */
	virtual unsigned short setVoltage(unsigned int chn_idx, double volts) = 0;

	/**
	 * @brief Sets the raw output of the DAC channel
	 * @param chn_idx - unsigned int representing the index of the channel
	 * @param raw - actual value to be set
	 * @return unsigned short - the value set in the raw attribute
	 */
	virtual unsigned short setVoltageRaw(unsigned int chn_idx, unsigned short raw) = 0;
};
}
}


#endif //M2KANALOGOUT_HPP
