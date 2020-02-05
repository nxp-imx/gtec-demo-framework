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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslUtil/OpenCL1_1/OpenCLHelper.hpp>
#include <RapidOpenCL1/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <cassert>

namespace Fsl
{
  namespace OpenCL
  {
    namespace
    {
      // PLATFORM_VERSION: OpenCL<space><major_version.minor_version><space><platform - specific information>
      // DEVICE_VERSION:   OpenCL<space><major_version.minor_version><space><vendor - specific information>
      // OpenCL major.minor vendor stuff
      bool TryParseVersionString(const std::string& strVersion, VersionInfo& rVersion)
      {
        rVersion = VersionInfo();

        // Would have been nice if sscanf had defined behavior for handling too big numbers :(
        // if (sscanf(strVersion.c_str(), "OpenCL %u.%u ", &major, &minor) < 2)
        //  return false;

        if (!StringUtil::StartsWith(strVersion, "OpenCL "))
        {
          return false;
        }

        int32_t currentIndex = 7;
        // +2 because there needs to be at least a dot and a number
        if (strVersion.size() <= (static_cast<std::size_t>(currentIndex) + 2u))
        {
          return false;
        }

        int32_t dotIndex = StringUtil::IndexOf(strVersion, '.', currentIndex);
        if (dotIndex < 0)
        {
          return false;
        }

        int32_t valueEndIndex = StringUtil::IndexOf(strVersion, ' ', currentIndex);
        if (valueEndIndex < 0)
        {
          return false;
        }

        uint32_t major = 0;
        uint32_t minor = 0;
        try
        {
          assert(currentIndex <= dotIndex);
          auto expectedLength = static_cast<std::size_t>(dotIndex - currentIndex);
          auto consumed = StringParseUtil::Parse(major, strVersion.c_str(), currentIndex, expectedLength);
          if (consumed != expectedLength)
          {
            return false;
          }

          currentIndex = dotIndex + 1;
          expectedLength = valueEndIndex - currentIndex;
          consumed = StringParseUtil::Parse(minor, strVersion.c_str(), currentIndex, expectedLength);
          if (consumed != expectedLength)
          {
            return false;
          }
        }
        catch (const std::exception&)
        {
          return false;
        }

        if (major >= 0xFFFF || minor >= 0xFFFF)
        {
          return false;
        }

        rVersion = VersionInfo(static_cast<uint16_t>(major), static_cast<uint16_t>(minor));
        return true;
      }
    }

    namespace OpenCLHelper
    {
      std::vector<cl_platform_id> GetPlatformIDs()
      {
        cl_uint count;
        RAPIDOPENCL_CHECK(clGetPlatformIDs(0, nullptr, &count));

        std::vector<cl_platform_id> result(count);
        RAPIDOPENCL_CHECK(clGetPlatformIDs(static_cast<cl_uint>(result.size()), result.data(), nullptr));
        return result;
      }


      std::vector<cl_device_id> GetDeviceIDs(const cl_platform_id platformId, const cl_device_type deviceType)
      {
        cl_uint count;
        auto errorCode = clGetDeviceIDs(platformId, deviceType, 0, nullptr, &count);
        if (errorCode == CL_DEVICE_NOT_FOUND)
        {
          return std::vector<cl_device_id>();
        }
        RAPIDOPENCL_CHECK2(errorCode, "clGetDeviceIDs");

        std::vector<cl_device_id> result(count);
        RAPIDOPENCL_CHECK(clGetDeviceIDs(platformId, deviceType, static_cast<cl_uint>(result.size()), result.data(), nullptr));
        return result;
      }


      bool TryGetDeviceIDs(std::vector<cl_device_id>& rDeviceIds, const cl_platform_id platformId, const cl_device_type deviceType)
      {
        cl_uint count;
        if (clGetDeviceIDs(platformId, deviceType, 0, nullptr, &count) != CL_SUCCESS)
        {
          rDeviceIds.resize(0);
          return false;
        }

        rDeviceIds.resize(count);
        if (clGetDeviceIDs(platformId, deviceType, static_cast<cl_uint>(rDeviceIds.size()), rDeviceIds.data(), nullptr) != CL_SUCCESS)
        {
          rDeviceIds.resize(0);
          return false;
        }
        return true;
      }


      bool TryGetPlatformVersion(const cl_platform_id platformId, VersionInfo& rVersionInfo)
      {
        rVersionInfo = VersionInfo();

        std::string strVersion;
        if (!TryGetPlatformInfo(platformId, CL_PLATFORM_VERSION, strVersion))
        {
          return false;
        }

        return TryParseVersionString(strVersion, rVersionInfo);
      }


      VersionInfo GetPlatformVersion(const cl_platform_id platformId)
      {
        const auto strVersion = GetPlatformInfo<std::string>(platformId, CL_PLATFORM_VERSION);

        VersionInfo version;
        if (TryParseVersionString(strVersion, version))
        {
          return version;
        }

        throw std::runtime_error("Failed to decode CL_DEVICE_VERSION string");
      }


      bool TryGetDeviceVersion(const cl_device_id deviceId, VersionInfo& rVersionInfo)
      {
        rVersionInfo = VersionInfo();

        std::string strVersion;
        if (!TryGetDeviceInfo(deviceId, CL_DEVICE_VERSION, strVersion))
        {
          return false;
        }

        return TryParseVersionString(strVersion, rVersionInfo);
      }


      VersionInfo GetDeviceVersion(const cl_device_id deviceId)
      {
        const auto strVersion = GetDeviceInfo<std::string>(deviceId, CL_DEVICE_VERSION);

        VersionInfo version;
        if (TryParseVersionString(strVersion, version))
        {
          return version;
        }

        throw std::runtime_error("Failed to decode CL_DEVICE_VERSION string");
      }


      std::string GetProgramBuildInfoString(const cl_program program, const cl_device_id deviceId, const cl_program_build_info paramName)
      {
        assert(paramName == CL_PROGRAM_BUILD_OPTIONS || paramName == CL_PROGRAM_BUILD_LOG);

        std::size_t contentSize;
        RAPIDOPENCL_CHECK(clGetProgramBuildInfo(program, deviceId, paramName, 0, nullptr, &contentSize));

        std::vector<char> content(contentSize);
        RAPIDOPENCL_CHECK(clGetProgramBuildInfo(program, deviceId, paramName, content.size(), content.data(), nullptr));
        return std::string(content.data());
      }


      bool TryGetProgramBuildInfo(const cl_program program, const cl_device_id deviceId, const cl_program_build_info paramName, std::string& rResult)
      {
        assert(paramName == CL_PROGRAM_BUILD_OPTIONS || paramName == CL_PROGRAM_BUILD_LOG);

        std::size_t contentSize;
        if (clGetProgramBuildInfo(program, deviceId, paramName, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rResult = std::string();
          return false;
        }

        std::vector<char> content(contentSize);
        if (clGetProgramBuildInfo(program, deviceId, paramName, content.size(), content.data(), nullptr) != CL_SUCCESS)
        {
          rResult = std::string();
          return false;
        }
        rResult = std::string(content.data());
        return true;
      }


      bool TryGetProgramBuildInfo(const cl_program program, const cl_device_id deviceId, const cl_program_build_info paramName,
                                  cl_build_status& rResult)
      {
        assert(paramName == CL_PROGRAM_BUILD_OPTIONS || paramName == CL_PROGRAM_BUILD_LOG);

        std::size_t contentSize;
        if (clGetProgramBuildInfo(program, deviceId, paramName, 0, nullptr, &contentSize) != CL_SUCCESS)
        {
          rResult = CL_BUILD_NONE;
          return false;
        }

        cl_build_status status;
        if (clGetProgramBuildInfo(program, deviceId, paramName, sizeof(cl_build_status), &status, nullptr) != CL_SUCCESS)
        {
          rResult = CL_BUILD_NONE;
          return false;
        }
        rResult = status;
        return true;
      }
    }
  }
}
