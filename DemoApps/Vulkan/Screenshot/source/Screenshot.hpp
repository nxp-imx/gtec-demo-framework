#ifndef VULKAN_SCREENSHOT_SCREENSHOT_HPP
#define VULKAN_SCREENSHOT_SCREENSHOT_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeBatch2D.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>

namespace Fsl
{
  class Screenshot
    : public VulkanBasic::DemoAppVulkanBasic
    , public UI::EventListener
  {
    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        MainRenderPass.Reset();
      }
    };

    std::shared_ptr<IGraphicsService> m_graphicsService;
    // A batcher that can be used API independently or with the native API
    std::shared_ptr<Vulkan::NativeBatch2D> m_nativeBatch;

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    Texture2D m_texture;
    DependentResources m_dependentResources;

    std::shared_ptr<UI::BackgroundLabelButton> m_btnScreenshot;
    std::shared_ptr<UI::Label> m_label;

    bool m_screenshotRequested{false};

  public:
    explicit Screenshot(const DemoAppConfig& config);
    void OnSelect(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowSelectEvent>& theEvent) override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) override;
    AppDrawResult TrySwapBuffers(const FrameInfo& frameInfo) override;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) override;
    void OnFreeResources() override;

  private:
    Bitmap TryCaptureScreenshot();
    Bitmap TryCaptureScreenshotViaBlit(const VulkanBasic::SwapchainInfo& swapchainInfo, const VkFormat dstImageFormat);
    Bitmap TryCaptureScreenshotViaCopy(const VulkanBasic::SwapchainInfo& swapchainInfo);
  };
}

#endif
