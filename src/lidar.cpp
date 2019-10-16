#include "private/lidar_impl.cpp"

using namespace std;
using namespace libm2k::contexts;
using namespace libm2k::utils;

Lidar::Lidar(std::string uri, iio_context* ctx, std::string name, bool sync) :
	Context(new LidarImpl(uri, ctx, name, sync))
{
	m_pimpl = dynamic_pointer_cast<LidarImpl>(Context::m_pimpl);
}

Lidar::~Lidar()
{
}

void Lidar::channelEnableDisable(string channel, bool status) {
	m_pimpl->channelEnableDisable(channel, status);
}

map<string, vector<int8_t>> Lidar::readChannels(unsigned int nb_samples) {
	return m_pimpl->readChannels(nb_samples);
}

void Lidar::setApdBias(unsigned int bias) {
	m_pimpl->setApdBias(bias);
}

void Lidar::setTiltVoltage(unsigned int voltage) {
	m_pimpl->setTiltVoltage(voltage);
}

void Lidar::laserEnable() {
	m_pimpl->laserEnable();
}

void Lidar::laserDisable() {
	m_pimpl->laserDisable();
}

void Lidar::setLaserPulseWidth(long long pulse_width) {
	m_pimpl->setLaserPulseWidth(pulse_width);
}

void Lidar::setLaserFrequency(long long frequency) {
	m_pimpl->setLaserFrequency(frequency);
}

void Lidar::enableChannelSequencer() {
	m_pimpl->setChannelSequencerEnableDisable(true);
}

void Lidar::disableChannelSequencer() {
	m_pimpl->setChannelSequencerEnableDisable(false);
}

void Lidar::setSequencerPulseDelay(long long ns) {
	m_pimpl->setSequencerPulseDelay(ns);
}

void Lidar::setChannelSequencerOpModeManual() {
	m_pimpl->setChannelSequencerOpMode("manual");
}

void Lidar::setChannelSequencerOpModeAuto() {
	m_pimpl->setChannelSequencerOpMode("auto");
}

void Lidar::setChannelSequencerOrderAutoMode(const char* order) {
	m_pimpl->setChannelSequencerOrderAutoMode(order);
}

void Lidar::setChannelSequencerOrderManualMode(const char* order) {
	m_pimpl->setChannelSequencerOrderManualMode(order);
}

