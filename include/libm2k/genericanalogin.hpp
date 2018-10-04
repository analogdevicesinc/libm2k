#ifndef GENERICANALOGIN_HPP
#define GENERICANALOGIN_HPP

#include "m2kglobal.hpp"
#include <string>
#include <vector>
#include <iio.h>

namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogIn {
public:
	GenericAnalogIn(struct iio_context* ctx, std::string adc_dev);
	virtual ~GenericAnalogIn();

	virtual double* getSamples(int nb_samples);
	virtual void openAnalogIn();
	virtual void closeAnalogIn();

	virtual double getSampleRate();
	virtual double getSampleRate(unsigned int);
	virtual double setSampleRate(double sampleRate);
	virtual double setSampleRate(unsigned int chn_idx, double sampleRate);

	void enableChannel(unsigned int index, bool enable);
	std::string getDeviceName();

	//iterator
	//if we use a stl container just forward definitions to that container
	typedef std::vector<double>::iterator iterator;

	iterator begin()
	{
		return m_data.begin();
	}

	iterator end()
	{
		return m_data.end();
	}

	double* data()
	{
		return m_data.data();
	}

protected:
	struct iio_context* m_ctx;
	struct iio_device* m_dev;
	std::vector<struct iio_channel*> m_channel_list;
	std::string m_dev_name;
	std::vector<double> m_data;
	unsigned int m_nb_channels;
	bool m_cyclic;
};
}
}

#endif // GENERICANALOGIN_HPP
