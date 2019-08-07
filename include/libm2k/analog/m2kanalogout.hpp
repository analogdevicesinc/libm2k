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

#ifndef M2KANALOGOUT_HPP
#define M2KANALOGOUT_HPP

#include <libm2k/m2kglobal.hpp>
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
	M2kAnalogOut(struct iio_context*, std::vector<std::string> dac_devs, bool sync);


	/**
	 * @private
	*/
	virtual ~M2kAnalogOut();


	/**
	* @private
	*/
	void init();

	/* chn can be 0 or 1, it actually refers to dac-a or dac-b) */
	/**
	* @brief Retrieve the global oversampling ratio
	*
	* @return The value of the global ratio
	*/
	std::vector<double> getOversamplingRatio();


	/**
	* @brief Retrieve the oversampling ratio for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The ratio value
	*/
	double getOversamplingRatio(unsigned int chn);

	/**
	* @brief Set the value of the oversampling ratio for each channel
	*
	* @param oversampling_ratio A list containing the ratios for each channel
	* @return A list containing the ratio value for each channel
	*/
	std::vector<double> setOversamplingRatio(std::vector<double> oversampling_ratio);


	/**
	* @brief Set the oversampling ratio for the given channel
	*
	* @param chn The index corresponding to the channel
	* @param oversampling_ratio A double value to set the oversampling ratio to
	* @return The current ratio value
	*/
	double setOversamplingRatio(unsigned int chn, double oversampling_ratio);

	/**
	* @brief Retrieve the sample rate of both DACs
	*
	* @return A list containing the sample rates
	*/
	std::vector<double> getSampleRate();


	/**
	* @brief Retrieve the sample rate for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The value of the sample rate
	*/
	double getSampleRate(unsigned int chn);


	/**
	* @brief Set the sample rate for both channels
	*
	* @param samplerates A list containing the sample rates of each channel
	* @return A list containing the previously setted sample rates
	*/
	std::vector<double> setSampleRate(std::vector<double> samplerates);


	/**
	* @brief Set the sample rate for the given channel
	*
	* @param chn The index corresponding to the channel
	* @param samplerate A double value to set the sample rate to
	* @return The value of the sample rate
	*/
	double setSampleRate(unsigned int chn, double samplerate);


	/**
	* @private
	*/
	void setSyncedDma(bool en, int chn = -1);

	/**
	* @private
	*/
	bool getSyncedDma(int chn = -1);


	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param en If true, enable cyclic mode
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void setCyclic(bool en);


	/**
	* @brief Enable or disable the cyclic mode for all digital channels
	*
	* @param chn The index corresponding to the channel
	* @param en If true, enable cyclic mode
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void setCyclic(unsigned int chn, bool en);


	/**
	* @brief Retrieve the value of the cyclic mode
	*
	* @param chn The index corresponding to the channel
	* @return A boolean value corresponding to the state of the cyclic mode
	*/
	bool getCyclic(unsigned int chn);


	/**
	* @brief Set the calibration scale for the given channel
	*
	* @param index The index corresponding to the channel
	* @param calibscale A double value to set the calibration scale to
	* @return The current value of the calibration scale
	*/
	double setCalibscale(unsigned int index, double calibscale);


	/**
	* @brief Retrieve the calibration scale for the given channel
	*
	* @param index The index corresponding to the channel
	* @return The value of the calibration scale
	*/
	double getCalibscale(unsigned int index);


	/**
	* @brief Retrieve the scaling factor for the given channel
	*
	* @param chn The index corresponding to the channel
	* @return The value of the scaling factor
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	double getScalingFactor(unsigned int chn);


	/**
	* @brief Retrieve the filter compensation for the given sample rate
	*
	* @param samplerate A double value representing the sample rate
	* @return The value of the filter compensation
	*/
	double getFilterCompensation(double samplerate);


	/**
	* @brief Convert a value measured in volts into a raw value
	*
	* @param voltage The value of the voltage
	* @param vlsb The value of the calibration gain
	* @param filterCompensation The value of the filter compensation
	* @return The raw value
	*/
	int convertVoltsToRaw(double voltage, double vlsb,
		double filterCompensation);

	void push(unsigned int chnIdx, double *data, unsigned int nb_samples);
	void pushRaw(unsigned int chnIdx, short *data, unsigned int nb_samples);
	void pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples_per_channel);
	void pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples_per_channel);

	/**
	* @brief Set the calibration gain for the given channel
	*
	* @param chn The index corresponding to the channel
	* @param vlsb A double value to set the calibration gain to
	*/
	void setDacCalibVlsb(unsigned int chn, double vlsb);


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A list of doubles containing all samples
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void push(unsigned int chnIdx, std::vector<double> const &data);


	/**
	* @brief Send the samples to the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param data A list of shorts containing all samples
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void pushRaw(unsigned int chnIdx, std::vector<short> const &data);


	/**
	* @brief Send samples to channels
	*
	* @param data A list containing lists of samples
	*
	* @note The index of each list of samples represents the channel's
	*/
	void push(std::vector<std::vector<double>> const &data);


	/**
	* @brief Send samples to channels
	*
	* @param data A list containing lists of samples
	*
	* @note The index of each list of samples represents the channel's index
	*/
	void pushRaw(std::vector<std::vector<short>> const &data);


	/**
	* @brief Stop all channels from sending the signals
	*
	*/
	void stop();


	/**
	* @brief Stop the given channels from sending the signals
	*
	* @param chn The index corresponding to the channel
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void stop(unsigned int chn);


	/**
	* @brief Enable or disable the given digital channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	void enableChannel(unsigned int chnIdx, bool enable);


	/**
	* @brief Check if the given channel is enabled
	*
	* @param chnIdx The index corresponding to the channel
	* @return A boolean value corresponding to the state of the channel
	*
	* @throw EXC_OUT_OF_RANGE No such channel
	*/
	bool isChannelEnabled(unsigned int chnIdx);

private:
	class M2kAnalogOutImpl;
	std::unique_ptr<M2kAnalogOutImpl> m_pimpl;
};
}
}


#endif //M2KANALOGOUT_HPP
