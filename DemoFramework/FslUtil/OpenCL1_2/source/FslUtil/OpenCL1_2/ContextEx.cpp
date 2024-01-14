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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslUtil/OpenCL1_2/ContextEx.hpp>
#include <FslUtil/OpenCL1_2/OpenCLHelper.hpp>
#include <RapidOpenCL1/Check.hpp>
#include <RapidOpenCL1/Exceptions.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl::OpenCL
{
  namespace
  {
    bool IsValidContextProperties(const ReadOnlySpan<cl_context_properties> contextPropertiesSpan)
    {
      return contextPropertiesSpan.empty() || (contextPropertiesSpan.size() > 1u && (contextPropertiesSpan.size() & 1u) == 1u &&
                                               contextPropertiesSpan[contextPropertiesSpan.size() - 1u] == 0);
    }

    int32_t IndexOfProperty(const ReadOnlySpan<cl_context_properties> contextPropertiesSpan, const cl_context_properties propertyToFind)
    {
      if (contextPropertiesSpan.empty())
      {
        return -1;
      }

      for (std::size_t i = 0; i < contextPropertiesSpan.size(); i += 2)
      {
        if (contextPropertiesSpan[i] == propertyToFind)
        {
          return NumericCast<int32_t>(i);
        }
      }
      return -1;
    }
  }

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


  // NOLINTNEXTLINE(misc-misplaced-const)
  ContextEx::ContextEx(const cl_platform_id platformId, const cl_context context)
    : ContextEx()
  {
    Reset(platformId, context);
  }


  // NOLINTNEXTLINE(misc-misplaced-const)
  ContextEx::ContextEx(const cl_device_type deviceType, cl_device_id* pDeviceId, const bool allowFallback)
    : ContextEx()
  {
    Reset(deviceType, pDeviceId, allowFallback);
  }


  // NOLINTNEXTLINE(misc-misplaced-const)
  ContextEx::ContextEx(const ReadOnlySpan<cl_context_properties> contextPropertiesSpan, const cl_device_type deviceType, cl_device_id* pDeviceId,
                       const bool allowFallback)
    : ContextEx()
  {
    Reset(contextPropertiesSpan, deviceType, pDeviceId, allowFallback);
  }


  ContextEx::~ContextEx()
  {
    Reset();
  }

  void ContextEx::Reset(const cl_device_type deviceType, cl_device_id* pDeviceId, const bool allowFallback)
  {
    Reset({}, deviceType, pDeviceId, allowFallback);
  }

  // NOLINTNEXTLINE(misc-misplaced-const)
  void ContextEx::Reset(const ReadOnlySpan<cl_context_properties> contextPropertiesSpan, const cl_device_type deviceType, cl_device_id* pDeviceId,
                        const bool allowFallback)
  {
    if (!IsValidContextProperties(contextPropertiesSpan))
    {
      throw std::invalid_argument("invalid contextPropertiesSpan");
    }
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
        SelectDevice(contextPropertiesSpan, *itr, deviceIds, pDeviceId);
        return;
      }
    }

    if (allowFallback && deviceType != CL_DEVICE_TYPE_ALL)
    {
      FSLLOG3_INFO("Trying to locate a device using CL_DEVICE_TYPE_ALL");
      for (auto itr = platformIds.begin(); itr != platformIds.end(); ++itr)
      {
        const auto deviceIds = OpenCLHelper::GetDeviceIDs(*itr, CL_DEVICE_TYPE_ALL);
        if (!deviceIds.empty())
        {
          SelectDevice(contextPropertiesSpan, *itr, deviceIds, pDeviceId);
          return;
        }
      }
    }

    throw NotSupportedException("No device found");
  }

  // NOLINTNEXTLINE(misc-misplaced-const)
  void ContextEx::SelectDevice(const ReadOnlySpan<cl_context_properties> contextPropertiesSpan, cl_platform_id platformId,
                               const std::vector<cl_device_id>& deviceIds, cl_device_id* pDeviceId)
  {
    // FIX: for now just select the first device
    const cl_uint targetDevice = 0;
    // FIX: for now just use one device
    const cl_uint uiNumDevsUsed = 1;

    const cl_device_id selectedDeviceId = deviceIds[targetDevice];

    // clCreateContext
    if (contextPropertiesSpan.empty())
    {
      m_context.Reset(nullptr, uiNumDevsUsed, &selectedDeviceId, nullptr, nullptr);
    }
    else
    {
      std::vector<cl_context_properties> patchesProperties = PatchProperties(platformId, contextPropertiesSpan);
      assert(!patchesProperties.empty() && IsValidContextProperties(ReadOnlySpanUtil::AsSpan(patchesProperties)));
      m_context.Reset(patchesProperties.data(), uiNumDevsUsed, &selectedDeviceId, nullptr, nullptr);
    }

    m_platformId = platformId;
    if (pDeviceId != nullptr)
    {
      *pDeviceId = selectedDeviceId;
    }
  }

  std::vector<cl_context_properties> ContextEx::PatchProperties(cl_platform_id platformId,
                                                                const ReadOnlySpan<cl_context_properties> contextPropertiesSpan)
  {
    assert(!contextPropertiesSpan.empty());
    assert(IsValidContextProperties(contextPropertiesSpan));

    std::vector<cl_context_properties> result(ReadOnlySpanUtil::ToVector(contextPropertiesSpan));
    int32_t indexOfProperty = IndexOfProperty(contextPropertiesSpan, CL_CONTEXT_PLATFORM);
    if (indexOfProperty >= 0)
    {
      assert(UncheckedNumericCast<std::size_t>(indexOfProperty) < result.size());
      assert(result[indexOfProperty] == CL_CONTEXT_PLATFORM);
      assert(UncheckedNumericCast<std::size_t>(indexOfProperty + 1) < result.size());
      if (result[indexOfProperty + 1] == 0)
      {
        FSLLOG3_VERBOSE3("The property CL_CONTEXT_PLATFORM was set to zero, patching it")
        result[indexOfProperty + 1] = reinterpret_cast<cl_context_properties>(platformId);
      }
    }
    else
    {
      FSLLOG3_VERBOSE3("The property CL_CONTEXT_PLATFORM was not found, inserting it");
      result.push_back(CL_CONTEXT_PLATFORM);
      result.push_back(reinterpret_cast<cl_context_properties>(platformId));
    }
    assert(IsValidContextProperties(ReadOnlySpanUtil::AsSpan(result)));
    return result;
  }
}
