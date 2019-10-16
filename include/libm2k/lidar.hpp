#ifndef LIDAR_H
#define LIDAR_H

#include <map>
#include <vector>
#include <list>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>

using namespace std;

namespace libm2k {
	namespace contexts {
		class LIBM2K_API Lidar : public Context {
		public:
			Lidar(std::string uri, iio_context* ctx, std::string name, bool sync);
			virtual ~Lidar();

			void channelEnableDisable(string channel, bool enable);
			map<string, vector<int8_t>> readChannels(unsigned int nb_samples);
	   
			void setApdBias(unsigned int bias);
			void setTiltVoltage(unsigned int voltage);
			void laserEnable();
			void laserDisable();		
			void setLaserPulseWidth(long long pulse_width);
			void setLaserFrequency(long long frequency);

			void enableChannelSequencer();
			void disableChannelSequencer();
			void setSequencerPulseDelay(long long ns);

			void setChannelSequencerOpModeManual();
			void setChannelSequencerOpModeAuto();
			void setChannelSequencerOrderAutoMode(const char* order);
			void setChannelSequencerOrderManualMode(const char* order);
    
			// void enableTempCompensation();
			// void disableTempCompensation();
			// int readSystemTemp();
			// void setTempComputationCoeff(unsigned int);
			// //// Req. Update the APD bias based on the system temperature.    
			// void autoUpdateApdBias();

		private:
			class LidarImpl;
			std::shared_ptr<LidarImpl> m_pimpl;
		};
	}
}

#endif
