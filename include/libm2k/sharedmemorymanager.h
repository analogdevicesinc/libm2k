#ifndef SHAREDMEMORYMANAGER_H
#define SHAREDMEMORYMANAGER_H

#include "m2kglobal.h"

#include "interprocess/shared_memory_object.hpp"
#include "interprocess/mapped_region.hpp"
#include "interprocess/sync/scoped_lock.hpp"
#include "interprocess/sync/interprocess_mutex.hpp"

using namespace boost::interprocess;

//exported just for testing purpose
class LIBM2K_API SharedMemoryManager
{

public:
	static SharedMemoryManager& getInstance();

	void write();
	void read();

	//void lock(Instrument *instrument);
	//void unlock(Instrument *instrument);

private:

	SharedMemoryManager();

	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("m2kSharedMemory"); }
		~shm_remove() { shared_memory_object::remove("m2kSharedMemory"); }
	};

	struct m2klib_config
	{
		m2klib_config()
		{
		}
		boost::interprocess::interprocess_mutex confic_mutex;
		std::bitset<10> config;
	};



};

#endif // SHAREDMEMORYMANAGER_H
