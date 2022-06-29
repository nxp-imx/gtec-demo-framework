#ifndef VULKAN_UI_BENCHMARK_BASICGPUPROFILER_HPP
#define VULKAN_UI_BENCHMARK_BASICGPUPROFILER_HPP
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

#include <RapidVulkan/QueryPool.hpp>
#include <Shared/UI/Benchmark/IBasicGpuProfiler.hpp>

namespace Fsl
{
  class BasicGpuProfiler final : public IBasicGpuProfiler
  {
    enum class TimestampState
    {
      NotSet,
      BeginSet,
      BothSet
    };

    struct DrawResources
    {
      VkCommandBuffer CommandBuffer{VK_NULL_HANDLE};

      DrawResources() = default;

      explicit DrawResources(const VkCommandBuffer commandBuffer)
        : CommandBuffer(commandBuffer)
      {
      }
    };


    struct Resources
    {
      bool IsEnabled{false};
      TimestampState QueueTimestampState{TimestampState::NotSet};
      uint64_t LastResult{0};
      DrawResources Draw;

      Resources() = default;
      explicit Resources(const bool enabled)
        : IsEnabled(enabled)
      {
      }
    };

    VkDevice m_device{VK_NULL_HANDLE};
    RapidVulkan::QueryPool m_queryPool;
    float m_timestampPeriod{0};
    Resources m_resources;

  public:
    BasicGpuProfiler(const VkPhysicalDeviceProperties& physicalDeviceProperties, const VkDevice device);

    bool IsEnabled() const noexcept final;
    void SetEnabled(const bool enabled) final;
    void BeginTimestamp() final;
    void EndTimestamp() final;
    uint64_t GetResult() const noexcept final;

    void BeginDraw(const VkCommandBuffer commandBuffer);
    void EndDraw();
    void ExtractResult();

    static bool IsTimestampSupported(const VkPhysicalDeviceProperties& physicalDeviceProperties);

  private:
  };
}

#endif
