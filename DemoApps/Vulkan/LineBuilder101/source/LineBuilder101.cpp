/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "LineBuilder101.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MatrixUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    constexpr uint32_t INITIAL_LINE_CAPACITY = 4096u;
  }


  LineBuilder101::LineBuilder101(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_example(config.DemoServiceProvider)
  {
    const auto contentManager = GetContentManager();
    m_resources.VertShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("VertexColorLine.vert.spv"));
    m_resources.FragShaderModule.Reset(m_device.Get(), 0, contentManager->ReadBytes("VertexColorLine.frag.spv"));

    m_resources.BufferManager =
      std::make_shared<VMBufferManager>(m_physicalDevice, m_device.Get(), m_deviceQueue.Queue, m_deviceQueue.QueueFamilyIndex);

    const uint32_t maxFramesInFlight = GetRenderConfig().MaxFramesInFlight;
    m_resources.LineDraw.Reset(m_device, m_resources.BufferManager, maxFramesInFlight, sizeof(VertexUBOData), INITIAL_LINE_CAPACITY);
  }


  void LineBuilder101::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_example.OnMouseButtonEvent(event);
  }


  void LineBuilder101::Update(const DemoTime& demoTime)
  {
    m_example.Update(demoTime, GetScreenResolution());

    // Deal with the new Vulkan coordinate system (see method description for more info).
    // Consider using: https://github.com/KhronosGroup/Vulkan-Docs/blob/master/appendices/VK_KHR_maintenance1.txt
    const auto vulkanClipMatrix = MatrixUtil::GetClipMatrix();

    m_vertexUboData.MatViewProjection = m_example.GetViewMatrix() * m_example.GetProjectionMatrix() * vulkanClipMatrix;
  }


  void LineBuilder101::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t frameIndex = drawContext.CurrentFrameIndex;
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;
    // Upload the changes
    m_resources.LineDraw.UpdateVertexUBO(&m_vertexUboData, sizeof(VertexUBOData), frameIndex);


    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {0.4f, 0.4f, 0.4f, 1.0f};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassBeginInfo.pClearValues = clearValues.data();

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        m_resources.LineDraw.Draw(hCmdBuffer, m_example.GetLineBuilder().GetVertexSpan(), frameIndex);

        // Allow the example to draw a bit too
        m_example.Draw(demoTime);

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass LineBuilder101::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();

    m_resources.LineDraw.BuildResources(context.SwapchainImageExtent, m_resources.VertShaderModule.Get(), m_resources.FragShaderModule.Get(),
                                        m_dependentResources.MainRenderPass.Get(), 0u);

    return m_dependentResources.MainRenderPass.Get();
  }


  void LineBuilder101::OnFreeResources()
  {
    m_resources.LineDraw.FreeResources();
    m_dependentResources = {};
  }
}
