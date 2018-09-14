#include "../include/libm2k/analogin.h"
#include "../include/libm2k/m2kexceptions.h"
#include <iio.h>

AnalogIn::AnalogIn(iio_context* ctx, std::string &_dev, bool buffered) :
	m_buffered(buffered)
{
	m_device = iio_context_find_device(ctx, _dev.c_str());
	if (!m_device) {
		m_device = nullptr;
		throw no_device_exception("Analog device not found");
	}

	unsigned int nb_channels = iio_device_get_channels_count(m_device);
}

AnalogIn::~AnalogIn()
{

}

void AnalogIn::calibrate(bool async)
{

}

uint16_t AnalogIn::getVoltageRaw(ANALOG_IN_CHANNEL ch)
{

}

double AnalogIn::getVoltage(ANALOG_IN_CHANNEL ch)
{

}

iio_device* AnalogIn::getIioDevice()
{
	return m_device;
}

std::string AnalogIn::getIioDeviceName()
{
	if (m_device) {
		return std::string(iio_device_get_name(m_device));
	}
	return "";
}
