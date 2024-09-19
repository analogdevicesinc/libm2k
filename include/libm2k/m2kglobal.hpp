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

#ifndef M2KGLOBAL_H
#define M2KGLOBAL_H

#if defined(SWIG_ON)
	#define LIBM2K_API
#else
	#ifdef _WIN32
		#ifdef LIBM2K_EXPORTS
			#define LIBM2K_API __declspec(dllexport)
		#else
			#define LIBM2K_API __declspec(dllimport)
		#endif
	#else
		#define LIBM2K_API __attribute__((visibility ("default")))
	#endif
#endif
#endif // M2KGLOBAL_H
