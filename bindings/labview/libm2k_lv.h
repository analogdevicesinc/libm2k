/*
 * Copyright (c) 2024 Analog Devices Inc.
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

#ifndef ADALM2000_LIBM2K_H
#define ADALM2000_LIBM2K_H

#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	/* Context */
LIBM2K_API int libm2k_context_open(uintptr_t *ret_m2k, uint8_t *uri, char *err);

LIBM2K_API int libm2k_context_close(uintptr_t *ret_m2k);

LIBM2K_API void libm2k_context_calibrate(uintptr_t _m2k_instance, char *err);

LIBM2K_API uint64_t libm2k_context_get_count();

LIBM2K_API int libm2k_context_get_at(uint64_t idx, char *uri, char *err);

LIBM2K_API int libm2k_context_get_description(uintptr_t _m2k_instance, char *description, char *err);

/* Analog  In*/

LIBM2K_API void libm2k_analog_channel_enable(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t enable, char * err);

LIBM2K_API double libm2k_analog_samplerate_get(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_samplerate_set(uintptr_t _m2k_instance, double samplerate, char *err);

/* Range_type: 0 - Low gain = PLUS_MINUS_25V
				1 - High gain = PLUS_MINUS_2_5V */
LIBM2K_API uint64_t libm2k_analog_range_get(uintptr_t _m2k_instance, uint64_t chn_idx, char *err);

LIBM2K_API void libm2k_analog_range_set(uintptr_t _m2k_instance, uint64_t chn_idx, uint64_t range_type, char *err);

LIBM2K_API double libm2k_analog_vertical_offset_get(uintptr_t _m2k_instance, uint64_t chn_idx, char *err);

LIBM2K_API void libm2k_analog_vertical_offset_set(uintptr_t _m2k_instance, uint64_t chn_idx, double vert_offset, char *err);

LIBM2K_API void libm2k_analog_init_stream_acquisition(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_get_samples(uintptr_t _m2k_instance, double *analog_samples, uint64_t nb_samples, char *err);

LIBM2K_API uint16_t libm2k_analog_get_nb_channels(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_in_stop(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_in_cancel_buffer(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_configure_channel_internal_trigger(uintptr_t _m2k_instance, uint64_t chnIdx, 
	double analog_level, uint64_t analog_condition, uint64_t analog_mode, char *err);

LIBM2K_API void libm2k_analog_configure_internal_trigger(uintptr_t _m2k_instance, int64_t analog_delay, 
	uint64_t analog_source, char *err);

LIBM2K_API double libm2k_analog_get_voltage_instant(uintptr_t _m2k_instance, uint64_t chnIdx, char *err);

LIBM2K_API void libm2k_analog_kernel_buffers_set(uintptr_t _m2k_instance, uint64_t kernel_buffers_count, char *err);

/* Digital */

LIBM2K_API uint64_t libm2k_digital_io_read(uintptr_t _m2k_instance, uint64_t group_idx, char *err);

LIBM2K_API void libm2k_digital_io_write(uintptr_t _m2k_instance, uint64_t group_idx, uint64_t value, char *err);

LIBM2K_API void libm2k_digital_channel_enable_out(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t output, uint64_t cyclic, char * err);

LIBM2K_API void libm2k_digital_all_channel_enable_out(uintptr_t _m2k_instance, uint64_t output, uint64_t cyclic, char *err);

LIBM2K_API double libm2k_digital_samplerate_get_in(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_samplerate_set_in(uintptr_t _m2k_instance, double samplerate, char *err);

LIBM2K_API void libm2k_digital_init_stream_acquisition(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_get_samples(uintptr_t _m2k_instance, uint16_t *digital_samples, uint64_t nb_samples, char *err);

LIBM2K_API uint16_t libm2k_digital_get_nb_channels(uintptr_t _m2k_instance, char *err);

LIBM2K_API double libm2k_digital_samplerate_get_out(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_samplerate_set_out(uintptr_t _m2k_instance, double samplerate, char *err);

LIBM2K_API void libm2k_digital_in_stop(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_in_cancel_buffer(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_out_push(uintptr_t _m2k_instance, uint64_t* data, uint64_t nb_samples, char *err);

LIBM2K_API void libm2k_digital_out_stop(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_out_cancel_buffer(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_digital_configure_trigger(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t dig_condition, uint64_t dig_mode, 
	int64_t digital_delay, char *err);

/* Analog  Out*/

LIBM2K_API void libm2k_analog_out_channel_enable_cyclic(uintptr_t _m2k_instance, uint64_t chnIdx, uint64_t enable, uint64_t cyclic, char * err);

LIBM2K_API double libm2k_analog_out_samplerate_get(uintptr_t _m2k_instance, uint64_t chnIdx, char *err);

LIBM2K_API void libm2k_analog_out_samplerate_set(uintptr_t _m2k_instance, uint64_t chnIdx, double samplerate, char *err);

LIBM2K_API void libm2k_analog_out_push(uintptr_t _m2k_instance, uint64_t chnIdx, double* data, uint64_t nb_samples, char *err);

LIBM2K_API void libm2k_analog_out_push_all(uintptr_t _m2k_instance, double* data_1, double* data_2, uint64_t nb_samples_per_chn, char *err);

LIBM2K_API uint16_t libm2k_analog_out_get_nb_channels(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_out_stop(uintptr_t _m2k_instance, char *err);

LIBM2K_API void libm2k_analog_out_cancel_buffer(uintptr_t _m2k_instance, char *err);

/* Power Supplies */
LIBM2K_API void libm2k_power_supply_out_enable(uintptr_t _m2k_instance, uint64_t enable, char *err);

LIBM2K_API void libm2k_power_supply_write_voltage(uintptr_t _m2k_instance, uint64_t chnIdx, double voltage, char *err);

LIBM2K_API double libm2k_power_supply_read_voltage(uintptr_t _m2k_instance, uint64_t chnIdx, char *err);

LIBM2K_API void libm2k_power_supply_powerdown(uintptr_t _m2k_instance, uint64_t powerdown, char *err);

libm2k::context::M2k* _get_context_instance(uintptr_t _m2k_instance, char *err);

#ifdef __cplusplus
}
#endif
#endif