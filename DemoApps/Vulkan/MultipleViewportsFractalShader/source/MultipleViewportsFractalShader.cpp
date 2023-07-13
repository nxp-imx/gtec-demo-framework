/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include "MultipleViewportsFractalShader.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/FractalShader/OptionParser.hpp>
#include <vulkan/vulkan.h>
#include "FractalShaderJulia.hpp"
#include "FractalShaderMandelbrot.hpp"

namespace Fsl
{
  namespace
  {
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

      return {device.Get(), descriptorPoolInfo};
    }
  }


  MultipleViewportsFractalShader::MultipleViewportsFractalShader(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_resources(
        std::make_shared<Vulkan::VMBufferManager>(m_device.GetPhysicalDevice(), m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex),
        CreateDescriptorPool(m_device, GetRenderConfig().MaxFramesInFlight * 4))
    , m_scene0(config, 0, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight)
    , m_scene1(config, 1, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight)
    , m_scene2(config, 0, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight)
    , m_scene3(config, 1, m_device, m_deviceQueue, m_resources.BufferManager, m_resources.DescriptorPool, GetRenderConfig().MaxFramesInFlight)
  {
    // std::shared_ptr<BasicOptionParser> options = config.GetOptions<BasicOptionParser>();
    // Config cfg = options->GetConfig();
  }


  void MultipleViewportsFractalShader::Update(const DemoTime& demoTime)
  {
    m_scene0.Update(demoTime);
    m_scene1.Update(demoTime);
    m_scene2.Update(demoTime);
    m_scene3.Update(demoTime);
  }


  void MultipleViewportsFractalShader::VulkanDraw(const DemoTime& /*demoTime*/, RapidVulkan::CommandBuffers& rCmdBuffers,
                                                  const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentFrameIndex = drawContext.CurrentFrameIndex;

    const VkCommandBuffer hCmdBuffer = rCmdBuffers[currentFrameIndex];
    rCmdBuffers.Begin(currentFrameIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
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
        const auto sizePx = GetWindowSizePx();

        const int32_t x0 = 0;
        const int32_t x1 = sizePx.RawWidth() / 2;
        const int32_t x2 = sizePx.RawWidth();
        const int32_t y0 = 0;
        const int32_t y1 = sizePx.RawHeight() / 2;
        const int32_t y2 = sizePx.RawHeight();

        m_scene0.Draw(currentFrameIndex, hCmdBuffer, Rectangle2D(x0, y0, x1 - x0, y2 - y1));
        m_scene1.Draw(currentFrameIndex, hCmdBuffer, Rectangle2D(x0, y1, x1 - x0, y1 - y0));
        m_scene2.Draw(currentFrameIndex, hCmdBuffer, Rectangle2D(x1, y0, x2 - x1, y2 - y1));
        m_scene3.Draw(currentFrameIndex, hCmdBuffer, Rectangle2D(x1, y1, x2 - x1, y1 - y0));

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass MultipleViewportsFractalShader::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    m_scene0.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    m_scene1.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    m_scene2.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    m_scene3.OnBuildResources(context, m_dependentResources.MainRenderPass.Get());
    return m_dependentResources.MainRenderPass.Get();
  }


  void MultipleViewportsFractalShader::OnFreeResources()
  {
    m_scene3.OnFreeResources();
    m_scene2.OnFreeResources();
    m_scene1.OnFreeResources();
    m_scene0.OnFreeResources();
    m_dependentResources = {};
  }
}
