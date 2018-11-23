#include "libm2k/devicebuilder.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/devices.hpp"
#include "libm2k/genericanalogin.hpp"
#include "libm2k/dmm.hpp"

#include "libm2k/m2k.hpp"
#include "libm2k/m2kanalogin.hpp"
#include "libm2k/m2kanalogout.hpp"
#include "libm2k/m2kpowersupply.hpp"
#include "libm2k/m2kdigital.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "assert.h"

using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;

void test() {
	throw no_device_exception("no device found!");
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
	std::vector<std::string> lst = DeviceBuilder::listDevices();
	if (lst.size() > 0) {
		GenericDevice *d = DeviceBuilder::deviceOpen(lst.at(0).c_str());
		M2K* dev = d->toM2k();
		if (d) {
			try {
				std::shared_ptr<GenericAnalogIn> aIn = d->getAnalogIn(0);
				aIn->setSampleRate(1000);
//				aIn->setSampleRate(0, 30720000);
//				auto aIn2 = d->getAnalogIn("m2k-adc");

				std::shared_ptr<M2kAnalogIn> maIn = dev->getAnalogIn(0);
				maIn->setRange(M2kAnalogIn::ANALOG_IN_CHANNEL_1,
					       M2kAnalogIn::PLUS_MINUS_25V);

				std::shared_ptr<DMM> dmm = d->getDMM(0);
				std::vector<DMM::dmm_reading> readings = dmm->readAll();
				for (DMM::dmm_reading read : readings) {
					std::cout << read.name << " " << read.value << " " << read.unit;
				}

				bool ret = dev->resetCalibration();
				auto samps = aIn->getSamples(20);
				for (int i = 0; i < 20; i++) {
					std::cout << samps[0][i] << " ";
					std::cout << samps[1][i] << " ";
				}
				std::cout << std::endl;

				ret = dev->calibrateADC();

				// Analog Out
				std::shared_ptr<M2kAnalogOut> maOut = dev->getAnalogOut("m2k-dac-a");
				std::shared_ptr<M2kAnalogOut> mbOut = dev->getAnalogOut("m2k-dac-b");
				ret = dev->calibrateDAC();
				maOut->setSampleRate(75e6);
				mbOut->setSampleRate(75e6);
				maOut->setOversamplingRatio(1);
				mbOut->setOversamplingRatio(1);

				std::cout << "scaling factor " << maOut->getScalingFactor() << std::endl;

				std::vector<double> vec_a(1024, 4.2);
				std::vector<short> vec_b(1024, 0);
				maOut->push(vec_a, true);
				mbOut->push(vec_b, true);

				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				maIn->setSourceChannel(M2kAnalogIn::ANALOG_IN_CHANNEL_1);
				maIn->setTriggerDelay(-10);
				maIn->setLevel(M2kAnalogIn::ANALOG_IN_CHANNEL_1, 1);
				maIn->setTriggerMode(M2kAnalogIn::ANALOG_IN_CHANNEL_1, M2kHardwareTrigger::ALWAYS);
				maIn->setAnalogCondition(M2kAnalogIn::ANALOG_IN_CHANNEL_1, M2kHardwareTrigger::FALLING_EDGE);
				samps = maIn->getSamples(48, true);
				for (int i = 0; i < 48; i++) {
					std::cout << samps[0][i] << " ";
					std::cout << samps[1][i] << " ";
				}

				for (auto s : aIn->getAvailableSamplerates()) {
					std::cout << " s " << s << std::endl;
				}

				std::shared_ptr<M2kPowerSupply> psupply = dev->getPowerSupply();
				psupply->pushChannel(0, 3);
				psupply->enableChannel(0, true);
				psupply->enableChannel(0, false);


				// DIO
				std::shared_ptr<M2kDigital> logic = dev->getDigital();
				logic->enableChannelIn(M2kDigital::DIO_CHANNEL_1, true);
				logic->enableChannelIn(M2kDigital::DIO_CHANNEL_2, true);
				auto data = logic->getSamples(32);
				for (auto d : data) {
					std::cout << d << " ";
				}
				std::cout << std::endl;


			} catch (std::runtime_error &e) {
				std::cout << e.what() << "\n";
			}
//			auto anIn = d->openAnalogIn();
			d->blinkLed();
//			d->closeAnalogIn()
			//DeviceBuilder::deviceClose(d);
		}
		delete d;
	}

//	FMCOMMS *fm = new FMCOMMS();
//	M2K *m2kd = new M2K();

//	delete fm;
//	delete m2kd;
//	try {
//		test();
//	} catch(no_device_exception& e) {
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

