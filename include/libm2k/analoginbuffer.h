#ifndef ANALOGINBUFFER_H
#define ANALOGINBUFFER_H

#include "m2kglobal.h"
#include "m2kexceptions.h"

#include <vector>

class LIBM2K_API AnalogIn
{
public:
	AnalogIn();
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


private:
	bool m_cyclic;
	std::vector<int> m_data; // int??
};

#endif // ANALOGINBUFFER_H
