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

#include "EasingFunctions.hpp"
#include <FslBase/Debug/Transition/DebugTransitionType.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/TypeConverter.hpp>
#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <string>

namespace Fsl
{
  namespace
  {
    constexpr StringViewLite TransitionTypeEnumName("TransitionType::");

    constexpr std::array<TransitionType, 31> GEasingFunctions = {
      TransitionType::EaseInSine,    TransitionType::EaseOutSine,    TransitionType::EaseInOutSine,
      TransitionType::EaseInQuad,    TransitionType::EaseOutQuad,    TransitionType::EaseInOutQuad,
      TransitionType::EaseInCubic,   TransitionType::EaseOutCubic,   TransitionType::EaseInOutCubic,
      TransitionType::EaseInQuart,   TransitionType::EaseOutQuart,   TransitionType::EaseInOutQuart,
      TransitionType::EaseInQuint,   TransitionType::EaseOutQuint,   TransitionType::EaseInOutQuint,
      TransitionType::EaseInExpo,    TransitionType::EaseOutExpo,    TransitionType::EaseInOutExpo,
      TransitionType::EaseInCirc,    TransitionType::EaseOutCirc,    TransitionType::EaseInOutCirc,
      TransitionType::EaseInBack,    TransitionType::EaseOutBack,    TransitionType::EaseInOutBack,
      TransitionType::EaseInElastic, TransitionType::EaseOutElastic, TransitionType::EaseInOutElastic,
      TransitionType::EaseInBounce,  TransitionType::EaseOutBounce,  TransitionType::EaseInOutBounce,
      TransitionType::Linear};


    StringViewLite SimplifyEnumName(const StringViewLite strView)
    {
      if (strView.starts_with(TransitionTypeEnumName))
      {
        return strView.substr(TransitionTypeEnumName.size());
      }
      return strView;
    }

  }

  EasingFunctions::EasingFunctions(const DemoAppConfig& config)
    : VulkanBasic::DemoAppVulkanBasic(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
    , m_renderSystem(config.DemoServiceProvider.Get<IGraphicsService>()->GetBasicRenderSystem())
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);


    auto context = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

    m_ui = CreateUI(uiControlFactory);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(m_ui.Main);

    m_texFill = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_ui.FillSprite);
  }


  void EasingFunctions::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    VulkanBasic::DemoAppVulkanBasic::ConfigurationChanged(windowMetrics);

    m_texFill = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_ui.FillSprite);
  }


  void EasingFunctions::OnKeyEvent(const KeyEvent& event)
  {
    VulkanBasic::DemoAppVulkanBasic::OnKeyEvent(event);

    if (!event.IsHandled() && event.IsPressed())
    {
      switch (event.GetKey())
      {
      case VirtualKey::LeftArrow:
        m_ui.Slider->SubValue(1);
        break;
      case VirtualKey::RightArrow:
        m_ui.Slider->AddValue(1);
        break;
      case VirtualKey::DownArrow:
        m_ui.Slider->SubValue(5);
        break;
      case VirtualKey::UpArrow:
        m_ui.Slider->AddValue(5);
        break;
      default:
        break;
      }
    }
  }


  void EasingFunctions::Update(const DemoTime& demoTime)
  {
  }


  void EasingFunctions::VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext)
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
        DrawNow();

        // Calling this last allows the UI to draw on top of everything.
        // Beware that the UI drawing methods might alter the OpenGL state!
        m_uiExtension->Draw();

        // Remember to call this as the last operation in your renderPass
        AddSystemUI(hCmdBuffer, currentFrameIndex);
      }
      rCmdBuffers.CmdEndRenderPass(currentFrameIndex);
    }
    rCmdBuffers.End(currentFrameIndex);
  }


  VkRenderPass EasingFunctions::OnBuildResources(const VulkanBasic::BuildResourcesContext& context)
  {
    m_dependentResources.MainRenderPass = CreateBasicRenderPass();
    return m_dependentResources.MainRenderPass.Get();
  }


  void EasingFunctions::OnFreeResources()
  {
    m_dependentResources.Reset();
  }


  void EasingFunctions::DrawNow()
  {
    PxRectangle renderRectanglePx = m_ui.RenderArea->WinGetContentRectanglePx();


    const auto transitionType = GEasingFunctions[m_ui.Slider->GetValue()];
    auto fnEase = EasingFunctionUtil::GetEasingFunction(transitionType);

    m_nativeBatch->Begin();

    // m_nativeBatch->DebugDrawRectangle(m_texFill, renderRectanglePx, Colors::Cyan());

    const auto pixelCount = (renderRectanglePx.RawRight() - renderRectanglePx.RawLeft());
    if (pixelCount > 0)
    {
      constexpr auto Color = Colors::White();
      constexpr auto ColorLine = Colors::Black();
      constexpr float PercentHeight = 0.55f;
      constexpr PxRectangle SrcRectanglePx = PxRectangle::Create(0, 0, 1, 1);

      const int areaWidthPx = renderRectanglePx.RawWidth();
      const int areaHeightPx = renderRectanglePx.RawHeight();
      const int heightPx = TypeConverter::ChangeTo<int32_t>(static_cast<float>(areaHeightPx) * PercentHeight);
      const int yOffsetPx = renderRectanglePx.RawTop() + ((areaHeightPx - heightPx) / 2);

      // Draw base lines
      m_nativeBatch->Draw(m_texFill, PxAreaRectangleF::Create(0.0f, static_cast<float>(yOffsetPx), static_cast<float>(areaWidthPx), 1.0f),
                          SrcRectanglePx, ColorLine);
      m_nativeBatch->Draw(m_texFill, PxAreaRectangleF::Create(0.0f, static_cast<float>(yOffsetPx + heightPx), static_cast<float>(areaWidthPx), 1.0f),
                          SrcRectanglePx, ColorLine);

      const float progressAdd = 1.0f / static_cast<float>(pixelCount);
      float progress = 0.0f;
      for (auto x = renderRectanglePx.RawLeft(); x < renderRectanglePx.RawRight(); ++x)
      {
        int32_t y = yOffsetPx + heightPx - TypeConverter::ChangeTo<int32_t>(fnEase(progress) * static_cast<float>(heightPx));
        m_nativeBatch->Draw(m_texFill, Vector2(x, y), SrcRectanglePx, Color);
        progress += progressAdd;
      }
    }


    m_nativeBatch->End();
  }


  EasingFunctions::UIRecord EasingFunctions::CreateUI(const std::shared_ptr<UI::Theme::IThemeControlFactory>& factory)
  {
    const auto context = factory->GetContext();

    auto renderArea = std::make_shared<UI::BaseWindow>(context);
    renderArea->SetAlignmentX(UI::ItemAlignment::Stretch);
    renderArea->SetAlignmentY(UI::ItemAlignment::Stretch);

    static_assert(!GEasingFunctions.empty());

    auto sliderValueLabel = factory->CreateLabel("?");
    sliderValueLabel->SetAlignmentX(UI::ItemAlignment::Center);

    const ConstrainedValue<uint32_t> sliderConstraints(0, 0, UncheckedNumericCast<uint32_t>(GEasingFunctions.size() - 1));
    auto slider = factory->CreateSlider(UI::LayoutOrientation::Horizontal, sliderConstraints);
    slider->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto barLayout = std::make_shared<UI::StackLayout>(context);
    barLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    barLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    barLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    barLayout->AddChild(sliderValueLabel);
    barLayout->AddChild(slider);

    auto bottomBar = factory->CreateBottomBar(barLayout, UI::Theme::BarType::Normal);


    auto mainLayout = std::make_shared<UI::ComplexStackLayout>(context);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddChild(renderArea, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    mainLayout->AddChild(bottomBar, UI::LayoutLength(UI::LayoutUnitType::Auto));

    auto fillSprite = factory->GetResources().GetFillSprite();


    {
      auto converterBinding = std::make_shared<Fsl::DataBinding::ConverterBinding<StringViewLite, uint32_t>>(
        [](const uint32_t value) { return SimplifyEnumName(Debug::ToString(GEasingFunctions[value])); });

      const auto hSrcSlider = slider->GetPropertyHandle(UI::Slider<uint32_t>::PropertyValue);
      sliderValueLabel->SetBinding(UI::Label::PropertyContent, DataBinding::Binding(converterBinding, hSrcSlider));
    }

    return {mainLayout, renderArea, slider, fillSprite};
  }
}
