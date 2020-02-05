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

#include "DFSimpleUI100.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  using namespace UI;

  DFSimpleUI100::DFSimpleUI100(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Fetch some textures from the default texture atlas loaded by the UI extension (this is the 'MainAtlas')
    AtlasTexture2D texBack = m_uiExtension->GetAtlasTexture2D("Player/Back");
    AtlasTexture2D texNext = m_uiExtension->GetAtlasTexture2D("Player/Next");
    AtlasTexture2D texPlay = m_uiExtension->GetAtlasTexture2D("Player/Play");
    AtlasTexture2D texStop = m_uiExtension->GetAtlasTexture2D("Player/Stop");

    // Allocate the four player buttons
    m_btnBack = std::make_shared<ImageButton>(context);
    m_btnNext = std::make_shared<ImageButton>(context);
    m_btnPlay = std::make_shared<ImageButton>(context);
    m_btnStop = std::make_shared<ImageButton>(context);

    // Set the texture for each button
    m_btnBack->SetContent(texBack);
    m_btnNext->SetContent(texNext);
    m_btnPlay->SetContent(texPlay);
    m_btnStop->SetContent(texStop);

    // Create a horizontal stack layout and add the four buttons (in the desired order)
    auto playerStack = std::make_shared<StackLayout>(context);
    playerStack->SetLayoutOrientation(LayoutOrientation::Horizontal);
    playerStack->SetAlignmentX(ItemAlignment::Center);
    playerStack->SetAlignmentY(ItemAlignment::Far);
    playerStack->AddChild(m_btnBack);
    playerStack->AddChild(m_btnPlay);
    playerStack->AddChild(m_btnStop);
    playerStack->AddChild(m_btnNext);

    // Create a label to write stuff into when a button is pressed
    m_label = std::make_shared<Label>(context);
    m_label->SetAlignmentX(ItemAlignment::Center);
    m_label->SetAlignmentY(ItemAlignment::Center);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(context);
    fillLayout->AddChild(playerStack);
    fillLayout->AddChild(m_label);

    // Finally add everything to the window manager (to ensure its seen)
    auto windowManager = m_uiExtension->GetWindowManager();
    windowManager->Add(fillLayout);
  }


  void DFSimpleUI100::Update(const DemoTime& demoTime)
  {
  }

  void DFSimpleUI100::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnBack)
    {
      m_label->SetContent("Back pressed");
    }
    else if (theEvent->GetSource() == m_btnNext)
    {
      m_label->SetContent("Next pressed");
    }
    else if (theEvent->GetSource() == m_btnPlay)
    {
      m_label->SetContent("Play pressed");
    }
    else if (theEvent->GetSource() == m_btnStop)
    {
      m_label->SetContent("Stop pressed");
    }
  }

  void DFSimpleUI100::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
  {
    const uint32_t currentSwapBufferIndex = drawContext.CurrentSwapBufferIndex;

    auto hCmdBuffer = rCmdBuffers[currentSwapBufferIndex];
    rCmdBuffers.Begin(currentSwapBufferIndex, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
    {
      std::array<VkClearValue, 1> clearValues{};
      clearValues[0].color = {0.5f, 0.5f, 0.5f, 1.0f};

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
        // Calling this last allows the UI to draw on top of everything.
        // Beware that the UI drawing methods might alter the OpenGL state!
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentSwapBufferIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentSwapBufferIndex);
    }
    rCmdBuffers.End(currentSwapBufferIndex);
  }


  VkRenderPass DFSimpleUI100::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    // Since we only draw using the NativeBatch we just create the most basic render pass that is compatible
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void DFSimpleUI100::OnFreeResources()
  {
    m_dependentResources = {};
  }
}
