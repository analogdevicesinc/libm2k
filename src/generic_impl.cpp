#include "generic_impl.hpp"
#include <vector>
#include <libm2k/utils/utils.hpp>
#include "utils/channel.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "analog/generic/genericanalogin_impl.hpp"
#include "analog/generic/genericanalogout_impl.hpp"
#include <iio.h>
#include <iostream>

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::context;
using namespace libm2k::utils;

GenericImpl::GenericImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync)
{
	/* Initialize the AnalogIn list */
	auto aIn_lst = scanAllAnalogIn();
	for (auto aIn : aIn_lst) {
		m_instancesAnalogIn.push_back(new GenericAnalogInImpl(ctx, aIn));
	}

	/* Initialize the AnalogIn list */
	auto aOut_lst = scanAllAnalogOut();
	for (auto aOut : aOut_lst) {
		m_instancesAnalogOut.push_back(new GenericAnalogOutImpl(ctx, aOut));
	}
}

GenericImpl::~GenericImpl()
{
	for (auto ain : m_instancesAnalogIn) {
		delete ain;
	}
	for (auto aout : m_instancesAnalogOut) {
		delete aout;
	}
	m_instancesAnalogIn.clear();
	m_instancesAnalogOut.clear();
}

std::vector<std::string> GenericImpl::scanAllAnalogIn()
{
	auto dev_list = getAllDevices();
	std::vector<std::string> aIn_lst = {};
	std::exception e;
	for (auto dev : dev_list) {
		if (isIioDeviceBufferCapable(dev) &&
				(getIioDeviceType(dev) == ANALOG_DEV) &&
				getIioDeviceDirection(dev) == INPUT) {
			aIn_lst.push_back(dev);
		}
	}
	return aIn_lst;
}

std::vector<std::string> GenericImpl::scanAllAnalogOut()
{
	auto dev_list = getAllDevices();
	std::vector<std::string> aOut_lst = {};
	for (auto dev : dev_list) {
		if (isIioDeviceBufferCapable(dev) &&
				(getIioDeviceType(dev) == ANALOG_DEV) &&
				getIioDeviceDirection(dev) == OUTPUT) {
			aOut_lst.push_back(dev);
		}
	}
	return aOut_lst;
}

unsigned int GenericImpl::getAnalogInCount()
{
	return m_instancesAnalogIn.size();
}

GenericAnalogIn* GenericImpl::getAnalogIn(unsigned int index)
{
	if (index < m_instancesAnalogIn.size()) {
		return m_instancesAnalogIn.at(index);
	} else {
		return nullptr;
	}
}

GenericAnalogIn* GenericImpl::getAnalogIn(std::string dev_name)
{
	for (GenericAnalogIn* d : m_instancesAnalogIn) {
		if (d->getName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

unsigned int GenericImpl::getAnalogOutCount()
{
	return m_instancesAnalogOut.size();
}

GenericAnalogOut* GenericImpl::getAnalogOut(unsigned int index)
{
	if (index < m_instancesAnalogOut.size()) {
		return m_instancesAnalogOut.at(index);
	} else {
		return nullptr;
	}
}

GenericAnalogOut* GenericImpl::getAnalogOut(std::string dev_name)
{
	for (GenericAnalogOut* d : m_instancesAnalogOut) {
		if (d->getName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

