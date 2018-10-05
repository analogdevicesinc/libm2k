/*
 * Copyright 2018 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SHAREDMEMORYMANAGER_H
#define SHAREDMEMORYMANAGER_H

#include "libm2k/m2kglobal.hpp"

#include "interprocess/shared_memory_object.hpp"
#include "interprocess/mapped_region.hpp"
#include "interprocess/sync/scoped_lock.hpp"
#include "interprocess/sync/interprocess_mutex.hpp"


#include <bitset>

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
