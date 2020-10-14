#ifndef M2KANALOGOUT_FAKE_IMPL_HPP
#define M2KANALOGOUT_FAKE_IMPL_HPP

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
class M2kFakeAnalogOutImpl : public M2kAnalogOut
{
public:
	M2kFakeAnalogOutImpl(struct iio_context*, std::vector<std::string> dac_devs, bool sync);
	virtual ~M2kFakeAnalogOutImpl();

	void reset();
	std::vector<int> getOversamplingRatio();

	int getOversamplingRatio(unsigned int chn);
	std::vector<int> setOversamplingRatio(std::vector<int> oversampling_ratio);
	int setOversamplingRatio(unsigned int chn, int oversampling_ratio);

	std::vector<double> getSampleRate();
	double getSampleRate(unsigned int chn);
	std::vector<double> getAvailableSampleRates(unsigned int chn);
	std::vector<double> setSampleRate(std::vector<double> samplerates);
	double setSampleRate(unsigned int chn, double samplerate);

	void setSyncedDma(bool en, int chn = -1);
	bool getSyncedDma(int chn = -1);
	void setSyncedStartDma(bool en, int chn = -1);
	bool getSyncedStartDma(int chn = -1);

	void setCyclic(bool en);
	void setCyclic(unsigned int chn, bool en);
	bool getCyclic(unsigned int chn);

	double setCalibscale(unsigned int index, double calibscale);
	double getCalibscale(unsigned int index);

	double getScalingFactor(unsigned int chn);

	double getFilterCompensation(double samplerate);

	short convVoltsToRaw(double voltage, double vlsb, double filterCompensation);

	void pushBytes(unsigned int chnIdx, double *data, unsigned int nb_samples);
	void pushRawBytes(unsigned int chnIdx, short *data, unsigned int nb_samples);

	void pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples);
	void pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples);

	void setDacCalibVlsb(unsigned int chn, double vlsb);

	void push(unsigned int chnIdx, std::vector<double> const &data);
	void pushRaw(unsigned int chnIdx, std::vector<short> const &data);
	void push(std::vector<std::vector<double>> const &data);
	void pushRaw(std::vector<std::vector<short>> const &data);

	void stop();
	void stop(unsigned int chn);

	void enableChannel(unsigned int chnIdx, bool enable);
	bool isChannelEnabled(unsigned int chnIdx);

	void setKernelBuffersCount(unsigned int chnIdx, unsigned int count);

	short convertVoltsToRaw(unsigned int channel, double voltage);
	double convertRawToVolts(unsigned int channel, short raw);

	struct IIO_OBJECTS getIioObjects();

	void cancelBuffer();
	void cancelBuffer(unsigned int chn);

	unsigned int getNbChannels();
	std::string getChannelName(unsigned int channel);
	double getMaximumSamplerate(unsigned int chn_idx) override;
	void deinitialize();

	static std::vector<std::vector<double>> data_buffers_raw;

private:
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_m2k_fabric;
	std::vector<double> m_max_samplerate;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;
	std::vector<double> m_samplerate;
	std::vector<int> m_oversampling_ratio;

	std::map<double, double> m_filter_compensation_table;
	std::vector<libm2k::utils::DeviceOut*> m_dac_devices;

	bool m_dma_start_sync_available;
	bool m_dma_data_available;
	std::vector<unsigned int> m_nb_kernel_buffers;


	DeviceOut* getDacDevice(unsigned int chnIdx);
	void syncDevice();
	double convRawToVolts(short raw, double vlsb, double filterCompensation);
};
}
}


#endif //M2KANALOGOUT_HPP
