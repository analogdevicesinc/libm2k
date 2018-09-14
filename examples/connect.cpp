#include "../include/libm2k/M2K.h"
#include "../include/libm2k/m2kexceptions.h"
#include "../include/libm2k/sharedmemorymanager.h"

#include <iostream>
#include "assert.h"
using namespace m2k;

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
//	std::vector<std::string> lst = DeviceBuilder::listDevices();
//	if (lst.size() > 0) {
//		DeviceBuilder *d = DeviceBuilder::deviceOpen(lst.at(0).c_str());
//		if (d) {
//			DeviceBuilder::deviceClose(d);
//		}
//	}

	std::vector<std::string> lst = DeviceBuilder::listDevices();
	if (lst.size() > 0) {
		GenericDevice *d = DeviceBuilder::deviceOpen(lst.at(0).c_str());
		if (d) {
			auto anIn = d->analogInOpen();
			DeviceBuilder::deviceClose(d);
		}
	}

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
	int x;
	std::cin >> x;
	if (x == 0) {
		SharedMemoryManager::getInstance().write();
	} else {
		SharedMemoryManager::getInstance().read();
	}

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

