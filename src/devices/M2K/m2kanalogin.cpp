#include "m2kanalogin.hpp"
#include "libm2k/genericanalogin.hpp"
#include "libm2k/m2kexceptions.hpp"

#include <iostream>

using namespace libm2k;
using namespace std;

libm2k::analog::M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
					 std::__cxx11::string adc_dev) :
	GenericAnalogIn(ctx, adc_dev)
{
	iio_device_attr_write_longlong(m_dev, "oversampling_ratio", 1);
	auto m2k_fabric = iio_context_find_device(m_ctx, "m2k-fabric");
	if (m2k_fabric) {
		auto chn0 = iio_device_find_channel(m2k_fabric, "voltage0", false);
		auto chn1 = iio_device_find_channel(m2k_fabric, "voltage1", false);
		if (chn0 && chn1) {
			iio_channel_attr_write_bool(chn0, "powerdown", false);
			iio_channel_attr_write_bool(chn1, "powerdown", false);
		}
	}
}

libm2k::analog::M2kAnalogIn::~M2kAnalogIn()
{

}

double libm2k::analog::M2kAnalogIn::convertRawToVolts(int)
{

}

int libm2k::analog::M2kAnalogIn::convertVoltsToRaw(double)
{

}

double* libm2k::analog::M2kAnalogIn::getSamples(int nb_samples)
{
//	double* samples;
//	if (!m_dev) {
//		throw no_device_exception("No such device: " + m_dev_name);
//	}
//	iio_context_set_timeout(m_ctx, 0);

//	/* First disable all channels */
//	int nb_channels = iio_device_get_channels_count(m_dev);
//	int i;
//	for (i = 0; i < nb_channels; i++)
//		iio_channel_disable(iio_device_get_channel(m_dev, i));

//	for (i = 0; i < nb_channels; i++)
//		iio_channel_enable(iio_device_get_channel(m_dev, i));

//	struct iio_buffer* buffer = iio_device_create_buffer(m_dev, nb_samples, false);
//	if (!buffer) {
//		throw instrument_already_in_use_exception("Cannot create buffer for " + m_dev_name);
//	}

//	ssize_t ret = iio_buffer_refill(buffer);
//	if (ret < 0) {
//		iio_buffer_destroy(buffer);
//		throw instrument_already_in_use_exception("Cannot refill buffer for " + m_dev_name);
//	}

//	samples = static_cast<double*> (iio_buffer_start(buffer));
////	iio_buffer_foreach_sample(buffer, print_sample, NULL);

//	iio_buffer_destroy(buffer);
//	return samples;

	return GenericAnalogIn::getSamples(nb_samples);
}

void analog::M2kAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void analog::M2kAnalogIn::closeAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void analog::M2kAnalogIn::calibrate(bool async)
{

}

double analog::M2kAnalogIn::getTimeTrigger()
{

}

