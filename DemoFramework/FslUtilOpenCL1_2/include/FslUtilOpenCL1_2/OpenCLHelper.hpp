#ifndef FSLUTILOPENCL1_2_OPENCLHELPER_HPP
#define FSLUTILOPENCL1_2_OPENCLHELPER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtilOpenCL1_2/Common.hpp>
#include <FslUtilOpenCL1_2/Check.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/VersionInfo.hpp>
#include <CL/cl.h>
#include <string>
#include <type_traits>
#include <vector>

namespace Fsl
{
  namespace OpenCL
  {
    namespace OpenCLHelper
    {
      //! @brief Enumerate the platform id's
      extern std::vector<cl_platform_id> GetPlatformIDs();

      //! @brief Enumerate the device id's
      extern std::vector<cl_device_id> GetDeviceIDs(const cl_platform_id platformId, const cl_device_type deviceType);

      //! @brief Try to enumerate the device id's
      //! @param rDeviceId's a array that will be filled with the device id's (if this returns false the rDeviceIds will be resized to zero)
      //! @return true if the device id's were enumerated, false otherwise
      extern bool TryGetDeviceIDs(std::vector<cl_device_id>& rDeviceIds, const cl_platform_id platformId, const cl_device_type deviceType);


      //! @brief Get and decode the version string.
      extern bool TryGetPlatformVersion(const cl_platform_id platformId, VersionInfo& rVersionInfo);

      //! @brief Get and decode the version string.
      extern VersionInfo GetPlatformVersion(const cl_platform_id platformId);

      //! @brief Get and decode the version string.
      extern bool TryGetDeviceVersion(const cl_device_id deviceId, VersionInfo& rVersionInfo);

      //! @brief Get and decode the version string.
      extern VersionInfo GetDeviceVersion(const cl_device_id deviceId);

      //! @brief Template version of GetPlatformInfo, its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline T GetPlatformInfo(const cl_platform_id platformId, const cl_platform_info platformInfo)
      {
        // If you want to fill a std::vector<POD type> then use the TryGetPlatformInfo
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        T result;
        FSLUTILOPENCL_CHECK(clGetPlatformInfo(platformId, platformInfo, sizeof(T), &result, nullptr));
        return result;
      }


      //! @brief Template version of GetPlatformInfo, its your responsibility to set the correct type for the platformInfo
      template<>
      inline std::string GetPlatformInfo<std::string>(const cl_platform_id platformId, const cl_platform_info platformInfo)
      {
        std::size_t contentSize;
        FSLUTILOPENCL_CHECK(clGetPlatformInfo(platformId, platformInfo, 0, nullptr, &contentSize));
        std::vector<char> content(contentSize);
        FSLUTILOPENCL_CHECK(clGetPlatformInfo(platformId, platformInfo, content.size(), content.data(), nullptr));
        return std::string(content.data());
      }


      //! @brief Template version of TryGetPlatformInfo, its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline bool TryGetPlatformInfo(const cl_platform_id platformId, const cl_platform_info platformInfo, T& rValue)
      {
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        if (clGetPlatformInfo(platformId, platformInfo, sizeof(T), &rValue, nullptr) == CL_SUCCESS)
          return true;

        rValue = T();
        return true;
      }


      //! @brief Template of TryGetPlatformInfo for std::vector<T> its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline bool TryGetPlatformInfo(const cl_platform_id platformId, const cl_platform_info platformInfo, std::vector<T>& rContainer)
      {
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        std::size_t contentSize;
        if (clGetPlatformInfo(platformId, platformInfo, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rContainer.resize(0);
          return false;
        }

        rContainer.resize(contentSize);
        if (clGetPlatformInfo(platformId, platformInfo, rContainer.size(), rContainer.data(), nullptr) == CL_SUCCESS)
          return true;
        else
        {
          rContainer.resize(0);
          return false;
        }
      }


      //! @brief Template of TryGetPlatformInfo for std::string its your responsibility to request the correct deviceInfo
      template<>
      inline bool TryGetPlatformInfo<std::string>(const cl_platform_id platformId, const cl_platform_info platformInfo, std::string& rValue)
      {
        std::size_t contentSize;
        if (clGetPlatformInfo(platformId, platformInfo, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rValue = std::string();
          return false;
        }

        std::vector<char> content(contentSize);
        if (clGetPlatformInfo(platformId, platformInfo, content.size(), content.data(), nullptr) == CL_SUCCESS)
        {
          rValue = std::string(content.data());
          return true;
        }
        else
        {
          rValue = std::string();
          return false;
        }
      }



      //! @brief Template version of GetDeviceInfo, its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline T GetDeviceInfo(const cl_device_id deviceId, const cl_device_info deviceInfo)
      {
        // If you want to fill a std::vector<POD type> then use the TryGetDeviceInfo
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        T result;
        FSLUTILOPENCL_CHECK(clGetDeviceInfo(deviceId, deviceInfo, sizeof(T), &result, nullptr));
        return result;
      }


      //! @brief Template version of GetDeviceInfo, its your responsibility to set the correct type for the platformInfo
      template<>
      inline std::string GetDeviceInfo<std::string>(const cl_device_id deviceId, const cl_device_info deviceInfo)
      {
        std::size_t contentSize;
        FSLUTILOPENCL_CHECK(clGetDeviceInfo(deviceId, deviceInfo, 0, nullptr, &contentSize));
        std::vector<char> content(contentSize);
        FSLUTILOPENCL_CHECK(clGetDeviceInfo(deviceId, deviceInfo, content.size(), content.data(), nullptr));
        return std::string(content.data());
      }


      //! @brief Template version of TryGetDeviceInfo, its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline bool TryGetDeviceInfo(const cl_device_id deviceId, const cl_device_info deviceInfo, T& rValue)
      {
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        if (clGetDeviceInfo(deviceId, deviceInfo, sizeof(T), &rValue, nullptr) == CL_SUCCESS)
          return true;

        rValue = T();
        return true;
      }


      //! @brief Template of TryGetDeviceInfo for std::vector<T> its your responsibility to set the correct type for the platformInfo
      template<typename T>
      inline bool TryGetDeviceInfo(const cl_device_id deviceId, const cl_device_info deviceInfo, std::vector<T>& rContainer)
      {
        static_assert(std::is_pod<T>::value, "We only support writing to Plain Old Data types");

        std::size_t contentSize;
        if (clGetDeviceInfo(deviceId, deviceInfo, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rContainer.resize(0);
          return false;
        }

        rContainer.resize(contentSize);
        if (clGetDeviceInfo(deviceId, deviceInfo, rContainer.size(), rContainer.data(), nullptr) == CL_SUCCESS)
          return true;
        else
        {
          rContainer.resize(0);
          return false;
        }
      }


      //! @brief Template of TryGetDeviceInfo for std::string its your responsibility to request the correct deviceInfo
      template<>
      inline bool TryGetDeviceInfo<std::string>(const cl_device_id deviceId, const cl_device_info deviceInfo, std::string& rValue)
      {
        std::size_t contentSize;
        if (clGetDeviceInfo(deviceId, deviceInfo, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rValue = std::string();
          return false;
        }

        std::vector<char> content(contentSize);
        if (clGetDeviceInfo(deviceId, deviceInfo, content.size(), content.data(), nullptr) == CL_SUCCESS)
        {
          rValue = std::string(content.data());
          return true;
        }
        else
        {
          rValue = std::string();
          return false;
        }
      }

    };

  }
}

#endif
