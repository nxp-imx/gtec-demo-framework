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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangle.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/ScrollViewer.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/AntiAliasing/AntiAliasingShared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
    }

    void AppendStringPart(std::string& rStr, const std::string_view str)
    {
      if (!rStr.empty())
      {
        rStr += "|";
      }
      rStr += str;
    }

    void LogConfigInfo(const AntiAliasingConfig& aaConfig)
    {
      std::string strSampleFlags;
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples1))
      {
        AppendStringPart(strSampleFlags, "1");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples2))
      {
        AppendStringPart(strSampleFlags, "2");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples4))
      {
        AppendStringPart(strSampleFlags, "4");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples8))
      {
        AppendStringPart(strSampleFlags, "8");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples16))
      {
        AppendStringPart(strSampleFlags, "16");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples32))
      {
        AppendStringPart(strSampleFlags, "32");
      }
      if (SupportedSampleCountFlagsUtil::IsEnabled(aaConfig.SupportedMSAASampleFlags, SupportedSampleCountFlags::Samples64))
      {
        AppendStringPart(strSampleFlags, "64");
      }

      FSLLOG3_INFO("Supported samples: {}", strSampleFlags);
    }

    PxRectangle CalcWindowScreenPosition(UI::BaseWindow& window)
    {
      const PxPoint2 offsetPx = window.PointToScreen(PxPoint2());
      const PxSize2D sizePx = window.RenderSizePx();
      return {offsetPx, sizePx};
    }
  }

  AntiAliasingShared::AntiAliasingShared(const DemoAppConfig& config, const AntiAliasingConfig& aaConfig, const CustomUIConfig& customUIConfig)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_defaultOptions(config.GetOptions<OptionParser>())
  {
    LogConfigInfo(aaConfig);

    for (auto& rEntry : m_windowRectangle)
    {
      rEntry = TransitionPxRectangle(TimeSpan::FromMilliseconds(500), TransitionType::EaseInOutSine);
    }

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

    m_ui = CreateUI(*uiControlFactory, aaConfig.SupportedMSAASampleFlags, customUIConfig);
    m_uiExtension->SetMainWindow(m_ui.MainLayout);
    SetToDefaults();

    OnConfigurationChanged(config.WindowMetrics);
  }


  AntiAliasingShared::~AntiAliasingShared() = default;


  PxRectangle AntiAliasingShared::GetContentWindowRectangleInScreenspace(const ContentWindowId contentWindowId) const
  {
    switch (contentWindowId)
    {
    case ContentWindowId::Car:
    default:
      return m_windowRectangle[0].GetValue();
    case ContentWindowId::Knight:
      return m_windowRectangle[1].GetValue();
    case ContentWindowId::WireframeCube:
      return m_windowRectangle[2].GetValue();
    }
  }


  UI::Custom::ZoomInfo AntiAliasingShared::GetResolvedZoomInfo() const
  {
    return m_ui.Content.ZoomArea->GetResolvedZoomInfo();
  }


  void AntiAliasingShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics != m_cachedWindowMetrics)
    {
      m_cachedWindowMetrics = windowMetrics;
      m_forceCompleteAnimation = true;
    }
  }


  void AntiAliasingShared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    const auto& source = theEvent->GetSource();
    {
      auto itrFind = std::find_if(m_ui.AntiAliasingSolutions.begin(), m_ui.AntiAliasingSolutions.end(),
                                  [source](const UIAntiAliasingRecord& entry) { return entry.Button == source; });
      if (itrFind != m_ui.AntiAliasingSolutions.end())
      {
        m_activeAntiAliasingMethod = itrFind->Method;
        theEvent->Handled();
      }
    }

    {
      auto itrFind =
        std::find_if(m_ui.Scene.begin(), m_ui.Scene.end(), [source](const UIContentSelectionRecord& entry) { return entry.Button == source; });
      if (itrFind != m_ui.Scene.end())
      {
        m_focusedWindow = std::distance(m_ui.Scene.begin(), itrFind);
        theEvent->Handled();
      }
    }
  }


  void AntiAliasingShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::F1:
      SafeSetIsChecked(m_ui.Scene, 0, true);
      break;
    case VirtualKey::F2:
      SafeSetIsChecked(m_ui.Scene, 1, true);
      break;
    case VirtualKey::F3:
      SafeSetIsChecked(m_ui.Scene, 2, true);
      break;
    case VirtualKey::Code1:
      SafeSetIsChecked(m_ui.AntiAliasingSolutions, 0, true);
      break;
    case VirtualKey::Code2:
      SafeSetIsChecked(m_ui.AntiAliasingSolutions, 1, true);
      break;
    case VirtualKey::Code3:
      SafeSetIsChecked(m_ui.AntiAliasingSolutions, 2, true);
      break;
    case VirtualKey::Code4:
      SafeSetIsChecked(m_ui.AntiAliasingSolutions, 3, true);
      break;
    case VirtualKey::Code5:
      SafeSetIsChecked(m_ui.AntiAliasingSolutions, 4, true);
      break;
    case VirtualKey::P:
      m_ui.CbPause->Toggle();
      break;
    case VirtualKey::Space:
      SetToDefaults();
      break;
    default:
      break;
    }
  }


  void AntiAliasingShared::Update(const DemoTime& demoTime)
  {
    const bool isPaused = IsPaused();

    const PxRectangle wireframeWindowRectanglePx(GetContentWindowRectangleInScreenspace(ContentWindowId::WireframeCube));
    const PxSize2DF wireframeWindowSizePxf = TypeConverter::To<PxSize2DF>(wireframeWindowRectanglePx.GetSize());
    const float aspectRatio = wireframeWindowSizePxf.RawHeight() > 0 ? wireframeWindowSizePxf.RawWidth() / wireframeWindowSizePxf.RawHeight() : 1.0f;
    m_matView = Matrix::CreateLookAt(Vector3(0.0f, 2.0f, 4.5f), Vector3(), Vector3::Up());
    m_matProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 0.001f, 500.0f);

    if (!isPaused)
    {
      m_lineAnim.Radians1 += m_lineAnim.RotationSpeed1 * demoTime.DeltaTime;
      m_lineAnim.Radians2 += m_lineAnim.RotationSpeed2 * demoTime.DeltaTime;
      m_lineAnim.Radians3 += m_lineAnim.RotationSpeed3 * demoTime.DeltaTime;
      m_lineAnim.Radians1 = MathHelper::WrapAngle(m_lineAnim.Radians1);
      m_lineAnim.Radians2 = MathHelper::WrapAngle(m_lineAnim.Radians2);
      m_lineAnim.Radians3 = MathHelper::WrapAngle(m_lineAnim.Radians3);
    }

    {    // Create a cube rendered with lines
      auto matAxisX = Matrix::CreateRotationX(m_lineAnim.Radians1);
      auto matAxisY = Matrix::CreateRotationY(m_lineAnim.Radians2);
      auto matAxisZ = Matrix::CreateRotationZ(m_lineAnim.Radians3);
      auto matAxisXYZ = matAxisX * matAxisY * matAxisZ;

      m_lineBuilder.Clear();
      m_lineBuilder.Add(BoundingBox(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)), Colors::White(), matAxisXYZ);
    }
  }


  void AntiAliasingShared::Resolve(const DemoTime& demoTime)
  {
    switch (m_focusedWindow)
    {
    case 0:
    default:
      m_windowRectangle[0].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content1));
      m_windowRectangle[1].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content2));
      m_windowRectangle[2].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content3));
      break;
    case 1:
      m_windowRectangle[0].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content2));
      m_windowRectangle[1].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content1));
      m_windowRectangle[2].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content3));
      break;
    case 2:
      m_windowRectangle[0].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content2));
      m_windowRectangle[1].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content3));
      m_windowRectangle[2].SetValue(CalcWindowScreenPosition(*m_ui.Content.Content1));
      break;
    }
    for (auto& rEntry : m_windowRectangle)
    {
      rEntry.Update(demoTime.ElapsedTime);
      if (m_forceCompleteAnimation)
      {
        rEntry.ForceComplete();
      }
    }

    {    // Add dividers
      constexpr Color LineColor(0xFF808080);
      const PxRectangle dividerH = CalcWindowScreenPosition(*m_ui.Content.DividerH);
      const PxRectangle dividerV = CalcWindowScreenPosition(*m_ui.Content.DividerV);

      auto& rLineBuilder = m_dividerLineBuilder;

      const float scaleFactor = (m_activeAntiAliasingMethod == AntiAliasingMethod::SSAA2X) ? 2.0f : 1.0f;

      const float divHX0 = (static_cast<float>(dividerH.RawLeft()) + 0.5f) * scaleFactor;
      const float divHX1 = (static_cast<float>(dividerH.RawRight()) + 0.5f) * scaleFactor;
      const float divHY = (static_cast<float>(dividerH.RawTop()) + 0.5f) * scaleFactor;

      const float divVX = (static_cast<float>(dividerV.RawLeft()) + 0.5f) * scaleFactor;
      const float divVY0 = (static_cast<float>(dividerV.RawTop()) + 0.5f) * scaleFactor;
      const float divVY1 = (static_cast<float>(dividerV.RawBottom()) + 0.5f) * scaleFactor;

      rLineBuilder.Clear();
      rLineBuilder.Add(Vector3(divHX0, divHY, 0.0f), Vector3(divHX1, divHY, 0.0f), LineColor);
      rLineBuilder.Add(Vector3(divVX, divVY0, 0.0f), Vector3(divVX, divVY1, 0.0f), LineColor);
    }


    m_forceCompleteAnimation = false;
  }


  void AntiAliasingShared::Draw(const DemoTime& demoTime)
  {
    m_uiExtension->Draw();
  }


  void AntiAliasingShared::SetToDefaults()
  {
    const ContentWindowId contentWindowId = m_defaultOptions->GetContentId();
    const AntiAliasingMethod aaMethod = m_defaultOptions->GetAAMethod();
    const bool enableFastResolve = m_defaultOptions->EnableFastResolve();
    const bool paused = m_defaultOptions->IsPaused();

    if (!TrySetAAMethod(aaMethod))
    {
      TrySetAAMethod(AntiAliasingMethod::Disabled);
    }
    TrySetContent(contentWindowId);
    if (m_ui.FastResolve->IsEnabled())
    {
      m_ui.FastResolve->SetIsChecked(enableFastResolve);
    }
    m_ui.CbPause->SetIsChecked(paused);
  }


  AntiAliasingShared::UIRecord AntiAliasingShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory,
                                                            const SupportedSampleCountFlags supportedMSAASampleFlags,
                                                            const CustomUIConfig& customUIConfig)
  {
    auto groupAA = uiFactory.CreateRadioGroup("Anti aliasing method");

    auto lblAntiAliasing = uiFactory.CreateLabel("Anti aliasing");

    auto rbMSAA2 = uiFactory.CreateRadioButton(groupAA, "MSAA 2");
    auto rbMSAA4 = uiFactory.CreateRadioButton(groupAA, "MSAA 4");
    auto rbMSAA8 = uiFactory.CreateRadioButton(groupAA, "MSAA 8");
    auto rbSSAA2 = uiFactory.CreateRadioButton(groupAA, "SSAA 2");
    auto rbDisabled = uiFactory.CreateRadioButton(groupAA, "Disabled", true);

    std::vector<UIAntiAliasingRecord> antiAliasingSolutions = {
      UIAntiAliasingRecord(AntiAliasingMethod::MSAA2X, rbMSAA2), UIAntiAliasingRecord(AntiAliasingMethod::MSAA4X, rbMSAA4),
      UIAntiAliasingRecord(AntiAliasingMethod::MSAA8X, rbMSAA8), UIAntiAliasingRecord(AntiAliasingMethod::SSAA2X, rbSSAA2),
      UIAntiAliasingRecord(AntiAliasingMethod::Disabled, rbDisabled)};

    auto groupScene = uiFactory.CreateRadioGroup("Scene");

    auto lblScene = uiFactory.CreateLabel("Focus scene");

    auto rbScene1 = uiFactory.CreateRadioButton(groupScene, "Car", true);
    auto rbScene2 = uiFactory.CreateRadioButton(groupScene, "Knight");
    auto rbScene3 = uiFactory.CreateRadioButton(groupScene, "Cube");

    std::vector<UIContentSelectionRecord> scene = {UIContentSelectionRecord(ContentWindowId::Car, rbScene1),
                                                   UIContentSelectionRecord(ContentWindowId::Knight, rbScene2),
                                                   UIContentSelectionRecord(ContentWindowId::WireframeCube, rbScene3)};

    auto switchFastResolve = uiFactory.CreateSwitch("MSAA fast resolve", customUIConfig.UseFastResolve);
    switchFastResolve->SetEnabled(customUIConfig.SupportFastResolve);
    switchFastResolve->SetVisibility(customUIConfig.ShowFastResolve ? UI::ItemVisibility::Visible : UI::ItemVisibility::Collapsed);


    auto cbPause = uiFactory.CreateCheckBox("Pause", false);

    auto stackLayout = std::make_shared<UI::StackLayout>(uiFactory.GetContext());
    stackLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    stackLayout->AddChild(lblAntiAliasing);
    for (const auto& entry : antiAliasingSolutions)
    {
      stackLayout->AddChild(entry.Button);
    }
    stackLayout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal));
    stackLayout->AddChild(lblScene);
    for (const auto& entry : scene)
    {
      stackLayout->AddChild(entry.Button);
    }
    stackLayout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal));
    stackLayout->AddChild(switchFastResolve);
    stackLayout->AddChild(cbPause);

    auto scrollViewer = uiFactory.CreateScrollViewer(stackLayout, UI::ScrollModeFlags::TranslateY, false);

    auto menuBarLayout = uiFactory.CreateLeftBar(scrollViewer);

    auto content = CreateContentUI(uiFactory);

    auto mainLayout = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    mainLayout->AddChild(menuBarLayout, UI::LayoutLength(UI::LayoutUnitType::Auto));
    mainLayout->AddChild(content.MainLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    // Enable / disable features based on capabilities
    rbMSAA2->SetEnabled(SupportedSampleCountFlagsUtil::IsEnabled(supportedMSAASampleFlags, SupportedSampleCountFlags::Samples2));
    rbMSAA4->SetEnabled(SupportedSampleCountFlagsUtil::IsEnabled(supportedMSAASampleFlags, SupportedSampleCountFlags::Samples4));
    rbMSAA8->SetEnabled(SupportedSampleCountFlagsUtil::IsEnabled(supportedMSAASampleFlags, SupportedSampleCountFlags::Samples8));
    return {mainLayout, std::move(antiAliasingSolutions), std::move(scene), std::move(switchFastResolve), cbPause, std::move(content)};
  }


  AntiAliasingShared::UIContentRecord AntiAliasingShared::CreateContentUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto dividerH = std::make_shared<UI::BaseWindow>(uiFactory.GetContext());
    auto dividerV = std::make_shared<UI::BaseWindow>(uiFactory.GetContext());
    dividerH->SetMinHeight(DpSize1DF::Create(1));
    dividerH->SetAlignmentX(UI::ItemAlignment::Stretch);
    dividerV->SetWidth(UI::DpLayoutSize1D::Create(1));
    dividerV->SetAlignmentX(UI::ItemAlignment::Near);
    dividerV->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto content1 = std::make_shared<UI::BaseWindow>(uiFactory.GetContext());
    auto content2 = std::make_shared<UI::BaseWindow>(uiFactory.GetContext());
    auto content3 = std::make_shared<UI::BaseWindow>(uiFactory.GetContext());
    content1->SetAlignmentX(UI::ItemAlignment::Stretch);
    content1->SetAlignmentY(UI::ItemAlignment::Stretch);
    content2->SetAlignmentX(UI::ItemAlignment::Stretch);
    content2->SetAlignmentY(UI::ItemAlignment::Stretch);
    content3->SetAlignmentX(UI::ItemAlignment::Stretch);
    content3->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto contentLayout = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    contentLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    contentLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    contentLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    contentLayout->AddChild(content2, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    contentLayout->AddChild(dividerH, UI::LayoutLength(UI::LayoutUnitType::Auto));
    contentLayout->AddChild(content3, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto contentLayout2 = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    contentLayout2->SetAlignmentX(UI::ItemAlignment::Stretch);
    contentLayout2->SetAlignmentY(UI::ItemAlignment::Stretch);
    contentLayout2->SetOrientation(UI::LayoutOrientation::Horizontal);
    contentLayout2->AddChild(content1, UI::LayoutLength(UI::LayoutUnitType::Star, 0.75f));
    contentLayout2->AddChild(dividerV, UI::LayoutLength(UI::LayoutUnitType::Auto));
    contentLayout2->AddChild(contentLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 0.25f));

    auto zoomArea = std::make_shared<UI::Custom::ZoomArea>(uiFactory.GetContext());
    zoomArea->SetAlignmentX(UI::ItemAlignment::Stretch);
    zoomArea->SetAlignmentY(UI::ItemAlignment::Stretch);
    zoomArea->SetAreaWidth(DpSize1DF::Create(500));
    zoomArea->SetAreaHeight(DpSize1DF::Create(500));
    zoomArea->SetContent(uiFactory.GetResources().GetLineListSprite(false));
    zoomArea->SetZoomFactor(4.0f);

    auto mainLayout = std::make_shared<UI::FillLayout>(uiFactory.GetContext());
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddChild(zoomArea);
    mainLayout->AddChild(contentLayout2);

    return {mainLayout, content1, content2, content3, zoomArea, dividerH, dividerV};
  }

  void AntiAliasingShared::SafeSetIsChecked(const std::vector<UIContentSelectionRecord>& entries, const std::size_t index, const bool value)
  {
    if (entries.size() > index)
    {
      entries[index].Button->SetIsChecked(value);
    }
  }

  void AntiAliasingShared::SafeSetIsChecked(const std::vector<UIAntiAliasingRecord>& entries, const std::size_t index, const bool value)
  {
    if (entries.size() > index)
    {
      entries[index].Button->SetIsChecked(value);
    }
  }

  bool AntiAliasingShared::TrySetAAMethod(const AntiAliasingMethod aaMethod)
  {
    auto itrFind = std::find_if(m_ui.AntiAliasingSolutions.begin(), m_ui.AntiAliasingSolutions.end(),
                                [aaMethod](const UIAntiAliasingRecord& entry) { return entry.Method == aaMethod; });
    if (itrFind != m_ui.AntiAliasingSolutions.end() && itrFind->Button->IsEnabled())
    {
      itrFind->Button->SetIsChecked(true);
      m_activeAntiAliasingMethod = aaMethod;
      return true;
    }
    return false;
  }

  bool AntiAliasingShared::TrySetContent(const ContentWindowId contentWindowId)
  {
    auto itrFind = std::find_if(m_ui.Scene.begin(), m_ui.Scene.end(),
                                [contentWindowId](const UIContentSelectionRecord& entry) { return entry.ContentId == contentWindowId; });
    if (itrFind != m_ui.Scene.end() && itrFind->Button->IsEnabled())
    {
      itrFind->Button->SetIsChecked(true);
      m_focusedWindow = std::distance(m_ui.Scene.begin(), itrFind);
      return true;
    }
    return false;
  }
}
