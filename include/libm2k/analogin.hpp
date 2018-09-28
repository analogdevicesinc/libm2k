#ifndef ANALOGIN_H
#define ANALOGIN_H

#include "m2kglobal.hpp"
#include "m2kexceptions.hpp"

#include <iio.h>
#include <vector>
#include <string>

class LIBM2K_API AnalogIn
{
public:
	AnalogIn(iio_context*, std::string&, bool buffered = false);
	~AnalogIn();

	enum ANALOG_IN_CHANNEL {
		ANALOG_IN_CHANNEL_1 = 1,
		ANALOG_IN_CHANNEL_2 = 2
	};

	enum GAIN_CONFIG {
		LOW_GAIN = 0,
		HIGH_GAIN = 1
	};

	enum M2K_RANGES {
		PLUS_MINUS_5V = 1,
		PLUS_MINUS_25V = 0
	};

	void calibrate(bool async = true);
	uint16_t getVoltageRaw(ANALOG_IN_CHANNEL ch);
	double getVoltage(ANALOG_IN_CHANNEL ch);
	double getScalingFactor(ANALOG_IN_CHANNEL ch);
	void setGain(ANALOG_IN_CHANNEL ch, GAIN_CONFIG gain);
	GAIN_CONFIG getGain(ANALOG_IN_CHANNEL ch);
	void setRange(M2K_RANGES range);
	void setRange(double min, double max);
	M2K_RANGES getRange();

	// buffered acquisition (Oscilloscope)
	// trigger channel?
	double getSampleRate();
	double setSampleRate(double sampleRate);
	double getTimeTrigger();
	void setTimeTrigger(double percent);
	double getVoltTrigger();
	void setVoltTrigger(double volt);
	void setEnableTrigger(bool en);
	bool getEnableTrigger();

	// buffer wrappers
	void createBuffer(size_t sampleCount, bool cyclic);
	size_t refillBuffer();

	//iterator
	//if we use a stl container just forward definitions to that container
	typedef std::vector<int>::iterator iterator;

	iterator begin()
	{
		return m_data.begin();
	}

	iterator end()
	{
		return m_data.end();
	}

	int* data()
	{
		return m_data.data();
	}

	struct iio_device* getIioDevice();
	std::string getIioDeviceName();


private:
	bool m_cyclic;
	bool m_buffered;
	struct iio_device* m_device;
	std::vector<int> m_data; // int??
};

#endif // ANALOGIN_H
