/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef LIBM2K_THREAD_SAFE_QUEUE_H
#define LIBM2K_THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:
	SafeQueue() = default;

	~SafeQueue() = default;

	T pop()
	{
		std::unique_lock<std::mutex> mlock(locker);
		while (data.empty())
		{
			cond_.wait(mlock);
		}
		auto item = data.front();
		data.pop();
		return item;
	}

	void push(const T &item)
	{
		std::unique_lock<std::mutex> mlock(locker);
		data.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

private:
	std::queue<T> data;
	std::mutex locker;
	std::condition_variable cond_;
};


#endif //LIBM2K_THREAD_SAFE_QUEUE_H
