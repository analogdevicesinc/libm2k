#include <libm2k/m2k.hpp>
#include <libm2k/lidar.hpp>
#include <libm2k/contextbuilder.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::context;

int main(int argc, char* argv[]) {
	// Make sure your board's IP is the same as the one specified in the
	// contextOpen
	Context *context = ContextBuilder::contextOpen("ip:10.48.65.123");
	libm2k::context::Lidar* lidar = dynamic_cast<Lidar*>(context);

	// Channel4 is enabled by default for internal implementation
	// reasons. The samples that can be received and used are available from
	// channels "voltage0", "voltage1", "voltage2" and "voltage3",
	// respectively, which are all disabled by default. Enable any one of
	// them, a combination or even all of them to receive samples.
	// In this example, "voltage0" and "voltage2" are enabled.
	lidar->channelEnableDisable("voltage0", true);
	// lidar->channelEnableDisable("voltage1", true);
	lidar->channelEnableDisable("voltage2", true);
	// lidar->channelEnableDisable("voltage3", true);

	// Read the given number of samples for all of the enabled channels. The
	// number of samples must be a multiple of 4
	unsigned int nb_samples = 512;
	auto values = lidar->readChannels(nb_samples);

	// values will now hold a map where each element is composed of a string
	// and a vector of int8 element. The string represents the channel name
	// and the vector of int8 elements contains the nb_samples read for
	// the given channel. All the enabled channels have en entry in this map.
	// The channel name and it's contents can now be printed out..
	for (auto itr = values.begin(); itr != values.end(); ++itr) {
		printf("%s\n", itr->first.c_str());
		for (auto v = itr->second.begin(); v != itr->second.end(); ++v)
			printf("%d ", *v);
		printf("\n\n");
	}
	// ...or used for whatever purposes you have in mind.

	return 0;
}
