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

#include "GpuTimestamp.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <array>

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

    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one ubo and one image sampler
      std::array<VkDescriptorPoolSize, 2> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSizes[0].descriptorCount = count;
      poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[1].descriptorCount = count * 2;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return RapidVulkan::DescriptorPool(device.Get(), descriptorPoolInfo);
    }

    RapidVulkan::QueryPool CreateQueryPool(const Vulkan::VUDevice& device)
    {
      // Create query pool
      VkQueryPoolCreateInfo queryPoolInfo{};
      queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
      queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
      queryPoolInfo.queryCount = LocalConfig::QueryCount;

      return RapidVulkan::QueryPool(device.Get(), queryPoolInfo);
    }

    bool IsTimestampSupported(const Vulkan::VUDevice& device)
    {
      const auto& physicalDeviceRecord = device.GetPhysicalDevice();

      FSLLOG3_INFO("Checking timestamp requirements");
      bool isSupported = true;
      if (physicalDeviceRecord.Properties.limits.timestampPeriod <= 0.0f)
      {
        FSLLOG3_INFO("- VkPhysicalDeviceLimits 'timestampPeriod' not supported");
        isSupported = false;
      }

      FSLLOG3_INFO("Timestamp requirements supported: {}", isSupported);
      return isSupported;
    }
  }


  GpuTimestamp::GpuTimestamp(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_isTimestampSupported(IsTimestampSupported(m_device))
    , m_shared(config, m_isTimestampSupported)
    , m_resources(CreateResources(m_device, m_deviceQueue, GetRenderConfig()))
    , m_scene(config, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetDemoAppExtension());

    if (m_isTimestampSupported)
    {
      m_resources.QueryPool = CreateQueryPool(m_device);
    }
  }


  void GpuTimestamp::EndDraw(const FrameInfo& frameInfo)
  {
    VulkanBasic::DemoAppVulkanBasic::EndDraw(frameInfo);
    if (m_dependentResources.QueueContainsTimestamp)
    {
      m_dependentResources.HasPendingQuery = true;
    }
  }


  void GpuTimestamp::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);
    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void GpuTimestamp::OnKeyEvent(const KeyEvent& event)
  {
    m_shared.OnKeyEvent(event);
  }


  void GpuTimestamp::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime);

    m_scene.SetMaxIterations(m_shared.GetIterations());
    m_scene.Update(demoTime, m_shared.IsPaused());

    if (m_resources.QueryPool.IsValid() && m_dependentResources.HasPendingQuery)
    {
      RAPIDVULKAN_CHECK(vkDeviceWaitIdle(m_device.Get()));
      std::array<uint64_t, LocalConfig::QueryCount> resultBuffer{};
      RAPIDVULKAN_CHECK(vkGetQueryPoolResults(m_device.Get(), m_resources.QueryPool.Get(), 0, LocalConfig::QueryCount,
                                              sizeof(uint64_t) * resultBuffer.size(), resultBuffer.data(), sizeof(uint64_t),
                                              VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));

      // timestampPeriod is the number of nanoseconds required for a timestamp query to be incremented by 1.
      const double timestampPeriod = m_device.GetPhysicalDevice().Properties.limits.timestampPeriod;
      const auto time = static_cast<uint64_t>(std::round((static_cast<double>(resultBuffer[1] - resultBuffer[0]) * timestampPeriod) / 1000.0));
      m_shared.SetProfileTime(time);
    }
  }


  void GpuTimestamp::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    FSL_PARAM_NOT_USED(demoTime);

    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      if (m_resources.QueryPool.IsValid())
      {
        assert(m_isTimestampSupported);
        vkCmdResetQueryPool(hCmdBuffer, m_resources.QueryPool.Get(), 0, LocalConfig::QueryCount);
      }

      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = UncheckedNumericCast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentFrameIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        if (m_resources.QueryPool.IsValid())
        {
          assert(m_isTimestampSupported);
          vkCmdWriteTimestamp(hCmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_resources.QueryPool.Get(), LocalConfig::Query0);
          m_dependentResources.QueueContainsTimestamp = true;
        }
        m_scene.Draw(drawContext.CurrentFrameIndex, hCmdBuffer);
        if (m_resources.QueryPool.IsValid())
        {
          assert(m_isTimestampSupported);
          vkCmdWriteTimestamp(hCmdBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_resources.QueryPool.Get(), LocalConfig::Query1);
        }

        m_shared.Draw(demoTime);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass GpuTimestamp::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    m_scene.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    return m_dependentResources.MainRenderPass.Get();
  }


  void GpuTimestamp::OnFreeResources()
  {
    m_scene.OnFreeResources();
    m_dependentResources = {};
  }

  GpuTimestamp::Resources GpuTimestamp::CreateResources(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                                        const RenderConfig& renderConfig)
  {
    auto bufferManager =
      std::make_shared<Vulkan::VMBufferManager>(device.GetPhysicalDevice(), device.Get(), deviceQueue.Queue, deviceQueue.QueueFamilyIndex);

    auto descriptorPool = CreateDescriptorPool(device, renderConfig.MaxFramesInFlight);
    return {bufferManager, std::move(descriptorPool), RapidVulkan::QueryPool()};
  }
}
