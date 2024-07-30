#ifndef OPENCL_INFO_LOGHELP_HPP
#define OPENCL_INFO_LOGHELP_HPP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <RapidOpenCL1/DebugStrings.hpp>
#include <sstream>
#include <string>
#include <vector>
#include "CustomWriter.hpp"

namespace Fsl
{
  class LogHelp : public CustomWriter
  {
  public:
    LogHelp() = default;

    template <typename T>
    inline std::string ToString(const std::vector<T>& container)
    {
      std::stringstream stream;
      auto itr = container.begin();
      const auto itrEnd = container.end();

      if (itr != itrEnd)
      {
        stream << *itr;
        ++itr;
      }

      while (itr != itrEnd)
      {
        stream << ", " << *itr;
        ++itr;
      }
      return stream.str();
    }

    template <typename T>
    void LogPlatformInfo(const std::string& text, const cl_platform_id platformId, const cl_platform_info platformInfo)
    {
      T result;
      Print("{}{}", text, (OpenCL::OpenCLHelper::TryGetPlatformInfo<T>(platformId, platformInfo, result) ? Fsl::ToString(result) : "Unavailable"));
    }

    template <typename T>
    void LogDeviceInfo(const std::string& text, const cl_device_id deviceId, const cl_device_info deviceInfo)
    {
      T result;
      Print("{}{}", text, (OpenCL::OpenCLHelper::TryGetDeviceInfo<T>(deviceId, deviceInfo, result) ? Fsl::ToString(result) : "Unavailable"));
    }

    // CL_PLATFORM_PROFILE (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_PROFILE(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_PROFILE
      LogPlatformInfo<std::string>("CL_PLATFORM_PROFILE: ", platformId, CL_PLATFORM_PROFILE);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_PROFILE: Unavailable");
#endif
    }

    // CL_PLATFORM_VERSION (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_VERSION(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_VERSION
      LogPlatformInfo<std::string>("CL_PLATFORM_VERSION: ", platformId, CL_PLATFORM_VERSION);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_VERSION: Unavailable");
#endif
    }

    // CL_PLATFORM_NAME (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_NAME(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_NAME
      LogPlatformInfo<std::string>("CL_PLATFORM_NAME: ", platformId, CL_PLATFORM_NAME);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_NAME: Unavailable");
#endif
    }

    // CL_PLATFORM_VENDOR (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_VENDOR(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_VENDOR
      LogPlatformInfo<std::string>("CL_PLATFORM_VENDOR: ", platformId, CL_PLATFORM_VENDOR);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_VENDOR: Unavailable");
#endif
    }

    // CL_PLATFORM_EXTENSIONS (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_EXTENSIONS(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_EXTENSIONS
      LogPlatformInfo<std::string>("CL_PLATFORM_EXTENSIONS: ", platformId, CL_PLATFORM_EXTENSIONS);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_EXTENSIONS: Unavailable");
#endif
    }


    // CL_PLATFORM_HOST_TIMER_RESOLUTION (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_HOST_TIMER_RESOLUTION(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_HOST_TIMER_RESOLUTION
      LogPlatformInfo<cl_ulong>("CL_PLATFORM_HOST_TIMER_RESOLUTION: ", platformId, CL_PLATFORM_HOST_TIMER_RESOLUTION);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_HOST_TIMER_RESOLUTION: Unavailable");
#endif
    }


    // CL_PLATFORM_ICD_SUFFIX_KHR (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_PLATFORM_ICD_SUFFIX_KHR(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_ICD_SUFFIX_KHR
      LogPlatformInfo<std::string>("CL_PLATFORM_ICD_SUFFIX_KHR: ", platformId, CL_PLATFORM_ICD_SUFFIX_KHR);
#else
      FSL_PARAM_NOT_USED(platformId);
      Print("CL_PLATFORM_ICD_SUFFIX_KHR: Unavailable");
#endif
    }


    // CL_DEVICE_ADDRESS_BITS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_ADDRESS_BITS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ADDRESS_BITS
      LogDeviceInfo<cl_uint>("CL_DEVICE_ADDRESS_BITS: ", deviceId, CL_DEVICE_ADDRESS_BITS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_ADDRESS_BITS: Unavailable");
#endif
    }

    // CL_DEVICE_AVAILABLE (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ADDRESS_BITS
      LogDeviceInfo<cl_bool>("CL_DEVICE_AVAILABLE: ", deviceId, CL_DEVICE_AVAILABLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_AVAILABLE: Unavailable");
#endif
    }

    // CL_DEVICE_BUILT_IN_KERNELS (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_BUILT_IN_KERNELS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_BUILT_IN_KERNELS
      LogDeviceInfo<std::string>("CL_DEVICE_BUILT_IN_KERNELS: ", deviceId, CL_DEVICE_BUILT_IN_KERNELS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_BUILT_IN_KERNELS: Unavailable");
#endif
    }


    // CL_DEVICE_COMPILER_AVAILABLE (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_COMPILER_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_COMPILER_AVAILABLE
      LogDeviceInfo<cl_bool>("CL_DEVICE_COMPILER_AVAILABLE: ", deviceId, CL_DEVICE_COMPILER_AVAILABLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_COMPILER_AVAILABLE: Unavailable");
#endif
    }

    // CL_DEVICE_DOUBLE_FP_CONFIG (cl_device_fp_config)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_DOUBLE_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
      LogDeviceInfo<cl_device_fp_config>("CL_DEVICE_DOUBLE_FP_CONFIG: ", deviceId, CL_DEVICE_DOUBLE_FP_CONFIG);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_DOUBLE_FP_CONFIG: Unavailable");
#endif
    }

    // CL_DEVICE_ENDIAN_LITTLE (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_ENDIAN_LITTLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ENDIAN_LITTLE
      LogDeviceInfo<cl_bool>("CL_DEVICE_ENDIAN_LITTLE: ", deviceId, CL_DEVICE_ENDIAN_LITTLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_ENDIAN_LITTLE: Unavailable");
#endif
    }

    // CL_DEVICE_ERROR_CORRECTION_SUPPORT (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_ERROR_CORRECTION_SUPPORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ERROR_CORRECTION_SUPPORT
      LogDeviceInfo<cl_bool>("CL_DEVICE_ERROR_CORRECTION_SUPPORT: ", deviceId, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_ERROR_CORRECTION_SUPPORT: Unavailable");
#endif
    }

    // CL_DEVICE_EXECUTION_CAPABILITIES (cl_device_exec_capabilities)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_EXECUTION_CAPABILITIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_EXECUTION_CAPABILITIES
      LogDeviceInfo<cl_device_exec_capabilities>("CL_DEVICE_EXECUTION_CAPABILITIES: ", deviceId, CL_DEVICE_EXECUTION_CAPABILITIES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_EXECUTION_CAPABILITIES: Unavailable");
#endif
    }


    // CL_DEVICE_EXTENSIONS (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_EXTENSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_EXTENSIONS
      LogDeviceInfo<std::string>("CL_DEVICE_EXTENSIONS: ", deviceId, CL_DEVICE_EXTENSIONS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_EXTENSIONS: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHE_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: ", deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHE_TYPE (cl_device_mem_cache_type)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHE_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
      LogDeviceInfo<cl_device_mem_cache_type>("CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: ", deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: ", deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_GLOBAL_MEM_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_GLOBAL_MEM_SIZE: ", deviceId, CL_DEVICE_GLOBAL_MEM_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_GLOBAL_MEM_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: ", deviceId, CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_HALF_FP_CONFIG (cl_device_fp_config)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_HALF_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_HALF_FP_CONFIG
      LogDeviceInfo<cl_device_fp_config>("CL_DEVICE_HALF_FP_CONFIG: ", deviceId, CL_DEVICE_HALF_FP_CONFIG);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_HALF_FP_CONFIG: Unavailable");
#endif
    }

    //  CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE?


    // CL_DEVICE_HOST_UNIFIED_MEMORY (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_HOST_UNIFIED_MEMORY(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
      LogDeviceInfo<cl_bool>("CL_DEVICE_HOST_UNIFIED_MEMORY: ", deviceId, CL_DEVICE_HOST_UNIFIED_MEMORY);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_HOST_UNIFIED_MEMORY: Unavailable");
#endif
    }


    // CL_DEVICE_IL_VERSION (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IL_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IL_VERSION
      LogDeviceInfo<std::string>("CL_DEVICE_IL_VERSION: ", deviceId, CL_DEVICE_IL_VERSION);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IL_VERSION: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_SUPPORT (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE_SUPPORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_SUPPORT
      LogDeviceInfo<cl_bool>("CL_DEVICE_IMAGE_SUPPORT: ", deviceId, CL_DEVICE_IMAGE_SUPPORT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE_SUPPORT: Unavailable");
#endif
    }

    // CL_DEVICE_IMAGE2D_MAX_HEIGHT (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE2D_MAX_HEIGHT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE2D_MAX_HEIGHT
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE2D_MAX_HEIGHT: ", deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE2D_MAX_HEIGHT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE2D_MAX_WIDTH (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE2D_MAX_WIDTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE2D_MAX_WIDTH
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE2D_MAX_WIDTH: ", deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE2D_MAX_WIDTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_DEPTH (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_DEPTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_DEPTH
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE3D_MAX_DEPTH: ", deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE3D_MAX_DEPTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_HEIGHT (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_HEIGHT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_HEIGHT
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE3D_MAX_HEIGHT: ", deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE3D_MAX_HEIGHT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_WIDTH (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_WIDTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_WIDTH
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE3D_MAX_WIDTH: ", deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE3D_MAX_WIDTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT
      LogDeviceInfo<cl_uint>("CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: ", deviceId, CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_MAX_BUFFER_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE_MAX_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_MAX_BUFFER_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE_MAX_BUFFER_SIZE: ", deviceId, CL_DEVICE_IMAGE_MAX_BUFFER_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE_MAX_BUFFER_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_IMAGE_PITCH_ALIGNMENT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE_PITCH_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_PITCH_ALIGNMENT
      LogDeviceInfo<cl_uint>("CL_DEVICE_IMAGE_PITCH_ALIGNMENT: ", deviceId, CL_DEVICE_IMAGE_PITCH_ALIGNMENT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE_PITCH_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_MAX_ARRAY_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_IMAGE_MAX_ARRAY_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_MAX_ARRAY_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_IMAGE_MAX_ARRAY_SIZE: ", deviceId, CL_DEVICE_IMAGE_MAX_ARRAY_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_IMAGE_MAX_ARRAY_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_LINKER_AVAILABLE (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_LINKER_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LINKER_AVAILABLE
      LogDeviceInfo<cl_bool>("CL_DEVICE_LINKER_AVAILABLE: ", deviceId, CL_DEVICE_LINKER_AVAILABLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_LINKER_AVAILABLE: Unavailable");
#endif
    }


    // CL_DEVICE_LOCAL_MEM_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_LOCAL_MEM_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LOCAL_MEM_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_LOCAL_MEM_SIZE: ", deviceId, CL_DEVICE_LOCAL_MEM_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_LOCAL_MEM_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_LOCAL_MEM_TYPE (cl_device_local_mem_type)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_LOCAL_MEM_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LOCAL_MEM_TYPE
      LogDeviceInfo<cl_device_local_mem_type>("CL_DEVICE_LOCAL_MEM_TYPE: ", deviceId, CL_DEVICE_LOCAL_MEM_TYPE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_LOCAL_MEM_TYPE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CLOCK_FREQUENCY (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_CLOCK_FREQUENCY(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CLOCK_FREQUENCY
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_CLOCK_FREQUENCY: ", deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_CLOCK_FREQUENCY: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_COMPUTE_UNITS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_COMPUTE_UNITS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_COMPUTE_UNITS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_COMPUTE_UNITS: ", deviceId, CL_DEVICE_MAX_COMPUTE_UNITS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_COMPUTE_UNITS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CONSTANT_ARGS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_CONSTANT_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CONSTANT_ARGS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_CONSTANT_ARGS: ", deviceId, CL_DEVICE_MAX_CONSTANT_ARGS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_CONSTANT_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: ", deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE: ", deviceId, CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_MEM_ALLOC_SIZE (cl_ulong)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_MEM_ALLOC_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_MEM_ALLOC_SIZE
      LogDeviceInfo<cl_ulong>("CL_DEVICE_MAX_MEM_ALLOC_SIZE: ", deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_MEM_ALLOC_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_NUM_SUB_GROUPS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_NUM_SUB_GROUPS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_NUM_SUB_GROUPS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_NUM_SUB_GROUPS: ", deviceId, CL_DEVICE_MAX_NUM_SUB_GROUPS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_NUM_SUB_GROUPS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_ON_DEVICE_EVENTS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_ON_DEVICE_EVENTS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_ON_DEVICE_EVENTS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_ON_DEVICE_EVENTS: ", deviceId, CL_DEVICE_MAX_ON_DEVICE_EVENTS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_ON_DEVICE_EVENTS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_ON_DEVICE_QUEUES (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_ON_DEVICE_QUEUES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_ON_DEVICE_QUEUES
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_ON_DEVICE_QUEUES: ", deviceId, CL_DEVICE_MAX_ON_DEVICE_QUEUES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_ON_DEVICE_QUEUES: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_PARAMETER_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_PARAMETER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_PARAMETER_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_MAX_PARAMETER_SIZE: ", deviceId, CL_DEVICE_MAX_PARAMETER_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_PARAMETER_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_PIPE_ARGS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_PIPE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_PIPE_ARGS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_PIPE_ARGS: ", deviceId, CL_DEVICE_MAX_PIPE_ARGS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_PIPE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_READ_IMAGE_ARGS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_READ_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_READ_IMAGE_ARGS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_READ_IMAGE_ARGS: ", deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_READ_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS: ", deviceId, CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_SAMPLERS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_SAMPLERS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_SAMPLERS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_SAMPLERS: ", deviceId, CL_DEVICE_MAX_SAMPLERS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_SAMPLERS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_GROUP_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_WORK_GROUP_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_GROUP_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_MAX_WORK_GROUP_SIZE: ", deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_WORK_GROUP_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: ", deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_ITEM_SIZES (size_t[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_WORK_ITEM_SIZES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_ITEM_SIZES
      std::vector<std::size_t> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, container))
      {
        Print("CL_DEVICE_MAX_WORK_ITEM_SIZES: Unavailable");
      }
      else
      {
        Print("CL_DEVICE_MAX_WORK_ITEM_SIZES: {}", ToString(container));
      }
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_WORK_ITEM_SIZES: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WRITE_IMAGE_ARGS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MAX_WRITE_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WRITE_IMAGE_ARGS
      LogDeviceInfo<cl_uint>("CL_DEVICE_MAX_WRITE_IMAGE_ARGS: ", deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MAX_WRITE_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MEM_BASE_ADDR_ALIGN (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MEM_BASE_ADDR_ALIGN(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MEM_BASE_ADDR_ALIGN
      LogDeviceInfo<cl_uint>("CL_DEVICE_MEM_BASE_ADDR_ALIGN: ", deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MEM_BASE_ADDR_ALIGN: Unavailable");
#endif
    }


    // CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
      LogDeviceInfo<cl_uint>("CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: ", deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_NAME (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NAME(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NAME
      LogDeviceInfo<std::string>("CL_DEVICE_NAME: ", deviceId, CL_DEVICE_NAME);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NAME: Unavailable");
#endif
    }

    // CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_INT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_INT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_INT
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF
      LogDeviceInfo<cl_uint>("CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF: ", deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF: Unavailable");
#endif
    }


    // CL_DEVICE_OPENCL_C_VERSION (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_OPENCL_C_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_OPENCL_C_VERSION
      LogDeviceInfo<std::string>("CL_DEVICE_OPENCL_C_VERSION: ", deviceId, CL_DEVICE_OPENCL_C_VERSION);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_OPENCL_C_VERSION: Unavailable");
#endif
    }


    // CL_DEVICE_PARENT_DEVICE (cl_device_id)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PARENT_DEVICE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARENT_DEVICE
      LogDeviceInfo<cl_device_id>("CL_DEVICE_PARENT_DEVICE: ", deviceId, CL_DEVICE_PARENT_DEVICE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PARENT_DEVICE: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_MAX_SUB_DEVICES (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PARTITION_MAX_SUB_DEVICES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_MAX_SUB_DEVICES
      LogDeviceInfo<cl_uint>("CL_DEVICE_PARTITION_MAX_SUB_DEVICES: ", deviceId, CL_DEVICE_PARTITION_MAX_SUB_DEVICES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PARTITION_MAX_SUB_DEVICES: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_PROPERTIES (cl_device_partition_property[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PARTITION_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_PROPERTIES
      std::vector<cl_device_partition_property> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_PARTITION_PROPERTIES, container))
      {
        Print("CL_DEVICE_PARTITION_PROPERTIES: Unavailable");
      }
      else
      {
        Print("CL_DEVICE_PARTITION_PROPERTIES: {}", ToString(container));
      }
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PARTITION_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_AFFINITY_DOMAIN (cl_device_affinity_domain)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PARTITION_AFFINITY_DOMAIN(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_AFFINITY_DOMAIN
      LogDeviceInfo<cl_device_affinity_domain>("CL_DEVICE_PARTITION_AFFINITY_DOMAIN: ", deviceId, CL_DEVICE_PARTITION_AFFINITY_DOMAIN);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PARTITION_AFFINITY_DOMAIN: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_TYPE (cl_device_partition_property[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PARTITION_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_TYPE
      std::vector<cl_device_partition_property> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_PARTITION_TYPE, container))
      {
        Print("CL_DEVICE_PARTITION_TYPE: Unavailable");
      }
      else
      {
        Print("CL_DEVICE_PARTITION_TYPE: {}", ToString(container));
      }
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PARTITION_TYPE: Unavailable");
#endif
    }

    // CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS
      LogDeviceInfo<cl_uint>("CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: ", deviceId, CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: Unavailable");
#endif
    }


    // CL_DEVICE_PIPE_MAX_PACKET_SIZE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PIPE_MAX_PACKET_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PIPE_MAX_PACKET_SIZE
      LogDeviceInfo<cl_uint>("CL_DEVICE_PIPE_MAX_PACKET_SIZE: ", deviceId, CL_DEVICE_PIPE_MAX_PACKET_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PIPE_MAX_PACKET_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_PLATFORM (cl_platform_id)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PLATFORM(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PLATFORM
      LogDeviceInfo<cl_platform_id>("CL_DEVICE_PLATFORM: ", deviceId, CL_DEVICE_PLATFORM);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PLATFORM: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: ", deviceId, CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }

    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF: ", deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF: Unavailable");
#endif
    }

    // CL_DEVICE_PRINTF_BUFFER_SIZE (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PRINTF_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PRINTF_BUFFER_SIZE
      LogDeviceInfo<std::size_t>("CL_DEVICE_PRINTF_BUFFER_SIZE: ", deviceId, CL_DEVICE_PRINTF_BUFFER_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PRINTF_BUFFER_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_PREFERRED_INTEROP_USER_SYNC (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_INTEROP_USER_SYNC(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_INTEROP_USER_SYNC
      LogDeviceInfo<cl_bool>("CL_DEVICE_PREFERRED_INTEROP_USER_SYNC: ", deviceId, CL_DEVICE_PREFERRED_INTEROP_USER_SYNC);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_INTEROP_USER_SYNC: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: ", deviceId, CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT
      LogDeviceInfo<cl_uint>("CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: ", deviceId, CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_PROFILE (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PROFILE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PROFILE
      LogDeviceInfo<std::string>("CL_DEVICE_PROFILE: ", deviceId, CL_DEVICE_PROFILE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PROFILE: Unavailable");
#endif
    }


    // CL_DEVICE_PROFILING_TIMER_RESOLUTION (size_t)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_PROFILING_TIMER_RESOLUTION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PROFILING_TIMER_RESOLUTION
      LogDeviceInfo<std::size_t>("CL_DEVICE_PROFILING_TIMER_RESOLUTION: ", deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_PROFILING_TIMER_RESOLUTION: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE
      LogDeviceInfo<cl_uint>("CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: ", deviceId, CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE
      LogDeviceInfo<cl_uint>("CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: ", deviceId, CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES (cl_command_queue_properties)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES
      LogDeviceInfo<cl_command_queue_properties>("CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES: ", deviceId, CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_HOST_PROPERTIES (cl_command_queue_properties)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_QUEUE_ON_HOST_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_HOST_PROPERTIES
      LogDeviceInfo<cl_command_queue_properties>("CL_DEVICE_QUEUE_ON_HOST_PROPERTIES: ", deviceId, CL_DEVICE_QUEUE_ON_HOST_PROPERTIES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_QUEUE_ON_HOST_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_PROPERTIES (cl_command_queue_properties)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_QUEUE_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_PROPERTIES
      LogDeviceInfo<cl_command_queue_properties>("CL_DEVICE_QUEUE_PROPERTIES: ", deviceId, CL_DEVICE_QUEUE_PROPERTIES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_QUEUE_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_REFERENCE_COUNT (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_REFERENCE_COUNT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_REFERENCE_COUNT
      LogDeviceInfo<cl_uint>("CL_DEVICE_REFERENCE_COUNT: ", deviceId, CL_DEVICE_REFERENCE_COUNT);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_REFERENCE_COUNT: Unavailable");
#endif
    }


    // CL_DEVICE_SINGLE_FP_CONFIG (cl_device_fp_config)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_SINGLE_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SINGLE_FP_CONFIG
      LogDeviceInfo<cl_device_fp_config>("CL_DEVICE_SINGLE_FP_CONFIG: ", deviceId, CL_DEVICE_SINGLE_FP_CONFIG);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_SINGLE_FP_CONFIG: Unavailable");
#endif
    }


    // CL_DEVICE_SPIR_VERSIONS (char [])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_SPIR_VERSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SPIR_VERSIONS
      LogGetDeviceInfo<std::string>("CL_DEVICE_SPIR_VERSIONS: ", deviceId, CL_DEVICE_SPIR_VERSIONS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_SPIR_VERSIONS: Unavailable");
#endif
    }


    // CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS (cl_bool)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS
      LogDeviceInfo<cl_bool>("CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS: ", deviceId, CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS: Unavailable");
#endif
    }


    // CL_DEVICE_SVM_CAPABILITIES (cl_device_svm_capabilities)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_SVM_CAPABILITIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SVM_CAPABILITIES
      LogDeviceInfo<cl_device_svm_capabilities>("CL_DEVICE_SVM_CAPABILITIES: ", deviceId, CL_DEVICE_SVM_CAPABILITIES);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_SVM_CAPABILITIES: Unavailable");
#endif
    }


    // CL_DEVICE_TERMINATE_CAPABILITY_KHR (cl_device_terminate_capability_khr)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_TERMINATE_CAPABILITY_KHR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_TERMINATE_CAPABILITY_KHR
      LogDeviceInfo<cl_device_terminate_capability_khr>("CL_DEVICE_TERMINATE_CAPABILITY_KHR: ", deviceId, CL_DEVICE_TERMINATE_CAPABILITY_KHR);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_TERMINATE_CAPABILITY_KHR: Unavailable");
#endif
    }


    // CL_DEVICE_TYPE (deviceType)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_TYPE
      cl_device_type result = 0;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo<cl_device_type>(deviceId, CL_DEVICE_TYPE, result))
      {
        Print("CL_DEVICE_TYPE: Unavailable");
      }
      else
      {
        Print("CL_DEVICE_TYPE: {}", RapidOpenCL1::Debug::DeviceTypeTostring(result));
      }
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_TYPE: Unavailable");
#endif
    }

    //  CL_DEVICE_VENDOR (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_VENDOR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VENDOR
      LogDeviceInfo<std::string>("CL_DEVICE_VENDOR: ", deviceId, CL_DEVICE_VENDOR);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_VENDOR: Unavailable");
#endif
    }


    //  CL_DEVICE_VENDOR_ID (cl_uint)
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_VENDOR_ID(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VENDOR_ID
      LogDeviceInfo<cl_uint>("CL_DEVICE_VENDOR_ID: ", deviceId, CL_DEVICE_VENDOR_ID);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_VENDOR_ID: Unavailable");
#endif
    }


    // CL_DEVICE_VERSION (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DEVICE_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VERSION
      LogDeviceInfo<std::string>("CL_DEVICE_VERSION: ", deviceId, CL_DEVICE_VERSION);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DEVICE_VERSION: Unavailable");
#endif
    }


    //  CL_DRIVER_VERSION (char[])
    // NOLINTNEXTLINE(readability-identifier-naming)
    inline void Log_CL_DRIVER_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DRIVER_VERSION
      LogDeviceInfo<std::string>("CL_DRIVER_VERSION: ", deviceId, CL_DRIVER_VERSION);
#else
      FSL_PARAM_NOT_USED(deviceId);
      Print("CL_DRIVER_VERSION: Unavailable");
#endif
    }
  };
}

#endif
