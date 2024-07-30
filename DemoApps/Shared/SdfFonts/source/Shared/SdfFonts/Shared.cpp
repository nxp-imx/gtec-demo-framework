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

#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/SdfFonts/Shared.hpp>
#include <cassert>

namespace Fsl
{
  // NOTE:
  // emulate 95dp is good for showing "non-pixel perfect" rendering issues with the current font.
  // emulate 474 is good for showing the difference between the various string rendering techniques
  // emulate 400dp with forced 480dp is good for finding rounding errors on the baseline
  // emulate 423dp with forced 480dp

  namespace LocalConfig
  {
    //! This should really be part of the font file
    constexpr const int32_t FontSpread = 4;
    constexpr const ConstrainedValue<float> OutlineDistanceDefaultValue(0.2f, 0.0f, 1.0f);
    constexpr const ConstrainedValue<float> ShadowSmoothingDefaultValue(0.2f, 0.0f, 1.0f);
    constexpr const ConstrainedValue<float> ShadowOffsetXDefaultValue(0.2f, -1.0f, 1.0f);
    constexpr const ConstrainedValue<float> ShadowOffsetYDefaultValue(0.2f, -1.0f, 1.0f);

    constexpr const bool KerningEnabled = true;
    constexpr const bool DrawBoundingBoxes = false;
    constexpr const bool DrawOutlineDefaultValue = false;
    constexpr const bool DrawShadowDefaultValue = false;
    constexpr const ConstrainedValue<float> Scale(4.0, 0.1f, 10.0f);
  }

  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_graphicsService(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_renderSystem(m_graphicsService->GetBasicRenderSystem())
  {
    FSLLOG3_INFO("windowMetrics.ExactDpi: {}", config.WindowMetrics.ExactDpi);
    FSLLOG3_INFO("windowMetrics.DensityDpi: {}", config.WindowMetrics.DensityDpi);
    FSLLOG3_INFO("windowMetrics.DensityScaleFactor: {}", config.WindowMetrics.DensityScaleFactor);

    auto nativeGraphics = m_graphicsService->GetNativeGraphics();
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    FSLLOG3_INFO("Preparing UI");
    {    // Build a simple UI
      auto windowContext = m_uiExtension->GetContext();
      auto uiFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

      m_fillSprite = uiFactory->GetResources().GetBasicFillSprite();

      m_uiRecord = CreateUI(windowContext, *uiFactory, config.WindowMetrics.DensityDpi);
      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_uiRecord.MainLayout);
    }

    SetDefaultValues();
    OnConfigurationChanged(config.WindowMetrics);
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.ButtonDefault)
    {
      SetDefaultValues();
    }
  }

  void Shared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.DrawOutlineCheckBox || theEvent->GetSource() == m_uiRecord.DrawShadowCheckBox)
    {
      UpdateLinkedUIState();
    }
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::K:
      {
        m_uiRecord.KerningCheckBox->Toggle();
        break;
      }
    case VirtualKey::Delete:
      m_uiRecord.FontScaleSlider->SubValue(0.01f);
      break;
    case VirtualKey::PageDown:
      m_uiRecord.FontScaleSlider->AddValue(0.01f);
      break;
    case VirtualKey::LeftArrow:
      m_uiRecord.FontScaleSlider->SubValue(0.5f);
      break;
    case VirtualKey::RightArrow:
      m_uiRecord.FontScaleSlider->AddValue(0.5f);
      break;
    case VirtualKey::Space:
      {
        SetDefaultValues();
        break;
      }
    default:
      break;
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& /*windowMetrics*/)
  {
    assert(m_renderSystem);
    m_fillTexture = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_fillSprite);
  }

  void Shared::Update(const DemoTime& /*demoTime*/)
  {
  }

  void Shared::Draw()
  {
    m_uiExtension->Draw();
  }


  PxPoint2 Shared::GetContentOffset() const
  {
    return m_uiRecord.MainLayout->PointFrom(m_uiRecord.MainContent.get(), PxPoint2());
  }

  SdfFontMode Shared::GetSdfMode() const
  {
    auto mode = SdfFontMode::Normal;
    mode |= (m_uiRecord.DrawOutlineCheckBox->IsChecked() ? SdfFontMode::Outline : SdfFontMode::Normal);
    mode |= (m_uiRecord.DrawShadowCheckBox->IsChecked() ? SdfFontMode::Shadow : SdfFontMode::Normal);
    return mode;
  }

  bool Shared::GetKerningEnabled() const
  {
    return m_uiRecord.KerningCheckBox->IsChecked();
  }

  bool Shared::GetBoundingBoxesEnabled() const
  {
    return m_uiRecord.DrawBoundingBoxesCheckBox->IsChecked();
  }

  FontDrawConfig Shared::GetFontDrawConfig() const
  {
    const auto fontScale = m_uiRecord.FontScaleSlider->GetValue();
    const auto outlineDistance = 1.0f - m_uiRecord.OutlineDistanceSlider->GetValue();
    const auto shadowSmoothing = m_uiRecord.ShadowSmoothingSlider->GetValue();
    const Vector2 shadowOffset(m_uiRecord.ShadowOffsetXSlider->GetValue(), m_uiRecord.ShadowOffsetYSlider->GetValue());
    return {LocalConfig::FontSpread, fontScale, outlineDistance, shadowOffset, shadowSmoothing};
  }


  void Shared::DrawBoundingBoxes(INativeBatch2D& nativeBatch, const PxPoint2& dstPositionPx, const StringViewLite& strView,
                                 const TextureAtlasSpriteFont& font, const BitmapFontConfig& fontConfig,
                                 std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    if (strView.size() > rPositionsScratchpad.size())
    {
      rPositionsScratchpad.resize(strView.size());
    }
    auto scratchpadSpan = SpanUtil::AsSpan(rPositionsScratchpad);
    if (font.ExtractRenderRules(scratchpadSpan, strView, fontConfig))
    {
      Span<SpriteFontGlyphPosition> positionSpan(scratchpadSpan.subspan(0, strView.size()));

      const PxVector2 dstPositionPxf = TypeConverter::To<PxVector2>(dstPositionPx);
      for (std::size_t i = 0; i < positionSpan.size(); ++i)
      {
        auto dstRectPxf = PxAreaRectangleF::AddLocation(dstPositionPxf, positionSpan[i].DstRectPxf);
        nativeBatch.DebugDrawRectangle(m_fillTexture, dstRectPxf, Color(0x80, 0, 0, 0xFF));
      }
    }
  }


  void Shared::SetDefaultValues()
  {
    m_uiRecord.KerningCheckBox->SetIsChecked(LocalConfig::KerningEnabled);
    m_uiRecord.DrawOutlineCheckBox->SetIsChecked(LocalConfig::DrawOutlineDefaultValue);
    m_uiRecord.DrawShadowCheckBox->SetIsChecked(LocalConfig::DrawShadowDefaultValue);
    m_uiRecord.FontScaleSlider->SetValue(LocalConfig::Scale.Get());
    m_uiRecord.OutlineDistanceSlider->SetValue(LocalConfig::OutlineDistanceDefaultValue.Get());
    m_uiRecord.ShadowSmoothingSlider->SetValue(LocalConfig::ShadowSmoothingDefaultValue.Get());
    m_uiRecord.ShadowOffsetXSlider->SetValue(LocalConfig::ShadowOffsetXDefaultValue.Get());
    m_uiRecord.ShadowOffsetYSlider->SetValue(LocalConfig::ShadowOffsetYDefaultValue.Get());
    UpdateLinkedUIState();
  }

  void Shared::UpdateLinkedUIState()
  {
    {    // Outline enabled / disabled
      auto isEnabled = m_uiRecord.DrawOutlineCheckBox->IsChecked();
      m_uiRecord.OutlineSliderLabel->SetEnabled(isEnabled);
      m_uiRecord.OutlineDistanceSlider->SetEnabled(isEnabled);
    }
    {    // Shadow enabled / disabled
      auto isEnabled = m_uiRecord.DrawShadowCheckBox->IsChecked();
      m_uiRecord.ShadowSmoothingLabel->SetEnabled(isEnabled);
      m_uiRecord.ShadowSmoothingSlider->SetEnabled(isEnabled);
      m_uiRecord.ShadowOffsetXLabel->SetEnabled(isEnabled);
      m_uiRecord.ShadowOffsetXSlider->SetEnabled(isEnabled);
      m_uiRecord.ShadowOffsetYLabel->SetEnabled(isEnabled);
      m_uiRecord.ShadowOffsetYSlider->SetEnabled(isEnabled);
    }
  }

  Shared::UIRecord Shared::CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory,
                                    const uint32_t /*densityDpi*/)
  {
    UIRecord record;

    auto labelKerning = rUIFactory.CreateLabel("Kerning");
    labelKerning->SetAlignmentY(UI::ItemAlignment::Center);
    auto labelDrawBoundingBoxes = rUIFactory.CreateLabel("B.Boxes");
    labelDrawBoundingBoxes->SetAlignmentY(UI::ItemAlignment::Center);

    auto scaleLabel = rUIFactory.CreateLabel("Scale");
    scaleLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto sdfOutlineLabel = rUIFactory.CreateLabel("SDF outline");
    sdfOutlineLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto outlineDistanceLabel = rUIFactory.CreateLabel("Thickness");
    outlineDistanceLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto sdfShadowLabel = rUIFactory.CreateLabel("SDF shadow");
    sdfShadowLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto shadowSmoothingLabel = rUIFactory.CreateLabel("Smoothing");
    shadowSmoothingLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto shadowOffsetXLabel = rUIFactory.CreateLabel("Offset.X");
    shadowOffsetXLabel->SetAlignmentY(UI::ItemAlignment::Center);
    auto shadowOffsetYLabel = rUIFactory.CreateLabel("Offset.Y");
    shadowOffsetYLabel->SetAlignmentY(UI::ItemAlignment::Center);

    auto scaleSlider = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::Scale);
    auto outlineDistanceSlider = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::OutlineDistanceDefaultValue);
    auto shadowSmoothingSlider = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::ShadowSmoothingDefaultValue);
    auto shadowOffsetXSlider = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::ShadowOffsetXDefaultValue);
    auto shadowOffsetYSlider = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::ShadowOffsetYDefaultValue);

    auto kerningCB = rUIFactory.CreateSwitch(LocalConfig::KerningEnabled);
    kerningCB->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto drawBoundingBoxesCB = rUIFactory.CreateSwitch(LocalConfig::DrawBoundingBoxes);
    drawBoundingBoxesCB->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto drawOutlineCB = rUIFactory.CreateSwitch(LocalConfig::DrawOutlineDefaultValue);
    drawOutlineCB->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto drawShadowCB = rUIFactory.CreateSwitch(LocalConfig::DrawShadowDefaultValue);
    drawShadowCB->SetAlignmentX(UI::ItemAlignment::Stretch);


    auto menuGrid = std::make_shared<UI::GridLayout>(context);
    menuGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    menuGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
    menuGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    menuGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    menuGrid->AddChild(scaleLabel, 0, 0);
    menuGrid->AddChild(scaleSlider, 1, 0);
    menuGrid->AddChild(labelKerning, 0, 1);
    menuGrid->AddChild(kerningCB, 1, 1);
    menuGrid->AddChild(labelDrawBoundingBoxes, 0, 2);
    menuGrid->AddChild(drawBoundingBoxesCB, 1, 2);
    menuGrid->AddChild(sdfOutlineLabel, 0, 3);
    menuGrid->AddChild(drawOutlineCB, 1, 3);
    menuGrid->AddChild(outlineDistanceLabel, 0, 4);
    menuGrid->AddChild(outlineDistanceSlider, 1, 4);
    menuGrid->AddChild(sdfShadowLabel, 0, 5);
    menuGrid->AddChild(drawShadowCB, 1, 5);
    menuGrid->AddChild(shadowSmoothingLabel, 0, 6);
    menuGrid->AddChild(shadowSmoothingSlider, 1, 6);
    menuGrid->AddChild(shadowOffsetXLabel, 0, 7);
    menuGrid->AddChild(shadowOffsetXSlider, 1, 7);
    menuGrid->AddChild(shadowOffsetYLabel, 0, 8);
    menuGrid->AddChild(shadowOffsetYSlider, 1, 8);
    menuGrid->SetWidth(UI::DpLayoutSize1D(DpValue(250)));


    auto menuBar = rUIFactory.CreateLeftBar(menuGrid);

    auto dummy = std::make_shared<UI::BaseWindow>(context);
    dummy->SetAlignmentX(UI::ItemAlignment::Stretch);
    dummy->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto btnDefault = rUIFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");
    btnDefault->SetAlignmentX(UI::ItemAlignment::Center);
    btnDefault->SetAlignmentY(UI::ItemAlignment::Far);

    // Create the root layout and add it to the window manager
    auto mainLayout = std::make_shared<UI::GridLayout>(context);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star));
    mainLayout->AddChild(dummy, 1, 0);
    mainLayout->AddChild(menuBar, 0, 0);
    mainLayout->AddChild(btnDefault, 0, 0);

    record.KerningCheckBox = kerningCB;
    record.DrawBoundingBoxesCheckBox = drawBoundingBoxesCB;
    record.DrawOutlineCheckBox = drawOutlineCB;
    record.DrawShadowCheckBox = drawShadowCB;
    record.FontScaleSlider = scaleSlider;
    record.OutlineSliderLabel = outlineDistanceLabel;
    record.OutlineDistanceSlider = outlineDistanceSlider;
    record.ShadowSmoothingLabel = shadowSmoothingLabel;
    record.ShadowSmoothingSlider = shadowSmoothingSlider;
    record.ShadowOffsetXLabel = shadowOffsetXLabel;
    record.ShadowOffsetXSlider = shadowOffsetXSlider;
    record.ShadowOffsetYLabel = shadowOffsetYLabel;
    record.ShadowOffsetYSlider = shadowOffsetYSlider;
    record.ButtonDefault = btnDefault;
    record.MainContent = dummy;
    record.MainLayout = mainLayout;
    return record;
  }
}
