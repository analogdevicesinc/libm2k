#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <bitset>

using namespace std;
using namespace libm2k;
using namespace libm2k::digital;
using namespace libm2k::devices;

#define N_BITS (4)

int main()

{
	M2k *ctx = m2kOpen();
	M2kDigital *dig = ctx->getDigital();

	dig->setSampleRateIn(100000);
	dig->setSampleRateOut(100000);

	for(int i=0;i<N_BITS;i++)
	{
		dig->setDirection(i, DIO_OUTPUT);
		dig->enableChannel(i, true);
	}

	vector<unsigned short> bufferOut;
	vector<unsigned short> bufferIn;

	for(int i=0;i< (1<<N_BITS); i++)
	{
		bufferOut.push_back(i);
	}
	dig->setCyclic(true);
	dig->push(bufferOut);

	bufferIn = dig->getSamples(1000);
	for(auto val : bufferIn)
	{
		cout<<bitset<16>(val)<<endl;
	}
	dig->stop();
	deviceClose(ctx);

	return 0;
}
