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

#include <FslBase/Debug/Transition/DebugTransitionType.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/RenderTest/CustomControls/EasingFunctionGraph.hpp>
#include <Shared/UI/RenderTest/CustomControls/PrimitiveTopologyTest.hpp>
#include <Shared/UI/RenderTest/RenderTestShared.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
    }

    constexpr StringViewLite TransitionTypeEnumName("TransitionType::");

    constexpr std::array<TransitionType, 31> EasingFunctions = {
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

    std::shared_ptr<UI::BaseWindow> CreateCaptionedBackground(UI::Theme::IThemeControlFactory& uiFactory,
                                                              const std::shared_ptr<UI::BaseWindow>& control, const StringViewLite caption)
    {
      auto lblCaption = uiFactory.CreateLabel(caption, UI::Theme::FontType::Header);
      lblCaption->SetAlignmentX(UI::ItemAlignment::Center);
      lblCaption->SetAlignmentY(UI::ItemAlignment::Center);

      auto layout = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
      layout->SetAlignmentX(UI::ItemAlignment::Stretch);
      layout->SetAlignmentY(UI::ItemAlignment::Stretch);
      layout->SetSpacing(DpSize1DF::Create(8));
      layout->SetOrientation(UI::LayoutOrientation::Vertical);
      layout->AddChild(lblCaption, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(control, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

      auto window = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::DialogNormal, layout);
      window->SetAlignmentX(UI::ItemAlignment::Stretch);
      window->SetAlignmentY(UI::ItemAlignment::Stretch);
      return window;
    }


    std::shared_ptr<UI::BaseWindow> CreatePrimitiveTopologyLineListTest(UI::Theme::IThemeControlFactory& uiFactory)
    {
      auto control = std::make_shared<UI::Custom::PrimitiveTopologyTest>(uiFactory.GetContext());
      control->SetAlignmentX(UI::ItemAlignment::Stretch);
      control->SetAlignmentY(UI::ItemAlignment::Stretch);
      control->SetMinWidth(DpSize1DF::Create(64));
      control->SetMinHeight(DpSize1DF::Create(64));
      control->SetContent(uiFactory.GetResources().GetLineListSprite(true));
      return CreateCaptionedBackground(uiFactory, control, "LineList");
    }


    std::shared_ptr<UI::BaseWindow> CreatePrimitiveTopologyTriangleList(UI::Theme::IThemeControlFactory& uiFactory)
    {
      auto control = std::make_shared<UI::Custom::PrimitiveTopologyTest>(uiFactory.GetContext());
      control->SetAlignmentX(UI::ItemAlignment::Stretch);
      control->SetAlignmentY(UI::ItemAlignment::Stretch);
      control->SetMinWidth(DpSize1DF::Create(64));
      control->SetMinHeight(DpSize1DF::Create(64));
      control->SetContent(uiFactory.GetResources().GetBasicFillSprite(true));
      return CreateCaptionedBackground(uiFactory, control, "TriangleList");
    }


    std::shared_ptr<UI::BaseWindow> CreateEasingFunctionGraph(UI::Theme::IThemeControlFactory& uiFactory, const TransitionType transitionType)
    {
      auto control = std::make_shared<UI::Custom::EasingFunctionGraph>(uiFactory.GetContext());
      control->SetAlignmentX(UI::ItemAlignment::Stretch);
      control->SetAlignmentY(UI::ItemAlignment::Stretch);
      control->SetMinWidth(DpSize1DF::Create(256));
      control->SetMinHeight(DpSize1DF::Create(128));
      control->SetTransitionType(transitionType);
      control->SetContent(uiFactory.GetResources().GetLineListSprite(true));
      return CreateCaptionedBackground(uiFactory, control, SimplifyEnumName(Debug::ToString(transitionType)));
    }
  }


  RenderTestShared::RenderTestShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
  {
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

    m_ui = CreateUI(*uiControlFactory);

    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(m_ui.MainLayout);
  }


  RenderTestShared::~RenderTestShared() = default;


  void RenderTestShared::OnKeyEvent(const KeyEvent& /*event*/)
  {
  }


  void RenderTestShared::OnConfigurationChanged(const DemoWindowMetrics& /*windowMetrics*/)
  {
  }


  void RenderTestShared::Update(const DemoTime& /*demoTime*/)
  {
  }


  void RenderTestShared::Draw()
  {
    m_uiExtension->Draw();
  }


  void RenderTestShared::SetDefaultValues()
  {
  }


  RenderTestShared::SimpleUI RenderTestShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& windowContext = uiFactory.GetContext();

    auto testPrimitiveLineList = CreatePrimitiveTopologyLineListTest(uiFactory);
    auto testPrimitiveTriangleList = CreatePrimitiveTopologyTriangleList(uiFactory);


    // Create the root layout and add it to the window manager
    auto mainLayout = std::make_shared<UI::UniformWrapLayout>(windowContext);
    mainLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddChild(testPrimitiveLineList);
    mainLayout->AddChild(testPrimitiveTriangleList);

    for (auto easingFunction : EasingFunctions)
    {
      auto testEasingFunction = CreateEasingFunctionGraph(uiFactory, easingFunction);
      mainLayout->AddChild(testEasingFunction);
    }

    return {std::move(mainLayout)};
  }
}
