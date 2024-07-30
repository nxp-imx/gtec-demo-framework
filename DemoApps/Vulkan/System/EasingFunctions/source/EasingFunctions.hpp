#ifndef VULKAN_SYSTEM_EASINGFUNCTIONS_EASINGFUNCTIONS_HPP
#define VULKAN_SYSTEM_EASINGFUNCTIONS_EASINGFUNCTIONS_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>

namespace Fsl
{
  namespace UI::Theme
  {
    class IThemeControlFactory;
  }

  class EasingFunctions final
    : public VulkanBasic::DemoAppVulkanBasic
    , public UI::EventListener
  {
    // struct Resources
    //{
    //   Resources() noexcept = default;
    //   Resources(const Resources&) = delete;
    //   Resources& operator=(const Resources&) = delete;
    //   Resources(Resources&& other) noexcept = delete;
    //   Resources& operator=(Resources&& other) noexcept = delete;
    // };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;

      DependentResources() noexcept = default;
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

    struct UIRecord
    {
      std::shared_ptr<UI::BaseWindow> Main;
      std::shared_ptr<UI::BaseWindow> RenderArea;
      std::shared_ptr<UI::Slider<uint32_t>> Slider;
      std::shared_ptr<ImageSprite> FillSprite;

      UIRecord() = default;
      UIRecord(std::shared_ptr<UI::BaseWindow> main, std::shared_ptr<UI::BaseWindow> renderArea, std::shared_ptr<UI::Slider<uint32_t>> slider,
               std::shared_ptr<ImageSprite> fillSprite)
        : Main(std::move(main))
        , RenderArea(std::move(renderArea))
        , Slider(std::move(slider))
        , FillSprite(std::move(fillSprite))
      {
      }
    };

    // The UI event listener is responsible for forwarding events to this classes implementation of the UI::EventListener (while its still alive).
    UI::CallbackEventListenerScope m_uiEventListener;
    // The UIDemoAppExtension is a simple extension that sets up the basic UI framework and listens for the events it needs.
    std::shared_ptr<UIDemoAppExtension> m_uiExtension;

    UIRecord m_ui;
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;
    std::shared_ptr<INativeBatch2D> m_nativeBatch;

    AtlasTexture2D m_texFill;

    // Resources m_resources;
    DependentResources m_dependentResources;

  public:
    explicit EasingFunctions(const DemoAppConfig& config);

  protected:
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;

    void OnKeyEvent(const KeyEvent& event) final;

    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

  private:
    void DrawNow();

    static UIRecord CreateUI(const std::shared_ptr<UI::Theme::IThemeControlFactory>& factory);
  };
}

#endif
