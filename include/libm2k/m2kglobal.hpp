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
