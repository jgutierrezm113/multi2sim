/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef RUNTIME_OPENCL_CLCPU_H
#define RUNTIME_OPENCL_CLCPU_H

#include "device.h"


struct clrt_device_type_t *clcpu_create_device_type(void);


cl_int clcpu_device_type_init_devices(
	cl_uint num_entries, 
	cl_device_id *devices, 
	cl_uint *num_devices);

cl_bool clcpu_device_type_is_valid_binary(
	size_t length,
	const unsigned char *binary);


void *clcpu_device_type_create_kernel(
	void *handle, 
	const char *kernel_name, 
	cl_int *errcode_ret);

cl_int clcpu_device_type_set_kernel_arg(
	void *kernel, 
	cl_uint arg_index, 
	size_t arg_size, 
	const void *arg_value);

void clcpu_device_exceute_ndrange(
	void *device,
	void *kernel, 
	cl_uint work_dim, 
	const size_t *global_work_offset, 
	const size_t *global_work_size, 
	const size_t *local_work_size);

cl_int clcpu_device_check_kernel(
	void *kernel);

void clcpu_device_kernel_destroy(
	void *kernel);










#endif
