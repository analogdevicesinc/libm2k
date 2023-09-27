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

#include "m2kanalogin_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <algorithm>
#include <cstring>
#include "utils/channel.hpp"

using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std::placeholders;

#define HIGH_MAX 2.5
#define HIGH_MIN -2.5
#define LOW_MAX 25
#define LOW_MIN -25

M2kAnalogInImpl::M2kAnalogInImpl(iio_context * ctx, std::string adc_dev, bool sync, M2kHardwareTrigger *trigger) :
	M2kAnalogIn(),
	m_need_processing(false),
	m_max_samplerate(-1),
	m_trigger(trigger)
{
	LIBM2K_LOG(INFO, "[BEGIN] Initialize M2kAnalogIn");
	firmware_version = Utils::getFirmwareVersion(ctx);

	m_m2k_adc = make_shared<DeviceIn>(ctx, adc_dev);
	m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
	m_ad5625_dev = make_shared<DeviceGeneric>(ctx, "ad5625");

	if (firmware_version >= "v0.32") {
		setCalibrateHDL("true"); 
	}

	/* Filters applied while decimating affect the
		/ amplitude of the received  data */
	m_filter_compensation_table[1E8] = 1.00;
	m_filter_compensation_table[1E7] = 1.05;
	m_filter_compensation_table[1E6] = 1.10;
	m_filter_compensation_table[1E5] = 1.15;
	m_filter_compensation_table[1E4] = 1.20;
	m_filter_compensation_table[1E3] = 1.26;

	// calibbias attribute is only available in firmware versions newer than 0.26
	m_calibbias_available = m_m2k_adc->getChannel(ANALOG_IN_CHANNEL_1, false)->hasAttribute("calibbias");
	m_samplerate = 1E8;
	m_nb_kernel_buffers = 4;

	for (unsigned int i = 0; i < getNbChannels(); i++) {
		m_input_range.push_back(PLUS_MINUS_25V);
		m_adc_calib_offset.push_back(2048);
		m_adc_calib_gain.push_back(1);
		m_adc_hw_vert_offset.push_back(0);
                m_adc_hw_offset_raw.push_back(2048);
		m_trigger->setCalibParameters(i, getScalingFactor(i), m_adc_hw_vert_offset.at(i));
	}

	// data_available attribute exists only in firmware versions newer than 0.23
	m_data_available = m_m2k_adc->hasBufferAttribute("data_available");

	if (sync) {
		syncDevice();
	}
	LIBM2K_LOG(INFO, "[END] Initialize M2kAnalogIn");
}



M2kAnalogInImpl::~M2kAnalogInImpl() = default;

void M2kAnalogInImpl::enableChannel(unsigned int chn_idx, bool enable)
{
	m_m2k_adc->enableChannel(chn_idx, enable, false);
}

bool M2kAnalogInImpl::isChannelEnabled(unsigned int chn_idx)
{
	if (chn_idx >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
		return false;
	}
	return m_m2k_adc->isChannelEnabled(chn_idx, false);
}

unsigned int M2kAnalogInImpl::getNbChannels()
{
	return m_m2k_adc->getNbChannels(false);
}

void M2kAnalogInImpl::reset()
{
	stopAcquisition();
	setOversamplingRatio(1);
	setSampleRate(1E8);

	for (unsigned int i = 0; i < m_m2k_adc->getNbChannels(false); i++) {
		enableChannel(i, false);
		auto ch = static_cast<ANALOG_IN_CHANNEL>(i);
		m_trigger->setAnalogMode(ch, ALWAYS);
		setRange(ch, PLUS_MINUS_25V);
		m_adc_calib_gain.at(i) = 1;
		setCalibscale(i, m_adc_calib_gain.at(i));
		setAdcCalibOffset(ch, 2048);
		setVerticalOffset(ch, 0);
	}
	setKernelBuffersCount(4);
}

void M2kAnalogInImpl::syncDevice()
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn Sync");
	m_samplerate = m_m2k_adc->getDoubleValue("sampling_frequency");
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		auto range = getRangeDevice(static_cast<ANALOG_IN_CHANNEL>(i));
		m_input_range.at(i) = range;

		// load calib_offset from the register - assuming it was deinitialized correctly
		if (m_calibbias_available) {
                        m_adc_calib_offset.at(i) = m_m2k_adc->getLongValue(i, "calibbias", false);
		} else {
                        m_adc_calib_offset.at(i) = 2048;
		}
		if (firmware_version >= "v0.32") {
			m_adc_calib_gain.at(i) = getCalibscale(i);
		}
		m_adc_hw_offset_raw.at(i) = m_ad5625_dev->getLongValue(2 + i, "raw", true);
		m_adc_hw_vert_offset.at(i) = convertRawToVoltsVerticalOffset(static_cast<ANALOG_IN_CHANNEL>(i), m_adc_hw_offset_raw.at(i) - m_adc_calib_offset.at(i));

		m_samplerate = m_m2k_adc->getLongValue("sampling_frequency");

		m_trigger->setCalibParameters(i, getScalingFactor(i), m_adc_hw_vert_offset.at(i));
	}
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn Sync");
}

void M2kAnalogInImpl::loadNbKernelBuffers()
{
	if (m_data_available) {
		const unsigned int buffersize = 16;
		handleChannelsEnableState(true);
		m_m2k_adc->initializeBuffer(buffersize);
		auto sampleSize = m_m2k_adc->getSampleSize();
		auto availableBufferSpace = m_m2k_adc->getBufferLongValue("data_available");
		m_nb_kernel_buffers = (availableBufferSpace / buffersize) / sampleSize;
		handleChannelsEnableState(false);
		m_m2k_adc->flushBuffer();
	} else {
		m_nb_kernel_buffers = 4;
	}
}

void M2kAnalogInImpl::setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_adc_calib_gain.at(channel) = gain;
	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

void M2kAnalogInImpl::setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int raw_offset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	const int rawVertOffset = m_adc_hw_offset_raw.at(channel) - m_adc_calib_offset.at(channel);
	if (m_calibbias_available) {
                m_adc_calib_offset.at(channel) = m_m2k_adc->setLongValue(channel, raw_offset, "calibbias", false);
        } else {
                m_adc_calib_offset.at(channel) = raw_offset;
	}

	int hw_offset_raw = rawVertOffset + m_adc_calib_offset.at(channel);
	m_adc_hw_offset_raw.at(channel) = m_ad5625_dev->setLongValue(channel + 2, hw_offset_raw, "raw", true);

	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

double M2kAnalogInImpl::convRawToVolts(int sample, double correctionGain,
				       double hw_gain, double filterCompensation, double offset) const
{
	// TO DO: explain this formula
	return ((sample * 0.78) / ((1u << 11u) * 1.3 * hw_gain) *
		correctionGain * filterCompensation) + offset;
}

double M2kAnalogInImpl::convertRawToVolts(unsigned int channel, short sample)
{
	if (channel >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return convRawToVolts(sample,
			      m_adc_calib_gain.at(channel),
			      getValueForRange(m_input_range.at(channel)),
			      getFilterCompensation(m_samplerate),
			      -m_adc_hw_vert_offset.at(channel));
}

short M2kAnalogInImpl::convVoltsToRaw(double voltage, double correctionGain,
				      double hw_gain, double filterCompensation, double offset)
{
	// TO DO: explain this formula
	return (short)(((voltage - offset) / (correctionGain * filterCompensation) *
			(2048 * 1.3 * hw_gain) / 0.78));
}

short M2kAnalogInImpl::convertVoltsToRaw(unsigned int channel, double voltage)
{
	if (channel >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return convVoltsToRaw(voltage,
			      m_adc_calib_gain.at(channel),
			      getValueForRange(m_input_range.at(channel)),
			      getFilterCompensation(m_samplerate),
			      -m_adc_hw_vert_offset.at(channel));
}

double M2kAnalogInImpl::getCalibscale(unsigned int index)
{
	if (index >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_m2k_adc->getDoubleValue(index, "calibscale");
}

double M2kAnalogInImpl::setCalibscale(unsigned int index, double calibscale)
{
	if (index >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	// update class attribute as well as the attribute in the device
	if (firmware_version >= "v0.32") {
		m_adc_calib_gain.at(index) = calibscale;
	}
	return m_m2k_adc->setDoubleValue(index, calibscale, "calibscale");
}

libm2k::M2kHardwareTrigger *M2kAnalogInImpl::getTrigger()
{
	return m_trigger;
}


void M2kAnalogInImpl::startAcquisition(unsigned int nb_samples)
{
	handleChannelsEnableState(true);
	m_m2k_adc->initializeBuffer(nb_samples);
	handleChannelsEnableState(false);
}

void M2kAnalogInImpl::stopAcquisition()
{
	m_m2k_adc->flushBuffer();
}

/**
	 * @brief Utility method to be used before and after ADC getSamples calls.
	 * For the m2k-adc, all the channels have to be enabled before refilling
	 */
void M2kAnalogInImpl::handleChannelsEnableState(bool before_refill)
{
	if (before_refill) {
		bool anyChannelEnabled = false;
		m_channels_enabled.clear();
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			bool en  = isChannelEnabled(i);
			m_channels_enabled.push_back(en);
			anyChannelEnabled = en ? true : anyChannelEnabled;
		}
		if (!anyChannelEnabled) {
			THROW_M2K_EXCEPTION("M2kAnalogIn: No channel enabled for RX buffer", libm2k::EXC_INVALID_PARAMETER);
		}

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enableChannel(i, true);
		}
	} else {
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enableChannel(i, m_channels_enabled.at(i));
		}
		m_channels_enabled.clear();
	}

}

void M2kAnalogInImpl::removeSamplesDisabledChannels(std::vector<std::vector<double>> &samples)
{
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		if (!m_channels_enabled.at(i)) {
			samples.at(i) = std::vector<double>();
		}
	}
}

std::vector<std::vector<double>> M2kAnalogInImpl::getSamples(unsigned int nb_samples)
{
	return this->getSamples(nb_samples, true);
}

std::vector<std::vector<double>> M2kAnalogInImpl::getSamplesRaw(unsigned int nb_samples)
{
	return this->getSamples(nb_samples, false);
}

std::vector<std::vector<double>> M2kAnalogInImpl::getSamples(unsigned int nb_samples, bool processed)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getSamples");
	if (processed) {
		m_need_processing = true;
	}
	m_samplerate = getSampleRate();
	handleChannelsEnableState(true);

	auto fp = std::bind(&M2kAnalogInImpl::processSample, this, std::placeholders::_1, std::placeholders::_2);
	auto samps = m_m2k_adc->getSamples(nb_samples, fp);

	removeSamplesDisabledChannels(samps);
	handleChannelsEnableState(false);
	if (processed) {
		m_need_processing = false;
	}
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getSamples");
	return samps;
}

void M2kAnalogInImpl::getSamples(std::vector<std::vector<double> > &data, unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getSamples");
	m_need_processing = true;
	m_samplerate = getSampleRate();
	handleChannelsEnableState(true);

	auto fp = std::bind(&M2kAnalogInImpl::processSample, this, _1, _2);
	m_m2k_adc->getSamples(data, nb_samples, fp);

	removeSamplesDisabledChannels(data);
	handleChannelsEnableState(false);
	m_need_processing = false;
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getSamples");
}

string M2kAnalogInImpl::getChannelName(unsigned int channel)
{
	if (channel >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	std::string name = "";
	auto chn = m_m2k_adc->getChannel(channel, false);
	if (chn) {
		name = chn->getId();
	}
	return name;
}

double M2kAnalogInImpl::getMaximumSamplerate()
{
	if (m_max_samplerate < 0) {
		auto values = getAvailableSampleRates();
		m_max_samplerate = *(max_element(values.begin(), values.end()));
	}
	return m_max_samplerate;
}

void M2kAnalogInImpl::deinitialize()
{
	// The vertical offset register in the device has dual purpose:
	// 1. use as ADC offset for calibration
	// 2. use as ADC vertical offset for measurement
	// This can cause some confusion when initializing the board
	// because it is not clear whether the value in the register is
	// the calibration value or the calibration+vertical offset value
	// This workaround will always set the vertical offset to 0 on deinitialization
	// and upon initialization the offset will be loaded from the register
	// (when no calibration is done)
	//
	// The correct fix would be adding a separate caliboffset register in the firmware
	// which will clear up the confusion

	if (!m_calibbias_available) {
		setVerticalOffset(ANALOG_IN_CHANNEL_1,0);
		setVerticalOffset(ANALOG_IN_CHANNEL_2,0);
	}
}

bool M2kAnalogInImpl::hasCalibbias()
{
	return m_calibbias_available;
}

const double* M2kAnalogInImpl::getSamplesInterleaved(unsigned int nb_samples_per_channel)
{
	return this->getSamplesInterleaved(nb_samples_per_channel, true);
}

const double *M2kAnalogInImpl::getSamplesInterleaved(unsigned int nb_samples, bool processed)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getSamplesInterleaved");
	if (processed) {
		m_need_processing = true;
	}
	m_samplerate = getSampleRate();
	handleChannelsEnableState(true);

	auto fp = std::bind(&M2kAnalogInImpl::processSample, this, std::placeholders::_1, std::placeholders::_2);
	auto samps = (const double *)m_m2k_adc->getSamplesInterleaved(nb_samples, fp);

	handleChannelsEnableState(false);
	if (processed) {
		m_need_processing = false;
	}
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getSamplesInterleaved");
	return samps;
}

const short *M2kAnalogInImpl::getSamplesRawInterleaved(unsigned int nb_samples_per_channel)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getSamplesRawInterleaved");
	m_samplerate = getSampleRate();
	handleChannelsEnableState(true);
	auto samps = m_m2k_adc->getSamplesRawInterleaved(nb_samples_per_channel);
	handleChannelsEnableState(false);
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getSamplesRawInterleaved");
	return samps;
}

const double *M2kAnalogInImpl::getSamplesInterleaved_matlab(unsigned int nb_samples)
{
	return this->getSamplesInterleaved(nb_samples / getNbChannels(), true);
}

const short *M2kAnalogInImpl::getSamplesRawInterleaved_matlab(unsigned int nb_samples)
{
	return this->getSamplesRawInterleaved(nb_samples / getNbChannels());
}

double M2kAnalogInImpl::processSample(int16_t sample, unsigned int channel)
{
	if (channel >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	if (m_need_processing) {
		return convRawToVolts(sample,
				      m_adc_calib_gain.at(channel),
				      getValueForRange(m_input_range.at(channel)),
				      getFilterCompensation(m_samplerate),
				      -m_adc_hw_vert_offset.at(channel));
	} else {
		return (double)sample;
	}
}

short M2kAnalogInImpl::getVoltageRaw(unsigned int ch)
{
	if (ch >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	auto chn = static_cast<ANALOG_IN_CHANNEL>(ch);
	return getVoltageRaw(chn);
}

short M2kAnalogInImpl::getVoltageRaw(ANALOG_IN_CHANNEL ch)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getVoltageRaw");
	unsigned int num_samples = 100;
	M2K_TRIGGER_MODE mode;
	bool enabled;

	if (static_cast<unsigned int>(ch) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
		return -1;
	}

	mode = m_trigger->getAnalogMode(ch);
	m_trigger->setAnalogMode(ch, ALWAYS);

	enabled = isChannelEnabled(ch);
	enableChannel(ch, true);

	auto samps = getSamples(num_samples, false);
	double avg = Utils::average(samps.at(ch).data(), num_samples);
	stopAcquisition();

	m_trigger->setAnalogMode(ch, mode);
	enableChannel(ch, enabled);
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getVoltageRaw");
	return (short)avg;
}

std::vector<short> M2kAnalogInImpl::getVoltageRaw()
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getVoltageRaw");
	unsigned int num_samples = 100;
	std::vector<short> avgs;
	std::vector<M2K_TRIGGER_MODE> modes = {};
	std::vector<bool> enabled = {};

	for (unsigned int i = 0; i < getNbChannels(); i++) {
		enabled.push_back(isChannelEnabled(i));
		enableChannel(i, true);
		modes.push_back(m_trigger->getAnalogMode(i));
		m_trigger->setAnalogMode(i, ALWAYS);
	}
	auto samps = getSamples(num_samples, false);
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		auto avg = (short)(Utils::average(samps.at(i).data(), num_samples));
		avgs.push_back(avg);
		m_trigger->setAnalogMode(i, modes.at(i));
		enableChannel(i, enabled.at(i));
	}
	stopAcquisition();
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getVoltageRaw");
	return avgs;
}

const short *M2kAnalogInImpl::getVoltageRawP()
{
	std::vector<short> avgs = getVoltageRaw();
	auto *values = new short[avgs.size()];
	std::memcpy(values, avgs.data(), avgs.size() * sizeof(short));
	return values;
}

double M2kAnalogInImpl::getVoltage(unsigned int ch)
{
	if (ch >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	auto chn = static_cast<ANALOG_IN_CHANNEL>(ch);
	return getVoltage(chn);
}

double M2kAnalogInImpl::getVoltage(ANALOG_IN_CHANNEL ch)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getVoltage");
	unsigned int num_samples = 100;
	M2K_TRIGGER_MODE mode;
	bool enabled;

	if (static_cast<unsigned int>(ch) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
		return -1;
	}
	mode = m_trigger->getAnalogMode(ch);
	m_trigger->setAnalogMode(ch, ALWAYS);

	enabled = isChannelEnabled(ch);
	enableChannel(ch, true);

	auto samps = getSamples(num_samples, true);
	double avg = Utils::average(samps.at(ch).data(), num_samples);
	stopAcquisition();

	m_trigger->setAnalogMode(ch, mode);
	enableChannel(ch, enabled);
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getVoltage");
	return avg;
}

std::vector<double> M2kAnalogInImpl::getVoltage()
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kAnalogIn getVoltage");
	size_t num_samples = 100;
	std::vector<double> avgs;
	std::vector<M2K_TRIGGER_MODE> modes = {};
	std::vector<bool> enabled = {};

	for (unsigned int i = 0; i < getNbChannels(); i++) {
		enabled.push_back(isChannelEnabled(i));
		enableChannel(i, true);
		modes.push_back(m_trigger->getAnalogMode(i));
		m_trigger->setAnalogMode(i, ALWAYS);
	}
	auto samps = getSamples(num_samples, true);
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		avgs.push_back(Utils::average(samps.at(i).data(), num_samples));
		m_trigger->setAnalogMode(i, modes.at(i));
		enableChannel(i, enabled.at(i));
	}
	stopAcquisition();
	LIBM2K_LOG(INFO, "[END] M2kAnalogIn getVoltage");
	return avgs;
}

const double *M2kAnalogInImpl::getVoltageP()
{
	std::vector<double> avgs = getVoltage();
	auto *values = new double[avgs.size()];
	std::memcpy(values, avgs.data(), avgs.size() * sizeof(double));
	return values;
}

double M2kAnalogInImpl::getScalingFactor(ANALOG_IN_CHANNEL ch)
{
	if (static_cast<unsigned int>(ch) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return (0.78 / ((1u << 11u) * 1.3 *
			getValueForRange(m_input_range.at(ch))) *
		m_adc_calib_gain.at(ch) *
		getFilterCompensation(getSampleRate()));
}

double M2kAnalogInImpl::getScalingFactor(unsigned int ch)
{
	if (ch >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	auto channel = static_cast<ANALOG_IN_CHANNEL>(ch);
	return getScalingFactor(channel);
}

std::pair<double, double> M2kAnalogInImpl::getHysteresisRange(ANALOG_IN_CHANNEL chn)
{
	if (static_cast<unsigned int>(chn) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	std::pair<double, double> m2k_range = getRangeLimits(getRange(chn));
	return (std::pair<double, double>(0, m2k_range.second / 10));
}

void M2kAnalogInImpl::setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range)
{
	const char *str_gain_mode;

	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_input_range[channel] = range;
	if (range == PLUS_MINUS_2_5V) {
		str_gain_mode = "high";
	} else {
		str_gain_mode = "low";
	}

	m_m2k_fabric->setStringValue(channel, "gain", std::string(str_gain_mode));
	setVerticalOffset(channel,m_adc_hw_vert_offset[channel]);
	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

void M2kAnalogInImpl::setRange(ANALOG_IN_CHANNEL channel, double min, double max)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	if ((min >= HIGH_MIN) && (max <= HIGH_MAX)) {
		setRange(channel, PLUS_MINUS_2_5V);
	} else {
		setRange(channel, PLUS_MINUS_25V);
	}
}

M2K_RANGE M2kAnalogInImpl::getRange(ANALOG_IN_CHANNEL channel)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_input_range[channel];
}

M2K_RANGE M2kAnalogInImpl::getRangeDevice(ANALOG_IN_CHANNEL channel)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	M2K_RANGE range = PLUS_MINUS_25V;
	auto gain = m_m2k_fabric->getStringValue(channel, "gain");
	if (gain == "high") {
		range = PLUS_MINUS_2_5V;
	}
	return range;
}

std::pair<double, double> M2kAnalogInImpl::getRangeLimits(M2K_RANGE range)
{
	auto limits = std::pair<double, double>(HIGH_MIN, HIGH_MAX);
	if (range == PLUS_MINUS_25V) {
		limits.first = LOW_MIN;
		limits.second = LOW_MAX;
	}
	return limits;
}

std::vector<std::pair<std::string, std::pair<double, double>>> M2kAnalogInImpl::getAvailableRanges()
{
	std::vector<std::pair<std::string, std::pair <double, double>>> ranges;
	std::pair<std::string, std::pair <double, double>> p;
	p.first = "PLUS_MINUS_25V";
	p.second = getRangeLimits(PLUS_MINUS_25V);
	ranges.push_back(p);

	p.first = "PLUS_MINUS_2_5V";
	p.second = getRangeLimits(PLUS_MINUS_2_5V);
	ranges.push_back(p);

	return ranges;
}


void M2kAnalogInImpl::setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	int raw_vert_offset = convertVoltsToRawVerticalOffset(channel, vertOffset);
	m_adc_hw_vert_offset.at(channel) = vertOffset;
	const int hw_offset_raw = raw_vert_offset + m_adc_calib_offset.at(channel);
	m_adc_hw_offset_raw.at(channel) = m_ad5625_dev->setLongValue(channel + 2, hw_offset_raw, "raw", true);
	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

double M2kAnalogInImpl::getVerticalOffset(ANALOG_IN_CHANNEL channel)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_adc_hw_vert_offset.at(channel);
}

int M2kAnalogInImpl::getOversamplingRatio()
{
	return m_m2k_adc->getLongValue("oversampling_ratio");
}

int M2kAnalogInImpl::getOversamplingRatio(unsigned int chn_idx)
{
	if (chn_idx >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_m2k_adc->getLongValue(chn_idx, "oversampling_ratio");
}

int M2kAnalogInImpl::setOversamplingRatio(int oversampling_ratio)
{
	return m_m2k_adc->setLongValue(oversampling_ratio, "oversampling_ratio");
}

int M2kAnalogInImpl::setOversamplingRatio(unsigned int chn_idx, int oversampling_ratio)
{
	if (chn_idx >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_m2k_adc->setLongValue(chn_idx, oversampling_ratio, "oversampling_ratio");
}


double M2kAnalogInImpl::getSampleRate()
{
	return m_m2k_adc->getDoubleValue("sampling_frequency");
}

double M2kAnalogInImpl::setSampleRate(double samplerate)
{
	m_samplerate = m_m2k_adc->setLongValue((int)samplerate, "sampling_frequency");
	m_trigger->setCalibParameters(0, getScalingFactor(0), m_adc_hw_vert_offset.at(0));
	m_trigger->setCalibParameters(1, getScalingFactor(1), m_adc_hw_vert_offset.at(1));
	return m_samplerate;
}

double M2kAnalogInImpl::getFilterCompensation(double samplerate)
{
	double compensation = 0.0;
	if(m_filter_compensation_table.find(samplerate) != m_filter_compensation_table.end()) {
		compensation = m_filter_compensation_table.at(samplerate);
	} else {
		THROW_M2K_EXCEPTION("Cannot get compensation value for the given samplerate.", libm2k::EXC_INVALID_PARAMETER);
	}
	return compensation;
}

double M2kAnalogInImpl::getValueForRange(M2K_RANGE range)
{
	if (range == PLUS_MINUS_25V) {
		return 0.02017;
	} else if (range == PLUS_MINUS_2_5V) {
		return 0.21229;
	} else {
		return 0;
	}
}

struct libm2k::IIO_OBJECTS M2kAnalogInImpl::getIioObjects()
{
	return m_m2k_adc->getIioObjects();
}

void M2kAnalogInImpl::cancelAcquisition()
{
	m_m2k_adc->cancelBuffer();
}

void M2kAnalogInImpl::setKernelBuffersCount(unsigned int count)
{
	m_m2k_adc->setKernelBuffersCount(count);
	m_nb_kernel_buffers = count;
}

unsigned int M2kAnalogInImpl::getKernelBuffersCount() const
{
	return m_nb_kernel_buffers;
}

std::vector<double> M2kAnalogInImpl::getAvailableSampleRates()
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues = m_m2k_adc->getAvailableAttributeValues("sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
		       [] (std::string &s) -> double {
		try {
			auto value = std::stod(s);
			return value;
		} catch (std::exception&) {
            THROW_M2K_EXCEPTION("Can't determine available sampling frequencies.", libm2k::EXC_RUNTIME_ERROR);
            return {};
        }
	});
	return values;
}

std::string M2kAnalogInImpl::getName()
{
	return m_m2k_adc->getName();
}

void M2kAnalogInImpl::convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src)
{
	if (chn_idx >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_m2k_adc->convertChannelHostFormat(chn_idx, avg, src, false);
}

void M2kAnalogInImpl::convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
{
	if (chn_idx >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_m2k_adc->convertChannelHostFormat(chn_idx, avg, src, false);
}

int M2kAnalogInImpl::convertVoltsToRawVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	double gain = 1.3;
	double vref = 1.2;
	double hw_range_gain = getValueForRange(m_input_range.at(channel));
        return static_cast<int>(vertOffset * (1u << 12u) * hw_range_gain * gain / 2.693 / vref);
}

double M2kAnalogInImpl::convertRawToVoltsVerticalOffset(ANALOG_IN_CHANNEL channel, int rawVertOffset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	double gain = 1.3;
	double vref = 1.2;
	double hw_range_gain = getValueForRange(m_input_range.at(channel));
        return rawVertOffset * 2.693 * vref / ((1u << 12u) * hw_range_gain * gain);
}

int M2kAnalogInImpl::getRawVerticalOffset(ANALOG_IN_CHANNEL channel)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_adc_hw_offset_raw.at(channel) - m_adc_calib_offset.at(channel);
}

void M2kAnalogInImpl::setRawVerticalOffset(ANALOG_IN_CHANNEL channel, int rawVertOffset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	double vertOffset = convertRawToVoltsVerticalOffset(channel, rawVertOffset);
	m_adc_hw_vert_offset.at(channel) = vertOffset;
	const int hw_offset_raw = rawVertOffset + m_adc_calib_offset.at(channel);
	m_adc_hw_offset_raw.at(channel) = m_ad5625_dev->setLongValue(channel + 2, hw_offset_raw, "raw", true);
	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

void M2kAnalogInImpl::setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset, int vert_offset)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	double vertOffset = convertRawToVoltsVerticalOffset(channel, vert_offset);
	m_adc_hw_vert_offset.at(channel) = vertOffset;

	if (m_calibbias_available) {
                m_adc_calib_offset.at(channel) = m_m2k_adc->setLongValue(channel, calib_offset, "calibbias", false);
        } else {
                m_adc_calib_offset.at(channel) = calib_offset;
	}

	const int hw_offset_raw = vert_offset + m_adc_calib_offset.at(channel);
	m_adc_hw_offset_raw.at(channel) = m_ad5625_dev->setLongValue(channel + 2, hw_offset_raw, "raw", true);

	m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

void M2kAnalogInImpl::setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset, double vert_offset)
{
    if (static_cast<unsigned int>(channel) >= getNbChannels()) {
        THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
    }
    m_adc_hw_vert_offset.at(channel) = vert_offset;

    if (m_calibbias_available) {
        m_adc_calib_offset.at(channel) = m_m2k_adc->setLongValue(channel, calib_offset, "calibbias", false);
    } else {
        m_adc_calib_offset.at(channel) = calib_offset;
    }

    const int hw_offset_raw = convertVoltsToRaw(channel, vert_offset) + m_adc_calib_offset.at(channel);
    m_adc_hw_offset_raw.at(channel) = m_ad5625_dev->setLongValue(channel + 2, hw_offset_raw, "raw", true);

    m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
}

int M2kAnalogInImpl::getAdcCalibOffset(ANALOG_IN_CHANNEL channel)
{
	if (static_cast<unsigned int>(channel) >= getNbChannels()) {
		THROW_M2K_EXCEPTION("M2kAnalogIn: no such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	if (m_calibbias_available) {
		return m_m2k_adc->getLongValue(channel, "calibbias", false);
	} else {
		auto adc_hw_offset = m_ad5625_dev->getLongValue(channel + 2, "raw", true);
		return (adc_hw_offset - convertVoltsToRawVerticalOffset(channel, m_adc_hw_vert_offset.at(channel)));
	}
}

void M2kAnalogInImpl::setCalibrateHDL(std::string calibrate_val)
{
	m_m2k_adc->setStringValue("calibrate", calibrate_val);
}
std::string M2kAnalogInImpl::getCalibrateHDL()
{
	return m_m2k_adc->getStringValue("calibrate");
}