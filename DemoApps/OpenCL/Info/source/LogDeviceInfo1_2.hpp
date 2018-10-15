#ifndef OPENCL_INFO_LOGDEVICEINFO1_2_HPP
#define OPENCL_INFO_LOGDEVICEINFO1_2_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include "LogHelp.hpp"

namespace Fsl
{
  inline void LogDeviceInfo1_2(LogHelp& rLog, const cl_device_id deviceId)
  {
    rLog.Log_CL_DEVICE_ADDRESS_BITS(deviceId);
    rLog.Log_CL_DEVICE_AVAILABLE(deviceId);
    rLog.Log_CL_DEVICE_BUILT_IN_KERNELS(deviceId);
    rLog.Log_CL_DEVICE_COMPILER_AVAILABLE(deviceId);
    rLog.Log_CL_DEVICE_DOUBLE_FP_CONFIG(deviceId);
    rLog.Log_CL_DEVICE_ENDIAN_LITTLE(deviceId);
    rLog.Log_CL_DEVICE_ERROR_CORRECTION_SUPPORT(deviceId);
    rLog.Log_CL_DEVICE_EXECUTION_CAPABILITIES(deviceId);
    rLog.Log_CL_DEVICE_EXTENSIONS(deviceId);
    rLog.Log_CL_DEVICE_GLOBAL_MEM_CACHE_SIZE(deviceId);
    rLog.Log_CL_DEVICE_GLOBAL_MEM_CACHE_TYPE(deviceId);
    rLog.Log_CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE(deviceId);
    rLog.Log_CL_DEVICE_GLOBAL_MEM_SIZE(deviceId);
    rLog.Log_CL_DEVICE_HALF_FP_CONFIG(deviceId);
    rLog.Log_CL_DEVICE_HOST_UNIFIED_MEMORY(deviceId);
    rLog.Log_CL_DEVICE_IMAGE_SUPPORT(deviceId);
    rLog.Log_CL_DEVICE_IMAGE2D_MAX_HEIGHT(deviceId);
    rLog.Log_CL_DEVICE_IMAGE2D_MAX_WIDTH(deviceId);
    rLog.Log_CL_DEVICE_IMAGE3D_MAX_DEPTH(deviceId);
    rLog.Log_CL_DEVICE_IMAGE3D_MAX_HEIGHT(deviceId);
    rLog.Log_CL_DEVICE_IMAGE3D_MAX_WIDTH(deviceId);
    rLog.Log_CL_DEVICE_IMAGE_MAX_BUFFER_SIZE(deviceId);
    rLog.Log_CL_DEVICE_IMAGE_MAX_ARRAY_SIZE(deviceId);
    rLog.Log_CL_DEVICE_LINKER_AVAILABLE(deviceId);
    rLog.Log_CL_DEVICE_LOCAL_MEM_SIZE(deviceId);
    rLog.Log_CL_DEVICE_LOCAL_MEM_TYPE(deviceId);
    rLog.Log_CL_DEVICE_MAX_CLOCK_FREQUENCY(deviceId);
    rLog.Log_CL_DEVICE_MAX_COMPUTE_UNITS(deviceId);
    rLog.Log_CL_DEVICE_MAX_CONSTANT_ARGS(deviceId);
    rLog.Log_CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE(deviceId);
    rLog.Log_CL_DEVICE_MAX_MEM_ALLOC_SIZE(deviceId);
    rLog.Log_CL_DEVICE_MAX_PARAMETER_SIZE(deviceId);
    rLog.Log_CL_DEVICE_MAX_READ_IMAGE_ARGS(deviceId);
    rLog.Log_CL_DEVICE_MAX_SAMPLERS(deviceId);
    rLog.Log_CL_DEVICE_MAX_WORK_GROUP_SIZE(deviceId);
    rLog.Log_CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS(deviceId);
    rLog.Log_CL_DEVICE_MAX_WORK_ITEM_SIZES(deviceId);
    rLog.Log_CL_DEVICE_MAX_WRITE_IMAGE_ARGS(deviceId);
    rLog.Log_CL_DEVICE_MEM_BASE_ADDR_ALIGN(deviceId);
    rLog.Log_CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE(deviceId);
    rLog.Log_CL_DEVICE_NAME(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_INT(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE(deviceId);
    rLog.Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF(deviceId);
    rLog.Log_CL_DEVICE_OPENCL_C_VERSION(deviceId);
    rLog.Log_CL_DEVICE_PARENT_DEVICE(deviceId);
    rLog.Log_CL_DEVICE_PARTITION_MAX_SUB_DEVICES(deviceId);
    rLog.Log_CL_DEVICE_PARTITION_PROPERTIES(deviceId);
    rLog.Log_CL_DEVICE_PARTITION_AFFINITY_DOMAIN(deviceId);
    rLog.Log_CL_DEVICE_PARTITION_TYPE(deviceId);
    rLog.Log_CL_DEVICE_PLATFORM(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF(deviceId);
    rLog.Log_CL_DEVICE_PRINTF_BUFFER_SIZE(deviceId);
    rLog.Log_CL_DEVICE_PREFERRED_INTEROP_USER_SYNC(deviceId);
    rLog.Log_CL_DEVICE_PROFILE(deviceId);
    rLog.Log_CL_DEVICE_PROFILING_TIMER_RESOLUTION(deviceId);
    rLog.Log_CL_DEVICE_QUEUE_PROPERTIES(deviceId);
    rLog.Log_CL_DEVICE_REFERENCE_COUNT(deviceId);
    rLog.Log_CL_DEVICE_SINGLE_FP_CONFIG(deviceId);
    rLog.Log_CL_DEVICE_TYPE(deviceId);
    rLog.Log_CL_DEVICE_VENDOR(deviceId);
    rLog.Log_CL_DEVICE_VENDOR_ID(deviceId);
    rLog.Log_CL_DEVICE_VERSION(deviceId);
    rLog.Log_CL_DRIVER_VERSION(deviceId);
  }
}

#endif
