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

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "clrt.h"
#include "context.h"
#include "debug.h"
#include "mhandle.h"
#include "platform.h"




/*
 * Private Functions
 */

void clrt_context_free(void *data)
{
	struct _cl_context *context; 

	context = (struct _cl_context *) data;
	free(context->devices);
	free(context);
}




/*
 * Public Functions
 */


struct opencl_context_t *opencl_context_create(void)
{
	struct opencl_context_t *context;

	/* Initialize */
	context = xcalloc(1, sizeof(struct opencl_context_t));

	/* Return */
	return context;
}


void opencl_context_free(struct opencl_context_t *context)
{
	free(context);
}




/*
 * OpenCL API Functions
 */

cl_context clCreateContext(
	const cl_context_properties *properties,
	cl_uint num_devices,
	const cl_device_id *devices,
	void (*pfn_notify)(const char *, const void *, size_t , void *),
	void *user_data,
	cl_int *errcode_ret)
{
	int i;
	struct _cl_context *context;

	/* Debug */
	opencl_debug("call '%s'", __FUNCTION__);
	opencl_debug("\tproperties = %p", properties);
	opencl_debug("\tnum_devices = %u", num_devices);
	opencl_debug("\tdevices = %p", devices);
	opencl_debug("\tcallback = %p", pfn_notify);
	opencl_debug("\tuser_data = %p", user_data);
	opencl_debug("\terrcode_ret = %p", errcode_ret);

	EVG_OPENCL_ARG_NOT_SUPPORTED_NEQ(num_devices, 1);
	EVG_OPENCL_ARG_NOT_SUPPORTED_NEQ((int) pfn_notify, 0);
	EVG_OPENCL_ARG_NOT_SUPPORTED_NEQ((int) user_data, 0);

	if (!opencl_platform)
	{
		if (errcode_ret)
			*errcode_ret = CL_INVALID_PLATFORM;		
		return NULL;
	}

	if (!devices || !num_devices)
	{
		if (errcode_ret)
			*errcode_ret = CL_INVALID_VALUE;
		return NULL;
	}	

	context = xmalloc(sizeof (struct _cl_context));
	clrt_object_create(context, CLRT_OBJECT_CONTEXT, clrt_context_free);


	for (i = 0; i < num_devices; i++)
	{
		if (!verify_device(devices[i]))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_DEVICE;
			return NULL;
		}
	}

	context->num_devices = num_devices;
	context->devices = xmalloc(sizeof (struct _cl_device_id *) * context->num_devices);
	memcpy(context->devices, devices, sizeof devices[0] * num_devices);

	if (properties)
	{
		context->prop_count = getPropertiesCount(properties, sizeof (cl_context_properties));
		context->props = xmalloc(sizeof (cl_context_properties) * context->prop_count);
		copyProperties(context->props, properties, sizeof (cl_context_properties), context->prop_count);
	}
	else
	{
		context->props = NULL;
		context->prop_count = 0;
	}

	if (errcode_ret)
		*errcode_ret = CL_SUCCESS;

	return context;
}


cl_context clCreateContextFromType(
	const cl_context_properties *properties,
	cl_device_type device_type,
	void (*pfn_notify)(const char *, const void *, size_t , void *),
	void *user_data,
	cl_int *errcode_ret)
{
	/* Debug */
	opencl_debug("call '%s'", __FUNCTION__);
	opencl_debug("\tproperties = %p", properties);
	opencl_debug("\tdevice_type = 0x%x", (int) device_type);
	opencl_debug("\tcallback = %p", pfn_notify);
	opencl_debug("\tuser_data = %p", user_data);
	opencl_debug("\terrcode_ret = %p", errcode_ret);


	cl_uint num_devices;
	clGetDeviceIDs(opencl_platform, device_type, 0, NULL, &num_devices);

	if (num_devices == 0)
	{
		if (errcode_ret)
			*errcode_ret = CL_DEVICE_NOT_FOUND;
		return NULL;
	}

	cl_device_id *devices = xmalloc(sizeof devices[0] * num_devices);
	
	clGetDeviceIDs(opencl_platform, device_type, num_devices, devices, NULL);

	cl_context context = clCreateContext(properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
	free(devices);
	return context;
}


cl_int clRetainContext(
	cl_context context)
{
	/* Debug */
	opencl_debug("call '%s'", __FUNCTION__);
	opencl_debug("\tcontext = %p", context);

	return clrt_object_retain(context, CLRT_OBJECT_CONTEXT, CL_INVALID_CONTEXT);
}


cl_int clReleaseContext(
	cl_context context)
{
	/* Debug */
	opencl_debug("call '%s'", __FUNCTION__);
	opencl_debug("\tcontext = %p", context);

	return clrt_object_release(context, CLRT_OBJECT_CONTEXT, CL_INVALID_CONTEXT);
}


cl_int clGetContextInfo(
	cl_context context,
	cl_context_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	if (!clrt_object_verify(context, CLRT_OBJECT_CONTEXT))
		return CL_INVALID_CONTEXT;

	switch (param_name)
	{
		case CL_CONTEXT_REFERENCE_COUNT:
		{
			cl_int count = clrt_object_find(context, CLRT_OBJECT_CONTEXT)->ref_count;
			return populateParameter(&count, sizeof count, param_value_size, param_value, param_value_size_ret);
		}
		
		case CL_CONTEXT_DEVICES:
			return populateParameter(context->devices, sizeof (cl_device_id) * context->num_devices, param_value_size, param_value, param_value_size_ret);
		case CL_CONTEXT_PROPERTIES:
			if (context->props)
				return populateParameter(context->props, sizeof (cl_context_properties) * context->prop_count, param_value_size, param_value, param_value_size_ret);
			else
				return populateParameter(NULL, 0, param_value_size, param_value, param_value_size_ret);
			return CL_SUCCESS;
		default:
			return CL_INVALID_VALUE;
	}	
	return 0;
}
