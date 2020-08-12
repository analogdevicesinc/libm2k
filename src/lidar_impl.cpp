#include "lidar_impl.hpp"
#include <vector>
#include <libm2k/utils/utils.hpp>
#include "utils/channel.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <iio.h>
#include <iostream>

using namespace std;
using namespace libm2k::context;
using namespace libm2k::utils;

LidarImpl::LidarImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync) {

	pulse_dev = new DeviceGeneric(ctx, "7c700000.axi-pulse-capture");
	afe_dev	  = new DeviceGeneric(ctx, "ad5627");
	adc_dev	  = new DeviceIn     (ctx, "axi-ad9094-hpc");

	setAllIIoAttributesToDefaultValues();
}

void LidarImpl::initialize() {
	;
}

void LidarImpl::channelEnableDisable(string channel, bool enable) {
	if	(channel == "voltage0")
		adc_dev->enableChannel(0, enable, false);
	else if (channel == "voltage1")
		adc_dev->enableChannel(1, enable, false);
	else if (channel == "voltage2")
		adc_dev->enableChannel(2, enable, false);
	else if (channel == "voltage3")
		adc_dev->enableChannel(3, enable, false);
	else if (channel == "voltage4")
		adc_dev->enableChannel(4, enable, false);
	else
		throw_exception(m2k_exception::make("The supplied channel, " +
						    channel + ", does not exist").type(libm2k::EXC_INVALID_PARAMETER).build());
}

vector<string> LidarImpl::enabledChannelsList() {
	vector<string> enabled_channels;
	if (adc_dev->isChannelEnabled(0, false))
		enabled_channels.push_back("voltage0");
	if (adc_dev->isChannelEnabled(1, false))
		enabled_channels.push_back("voltage1");
	if (adc_dev->isChannelEnabled(2, false))
		enabled_channels.push_back("voltage2");
	if (adc_dev->isChannelEnabled(3, false))
		enabled_channels.push_back("voltage3");
	if (adc_dev->isChannelEnabled(4, false))
		enabled_channels.push_back("voltage4");
	return enabled_channels;
}

map<string, vector<int8_t>> LidarImpl::readChannels(unsigned int nb_samples) {
	vector<string> enabled_channels = enabledChannelsList();
	unsigned int enabled_channels_count = enabled_channels.size();
	
	int8_t* start = (int8_t*)adc_dev->getSamplesRawInterleavedVoid(nb_samples);
	int8_t* sample;
	int8_t* end   = start + (nb_samples * enabled_channels_count);
	std::ptrdiff_t step = 1 * enabled_channels_count; // one byte/sample for Lidar

	std::vector<int8_t> samples;
	std::map<string, vector<int8_t>> result;

	// For every channel that is enabled at the time of buffer refill..
	for (vector<string>::size_type i = 0; i != enabled_channels_count - 1; i++) {
		// ..ignoring the last channel, channel 4 which is not interesting.
		// Go through each buffer sample and select the data that belongs to
		// that channel only. If the sample size is 3, for example (3
		// channels are enabled when the buffer is refilled), the first byte
		// will belong to the enabled channel with the lowest index, the
		// second byte to the channel with the second lowest index, and so
		// on.
		samples.clear();
		for (sample = start; sample < end; sample += step) {
			samples.push_back(sample[i]);
		}
		result.insert(pair<string, vector<int8_t>>(enabled_channels[i], samples));
	}
	
	return result;
}

double LidarImpl::adp_bias_volts_to_raw_convert(double value, bool inverse) {
	double ret;
	if (inverse)
		ret = -((value * 5 * 18.18) / 4096) + 122;
	else
		ret = ((-122 - value) * 4096) / (5 * 18.18);
	return ret;
}

double LidarImpl::tilt_volts_to_raw_convert(double value, bool inverse) {
	double ret;
	if (inverse)
		ret = (value * 5) / 4096;
	else
		ret = (value * 4096) / 5;
	return ret;
}

void LidarImpl::setApdBias(unsigned int bias) {
	afe_dev->setLongValue(0, adp_bias_volts_to_raw_convert(bias, false), "raw", true);
}

void LidarImpl::setTiltVoltage(unsigned int voltage) {
	afe_dev->setLongValue(1, tilt_volts_to_raw_convert(voltage, false), "raw", true);
}

void LidarImpl::laserEnable() {
	pulse_dev->setStringValue(0, "en", "1", true);
}

void LidarImpl::laserDisable() {
	pulse_dev->setStringValue(0, "en", "0", true);
}

void LidarImpl::setLaserPulseWidth(long long pulse_width) {
	pulse_dev->setLongValue(0, pulse_width, "pulse_width_ns", true);
}

void LidarImpl::setLaserFrequency(long long frequency) {
	pulse_dev->setLongValue(0, frequency, "frequency", true);
}

void LidarImpl::enableChannelSequencer()
{
	setChannelSequencerEnableDisable(true);
}

void LidarImpl::disableChannelSequencer()
{
	setChannelSequencerEnableDisable(false);
}

void LidarImpl::setChannelSequencerEnableDisable(bool status) {
	pulse_dev->setBoolValue(status, "sequencer_en");
}

void LidarImpl::setChannelSequencerOpMode(const char* mode) {
	pulse_dev->setStringValue("sequencer_mode", string(mode));
}

void LidarImpl::setChannelSequencerOrderAutoMode(const char* order) {
	pulse_dev->setStringValue("sequencer_auto_cfg", string(order));
}

void LidarImpl::setChannelSequencerOrderManualMode(const char* order) {
	pulse_dev->setStringValue("sequencer_manual_chsel", string(order));
}

void LidarImpl::setSequencerPulseDelay(long long ns) {
	pulse_dev->setLongValue(ns, "sequencer_pulse_delay_ns");
}

void LidarImpl::setChannelSequencerOpModeManual()
{
	setChannelSequencerOpMode("manual");
}

void LidarImpl::setChannelSequencerOpModeAuto()
{
	setChannelSequencerOpMode("auto");
}

void LidarImpl::setAllIIoAttributesToDefaultValues() {
	setChannelSequencerEnableDisable(false);
	setChannelSequencerOpMode("auto");
	setChannelSequencerOrderManualMode("0, 0, 0, 0");
	setChannelSequencerOrderAutoMode("0, 1, 2, 3");
	setSequencerPulseDelay(248);
	laserEnable();
	setLaserFrequency(50000);
	setLaserPulseWidth(20);
	setApdBias(-160);
	setTiltVoltage(0);
	// Channel4 must be enabled everytime we want to read any of the other
	// channels. Enabled it here so that the user doesn't forget to.
	channelEnableDisable("voltage4", true);
}

LidarImpl::~LidarImpl() {
	;
}


