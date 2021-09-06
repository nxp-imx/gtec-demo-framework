/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include "BasicGpuProfiler.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint32_t Query0 = 0;
      constexpr const uint32_t Query1 = 1;
      constexpr const uint32_t QueryCount = 2;
    }

    RapidVulkan::QueryPool CreateQueryPool(const VkDevice device)
    {
      // Create query pool
      VkQueryPoolCreateInfo queryPoolInfo{};
      queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
      queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
      queryPoolInfo.queryCount = LocalConfig::QueryCount;

      return RapidVulkan::QueryPool(device, queryPoolInfo);
    }
  }


  BasicGpuProfiler::BasicGpuProfiler(const VkPhysicalDeviceProperties& physicalDeviceProperties, const VkDevice device)
    : m_device(device)
    , m_queryPool(CreateQueryPool(m_device))
    , m_timestampPeriod(physicalDeviceProperties.limits.timestampPeriod)
  {
    if (!IsTimestampSupported(physicalDeviceProperties))
    {
      throw UsageErrorException("the physical device does not support timestamps");
    }
    if (m_device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("device can not be VK_NULL_HANDLE");
    }
  }


  bool BasicGpuProfiler::IsEnabled() const noexcept
  {
    return m_resources.IsEnabled;
  }


  void BasicGpuProfiler::SetEnabled(const bool enabled)
  {
    if (m_resources.IsEnabled != enabled)
    {
      if (enabled)
      {
        m_resources = Resources(true);
      }
      else
      {
        m_resources = {};
      }
    }
  }


  void BasicGpuProfiler::BeginTimestamp()
  {
    if (m_resources.Draw.CommandBuffer != VK_NULL_HANDLE && m_resources.QueueTimestampState == TimestampState::NotSet)
    {
      assert(m_queryPool.IsValid());
      vkCmdWriteTimestamp(m_resources.Draw.CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_queryPool.Get(), LocalConfig::Query0);
      m_resources.QueueTimestampState = TimestampState::BeginSet;
    }
    else
    {
      FSLLOG3_DEBUG_WARNING_IF(m_resources.Draw.CommandBuffer, "BeginTimestamp: BeginFrame was not called, ignoring request");
      FSLLOG3_DEBUG_WARNING_IF(m_resources.QueueTimestampState != TimestampState::NotSet, "BeginTimestamp: incorrect state, ignoring request");
    }
  }

  void BasicGpuProfiler::EndTimestamp()
  {
    if (m_resources.Draw.CommandBuffer != VK_NULL_HANDLE && m_resources.QueueTimestampState == TimestampState::BeginSet)
    {
      assert(m_queryPool.IsValid());
      vkCmdWriteTimestamp(m_resources.Draw.CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPool.Get(), LocalConfig::Query1);
      m_resources.QueueTimestampState = TimestampState::BothSet;
    }
    else
    {
      FSLLOG3_DEBUG_WARNING_IF(m_resources.Draw.CommandBuffer, "BeginTimestamp: BeginFrame was not called, ignoring request");
      FSLLOG3_DEBUG_WARNING_IF(m_resources.QueueTimestampState != TimestampState::BeginSet, "BeginTimestamp: incorrect state, ignoring request");
    }
  }

  uint64_t BasicGpuProfiler::GetResult() const noexcept
  {
    return m_resources.LastResult;
  }


  void BasicGpuProfiler::BeginDraw(const VkCommandBuffer commandBuffer)
  {
    if (commandBuffer == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("command buffer can not be null");
    }
    if (m_resources.IsEnabled)
    {
      assert(m_queryPool.IsValid());
      m_resources.Draw = DrawResources(commandBuffer);
      vkCmdResetQueryPool(m_resources.Draw.CommandBuffer, m_queryPool.Get(), 0, LocalConfig::QueryCount);
    }
  }

  void BasicGpuProfiler::EndDraw()
  {
    m_resources.Draw = {};
  }


  void BasicGpuProfiler::ExtractResult()
  {
    m_resources.LastResult = 0;
    if (m_resources.IsEnabled)
    {
      switch (m_resources.QueueTimestampState)
      {
      case TimestampState::NotSet:
        break;
      case TimestampState::BeginSet:
        FSLLOG3_WARNING("Begin was set, but end was not");
        break;
      case TimestampState::BothSet:
      {
        // To make this simple we just do a wait for idle
        RAPIDVULKAN_CHECK(vkDeviceWaitIdle(m_device));
        std::array<uint64_t, LocalConfig::QueryCount> resultBuffer{};
        RAPIDVULKAN_CHECK(vkGetQueryPoolResults(m_device, m_queryPool.Get(), 0, LocalConfig::QueryCount, sizeof(uint64_t) * resultBuffer.size(),
                                                resultBuffer.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
        // timestampPeriod is the number of nanoseconds required for a timestamp query to be incremented by 1.
        m_resources.LastResult =
          static_cast<uint64_t>(std::round((static_cast<double>(resultBuffer[1] - resultBuffer[0]) * m_timestampPeriod) / 1000.0));
        break;
      }
      default:
        FSLLOG3_WARNING("Unsupported state");
        break;
      }
      m_resources.QueueTimestampState = TimestampState::NotSet;
    }
  }


  bool BasicGpuProfiler::IsTimestampSupported(const VkPhysicalDeviceProperties& physicalDeviceProperties)
  {
    bool isSupported = true;
    if (physicalDeviceProperties.limits.timestampPeriod <= 0.0f)
    {
      isSupported = false;
    }
    return isSupported;
  }
}
