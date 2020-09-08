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
class M2kFakeAnalogInImpl : public M2kAnalogIn
{
public:
	M2kFakeAnalogInImpl(struct iio_context*, std::string adc_dev, bool sync, M2kHardwareTrigger *trigger);
	~M2kFakeAnalogInImpl() override;

	void reset() override;
	void startAcquisition(unsigned int nb_samples) override;
	void stopAcquisition() override;

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples) override;
	std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples) override;

	const double* getSamplesInterleaved(unsigned int nb_samples) override;
	const short* getSamplesRawInterleaved(unsigned int nb_samples) override;

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
	double getSampleRate(unsigned int chn_idx) override;
	std::vector<double> getAvailableSampleRates() override;
	double setSampleRate(double samplerate) override;
	double setSampleRate(unsigned int chn_idx, double samplerate) override;

	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL chn) override;

	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset);
	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset, int vert_offset);

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

	libm2k::M2kHardwareTrigger* getTrigger() override;
	struct IIO_OBJECTS getIioObjects() override;

	void cancelAcquisition() override;

	void getSamples(std::vector<std::vector<double> > &data, unsigned int nb_samples);

	std::string getChannelName(unsigned int channel);
	double getMaximumSamplerate() override;

	void deinitialize();
	bool hasCalibbias();
private:
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

	void syncDevice();

	M2K_RANGE getRangeDevice(ANALOG_IN_CHANNEL channel);

	static short convVoltsToRaw(double voltage, double correctionGain, double hw_gain, double filterCompensation, double offset);

	double getScalingFactor(unsigned int ch);

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples, bool processed);

	void handleChannelsEnableState(bool before_refill);
	void removeSamplesDisabledChannels(std::vector<std::vector<double>> &samples);

	const double *getSamplesInterleaved(unsigned int nb_samples, bool processed = false);

	double processSample(int16_t sample, unsigned int channel);

	const int convertVoltsToRawVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset);
	const double convertRawToVoltsVerticalOffset(ANALOG_IN_CHANNEL channel, int rawVertOffset);
};
}
}

#endif //M2KANALOGIN_IMPL_HPP
