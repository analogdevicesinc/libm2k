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

#ifndef M2KANALOGIN_HPP
#define M2KANALOGIN_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/analog/m2khardwaretrigger.hpp>
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
	M2kAnalogIn(struct iio_context*, std::string adc_dev, bool sync);


	/**
	* @private
	*/
	virtual ~M2kAnalogIn();


	/**
	* @private
	*/
	void init();


	/**
	* @brief Retrieve a specific number of samples from each channel
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list containing lists of samples for each channel
	*
	* @note The index of the list corresponds to the index of the channel
	*/
	std::vector<std::vector<double>> getSamples(unsigned int nb_samples);


	/**
	* @brief Retrieve a specific number of raw samples from each channel
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A list containing lists of raw samples for each channel
	*
	* @note The index of the list corresponds to the index of the channel
	*/
	std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples);


	/**
	* @brief Retrieve a specific number of samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved samples
	*/
	double* getSamplesInterleaved(unsigned int nb_samples);


	/**
	* @brief Retrieve a specific number of raw samples from both channels
	*
	* @param nb_samples The number of samples that will be retrieved
	* @return A pointer to the interleaved raw samples
	*/
	short* getSamplesRawInterleaved(unsigned int nb_samples);

	/**
	* @brief Convert the raw value of a sample into volts
	*
	* @param sample The raw value of a sample
	* @param channel The index corresponding to the channel
	* @return The value of a sample converted into volts
	*/
	double processSample(int16_t sample, unsigned int channel);


	/**
	* @brief Retrieve the average raw value of the given channel
	*
	* @param ch The index corresponding to the channel
	* @return The average raw value
	*/
	short getVoltageRaw(unsigned int ch);


	/**
	* @brief Retrieve the average voltage of the given channel
	*
	* @param ch The index corresponding to the channel
	* @return The average voltage
	*/
	double getVoltage(unsigned int ch);


	/**
	* @brief Retrieve the average raw value of the given channel
	*
	* @param ch  An enumerator corresponding to the channel's index
	* @return The average raw value
	*/
	short getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch);


	/**
	* @brief Retrieve the average voltage of the given channel
	*
	* @param ch An enumerator corresponding to the channel's index
	* @return The average voltage
	*/
	double getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch);


	/**
	* @brief Retrieve the average raw value for each channel
	*
	* @return A list containing the average raw value of each channel
	*
	* @note The index of the value corresponds to the channel's index
	*/
	std::vector<short> getVoltageRaw();


	/**
	* @brief Retrieve the average voltage for each channel
	*
	* @return A list containing the average voltage of each channel
	*
	* @note The index of the voltage corresponds to the channel's index
	*/
	std::vector<double> getVoltage();


	/**
	* @brief Retrieve the average raw value for both channels
	*
	* @return A pointer to the average raw value of both channels
	*/
	short *getVoltageRawP();


	/**
	* @brief Retrieve the average voltage for both channels
	*
	* @return A pointer to the average voltage of both channels
	*/
	double *getVoltageP();

	/**
	 * @brief Set the vertical offset, in Volts, of a specific channel
	 * @param channel the index of the channel
	 * @param vertOffset the value of the offset in Volts
	 */
	void setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset);

	/**
	 * @brief getVerticalOffset
	 * @param channel the index of the channel
	 * @return the value of the offset in Volts
	 */
	double getVerticalOffset(ANALOG_IN_CHANNEL channel);


	/**
	* @brief Retrieve the scaling factor
	*
	* @param ch An enumerator corresponding to the channel's index
	* @return The value of the scaling factor
	*/
	double getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch);


	/**
	* @brief Set the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param range  An enumerator corresponding to a range
	*/
	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range);


	/**
	* @brief Set the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param min Upper bound
	* @param max Lower bound
	*/
	void setRange(ANALOG_IN_CHANNEL channel, double min, double max);


	/**
	* @brief Retrieve the range for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @return An enumerator corresponding to the range
	*/
	libm2k::analog::M2K_RANGE getRange(libm2k::analog::ANALOG_IN_CHANNEL channel);


	/**
	* @brief Retrieve the bounds for the given range
	*
	* @param range An enumerator corresponding to the range
	* @return A pair containing the lower and upper bound
	*/
	std::pair<double, double> getRangeLimits(libm2k::analog::M2K_RANGE range);


	/**
	* @brief Retrieve all ranges that are available
	*
	* @return A list of pairs containing all available ranges and their name
	*/
	std::vector<std::pair<std::string, std::pair<double, double>>> getAvailableRanges();

	/**
	* @brief Retrieve the global oversampling ratio
	*
	* @return The value of the global ratio
	*/
	double getOversamplingRatio();


	/**
	* @brief Retrieve the oversampling ratio for the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @return The ratio value
	*/
	double getOversamplingRatio(unsigned int chn_idx);


	/**
	* @brief Set the global oversampling ratio
	*
	* @param oversampling A double value to set the oversampling ratio to
	* @return The current value of the global ratio
	*/
	double setOversamplingRatio(double oversampling);


	/**
	* @brief Set the oversampling ratio for the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @param oversampling A double value to set the oversampling ratio to
	* @return The current ratio value
	*/
	double setOversamplingRatio(unsigned int chn_idx, double oversampling);


	/**
	* @brief Retrieve the global sample rate
	*
	* @return The value of the sample rate
	*/
	double getSampleRate();


	/**
	* @brief Retrieve the sample rate of the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @return The value of the sample rate
	*/
	double getSampleRate(unsigned int chn_idx);


	/**
	* @brief Set the global sample rate
	*
	* @param samplerate A double value to set the sample rate to
	* @return The value of the global sample rate
	*/
	double setSampleRate(double samplerate);


	/**
	* @brief Set the sample rate of the given channel
	*
	* @param chn_idx The index corresponding to the channel
	* @param samplerate A double value to set the sample rate to
	* @return The value of the global sample rate
	*/
	double setSampleRate(unsigned int chn_idx, double samplerate);


	/**
	* @brief Retrieve the bounds of the analogical trigger's hysteresis for the given channel
	*
	* @param chn An enumerator corresponding to the channel's index
	* @return A pair containing the lower and upper bound
	*/
	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL chn);


	/**
	* @brief Set the value of the calibration offset for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param calib_offset Int value to set the calibration offset to
	*/
	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset);


	/**
	* @brief Set the value of the calibration scale for the given channel
	*
	* @param index The index corresponding the the channel
	* @param calibscale A double value to set the calibration scale to
	* @return The current value of the calibration scale
	*/
	double setCalibscale(unsigned int index, double calibscale);


	/**
	* @brief Retrieve the value of the calibration scale for the given channel
	*
	* @param index The index corresponding to the channel
	* @return The value of the calibration scale
	*/
	double getCalibscale(unsigned int index);


	/**
	* @brief Set the value of the calibration gain for the given channel
	*
	* @param channel An enumerator corresponding to the channel's index
	* @param gain A double value to set the calibration gain to
	*/
	void setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain);


	/**
	* @brief Retrieve the filter compensation for the given sample rate
	*
	* @param samplerate A double value representing the sample rate
	* @return The value of the filter compensation
	*/
	double getFilterCompensation(double samplerate);


	/**
	* @brief Retrieve the numeric value corresponding to the given range
	*
	* @param range
	* @return double
	*/
	double getValueForRange(M2K_RANGE range);


	/**
	 * @private
	 */
	double convertRawToVolts(int sample, double correctionGain = 1,
		double hw_gain = 0.02,
		double filterCompensation = 1,
		double offset = 0);


	/**
	 * @private
	 */
	int convertVoltsToRaw(double voltage, double correctionGain = 1,
		double hw_gain = 0.02,
		double filterCompensation = 1,
		double offset = 0);


	/**
	* @brief Retrieve the number of analogical channels
	*
	* @return The number of channels
	*/
	unsigned int getNbChannels();


	/**
	* @brief Retrieve the name of the device
	*
	* @return The name of the device
	*/
	std::string getName();


	/**
	* @brief Enable or disable the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @param enable A boolean value corresponding to the channel's state
	*/
	void enableChannel(unsigned int chnIdx, bool enable);


	/**
	* @brief Retrieve the state of the given channel
	*
	* @param chnIdx The index corresponding to the channel
	* @return True if the channel is enabled
	* @return False if the channel is disabled
	*/
	bool isChannelEnabled(unsigned int chnIdx);


	/**
	 * @private
	 */
	void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src);


	/**
	* @private
	*/
	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src);

	/**
	 * @brief Set the kernel buffers to a specific value
	 * @param count the number of kernel buffers
	 */
	void setKernelBuffersCount(unsigned int count);


	/**
	* @brief Get the hardware trigger handler
	*
	* @return A pointer to the hardware trigger trigger
	*/
	libm2k::analog::M2kHardwareTrigger* getTrigger();

private:
	class M2kAnalogInImpl;
	std::unique_ptr<M2kAnalogInImpl> m_pimpl;
};
}
}

#endif //M2KANALOGIN_HPP
