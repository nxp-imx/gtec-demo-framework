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

#include <FslBase/Log/Log.hpp>
#include <FslUtilOpenCL1_1/DebugStrings.hpp>
#include <FslUtilOpenCL1_1/OpenCLHelper.hpp>
#include <string>
#include <vector>
#include <sstream>

namespace Fsl
{
  class LogHelp
  {
    uint32_t m_currentIndent;
  public:
    std::string Indent;

    LogHelp()
      : m_currentIndent(0)
      , Indent()
    {
    }

    void PushIndent()
    {
      if (m_currentIndent >= 0x10)
        return;
      ++m_currentIndent;
      RebuildIndentString();
    }

    void PopIndent()
    {
      if (m_currentIndent == 0)
        return;

      --m_currentIndent;
      RebuildIndentString();
    }

    void RebuildIndentString()
    {
      Indent.clear();
      for (uint32_t i = 0; i < m_currentIndent; ++i)
      {
        Indent += "  ";
      }
    }


    template<typename T>
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

    // CL_PLATFORM_PROFILE (char[])
    inline void Log_CL_PLATFORM_PROFILE(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_PROFILE
      FSLLOG(Indent << "CL_PLATFORM_PROFILE: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_PROFILE));
#else
      FSLLOG(Indent << "CL_PLATFORM_PROFILE: Unavailable");
#endif
    }

    // CL_PLATFORM_VERSION (char[])
    inline void Log_CL_PLATFORM_VERSION(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_VERSION
      FSLLOG(Indent << "CL_PLATFORM_VERSION: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_VERSION));
#else
      FSLLOG(Indent << "CL_PLATFORM_VERSION: Unavailable");
#endif
    }

    // CL_PLATFORM_NAME (char[])
    inline void Log_CL_PLATFORM_NAME(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_NAME
      FSLLOG(Indent << "CL_PLATFORM_NAME: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_NAME));
#else
      FSLLOG(Indent << "CL_PLATFORM_NAME: Unavailable");
#endif
    }

    // CL_PLATFORM_VENDOR (char[])
    inline void Log_CL_PLATFORM_VENDOR(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_VENDOR
      FSLLOG(Indent << "CL_PLATFORM_VENDOR: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_VENDOR));
#else
      FSLLOG(Indent << "CL_PLATFORM_VENDOR: Unavailable");
#endif
    }

    // CL_PLATFORM_EXTENSIONS (char[])
    inline void Log_CL_PLATFORM_EXTENSIONS(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_EXTENSIONS
      FSLLOG(Indent << "CL_PLATFORM_EXTENSIONS: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_EXTENSIONS));
#else
      FSLLOG(Indent << "CL_PLATFORM_EXTENSIONS: Unavailable");
#endif
    }


    // CL_PLATFORM_HOST_TIMER_RESOLUTION (cl_ulong)
    inline void Log_CL_PLATFORM_HOST_TIMER_RESOLUTION(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_HOST_TIMER_RESOLUTION
      FSLLOG(Indent << "CL_PLATFORM_HOST_TIMER_RESOLUTION: " << OpenCL::OpenCLHelper::GetPlatformInfo<cl_ulong>(platformId, CL_PLATFORM_HOST_TIMER_RESOLUTION));
#else
      FSLLOG(Indent << "CL_PLATFORM_HOST_TIMER_RESOLUTION: Unavailable");
#endif
    }


    // CL_PLATFORM_ICD_SUFFIX_KHR (char[])
    inline void Log_CL_PLATFORM_ICD_SUFFIX_KHR(const cl_platform_id platformId)
    {
#ifdef CL_PLATFORM_ICD_SUFFIX_KHR
      FSLLOG(Indent << "CL_PLATFORM_ICD_SUFFIX_KHR: " << OpenCL::OpenCLHelper::GetPlatformInfo<std::string>(platformId, CL_PLATFORM_ICD_SUFFIX_KHR));
#else
      FSLLOG(Indent << "CL_PLATFORM_ICD_SUFFIX_KHR: Unavailable");
#endif
    }



    // CL_DEVICE_ADDRESS_BITS (cl_uint)
    inline void Log_CL_DEVICE_ADDRESS_BITS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ADDRESS_BITS
      FSLLOG(Indent << "CL_DEVICE_ADDRESS_BITS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_ADDRESS_BITS));
#else
      FSLLOG(Indent << "CL_DEVICE_ADDRESS_BITS: Unavailable");
#endif
    }

    // CL_DEVICE_AVAILABLE (cl_bool)
    inline void Log_CL_DEVICE_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ADDRESS_BITS
      FSLLOG(Indent << "CL_DEVICE_AVAILABLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_AVAILABLE));
#else
      FSLLOG(Indent << "CL_DEVICE_AVAILABLE: Unavailable");
#endif
    }

    // CL_DEVICE_BUILT_IN_KERNELS (char[])
    inline void Log_CL_DEVICE_BUILT_IN_KERNELS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_BUILT_IN_KERNELS
      FSLLOG(Indent << "CL_DEVICE_BUILT_IN_KERNELS: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_BUILT_IN_KERNELS));
#else
      FSLLOG(Indent << "CL_DEVICE_BUILT_IN_KERNELS: Unavailable");
#endif
    }


    // CL_DEVICE_COMPILER_AVAILABLE (cl_bool)
    inline void Log_CL_DEVICE_COMPILER_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_COMPILER_AVAILABLE
      FSLLOG(Indent << "CL_DEVICE_COMPILER_AVAILABLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_COMPILER_AVAILABLE));
#else
      FSLLOG(Indent << "CL_DEVICE_COMPILER_AVAILABLE: Unavailable");
#endif
    }

    // CL_DEVICE_DOUBLE_FP_CONFIG (cl_device_fp_config)
    inline void Log_CL_DEVICE_DOUBLE_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
      FSLLOG(Indent << "CL_DEVICE_DOUBLE_FP_CONFIG: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_fp_config>(deviceId, CL_DEVICE_DOUBLE_FP_CONFIG));
#else
      FSLLOG(Indent << "CL_DEVICE_DOUBLE_FP_CONFIG: Unavailable");
#endif
    }

    // CL_DEVICE_ENDIAN_LITTLE (cl_bool)
    inline void Log_CL_DEVICE_ENDIAN_LITTLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ENDIAN_LITTLE
      FSLLOG(Indent << "CL_DEVICE_ENDIAN_LITTLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_ENDIAN_LITTLE));
#else
      FSLLOG(Indent << "CL_DEVICE_ENDIAN_LITTLE: Unavailable");
#endif
    }

    // CL_DEVICE_ERROR_CORRECTION_SUPPORT (cl_bool)
    inline void Log_CL_DEVICE_ERROR_CORRECTION_SUPPORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_ERROR_CORRECTION_SUPPORT
      FSLLOG(Indent << "CL_DEVICE_ERROR_CORRECTION_SUPPORT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_ERROR_CORRECTION_SUPPORT));
#else
      FSLLOG(Indent << "CL_DEVICE_ERROR_CORRECTION_SUPPORT: Unavailable");
#endif
    }

    // CL_DEVICE_EXECUTION_CAPABILITIES (cl_device_exec_capabilities)
    inline void Log_CL_DEVICE_EXECUTION_CAPABILITIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_EXECUTION_CAPABILITIES
      FSLLOG(Indent << "CL_DEVICE_EXECUTION_CAPABILITIES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_exec_capabilities>(deviceId, CL_DEVICE_EXECUTION_CAPABILITIES));
#else
      FSLLOG(Indent << "CL_DEVICE_EXECUTION_CAPABILITIES: Unavailable");
#endif
    }


    // CL_DEVICE_EXTENSIONS (char[])
    inline void Log_CL_DEVICE_EXTENSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_EXTENSIONS
      FSLLOG(Indent << "CL_DEVICE_EXTENSIONS: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_EXTENSIONS));
#else
      FSLLOG(Indent << "CL_DEVICE_EXTENSIONS: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHE_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHE_TYPE (cl_device_mem_cache_type)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHE_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_mem_cache_type>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE));
#else
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_MEM_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_GLOBAL_MEM_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_MEM_SIZE
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_MEM_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE (size_t)
    inline void Log_CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_HALF_FP_CONFIG (cl_device_fp_config)
    inline void Log_CL_DEVICE_HALF_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_HALF_FP_CONFIG
      FSLLOG(Indent << "CL_DEVICE_HALF_FP_CONFIG: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_fp_config>(deviceId, CL_DEVICE_HALF_FP_CONFIG));
#else
      FSLLOG(Indent << "CL_DEVICE_HALF_FP_CONFIG: Unavailable");
#endif
    }

    //  CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE?


    // CL_DEVICE_HOST_UNIFIED_MEMORY (cl_bool)
    inline void Log_CL_DEVICE_HOST_UNIFIED_MEMORY(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
      FSLLOG(Indent << "CL_DEVICE_HOST_UNIFIED_MEMORY: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_HOST_UNIFIED_MEMORY));
#else
      FSLLOG(Indent << "CL_DEVICE_HOST_UNIFIED_MEMORY: Unavailable");
#endif
    }


    // CL_DEVICE_IL_VERSION (char[])
    inline void Log_CL_DEVICE_IL_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IL_VERSION
      FSLLOG(Indent << "CL_DEVICE_IL_VERSION: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_IL_VERSION));
#else
      FSLLOG(Indent << "CL_DEVICE_IL_VERSION: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_SUPPORT (cl_bool)
    inline void Log_CL_DEVICE_IMAGE_SUPPORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_SUPPORT
      FSLLOG(Indent << "CL_DEVICE_IMAGE_SUPPORT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_IMAGE_SUPPORT));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE_SUPPORT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE2D_MAX_HEIGHT (size_t)
    inline void Log_CL_DEVICE_IMAGE2D_MAX_HEIGHT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE2D_MAX_HEIGHT
      FSLLOG(Indent << "CL_DEVICE_IMAGE2D_MAX_HEIGHT: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE2D_MAX_HEIGHT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE2D_MAX_WIDTH (size_t)
    inline void Log_CL_DEVICE_IMAGE2D_MAX_WIDTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE2D_MAX_WIDTH
      FSLLOG(Indent << "CL_DEVICE_IMAGE2D_MAX_WIDTH: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE2D_MAX_WIDTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_DEPTH (size_t)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_DEPTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_DEPTH
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_DEPTH: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_DEPTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_HEIGHT (size_t)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_HEIGHT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_HEIGHT
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_HEIGHT: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_HEIGHT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE3D_MAX_WIDTH (size_t)
    inline void Log_CL_DEVICE_IMAGE3D_MAX_WIDTH(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE3D_MAX_WIDTH
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_WIDTH: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE3D_MAX_WIDTH: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT (cl_uint)
    inline void Log_CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT
      FSLLOG(Indent << "CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_MAX_BUFFER_SIZE (size_t)
    inline void Log_CL_DEVICE_IMAGE_MAX_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_MAX_BUFFER_SIZE
      FSLLOG(Indent << "CL_DEVICE_IMAGE_MAX_BUFFER_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE_MAX_BUFFER_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE_MAX_BUFFER_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_IMAGE_PITCH_ALIGNMENT (cl_uint)
    inline void Log_CL_DEVICE_IMAGE_PITCH_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_PITCH_ALIGNMENT
      FSLLOG(Indent << "CL_DEVICE_IMAGE_PITCH_ALIGNMENT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_IMAGE_PITCH_ALIGNMENT));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE_PITCH_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_IMAGE_MAX_ARRAY_SIZE (size_t)
    inline void Log_CL_DEVICE_IMAGE_MAX_ARRAY_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_IMAGE_MAX_ARRAY_SIZE
      FSLLOG(Indent << "CL_DEVICE_IMAGE_MAX_ARRAY_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_IMAGE_MAX_ARRAY_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_IMAGE_MAX_ARRAY_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_LINKER_AVAILABLE (cl_bool)
    inline void Log_CL_DEVICE_LINKER_AVAILABLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LINKER_AVAILABLE
      FSLLOG(Indent << "CL_DEVICE_LINKER_AVAILABLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_LINKER_AVAILABLE));
#else
      FSLLOG(Indent << "CL_DEVICE_LINKER_AVAILABLE: Unavailable");
#endif
    }


    // CL_DEVICE_LOCAL_MEM_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_LOCAL_MEM_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LOCAL_MEM_SIZE
      FSLLOG(Indent << "CL_DEVICE_LOCAL_MEM_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_LOCAL_MEM_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_LOCAL_MEM_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_LOCAL_MEM_TYPE (cl_device_local_mem_type)
    inline void Log_CL_DEVICE_LOCAL_MEM_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_LOCAL_MEM_TYPE
      FSLLOG(Indent << "CL_DEVICE_LOCAL_MEM_TYPE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_local_mem_type>(deviceId, CL_DEVICE_LOCAL_MEM_TYPE));
#else
      FSLLOG(Indent << "CL_DEVICE_LOCAL_MEM_TYPE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CLOCK_FREQUENCY (cl_uint)
    inline void Log_CL_DEVICE_MAX_CLOCK_FREQUENCY(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CLOCK_FREQUENCY
      FSLLOG(Indent << "CL_DEVICE_MAX_CLOCK_FREQUENCY: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_CLOCK_FREQUENCY: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_COMPUTE_UNITS (cl_uint)
    inline void Log_CL_DEVICE_MAX_COMPUTE_UNITS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_COMPUTE_UNITS
      FSLLOG(Indent << "CL_DEVICE_MAX_COMPUTE_UNITS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_COMPUTE_UNITS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CONSTANT_ARGS (cl_uint)
    inline void Log_CL_DEVICE_MAX_CONSTANT_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CONSTANT_ARGS
      FSLLOG(Indent << "CL_DEVICE_MAX_CONSTANT_ARGS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_CONSTANT_ARGS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_CONSTANT_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
      FSLLOG(Indent << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE (size_t)
    inline void Log_CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE
      FSLLOG(Indent << "CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_MEM_ALLOC_SIZE (cl_ulong)
    inline void Log_CL_DEVICE_MAX_MEM_ALLOC_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_MEM_ALLOC_SIZE
      FSLLOG(Indent << "CL_DEVICE_MAX_MEM_ALLOC_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_MEM_ALLOC_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_NUM_SUB_GROUPS (cl_uint)
    inline void Log_CL_DEVICE_MAX_NUM_SUB_GROUPS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_NUM_SUB_GROUPS
      FSLLOG(Indent << "CL_DEVICE_MAX_NUM_SUB_GROUPS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_NUM_SUB_GROUPS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_NUM_SUB_GROUPS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_ON_DEVICE_EVENTS (cl_uint)
    inline void Log_CL_DEVICE_MAX_ON_DEVICE_EVENTS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_ON_DEVICE_EVENTS
      FSLLOG(Indent << "CL_DEVICE_MAX_ON_DEVICE_EVENTS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_ON_DEVICE_EVENTS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_ON_DEVICE_EVENTS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_ON_DEVICE_QUEUES (cl_uint)
    inline void Log_CL_DEVICE_MAX_ON_DEVICE_QUEUES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_ON_DEVICE_QUEUES
      FSLLOG(Indent << "CL_DEVICE_MAX_ON_DEVICE_QUEUES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_ON_DEVICE_QUEUES));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_ON_DEVICE_QUEUES: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_PARAMETER_SIZE (size_t)
    inline void Log_CL_DEVICE_MAX_PARAMETER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_PARAMETER_SIZE
      FSLLOG(Indent << "CL_DEVICE_MAX_PARAMETER_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_MAX_PARAMETER_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_PARAMETER_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_PIPE_ARGS (cl_uint)
    inline void Log_CL_DEVICE_MAX_PIPE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_PIPE_ARGS
      FSLLOG(Indent << "CL_DEVICE_MAX_PIPE_ARGS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_PIPE_ARGS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_PIPE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_READ_IMAGE_ARGS (cl_uint)
    inline void Log_CL_DEVICE_MAX_READ_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_READ_IMAGE_ARGS
      FSLLOG(Indent << "CL_DEVICE_MAX_READ_IMAGE_ARGS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_READ_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS (cl_uint)
    inline void Log_CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS
      FSLLOG(Indent << "CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_SAMPLERS (cl_uint)
    inline void Log_CL_DEVICE_MAX_SAMPLERS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_SAMPLERS
      FSLLOG(Indent << "CL_DEVICE_MAX_SAMPLERS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_SAMPLERS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_SAMPLERS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_GROUP_SIZE (size_t)
    inline void Log_CL_DEVICE_MAX_WORK_GROUP_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_GROUP_SIZE
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_GROUP_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_GROUP_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS (cl_uint)
    inline void Log_CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WORK_ITEM_SIZES (size_t[])
    inline void Log_CL_DEVICE_MAX_WORK_ITEM_SIZES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WORK_ITEM_SIZES
      std::vector<std::size_t> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, container))
        throw std::runtime_error("clGetDeviceInfo for CL_DEVICE_MAX_WORK_ITEM_SIZES failed");
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_ITEM_SIZES: " << ToString(container));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_WORK_ITEM_SIZES: Unavailable");
#endif
    }


    // CL_DEVICE_MAX_WRITE_IMAGE_ARGS (cl_uint)
    inline void Log_CL_DEVICE_MAX_WRITE_IMAGE_ARGS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MAX_WRITE_IMAGE_ARGS
      FSLLOG(Indent << "CL_DEVICE_MAX_WRITE_IMAGE_ARGS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS));
#else
      FSLLOG(Indent << "CL_DEVICE_MAX_WRITE_IMAGE_ARGS: Unavailable");
#endif
    }


    // CL_DEVICE_MEM_BASE_ADDR_ALIGN (cl_uint)
    inline void Log_CL_DEVICE_MEM_BASE_ADDR_ALIGN(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MEM_BASE_ADDR_ALIGN
      FSLLOG(Indent << "CL_DEVICE_MEM_BASE_ADDR_ALIGN: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN));
#else
      FSLLOG(Indent << "CL_DEVICE_MEM_BASE_ADDR_ALIGN: Unavailable");
#endif
    }


    // CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE (cl_uint)
    inline void Log_CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
      FSLLOG(Indent << "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_NAME (char[])
    inline void Log_CL_DEVICE_NAME(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NAME
      FSLLOG(Indent << "CL_DEVICE_NAME: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_NAME));
#else
      FSLLOG(Indent << "CL_DEVICE_NAME: Unavailable");
#endif
    }

    // CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_INT (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_INT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_INT
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: Unavailable");
#endif
    }


    // CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF (cl_uint)
    inline void Log_CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF));
#else
      FSLLOG(Indent << "CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF: Unavailable");
#endif
    }


    // CL_DEVICE_OPENCL_C_VERSION (char[])
    inline void Log_CL_DEVICE_OPENCL_C_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_OPENCL_C_VERSION
      FSLLOG(Indent << "CL_DEVICE_OPENCL_C_VERSION: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_OPENCL_C_VERSION));
#else
      FSLLOG(Indent << "CL_DEVICE_OPENCL_C_VERSION: Unavailable");
#endif
    }


    // CL_DEVICE_PARENT_DEVICE (cl_device_id)
    inline void Log_CL_DEVICE_PARENT_DEVICE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARENT_DEVICE
      FSLLOG(Indent << "CL_DEVICE_PARENT_DEVICE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_id>(deviceId, CL_DEVICE_PARENT_DEVICE));
#else
      FSLLOG(Indent << "CL_DEVICE_PARENT_DEVICE: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_MAX_SUB_DEVICES (cl_uint)
    inline void Log_CL_DEVICE_PARTITION_MAX_SUB_DEVICES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_MAX_SUB_DEVICES
      FSLLOG(Indent << "CL_DEVICE_PARTITION_MAX_SUB_DEVICES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PARTITION_MAX_SUB_DEVICES));
#else
      FSLLOG(Indent << "CL_DEVICE_PARTITION_MAX_SUB_DEVICES: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_PROPERTIES (cl_device_partition_property[])
    inline void Log_CL_DEVICE_PARTITION_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_PROPERTIES
      std::vector<cl_device_partition_property> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_PARTITION_PROPERTIES, container))
        throw std::runtime_error("clGetDeviceInfo for CL_DEVICE_PARTITION_PROPERTIES failed");
      FSLLOG(Indent << "CL_DEVICE_PARTITION_PROPERTIES: " << ToString(container));
#else
      FSLLOG(Indent << "CL_DEVICE_PARTITION_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_AFFINITY_DOMAIN (cl_device_affinity_domain)
    inline void Log_CL_DEVICE_PARTITION_AFFINITY_DOMAIN(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_AFFINITY_DOMAIN
      FSLLOG(Indent << "CL_DEVICE_PARTITION_AFFINITY_DOMAIN: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_affinity_domain>(deviceId, CL_DEVICE_PARTITION_AFFINITY_DOMAIN));
#else
      FSLLOG(Indent << "CL_DEVICE_PARTITION_AFFINITY_DOMAIN: Unavailable");
#endif
    }


    // CL_DEVICE_PARTITION_TYPE (cl_device_partition_property[])
    inline void Log_CL_DEVICE_PARTITION_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PARTITION_TYPE
      std::vector<cl_device_partition_property> container;
      if (!OpenCL::OpenCLHelper::TryGetDeviceInfo(deviceId, CL_DEVICE_PARTITION_TYPE, container))
        throw std::runtime_error("clGetDeviceInfo for CL_DEVICE_PARTITION_TYPE failed");
      FSLLOG(Indent << "CL_DEVICE_PARTITION_TYPE: " << ToString(container));
#else
      FSLLOG(Indent << "CL_DEVICE_PARTITION_TYPE: Unavailable");
#endif
    }

    // CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS (cl_uint)
    inline void Log_CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS
      FSLLOG(Indent << "CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS));
#else
      FSLLOG(Indent << "CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS: Unavailable");
#endif
    }


    // CL_DEVICE_PIPE_MAX_PACKET_SIZE (cl_uint)
    inline void Log_CL_DEVICE_PIPE_MAX_PACKET_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PIPE_MAX_PACKET_SIZE
      FSLLOG(Indent << "CL_DEVICE_PIPE_MAX_PACKET_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PIPE_MAX_PACKET_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_PIPE_MAX_PACKET_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_PLATFORM (cl_platform_id)
    inline void Log_CL_DEVICE_PLATFORM(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PLATFORM
      FSLLOG(Indent << "CL_DEVICE_PLATFORM: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_platform_id>(deviceId, CL_DEVICE_PLATFORM));
#else
      FSLLOG(Indent << "CL_DEVICE_PLATFORM: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }

    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF: Unavailable");
#endif
    }

    // CL_DEVICE_PRINTF_BUFFER_SIZE (size_t)
    inline void Log_CL_DEVICE_PRINTF_BUFFER_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PRINTF_BUFFER_SIZE
      FSLLOG(Indent << "CL_DEVICE_PRINTF_BUFFER_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_PRINTF_BUFFER_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_PRINTF_BUFFER_SIZE: Unavailable");
#endif
    }

    // CL_DEVICE_PREFERRED_INTEROP_USER_SYNC (cl_bool)
    inline void Log_CL_DEVICE_PREFERRED_INTEROP_USER_SYNC(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_INTEROP_USER_SYNC
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_INTEROP_USER_SYNC: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_PREFERRED_INTEROP_USER_SYNC));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_INTEROP_USER_SYNC: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT (cl_uint)
    inline void Log_CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT));
#else
      FSLLOG(Indent << "CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT: Unavailable");
#endif
    }


    // CL_DEVICE_PROFILE (char[])
    inline void Log_CL_DEVICE_PROFILE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PROFILE
      FSLLOG(Indent << "CL_DEVICE_PROFILE: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_PROFILE));
#else
      FSLLOG(Indent << "CL_DEVICE_PROFILE: Unavailable");
#endif
    }


    // CL_DEVICE_PROFILING_TIMER_RESOLUTION (size_t)
    inline void Log_CL_DEVICE_PROFILING_TIMER_RESOLUTION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_PROFILING_TIMER_RESOLUTION
      FSLLOG(Indent << "CL_DEVICE_PROFILING_TIMER_RESOLUTION: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::size_t>(deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION));
#else
      FSLLOG(Indent << "CL_DEVICE_PROFILING_TIMER_RESOLUTION: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE (cl_uint)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE (cl_uint)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE));
#else
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES (cl_command_queue_properties)
    inline void Log_CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_command_queue_properties>(deviceId, CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES));
#else
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_ON_HOST_PROPERTIES (cl_command_queue_properties)
    inline void Log_CL_DEVICE_QUEUE_ON_HOST_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_ON_HOST_PROPERTIES
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_HOST_PROPERTIES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_command_queue_properties>(deviceId, CL_DEVICE_QUEUE_ON_HOST_PROPERTIES));
#else
      FSLLOG(Indent << "CL_DEVICE_QUEUE_ON_HOST_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_QUEUE_PROPERTIES (cl_command_queue_properties)
    inline void Log_CL_DEVICE_QUEUE_PROPERTIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_QUEUE_PROPERTIES
      FSLLOG(Indent << "CL_DEVICE_QUEUE_PROPERTIES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_command_queue_properties>(deviceId, CL_DEVICE_QUEUE_PROPERTIES));
#else
      FSLLOG(Indent << "CL_DEVICE_QUEUE_PROPERTIES: Unavailable");
#endif
    }


    // CL_DEVICE_REFERENCE_COUNT (cl_uint)
    inline void Log_CL_DEVICE_REFERENCE_COUNT(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_REFERENCE_COUNT
      FSLLOG(Indent << "CL_DEVICE_REFERENCE_COUNT: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_REFERENCE_COUNT));
#else
      FSLLOG(Indent << "CL_DEVICE_REFERENCE_COUNT: Unavailable");
#endif
    }


    // CL_DEVICE_SINGLE_FP_CONFIG (cl_device_fp_config)
    inline void Log_CL_DEVICE_SINGLE_FP_CONFIG(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SINGLE_FP_CONFIG
      FSLLOG(Indent << "CL_DEVICE_SINGLE_FP_CONFIG: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_fp_config>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG));
#else
      FSLLOG(Indent << "CL_DEVICE_SINGLE_FP_CONFIG: Unavailable");
#endif
    }


    // CL_DEVICE_SPIR_VERSIONS (char [])
    inline void Log_CL_DEVICE_SPIR_VERSIONS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SPIR_VERSIONS
      FSLLOG(Indent << "CL_DEVICE_SPIR_VERSIONS: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_SPIR_VERSIONS));
#else
      FSLLOG(Indent << "CL_DEVICE_SPIR_VERSIONS: Unavailable");
#endif
    }


    // CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS (cl_bool)
    inline void Log_CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS
      FSLLOG(Indent << "CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_bool>(deviceId, CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS));
#else
      FSLLOG(Indent << "CL_DEVICE_SUBGROUP_INDEPENDENT_FORWARD_PROGRESS: Unavailable");
#endif
    }


    // CL_DEVICE_SVM_CAPABILITIES (cl_device_svm_capabilities)
    inline void Log_CL_DEVICE_SVM_CAPABILITIES(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_SVM_CAPABILITIES
      FSLLOG(Indent << "CL_DEVICE_SVM_CAPABILITIES: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_svm_capabilities>(deviceId, CL_DEVICE_SVM_CAPABILITIES));
#else
      FSLLOG(Indent << "CL_DEVICE_SVM_CAPABILITIES: Unavailable");
#endif
    }


    // CL_DEVICE_TERMINATE_CAPABILITY_KHR (cl_device_terminate_capability_khr)
    inline void Log_CL_DEVICE_TERMINATE_CAPABILITY_KHR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_TERMINATE_CAPABILITY_KHR
      FSLLOG(Indent << "CL_DEVICE_TERMINATE_CAPABILITY_KHR: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_terminate_capability_khr>(deviceId, CL_DEVICE_TERMINATE_CAPABILITY_KHR));
#else
      FSLLOG(Indent << "CL_DEVICE_TERMINATE_CAPABILITY_KHR: Unavailable");
#endif
    }


    // CL_DEVICE_TYPE (deviceType)
    inline void Log_CL_DEVICE_TYPE(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_TYPE
      const auto deviceType = OpenCL::OpenCLHelper::GetDeviceInfo<cl_device_type>(deviceId, CL_DEVICE_TYPE);
      FSLLOG(Indent << "CL_DEVICE_TYPE: " << OpenCL::Debug::DeviceTypeTostring(deviceType));
#else
      FSLLOG(Indent << "CL_DEVICE_TYPE: Unavailable");
#endif
    }


    //  CL_DEVICE_VENDOR (char[])
    inline void Log_CL_DEVICE_VENDOR(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VENDOR
      FSLLOG(Indent << "CL_DEVICE_VENDOR: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_VENDOR));
#else
      FSLLOG(Indent << "CL_DEVICE_VENDOR: Unavailable");
#endif
    }


    //  CL_DEVICE_VENDOR_ID (cl_uint)
    inline void Log_CL_DEVICE_VENDOR_ID(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VENDOR_ID
      FSLLOG(Indent << "CL_DEVICE_VENDOR_ID: " << OpenCL::OpenCLHelper::GetDeviceInfo<cl_uint>(deviceId, CL_DEVICE_VENDOR_ID));
#else
      FSLLOG(Indent << "CL_DEVICE_VENDOR_ID: Unavailable");
#endif
    }


    // CL_DEVICE_VERSION (char[])
    inline void Log_CL_DEVICE_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DEVICE_VERSION
      FSLLOG(Indent << "CL_DEVICE_VERSION: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DEVICE_VERSION));
#else
      FSLLOG(Indent << "CL_DEVICE_VERSION: Unavailable");
#endif
    }


    //  CL_DRIVER_VERSION (char[])
    inline void Log_CL_DRIVER_VERSION(const cl_device_id deviceId)
    {
#ifdef CL_DRIVER_VERSION
      FSLLOG(Indent << "CL_DRIVER_VERSION: " << OpenCL::OpenCLHelper::GetDeviceInfo<std::string>(deviceId, CL_DRIVER_VERSION));
#else
      FSLLOG(Indent << "CL_DRIVER_VERSION: Unavailable");
#endif
    }
  };
}

#endif
