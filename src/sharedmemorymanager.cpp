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


#include "sharedmemorymanager.hpp"
#include <iostream>



SharedMemoryManager &SharedMemoryManager::getInstance()
{
	static SharedMemoryManager SMM;

	return SMM;
}

void SharedMemoryManager::write()
{
	shm_remove shm_cleaner;

	shared_memory_object shm(create_only, "m2kSharedMemory", read_write);

	shm.truncate(sizeof(m2klib_config));

	mapped_region region(shm, read_write);

	void *address = region.get_address();

	m2klib_config* data = new (address) m2klib_config;

	{
		for (int i = 0 ; i < data->config.size(); ++i) {
			scoped_lock<interprocess_mutex> lock(data->confic_mutex);
			data->config.set(i, i % 2);
		}
	}
	int x;
	while (std::cin >> x && x) {
		std::cout << x << std::endl;
	}
}

void SharedMemoryManager::read()
{
	shared_memory_object shm(open_only, "m2kSharedMemory", read_write);

	mapped_region region(shm, read_write);

	void *address = region.get_address();

	m2klib_config* data = static_cast<m2klib_config *> (address);

	{

//		for (int i = 0; i < 10; ++i) {
			scoped_lock<interprocess_mutex> lock(data->confic_mutex);
			std::cout << data->config.to_string() << std::endl;
//		}
	}
}

SharedMemoryManager::SharedMemoryManager()
{

}
