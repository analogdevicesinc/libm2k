#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <thread>
#include <chrono>

#include <libm2k/devicebuilder.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/m2k.hpp>

#define M_PI 3.14159

using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;

int main(int argc, char **argv) {

	auto deviceList = DeviceBuilder::listDevices();
	if (deviceList.size() <= 0) {
		std::cout << "No device found! " << std::endl;
		return -1;
	}

	// GenericDeviced::sptr device
	Context* device = DeviceBuilder::deviceOpen(deviceList[0].c_str());
	M2K* m2k_device = device->toM2k();

	if (!m2k_device) {
		std::cout << "Need to plug in an m2k device!" << std::endl;
		return -1;
	}

	double frequency = 1000;
	size_t sample_rate = 750000;

	if (argc < 3) {
		std::cout << "Usage: ./siggen <frequency> <sample_rate>" << std::endl;
		std::cout << "Using default frequency: " << frequency
			  << " and default sample_rate: " << sample_rate << std::endl;
	} else {
		frequency = std::atof(argv[1]);
		sample_rate = static_cast<size_t>(std::atoi(argv[2]));
		std::cout << "Using frequency: " << frequency
			  << " and sample_rate: " << sample_rate << std::endl;
	}

	size_t buffer_size = (sample_rate / static_cast<size_t>(frequency));
	size_t buffer_size_per_period = (sample_rate / static_cast<size_t>(frequency));

	while (buffer_size & 0x3) {
		buffer_size <<=1;
	}
	while (buffer_size < 1024) {
		buffer_size <<=1;
	}

	float amplitude = 5;


	std::vector<std::vector<double>> data_all;
	std::vector<double> data;
	double angle = 0.0;
	for (size_t i = 0; i < buffer_size; ++i) {
		angle += (2 * M_PI) / static_cast<double>(buffer_size_per_period);
		float value = amplitude * static_cast<float>(std::sin(angle));
		data.push_back(static_cast<double>(value));
	}
	data_all.push_back(data);

	M2kAnalogOut* maOut = m2k_device->getAnalogOut();
	m2k_device->calibrateDAC();
	maOut->setSamplerate(0, sample_rate);
	maOut->setOversamplingRatio(0, 1);
	maOut->enableChannel(0, true);
	maOut->push(data_all);

	std::cout << "Sine wave generated; Sleeping 3 seconds before stopping" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	return 0;
}
