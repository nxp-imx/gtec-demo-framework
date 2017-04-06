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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslUtilOpenCL1_1/Check.hpp>
#include <FslUtilOpenCL1_1/Context.hpp>
#include <FslUtilOpenCL1_1/OpenCLHelper.hpp>
#include "Info.hpp"
#include "LogHelp.hpp"
#include "LogDeviceInfo1_0.hpp"
#include "LogDeviceInfo1_1.hpp"
#include "LogDeviceInfo1_2.hpp"
#include "LogDeviceInfo2_0.hpp"
#include "LogDeviceInfo2_1.hpp"
#include "LogPlatformInfo1_0.hpp"
#include "LogPlatformInfo1_1.hpp"
#include "LogPlatformInfo1_2.hpp"
#include "LogPlatformInfo2_0.hpp"
#include "LogPlatformInfo2_1.hpp"
#include <array>
#include <CL/cl.h>

namespace Fsl
{
  using namespace OpenCL;

  namespace
  {
    void DumpPlatformInfo(LogHelp& rLog, cl_platform_id platformId)
    {
      const VersionInfo version = OpenCLHelper::GetPlatformVersion(platformId);

      bool useFallback = false;
      if (version.Major == 1)
      {
        switch (version.Minor)
        {
        case 0:
          LogPlatformInfo1_0(rLog, platformId);
          break;
        case 1:
          LogPlatformInfo1_1(rLog, platformId);
          break;
        case 2:
          LogPlatformInfo1_2(rLog, platformId);
          break;
        default:
          useFallback = true;
        }
      }
      else if (version.Major == 2)
      {
        switch (version.Minor)
        {
        case 0:
          LogPlatformInfo2_0(rLog, platformId);
          break;
        case 1:
          LogPlatformInfo2_1(rLog, platformId);
          break;
        default:
          useFallback = true;
        }
      }
      if (useFallback)
      {
        FSLLOG_WARNING("CL_PLATFORM_VERSION " << version.Major << "." << version.Minor << " is not supported.");
        return;
      }
    }


    // From stack overflow:
    //   There are some features in OpenCL that have hardware requirements.This means that even if a
    //   particular vendor's OpenCL implementation (the platform) supports an OpenCL version, the
    //   device might not. So, it is entirely possible for the versions returned from the
    //   CL_DEVICE_VERSION and CL_PLATFORM_VERSION queries to differ.
    //   This will probably start to happen more frequently when OpenCL 2.0 implementations start
    //   appearing, as there is plenty of hardware on the market that doesn't have the necessary
    //   support for OpenCL 2.0 features. Imagine a system that has two devices from Vendor X:
    //   a new Device A that can run OpenCL 2.0, and a much older Device B that can't.
    //   In this instance, the platform version may be OpenCL 2.0, but the device version could
    //   be OpenCL 2.0 for Device A and OpenCL 1.2 for Device B.
    //   The CL_DRIVER_VERSION is for getting a vendor specific version number for the implementation.
    //   This number could using any version numbering system that the vendor uses to keep track of
    //   different software releases, and is completely independent from OpenCL version numbers
    //   (although some vendors may well include the OpenCL version here too).
    //   So, in order to be sure that both the device and platform support your required
    //   OpenCL version, you should just need to check CL_DEVICE_VERSION.
    void LogDeviceInfo(LogHelp& rLog, const cl_device_id deviceId)
    {
      const VersionInfo version = OpenCLHelper::GetDeviceVersion(deviceId);

      bool useFallback = false;
      if (version.Major == 1)
      {
        switch (version.Minor)
        {
        case 0:
          LogDeviceInfo1_0(rLog, deviceId);
          break;
        case 1:
          LogDeviceInfo1_1(rLog, deviceId);
          break;
        case 2:
          LogDeviceInfo1_2(rLog, deviceId);
          break;
        default:
          useFallback = true;
        }
      }
      else if (version.Major == 2)
      {
        switch (version.Minor)
        {
        case 0:
          LogDeviceInfo2_0(rLog, deviceId);
          break;
        case 1:
          LogDeviceInfo2_1(rLog, deviceId);
          break;
        default:
          useFallback = true;
        }
      }
      if (useFallback)
      {
        FSLLOG_WARNING("CL_DEVICE_VERSION " << version.Major << "." << version.Minor << " is not supported.");
        return;
      }
    }


    void LogPlatformInfo(LogHelp& rLog, const std::vector<cl_platform_id>& platformIds)
    {
      FSLLOG(rLog.Indent << "Dumping platform info for " << platformIds.size() << " platforms.");
      for (std::size_t platformIndex = 0; platformIndex < platformIds.size(); ++platformIndex)
      {
        rLog.PushIndent();
        FSLLOG(rLog.Indent << "*** Platform #" << platformIndex << " ***");
        DumpPlatformInfo(rLog, platformIds[platformIndex]);
        FSLLOG("");
        rLog.PopIndent();
      }
    }

    void LogDetailedInformation(LogHelp& rLog, const std::vector<cl_platform_id>& platformIds)
    {
      FSLLOG(rLog.Indent << "Dumping detailed device info for " << platformIds.size() << " platforms.");
      rLog.PushIndent();

      cl_device_type deviceTypes[] =
      {
        CL_DEVICE_TYPE_CPU,
        CL_DEVICE_TYPE_GPU,
        CL_DEVICE_TYPE_ACCELERATOR,
#ifdef CL_DEVICE_TYPE_CUSTOM
        CL_DEVICE_TYPE_CUSTOM,
#endif
        CL_DEVICE_TYPE_ALL
      };
      const uint32_t deviceTypeEntries = (sizeof(deviceTypes) / sizeof(cl_device_type));

      for (std::size_t platformIndex = 0; platformIndex < platformIds.size(); ++platformIndex)
      {
        FSLLOG(rLog.Indent << "*** Platform #" << platformIndex << " ***");
        DumpPlatformInfo(rLog, platformIds[platformIndex]);

        for (uint32_t deviceTypeIndex = 0; deviceTypeIndex<deviceTypeEntries; ++deviceTypeIndex)
        {
          FSLLOG(rLog.Indent << "Enumerating devices of type: " << Debug::DeviceTypeTostring(deviceTypes[deviceTypeIndex]));
          rLog.PushIndent();

          std::vector<cl_device_id> deviceIds;
          if (OpenCLHelper::TryGetDeviceIDs(deviceIds, platformIds[platformIndex], deviceTypes[deviceTypeIndex]))
          {
            for (std::size_t deviceIndex = 0; deviceIndex < deviceIds.size(); ++deviceIndex)
            {

              FSLLOG(rLog.Indent << "--- Device #" << deviceIndex << " ---");
              LogDeviceInfo(rLog, deviceIds[deviceIndex]);
            }
          }
          else
          {
            FSLLOG(rLog.Indent << "- Not supported");
          }
          rLog.PopIndent();
        }
      }
      rLog.PopIndent();
    }

  }

  //clGetDeviceInfo(cl_device_id    /* device */,
  //  cl_device_info  /* param_name */,
  //  size_t          /* param_value_size */,
  //  void *          /* param_value */,
  //  size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;



  Info::Info(const DemoAppConfig& config)
    : DemoAppOpenCL(config)
  {
  }


  Info::~Info()
  {
  }


  void Info::Run()
  {
    LogHelp log;

    const auto platformIds = OpenCLHelper::GetPlatformIDs();
    LogPlatformInfo(log, platformIds);
    FSLLOG("");

    LogDetailedInformation(log, platformIds);


  }
}
