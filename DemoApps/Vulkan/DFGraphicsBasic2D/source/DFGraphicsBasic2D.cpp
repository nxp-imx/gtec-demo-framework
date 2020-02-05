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

#include "DFGraphicsBasic2D.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  DFGraphicsBasic2D::DFGraphicsBasic2D(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_shared(config)
  {
  }


  void DFGraphicsBasic2D::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime, GetScreenResolution());
  }


  void DFGraphicsBasic2D::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      VkClearColorValue clearColorValue{};
      clearColorValue.float32[0] = 0.0f;
      clearColorValue.float32[1] = 0.0f;
      clearColorValue.float32[2] = 0.0f;
      clearColorValue.float32[3] = 1.0f;

      VkClearValue clearValues[1] = {clearColorValue};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = m_dependentResources.MainRenderPass.Get();
      renderPassBeginInfo.framebuffer = drawContext.Framebuffer;
      renderPassBeginInfo.renderArea.offset.x = 0;
      renderPassBeginInfo.renderArea.offset.y = 0;
      renderPassBeginInfo.renderArea.extent = drawContext.SwapchainImageExtent;
      renderPassBeginInfo.clearValueCount = 1;
      renderPassBeginInfo.pClearValues = clearValues;

      rCmdBuffers.CmdBeginRenderPass(currentSwapBufferIndex, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      {
        m_shared.Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass DFGraphicsBasic2D::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void DFGraphicsBasic2D::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
