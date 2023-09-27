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

#ifndef M2KANALOGIN_IMPL_HPP
#define M2KANALOGIN_IMPL_HPP

#include <libm2k/analog/m2kanalogin.hpp>
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include <libm2k/analog/enums.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <vector>
#include <map>

namespace libm2k {
namespace analog {
class M2kAnalogInImpl : public M2kAnalogIn
{
public:
        M2kAnalogInImpl(struct iio_context*, std::string adc_dev, bool sync, M2kHardwareTrigger *trigger);
	~M2kAnalogInImpl() override;

	void reset() override;
	void startAcquisition(unsigned int nb_samples) override;
	void stopAcquisition() override;

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples) override;
	std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples) override;

	const double* getSamplesInterleaved(unsigned int nb_samples_per_channel) override;
	const short* getSamplesRawInterleaved(unsigned int nb_samples_per_channel) override;

	const double* getSamplesInterleaved_matlab(unsigned int nb_samples) override;
	const short* getSamplesRawInterleaved_matlab(unsigned int nb_samples) override;

	short getVoltageRaw(unsigned int ch) override;
	double getVoltage(unsigned int ch) override;
	short getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch) override;
	double getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch) override;
	std::vector<short> getVoltageRaw() override;
	std::vector<double> getVoltage() override;
	const short *getVoltageRawP() override;
	const double *getVoltageP() override;

	void setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset) override;
	void setRawVerticalOffset(ANALOG_IN_CHANNEL channel, int rawVertOffset);
	double getVerticalOffset(ANALOG_IN_CHANNEL channel) override;
	int getRawVerticalOffset(ANALOG_IN_CHANNEL channel);

	double getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch) override;

	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range) override;
	void setRange(ANALOG_IN_CHANNEL channel, double min, double max) override;
	libm2k::analog::M2K_RANGE getRange(libm2k::analog::ANALOG_IN_CHANNEL channel) override;
	std::pair<double, double> getRangeLimits(libm2k::analog::M2K_RANGE range) override;
	std::vector<std::pair<std::string, std::pair<double, double>>> getAvailableRanges() override;

	int getOversamplingRatio() override;
	int getOversamplingRatio(unsigned int chn_idx) override;
	int setOversamplingRatio(int oversampling) override;
	int setOversamplingRatio(unsigned int chn_idx, int oversampling) override;

	double getSampleRate() override;
	std::vector<double> getAvailableSampleRates() override;
	double setSampleRate(double samplerate) override;

	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL chn) override;

	/**
	 * The calibration offset is store in the same register with the vertical offset as a sum (CalibOff + VertOff)
	 * The calibration offset is always represented as an integer raw value
	 * The vertical offset is store as an integer raw value, but it can be converted to a voltage representation
	 * There are several ways to set these coefficients
	 * */
	 // set the calibration offset - use the current vertical offset
	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset);
	// set both the calibration offset and the vertical offset (raw value)
	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset, int vert_offset);
	// set both the calibration offset and the vertical offset (value in volts)
	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset, double vert_offset);

	double setCalibscale(unsigned int index, double calibscale);
	double getCalibscale(unsigned int index);

	void setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain);
	int getAdcCalibOffset(ANALOG_IN_CHANNEL channel);

	double getFilterCompensation(double samplerate) override;

	double getValueForRange(M2K_RANGE range) override;

	double convRawToVolts(int sample, double correctionGain = 1,
		double hw_gain = 0.02,
		double filterCompensation = 1,
		double offset = 0) const;

	double convertRawToVolts(unsigned int channel, short raw) override;
	short convertVoltsToRaw(unsigned int channel, double voltage) override;

	unsigned int getNbChannels() override;
	std::string getName() override;

	void enableChannel(unsigned int chnIdx, bool enable) override;
	bool isChannelEnabled(unsigned int chnIdx) override;

	void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src);
	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src);

	void setKernelBuffersCount(unsigned int count) override;
	unsigned int getKernelBuffersCount() const override;

	libm2k::M2kHardwareTrigger* getTrigger() override;
	struct IIO_OBJECTS getIioObjects() override;

	void cancelAcquisition() override;

	void getSamples(std::vector<std::vector<double> > &data, unsigned int nb_samples) override;

	std::string getChannelName(unsigned int channel) override;
	double getMaximumSamplerate() override;

	void deinitialize();
	bool hasCalibbias();
	void loadNbKernelBuffers();
private:
	std::string firmware_version;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_ad5625_dev;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_m2k_fabric;
	std::shared_ptr<libm2k::utils::DeviceIn> m_m2k_adc;
	bool m_need_processing;
	double m_max_samplerate;

	double m_samplerate;
	libm2k::M2kHardwareTrigger *m_trigger;
	std::vector<M2K_RANGE> m_input_range;

	bool m_calibbias_available;
	std::vector<double> m_adc_calib_gain;
	std::vector<int> m_adc_calib_offset;
	std::vector<int> m_adc_hw_offset_raw;
	std::vector<double> m_adc_hw_vert_offset;
	std::map<double, double> m_filter_compensation_table;
	std::vector<bool> m_channels_enabled;
	unsigned int m_nb_kernel_buffers;
	bool m_data_available;

	void syncDevice();

	M2K_RANGE getRangeDevice(ANALOG_IN_CHANNEL channel);

	static short convVoltsToRaw(double voltage, double correctionGain, double hw_gain, double filterCompensation, double offset);

	double getScalingFactor(unsigned int ch);

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples, bool processed);

	void handleChannelsEnableState(bool before_refill);
	void removeSamplesDisabledChannels(std::vector<std::vector<double>> &samples);

	const double *getSamplesInterleaved(unsigned int nb_samples, bool processed = false);

	double processSample(int16_t sample, unsigned int channel);

	int convertVoltsToRawVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset);
	double convertRawToVoltsVerticalOffset(ANALOG_IN_CHANNEL channel, int rawVertOffset);

	/**
	 * This function can stop the calibration process for the ADC at the HW level.
	 * The possible values can be checked in IIO in the "calibrate_available" device attribute of m2k-adc.
	 * 
	 * When calibrate is set to true, the calibration won't affect the ADC samples. When set to false 
	 * the calibration is applied. It should be set before doing calibration to avoid sending samples.
	 * 
	 * Introduced in fw v0.32 to solve a bug where after HW calibration, some samples were missing due to rounding errors.
	 * 
	 * @param calibrate_val The calibration value to set. Can take values "true" or "false" as std::strings.
	 *                      
	 */
	void setCalibrateHDL(std::string calibrate_val);
	std::string getCalibrateHDL();
};
}
}

#endif //M2KANALOGIN_IMPL_HPP
