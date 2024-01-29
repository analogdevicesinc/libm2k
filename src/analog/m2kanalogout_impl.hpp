#ifndef M2KANALOGOUT_IMPL_HPP
#define M2KANALOGOUT_IMPL_HPP

#include <libm2k/analog/m2kanalogout.hpp>
#include "utils/devicegeneric.hpp"
#include "utils/deviceout.hpp"
#include <libm2k/enums.hpp>
#include <vector>
#include <memory>
#include <map>

using namespace libm2k;
using namespace libm2k::utils;

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
class M2kAnalogOutImpl : public M2kAnalogOut
{
public:
	M2kAnalogOutImpl(struct iio_context*, std::vector<std::string> dac_devs, bool sync);
	virtual ~M2kAnalogOutImpl();

	void reset() override;
	std::vector<int> getOversamplingRatio() override;

	int getOversamplingRatio(unsigned int chn) override;
	std::vector<int> setOversamplingRatio(std::vector<int> oversampling_ratio) override;
	int setOversamplingRatio(unsigned int chn, int oversampling_ratio) override;

	std::vector<double> getSampleRate() override;
	double getSampleRate(unsigned int chn) override;
	std::vector<double> getAvailableSampleRates(unsigned int chn) override;
	std::vector<double> setSampleRate(std::vector<double> samplerates) override;
	double setSampleRate(unsigned int chn, double samplerate) override;

	void setSyncedDma(bool en, int chn = -1) override;
	bool getSyncedDma(int chn = -1) override;
	void setSyncedStartDma(bool en, int chn = -1) override;
	bool getSyncedStartDma(int chn = -1) override;

	void setCyclic(bool en) override;
	void setCyclic(unsigned int chn, bool en) override;
	bool getCyclic(unsigned int chn) override;

	double setCalibscale(unsigned int index, double calibscale);
	double getCalibscale(unsigned int index);

	double getScalingFactor(unsigned int chn) override;

	double getFilterCompensation(double samplerate) override;

	short convVoltsToRaw(double voltage, double vlsb, double filterCompensation);

	void pushBytes(unsigned int chnIdx, double *data, unsigned int nb_samples) override;
	void pushRawBytes(unsigned int chnIdx, short *data, unsigned int nb_samples) override;

	void pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples) override;
	void pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples) override;

	void setDacCalibVlsb(unsigned int chn, double vlsb);

	void push(unsigned int chnIdx, std::vector<double> const &data) override;
	void pushRaw(unsigned int chnIdx, std::vector<short> const &data) override;
	void push(std::vector<std::vector<double>> const &data) override;
	void pushRaw(std::vector<std::vector<short>> const &data) override;

	void stop() override;
	void stop(unsigned int chn) override;

	void enableChannel(unsigned int chnIdx, bool enable) override;
	bool isChannelEnabled(unsigned int chnIdx) override;
	bool isPushDone(unsigned int chnIdx) const override;

	void setKernelBuffersCount(unsigned int chnIdx, unsigned int count) override;
	unsigned int getKernelBuffersCount(unsigned int chnIdx) const override;

	short convertVoltsToRaw(unsigned int channel, double voltage) override;
	double convertRawToVolts(unsigned int channel, short raw) override;

	struct IIO_OBJECTS getIioObjects() override;

	void cancelBuffer() override;
	void cancelBuffer(unsigned int chn) override;

	unsigned int getNbChannels() override;
	std::string getChannelName(unsigned int channel) override;
	double getMaximumSamplerate(unsigned int chn_idx) override;
	void deinitialize();

	void loadNbKernelBuffers();
	unsigned short setVoltage(unsigned int chn_idx, double volts) override;
	unsigned short setVoltageRaw(unsigned int chn_idx, unsigned short raw) override;

private:
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_m2k_fabric;
	std::vector<double> m_max_samplerate;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;
	std::vector<double> m_samplerate;

	std::map<double, double> m_filter_compensation_table;
	std::vector<libm2k::utils::DeviceOut*> m_dac_devices;

	bool m_dma_start_sync_available;
	bool m_dma_data_available;
	std::vector<unsigned int> m_nb_kernel_buffers;
	std::vector<bool> m_raw_enable_available;
	std::vector<bool> m_raw_available;

	DeviceOut* getDacDevice(unsigned int chnIdx) const;
	void syncDevice();
	double convRawToVolts(short raw, double vlsb, double filterCompensation);
};
}
}


#endif //M2KANALOGOUT_HPP
