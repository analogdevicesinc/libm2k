#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/devices.hpp>
#include <libm2k/analog/genericanalogin.hpp>
#include <libm2k/analog/genericanalogout.hpp>
#include <libm2k/analog/dmm.hpp>

#include <libm2k/m2k.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/digital/enums.hpp>
#include <math.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "assert.h"
#include <sstream>

using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;

void test() {
	throw_exception(EXC_RUNTIME_ERROR, "no device found");
}


// FACTORY DP
class interface {
public:
	virtual void A() = 0;
	virtual void B() = 0;
};

class AA: public interface {


	// interface interface
public:
	void A() override {
		std::cout << "AA::A()" << std::endl;
	}
	void B() override {
		std::cout << "AA::B()" << std::endl;
	}
};

class BB: public interface {


	// interface interface
public:
	void A() override {
		std::cout << "BB::A()" << std::endl;
	}
	void B() override {
		std::cout << "BB::B()" << std::endl;
	}
	void C() {
		std::cout << "BB::C()" << std::endl;
	}
};

class Factory {
public:
	static interface* make(int code) {
		if (code % 2) {
			return new AA();
		} else {
			return new BB();
		}
	}
};

// END FACTORY DP

// BRIDGE
class widget;

class widget_imp {
public:
	widget_imp(widget* w) : q_ptr(w) {
		std::cout << this << std::endl;
		std::cout << q_ptr << std::endl;
	}

	void a() {
		std::cout << "implementation a()" << std::endl;
	}
	void b() {
		std::cout << "implementation b()" << std::endl;
	}

private:
	widget *q_ptr; // => pointer to API
};

class widget {
public:
	widget() {
		d_ptr = new widget_imp(this);
		std::cout << d_ptr << std::endl;
		std::cout << this << std::endl;
	}

	~widget() {
		delete d_ptr;
	}

	void a() {
		d_ptr->a();
	}

	void b() {
		d_ptr->b();
	}

private:
	widget_imp *d_ptr; // d_impl // pimpl => pointer to implementation
};

//END BRIDGE


int main(int argc, char **argv)
{



	std::exception e ;
	std::vector<std::string> lst = ContextBuilder::listDevices();
	for (auto l : lst) {
		__try {
//			std::shared_ptr<GenericDevice> d = ContextBuilder::deviceOpen("ip:192.168.2.1");//lst.at(0).c_str());
//			std::shared_ptr<GenericDevice> d2 = ContextBuilder::deviceOpen("ip:192.168.2.1");//lst.at(0).c_str());
			Context* d = ContextBuilder::deviceOpen();//l.c_str());

			M2K* dev = d->toM2k();
			if (!dev) { // PLUTO
				GenericAnalogOut* plutoOut = d->getAnalogOut(0);
				plutoOut->enableChannel(0, true);
				plutoOut->enableChannel(1, true);
				plutoOut->setSampleRate(0, 3840000);
				plutoOut->setSampleRate(1, 3840000);
				vector<short> data(1024, 2048);

//				float amplitude = 2.5;
//				std::vector<std::vector<double>> data_all;
//				std::vector<double> data;
//				double angle = 0.0;
//				for (size_t i = 0; i < 1024  * 1024; ++i) {
//					angle += (2 * M_PI) / static_cast<double>(1024  * 1024);
//					float value = amplitude * static_cast<float>(std::sin(angle));
//					data.push_back(static_cast<double>(value));
//				}

				plutoOut->setCyclic(true);
				plutoOut->push(data, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(20000));
			}

			if (dev) { // M2K
				__try {
					dev->setLed(false);
					dev->setLed(true);
					GenericAnalogIn* aIn = d->getAnalogIn(0);
					aIn->setSampleRate(1000);

					M2kAnalogIn* maIn = dev->getAnalogIn();
					maIn->setRange(ANALOG_IN_CHANNEL_1,
						       PLUS_MINUS_25V);

					DMM* dmm = d->getDMM(0);
					std::vector<DMM_READING> readings = dmm->readAll();
					for (DMM_READING read : readings) {
						std::cout << read.name << " " << read.value << " " << read.unit;
					}

					dmm = d->getDMM(1);
					readings = dmm->readAll();
					for (DMM_READING read : readings) {
						std::cout << read.name << " " << read.value << " " << read.unit;
					}

					bool ret = dev->resetCalibration();
					aIn->enableChannel(0, true);
					aIn->enableChannel(1, true);
					auto samps = aIn->getSamplesRaw(20);
					for (int i = 0; i < 20; i++) {
						std::cout << samps[0][i] << " ";
						std::cout << samps[1][i] << " ";
					}
					std::cout << std::endl;

					ret = dev->calibrateADC();

					// Analog Out
					M2kAnalogOut* maOut = dev->getAnalogOut();
					ret = dev->calibrateDAC();
					maOut->setSampleRate(0, 75e6);
					maOut->setSampleRate(1, 75e6);
					maOut->setOversamplingRatio(0, 1);
					maOut->setOversamplingRatio(1, 1);

					std::cout << "scaling factor chn 0 " << maOut->getScalingFactor(0) << std::endl;
					std::cout << "scaling factor chn 1 " << maOut->getScalingFactor(1) << std::endl;

					std::vector<double> vec_a(1024, 4.1);
					std::vector<double> vec_b(1024, 2.1);
					std::vector<std::vector<double>> vec_all;
					vec_all.push_back(vec_a);
					vec_all.push_back(vec_b);

					maOut->setCyclic(true);
					maOut->enableChannel(0, true);
					maOut->enableChannel(1, true);
					maOut->push(vec_all);

					std::this_thread::sleep_for(std::chrono::milliseconds(100));

					auto trigger = maIn->getTrigger();
					if (trigger) {
						trigger->setSourceChannel(0);
						trigger->setDelay(-10);
						trigger->setLevel(ANALOG_IN_CHANNEL_1, 1);
						trigger->setTriggerMode(ANALOG_IN_CHANNEL_1, ALWAYS);
						trigger->setAnalogCondition(0, FALLING_EDGE);
					} else {
						maIn->setSourceChannel(ANALOG_IN_CHANNEL_1);
						maIn->setDelay(-10);
						maIn->setLevel(ANALOG_IN_CHANNEL_1, 1);
						maIn->setTriggerMode(ANALOG_IN_CHANNEL_1, ALWAYS);
						maIn->setAnalogCondition(ANALOG_IN_CHANNEL_1, FALLING_EDGE);
					}
					maIn->enableChannel(0, true);
					samps = maIn->getSamples(64);
					for (int i = 0; i < 64; i++) {
						std::cout << samps[0][i] << "\t ";
						std::cout << samps[1][i] << "\n ";
					}

					auto avgs = maIn->getVoltage();
					auto avgs2 = maIn->getVoltageRaw();

					std::cout << "Average voltage  " << avgs.at(0) << " " << avgs.at(1) << "\n";
					std::cout << "Average voltage  " << avgs2.at(0) << " " << avgs2.at(1) << "\n";
					std::cout << "Average voltage  " << maIn->getVoltage(0) << " " << maIn->getVoltage(1) << "\n";
					std::cout << "Average voltage  " << maIn->getVoltageRaw(0) << " " << maIn->getVoltageRaw(1) << "\n";
					std::cout << "Average voltage  " << maIn->getVoltage(ANALOG_IN_CHANNEL_1) << " "
						  << maIn->getVoltage(ANALOG_IN_CHANNEL_2) << "\n";

					for (auto s : aIn->getAvailableSampleRates()) {
						std::cout << " s " << s << std::endl;
					}

					M2kPowerSupply* psupply = dev->getPowerSupply();
					psupply->pushChannel(0, 3);
					psupply->enableChannel(0, true);
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					std::cout << "Psupply read 3V " << psupply->readChannel(0) << std::endl;
					psupply->enableChannel(0, false);


					// DIO
					M2kDigital* logic = dev->getDigital();
					//				logic->setTrigger(M2kDigital::DIO_CHANNEL_1, M2kHardwareTrigger::RISING_EDGE);
					auto data = logic->getSamples(32);
					for (auto d : data) {
						std::cout << d << " ";
					}
					std::cout << std::endl;

					logic->setTriggerMode(DIO_AND);
					logic->enableChannel(1, true);
					logic->setSampleRateIn(1000);
					logic->setDirection(DIO_CHANNEL_4, DIO_OUTPUT);
					logic->setDirection(DIO_CHANNEL_5, DIO_OUTPUT);
					logic->setValueRaw(DIO_CHANNEL_4, libm2k::digital::HIGH);
					logic->setValueRaw(DIO_CHANNEL_5, libm2k::digital::HIGH);


					logic->enableAllOut(true);
					logic->setDirection(65535);

					std::vector<short> vec_digital(512, 7);
					logic->setCyclic(true);
					logic->push(vec_digital);

					for (auto att : dev->getAvailableContextAttributes()) {
						std::cout << att << "   " << dev->getContextAttributeValue(att) << std::endl;
					}
				} __catch (exception_type &e) {
					std::cout << e.what() << "\n";
				}
				//			auto anIn = d->openAnalogIn();
//				dev->blinkLed();
				//			d->closeAnalogIn()
//				ContextBuilder::deviceClose(d);
//				ContextBuilder::deviceClose(d2);
				ContextBuilder::deviceClose(d);
			}
		} __catch (exception_type &e) {
			std::cout << "Err on connect: " << e.what() << std::endl;
			continue;

		}

	}

//	FMCOMMS *fm = new FMCOMMS();
//	M2K *m2kd = new M2K();

//	delete fm;
//	delete m2kd;
//	__try {
//		test();
//	} __catch(no_device_exception& e) {
//		std::cout << e.what() << std::endl;
//	}


	/*
	 *
	 *
	 *
	 * */
//	int x;
//	std::cin >> x;
//	if (x == 0) {
//		SharedMemoryManager::getInstance().write();
//	} else {
//		SharedMemoryManager::getInstance().read();
//	}

	//assert(false);

//	std::vector<interface *> v;
//	for (int i = 0; i < 10; ++i) {
//		v.push_back(Factory::make(i));
//	}
//	for(const auto& x : v) {
//		x->A();
//	}

//	widget a;
//	a.a();
//	a.b();

	//static_cast<BB*>(v[1])->C();

	return 0;
}

