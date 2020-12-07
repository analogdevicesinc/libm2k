#include "libm2k_lv.h"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2k.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <string.h>
#include <vector>

extern "C" {

/* Context */
uint64_t libm2k_context_get_count()
{
	auto uri_list = libm2k::context::getAllContexts();
	return uri_list.size();
}

int libm2k_context_get_at(uint64_t idx, char *uri, char *err)
{
	auto uri_list = libm2k::context::getAllContexts();
	auto uri_list_cnt = uri_list.size();
	if (idx >= uri_list_cnt) {
		auto str = "Can't return URI, bad index. \n";
		strcpy(err, str);
		return -1;
	}
	if (!uri) {
		auto str = "Can't return URI, bad allocation. \n";
		strcpy(err, str);
		return -1;
	}

	strcpy(uri, (char*)uri_list.at(idx).c_str());
	return 0;
}

int libm2k_context_open(uintptr_t *ret_m2k, uint8_t *_uri, char *err)
{
	strcpy(err, "");
	char* uri = (char*)_uri;
	libm2k::context::M2k *ctx = nullptr;
	if (strlen(uri) == 0) {
		ctx = libm2k::context::m2kOpen();
	} else {
		ctx = libm2k::context::m2kOpen(uri);
	}
	if (!ctx) {
		*ret_m2k = (uintptr_t)nullptr;
		auto str = "No instrument handle; Check URI. \n";
		strcpy(err, str);
		return -1;
	}
	*ret_m2k = reinterpret_cast<uintptr_t>(ctx);
	return 0;
}

int libm2k_context_get_description(uintptr_t _m2k_instance, char *description, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}
	try {
		auto descr = instance->getContextDescription();
		strcpy(description, descr.c_str());
	}
	catch (std::exception &e) {
		auto str = "Instrument not calibrated; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
	return 0;
}

int libm2k_context_close(uintptr_t *ret_m2k)
{
	auto ctx = (libm2k::context::M2k *) *ret_m2k;
	if (!ctx) {
		return -1;
	}
	libm2k::context::contextClose(ctx);
	*ret_m2k = NULL;
	return 0;
}

void libm2k_context_calibrate(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		instance->calibrate();
	}
	catch (std::exception &e) {
		auto str = "Instrument not calibrated; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}


/* Analog */
void libm2k_analog_channel_enable(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t enable, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		analogIn->enableChannel(chnIdx, static_cast<bool>(enable));
	}
	catch (std::exception &e) {
		auto str = "Channel not enabled; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_analog_samplerate_get(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}

	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		return analogIn->getSampleRate();
	}
	catch (std::exception &e) {
		auto str = "Can't read samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_analog_samplerate_set(uintptr_t _m2k_instance, double samplerate, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
	analogIn->setSampleRate(samplerate);
}

uint64_t libm2k_analog_range_get(uintptr_t _m2k_instance, uint64_t chn_idx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		auto ch = static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(chn_idx);
		return analogIn->getRange(ch);
	}
	catch (std::exception &e) {
		auto str = "Can't read range; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_analog_range_set(uintptr_t _m2k_instance, uint64_t chn_idx, uint64_t range_type, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		auto ch = static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(chn_idx);
		auto range = static_cast<libm2k::analog::M2K_RANGE>(range_type);
		analogIn->setRange(ch, range);
	}
	catch (std::exception &e) {
		auto str = "Can't write range; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_analog_vertical_offset_get(uintptr_t _m2k_instance, uint64_t chn_idx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		return analogIn->getVerticalOffset(static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(chn_idx));
	}
	catch (std::exception &e) {
		auto str = "Can't read vertical offset; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_analog_vertical_offset_set(uintptr_t _m2k_instance, uint64_t chn_idx, double vert_offset, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		auto ch = static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(chn_idx);
		analogIn->setVerticalOffset(ch, vert_offset);
	}
	catch (std::exception &e) {
		auto str = "Can't write vertical offset; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_init_stream_acquisition(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		libm2k::M2kHardwareTrigger *trig = analogIn->getTrigger();
		trig->setAnalogMode(0, libm2k::ALWAYS);
		trig->setAnalogMode(1, libm2k::ALWAYS);
		trig->setAnalogStreamingFlag(false);
		trig->setAnalogStreamingFlag(true);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't init streaming; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_get_samples(uintptr_t _m2k_instance, double *analog_samples, uint64_t nb_samples, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		double *samps = (double *)analogIn->getSamplesInterleaved(nb_samples);
		for (auto i = 0; i < 10; i++) {
			auto v = samps[i];
			printf("%f\n", samps[i]);
		}
		memcpy(analog_samples, samps, nb_samples * sizeof(double));
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't read samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

libm2k::context::M2k* _get_context_instance(uintptr_t _m2k_instance, char *err)
{
	strcpy(err, "");
	libm2k::context::M2k* instance = nullptr;
	instance = reinterpret_cast<libm2k::context::M2k*>(_m2k_instance);
	if (!instance) {
		std::string s_err = "No ADALM2000 instance.\n";
		strcpy(err, s_err.c_str());
		return nullptr;
	}
	return instance;
}

uint16_t libm2k_analog_get_nb_channels(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return 0;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		return analogIn->getNbChannels();
	}
	catch (std::exception &e) {
		auto str = "Can't read nb channels; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return 0;
	}
}

void libm2k_analog_in_stop(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		analogIn->stopAcquisition();
	}
	catch (std::exception &e) {
		auto str = "Can't stop ADC; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_in_cancel_buffer(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		analogIn->cancelAcquisition();
	}
	catch (std::exception &e) {
		auto str = "Can't cancel ADC buffer; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_configure_channel_internal_trigger(uintptr_t _m2k_instance, uint64_t chnIdx, double analog_level,
	uint64_t analog_condition, uint64_t analog_mode, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		libm2k::M2kHardwareTrigger *trig = analogIn->getTrigger();
		trig->setAnalogLevel(chnIdx, analog_level);
		trig->setAnalogCondition(chnIdx, static_cast<libm2k::M2K_TRIGGER_CONDITION_ANALOG>(analog_condition));
		trig->setAnalogMode(chnIdx, static_cast<libm2k::M2K_TRIGGER_MODE>(analog_mode));
	}
	catch (std::exception &e) {
		auto str = "Can't configure channel analog internal trigger; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_configure_internal_trigger(uintptr_t _m2k_instance, int64_t analog_delay,
	uint64_t analog_source, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		libm2k::M2kHardwareTrigger *trig = analogIn->getTrigger();
		trig->setAnalogDelay(analog_delay);
		trig->setAnalogSource(static_cast<libm2k::M2K_TRIGGER_SOURCE_ANALOG>(analog_source));
	}
	catch (std::exception &e) {
		auto str = "Can't configure analog internal trigger; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_analog_get_voltage_instant(uintptr_t _m2k_instance, uint64_t chnIdx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		return analogIn->getVoltage(chnIdx);
	}
	catch (std::exception &e) {
		auto str = "Can't return analog instant voltage; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_analog_kernel_buffers_set(uintptr_t _m2k_instance, uint64_t kernel_buffers_count, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogIn *analogIn = instance->getAnalogIn();
		analogIn->setKernelBuffersCount(kernel_buffers_count);
	}
	catch (std::exception &e) {
		auto str = "Can't return analog instant voltage; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}


/* Digital */

uint64_t libm2k_digital_io_read(uintptr_t _m2k_instance, uint64_t group_idx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return 0;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		uint64_t chn_idx_start = (group_idx <= 1) ? group_idx * 8 : 0;
		uint64_t chn_idx_stop = (group_idx <= 1) ? group_idx * 8 + 8 : 0;
		uint64_t temp_val = 0;
		for (uint64_t i = chn_idx_start; i < chn_idx_stop; i++) {
			uint64_t val = digital->getValueRaw(i);
			val = val << (i % 8);
			temp_val += val;
		}
		return temp_val;
	}
	catch (std::exception &e) {
		auto str = "Can't read raw values; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return 0;
	}
}

void libm2k_digital_io_write(uintptr_t _m2k_instance, uint64_t group_idx, uint64_t value, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		uint64_t chn_idx_start = (group_idx <= 1) ? group_idx * 8 : 0;
		uint64_t chn_idx_stop = (group_idx <= 1) ? group_idx * 8 + 8 : 0;
		uint64_t temp_val = value;
		for (uint64_t i = chn_idx_start; i < chn_idx_stop; i++) {
			auto out_bit_val = temp_val & 1;
			digital->enableChannel(i, true);
			digital->setDirection(i, libm2k::digital::DIO_OUTPUT);
			digital->setValueRaw(i, static_cast<libm2k::digital::DIO_LEVEL>(out_bit_val));
			temp_val = temp_val >> 1;
 		}
	}
	catch (std::exception &e) {
		auto str = "Can't write raw values; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_channel_enable_out(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t output, uint64_t cyclic, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		digital->enableChannel(chnIdx, static_cast<bool>(output));
		digital->setCyclic(static_cast<bool>(cyclic));
		if (output) {
			digital->setDirection(chnIdx, libm2k::digital::DIO_OUTPUT);
		}
	}
	catch (std::exception &e) {
		auto str = "Channel not enabled; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_all_channel_enable_out(uintptr_t _m2k_instance, uint64_t output, uint64_t cyclic, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		digital->enableAllOut(static_cast<bool>(output));
		digital->setCyclic(static_cast<bool>(cyclic));
		if (output) {
			for (unsigned int i = 0; i < digital->getNbChannelsOut(); i++) {
				digital->setDirection(i, libm2k::digital::DIO_OUTPUT);
			}
		}
	}
	catch (std::exception &e) {
		auto str = "Channel not enabled; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_digital_samplerate_get_in(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}

	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		return digital->getSampleRateIn();
	}
	catch (std::exception &e) {
		auto str = "Can't read samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_digital_samplerate_set_in(uintptr_t _m2k_instance, double samplerate, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}

	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		digital->setSampleRateIn(samplerate);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't write samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_init_stream_acquisition(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		libm2k::M2kHardwareTrigger *trig = digital->getTrigger();
		for (unsigned int i = 0; i < digital->getNbChannelsIn(); i++) {
			trig->setDigitalCondition(i, libm2k::NO_TRIGGER_DIGITAL);
		}
		trig->setDigitalStreamingFlag(false);
		trig->setDigitalStreamingFlag(true);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't init streaming; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_get_samples(uintptr_t _m2k_instance, uint16_t *digital_samples, uint64_t nb_samples, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		uint16_t *samps = (uint16_t *)digital->getSamplesP(nb_samples);
		for (auto i = 0; i < 10; i++) {
			auto v = samps[i];
			printf("%f\n", samps[i]);
		}
		memcpy(digital_samples, samps, nb_samples * sizeof(uint16_t));
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't read samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

uint16_t libm2k_digital_get_nb_channels(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return 0;
	}
	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		return digital->getNbChannelsIn();
	}
	catch (std::exception &e) {
		auto str = "Can't read samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return 0;
	}
}

double libm2k_digital_samplerate_get_out(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}

	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		return digital->getSampleRateOut();
	}
	catch (std::exception &e) {
		auto str = "Can't read samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_digital_samplerate_set_out(uintptr_t _m2k_instance, double samplerate, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}

	try {
		libm2k::digital::M2kDigital *digital = instance->getDigital();
		digital->setSampleRateOut(samplerate);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't write samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_in_stop(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		dig->stopAcquisition();
	}
	catch (std::exception &e) {
		auto str = "Can't stop Digital IN; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_in_cancel_buffer(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		dig->cancelAcquisition();
	}
	catch (std::exception &e) {
		auto str = "Can't cancel Digital IN; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_out_push(uintptr_t _m2k_instance, uint64_t* data, uint64_t nb_samples, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		dig->push((unsigned short*)data, nb_samples);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't write samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_out_stop(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		dig->stopBufferOut();
	}
	catch (std::exception &e) {
		auto str = "Can't stop Digital OUT; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_out_cancel_buffer(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		dig->cancelBufferOut();
	}
	catch (std::exception &e) {
		auto str = "Can't cancel Digital OUT; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_digital_configure_trigger(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t dig_condition, uint64_t dig_mode, 
	int64_t digital_delay, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::digital::M2kDigital *dig = instance->getDigital();
		libm2k::M2kHardwareTrigger *trig = dig->getTrigger();
		trig->setDigitalCondition(chnIdx, static_cast<libm2k::M2K_TRIGGER_CONDITION_DIGITAL>(dig_condition));
		trig->setDigitalMode(static_cast<libm2k::digital::DIO_TRIGGER_MODE>(dig_mode));
		trig->setDigitalDelay(digital_delay);
	}
	catch (std::exception &e) {
		auto str = "Can't configure digital trigger; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}


/* Analog Out */
void libm2k_analog_out_channel_enable_cyclic(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t enable, uint64_t cyclic, char * err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
		aout->enableChannel(chnIdx, static_cast<bool>(enable));
		aout->setCyclic(chnIdx, static_cast<bool>(cyclic));
	}
	catch (std::exception &e) {
		auto str = "Channel not enabled; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_analog_out_samplerate_get(uintptr_t _m2k_instance, uint64_t chnIdx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return -1;
	}

	try {
		libm2k::analog::M2kAnalogOut *analogOut = instance->getAnalogOut();
		return analogOut->getSampleRate(chnIdx);
	}
	catch (std::exception &e) {
		auto str = "Can't read samplerate; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return -1;
	}
}

void libm2k_analog_out_samplerate_set(uintptr_t _m2k_instance, uint64_t chnIdx, double samplerate, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
	aout->setSampleRate(chnIdx, samplerate);
}

void libm2k_analog_out_push(uintptr_t _m2k_instance, uint64_t chnIdx, double* data, uint64_t nb_samples, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
		aout->pushBytes(chnIdx, data, nb_samples);
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't write samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_out_push_all(uintptr_t _m2k_instance, double* data_1, double* data_2, uint64_t nb_samples_per_chn, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();

		std::vector<double> datav_1(data_1, data_1 + nb_samples_per_chn);
		std::vector<double> datav_2(data_2, data_2 + nb_samples_per_chn);
		aout->push({datav_1, datav_2});
		return;
	}
	catch (std::exception &e) {
		auto str = "Can't write synchronized samples; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

uint16_t libm2k_analog_out_get_nb_channels(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return 0;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
		return aout->getNbChannels();
	}
	catch (std::exception &e) {
		auto str = "Can't read nb channels; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return 0;
	}
}

void libm2k_analog_out_stop(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
		aout->stop();
	}
	catch (std::exception &e) {
		auto str = "Can't stop DAC; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_analog_out_cancel_buffer(uintptr_t _m2k_instance, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kAnalogOut *aout = instance->getAnalogOut();
		aout->cancelBuffer();
	}
	catch (std::exception &e) {
		auto str = "Can't cancel DAC buffer; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}


/* Power Supplies */
void libm2k_power_supply_out_enable(uintptr_t _m2k_instance, uint64_t enable, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kPowerSupply *ps = instance->getPowerSupply();
		ps->enableChannel(0, static_cast<bool>(enable));
		ps->enableChannel(1, static_cast<bool>(enable));
	}
	catch (std::exception &e) {
		auto str = "Can't enable/disable Power Supplies; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

void libm2k_power_supply_write_voltage(uintptr_t _m2k_instance, uint64_t chnIdx, double voltage, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kPowerSupply *ps = instance->getPowerSupply();
		ps->pushChannel(chnIdx, voltage);
	}
	catch (std::exception &e) {
		auto str = "Can't write Power Supplies; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

double libm2k_power_supply_read_voltage(uintptr_t _m2k_instance, uint64_t chnIdx, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return 0;
	}
	try {
		libm2k::analog::M2kPowerSupply *ps = instance->getPowerSupply();
		return ps->readChannel(chnIdx);
	}
	catch (std::exception &e) {
		auto str = "Can't read Power Supplies; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return 0;
	}
}

void libm2k_power_supply_powerdown(uintptr_t _m2k_instance, uint64_t powerdown, char *err)
{
	libm2k::context::M2k* instance = _get_context_instance(_m2k_instance, err);
	if (!instance) {
		return;
	}
	try {
		libm2k::analog::M2kPowerSupply *ps = instance->getPowerSupply();
		ps->powerDownDacs(static_cast<bool>(powerdown));
	}
	catch (std::exception &e) {
		auto str = "Can't powerdown Power Supplies; Reason: " + std::string(e.what()) + "\n";
		strcpy(err, str.c_str());
		return;
	}
}

}