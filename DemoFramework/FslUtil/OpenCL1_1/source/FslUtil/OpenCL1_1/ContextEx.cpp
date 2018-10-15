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

#include <FslUtil/OpenCL1_1/ContextEx.hpp>
#include <FslUtil/OpenCL1_1/OpenCLHelper.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Exceptions.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace OpenCL
  {
    // move assignment operator
    ContextEx& ContextEx::operator=(ContextEx&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_platformId = other.m_platformId;
        m_context = std::move(other.m_context);

        // Remove the data from other
        other.m_platformId = nullptr;
      }
      return *this;
    }


    // Transfer ownership from other to this
    ContextEx::ContextEx(ContextEx&& other) noexcept
      : m_platformId(other.m_platformId)
      , m_context(std::move(other.m_context))
    {
      other.m_platformId = nullptr;
    }


    ContextEx::ContextEx()
      : m_platformId(nullptr)
    {
    }


    ContextEx::ContextEx(const cl_platform_id platformId, const cl_context context)
      : ContextEx()
    {
      Reset(platformId, context);
    }


    ContextEx::ContextEx(const cl_device_type deviceType, cl_device_id* pDeviceId, const bool allowFallback)
      : ContextEx()
    {
      Reset(deviceType, pDeviceId, allowFallback);
    }


    ContextEx::~ContextEx()
    {
      Reset();
    }


    void ContextEx::Reset(const cl_device_type deviceType, cl_device_id* pDeviceId, const bool allowFallback)
    {
      if (IsValid())
      {
        Reset();
      }

      const auto platformIds = OpenCLHelper::GetPlatformIDs();
      if (platformIds.empty())
      {
        throw NotSupportedException("No OpenCL platform is available");
      }

      for (auto itr = platformIds.begin(); itr != platformIds.end(); ++itr)
      {
        const auto deviceIds = OpenCLHelper::GetDeviceIDs(*itr, deviceType);
        if (!deviceIds.empty())
        {
          SelectDevice(*itr, deviceIds, pDeviceId);
          return;
        }
      }

      if (allowFallback && deviceType != CL_DEVICE_TYPE_ALL)
      {
        FSLLOG("Trying to locate a device using CL_DEVICE_TYPE_ALL");
        for (auto itr = platformIds.begin(); itr != platformIds.end(); ++itr)
        {
          const auto deviceIds = OpenCLHelper::GetDeviceIDs(*itr, CL_DEVICE_TYPE_ALL);
          if (!deviceIds.empty())
          {
            SelectDevice(*itr, deviceIds, pDeviceId);
            return;
          }
        }
      }

      throw NotSupportedException("No device found");
    }

    void ContextEx::SelectDevice(cl_platform_id platformId, const std::vector<cl_device_id>& deviceIds, cl_device_id* pDeviceId)
    {
      // FIX: for now just select the first device
      const cl_uint targetDevice = 0;
      // FIX: for now just use one device
      const cl_uint uiNumDevsUsed = 1;

      // clCreateContext
      m_context.Reset(nullptr, uiNumDevsUsed, &deviceIds[targetDevice], nullptr, nullptr);
      m_platformId = platformId;
      if (pDeviceId != nullptr)
      {
        *pDeviceId = deviceIds[targetDevice];
      }
    }
  }
}
