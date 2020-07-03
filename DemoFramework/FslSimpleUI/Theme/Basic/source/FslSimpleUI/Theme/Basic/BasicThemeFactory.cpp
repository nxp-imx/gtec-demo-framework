/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/Theme/Basic/BasicThemeFactory.hpp>
#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/Control/BackgroundNineSlice.hpp>
#include <FslSimpleUI/Base/Control/BasicImage.hpp>
#include <FslSimpleUI/Base/Control/BasicNineSliceImage.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelNineSliceButton.hpp>
#include <FslSimpleUI/Base/Control/Logic/RadioGroup.hpp>
#include <FslSimpleUI/Base/Control/NineSliceImage.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>

namespace Fsl
{
  namespace UI
  {
    namespace Theme
    {
      namespace ThemeColor
      {
        // constexpr const Color White(0xFFFFFFFF);

        constexpr const float DisabledOpacity = 0.38f;
        constexpr const float SliderHoverAlpha = 0.15f;
        constexpr const float HoverAlpha = 0.125f;
        constexpr const float HoverUnselectedAlpha = 0.04f;
        constexpr const float ButtonDownAlpha = 0.85f;

        namespace Palette
        {
          constexpr const Color Primary(0xFF1976d2);
          constexpr const Color FontOnPrimary(0xFFFFFFFF);
          constexpr const Color ButtonOutlineFont(0xFF2196f3);
          constexpr const Color RadioButtonBackground(0xFF6a6a6a);

          constexpr const Color SwitchCursorChecked = Primary;
          constexpr const Color SwitchCursorUnchecked(0xFFfafafa);
          constexpr const Color SwitchBackground(0xFF9a9999);

          constexpr const Color BarColor(0xFF303030);
          constexpr const Color TransparentBarColor(Color::Premultiply(BarColor, 0.85f));

          constexpr const Color PrimaryDark(0xFF004BA0);
          // constexpr const Color FontOnPrimaryDark(0xFFFFFFFF);

          constexpr const Color FontOnPrimaryDisabled(Color::Premultiply(Palette::FontOnPrimary, DisabledOpacity));
          constexpr const Color PrimaryDisabled(Color::Premultiply(Palette::Primary, DisabledOpacity));

          constexpr const Color ImageOnPrimary = Color::White();
        }


        namespace Button
        {
          constexpr const Color BackgroundHoverUp(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundUp(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundDown(Color::Premultiply(Palette::Primary, ButtonDownAlpha));
          constexpr const Color BackgroundDisabled(Palette::PrimaryDisabled);
          constexpr const Color BackgroundHoverOverlayUp(Color::PremultiplyRGB(Palette::Primary, 0.10f, 0));
          constexpr const Color BackgroundHoverOverlayDown(Color::PremultiplyRGB(Palette::Primary, 0.08f, 0));

          constexpr const Color FontUp(Color::Premultiply(Palette::FontOnPrimary));
          constexpr const Color FontDown(Color::Premultiply(Palette::FontOnPrimary, ButtonDownAlpha));

          constexpr const Color OutlineFontUp(Color::Premultiply(Palette::ButtonOutlineFont));
          constexpr const Color OutlineFontDown(Color::Premultiply(Palette::ButtonOutlineFont, ButtonDownAlpha));
          constexpr const Color OutlineFontDisabled(Color::Premultiply(Palette::ButtonOutlineFont, DisabledOpacity));
        }


        // CheckBox
        namespace CheckBox
        {
          constexpr const Color HoverChecked(Color::PremultiplyRGB(Palette::Primary, HoverAlpha, 0));
          constexpr const Color HoverUnchecked(Color::PremultiplyRGB(Palette::RadioButtonBackground, HoverAlpha, 0));
          constexpr const Color CursorChecked(Color::Premultiply(Palette::Primary));
          constexpr const Color CursorCheckedDisabled(Color::Premultiply(Palette::Primary, DisabledOpacity));
          constexpr const Color CursorUnchecked(Color::Transparent());
          constexpr const Color CursorUncheckedDisabled(Color::Transparent());
          constexpr const Color BackgroundChecked(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundCheckedDisabled(Color::Premultiply(Palette::Primary, DisabledOpacity));
          constexpr const Color BackgroundUnchecked(Color::Premultiply(Palette::RadioButtonBackground));
          constexpr const Color BackgroundUncheckedDisabled(Color::Premultiply(Palette::RadioButtonBackground, DisabledOpacity));
          constexpr const Color CheckedFont(Palette::FontOnPrimary);
          constexpr const Color UncheckedFont(Palette::FontOnPrimary);
        }

        namespace ImageButton
        {
          constexpr const Color BackgroundHoverUp(Color::Premultiply(Palette::Primary, 1.10f));
          constexpr const Color BackgroundUp(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundDown(Color::Premultiply(Palette::Primary, ButtonDownAlpha));
          constexpr const Color BackgroundDisabled(Palette::PrimaryDisabled);

          constexpr const Color ImageUp(Color::Premultiply(Palette::ImageOnPrimary));
          constexpr const Color ImageDown(Color::Premultiply(Palette::ImageOnPrimary, ButtonDownAlpha));
          constexpr const Color ImageDisabled(Color::Premultiply(Palette::ImageOnPrimary, DisabledOpacity));
        }


        // RadioButton
        namespace RadioButton
        {
          constexpr const Color HoverChecked(Color::PremultiplyRGB(Palette::Primary, HoverAlpha, 0));
          constexpr const Color HoverUnchecked(Color::PremultiplyRGB(Palette::RadioButtonBackground, HoverAlpha, 0));
          constexpr const Color CursorChecked(Color::Premultiply(Palette::Primary));
          constexpr const Color CursorCheckedDisabled(Color::Premultiply(Palette::Primary, DisabledOpacity));
          constexpr const Color CursorUnchecked(Color::Transparent());
          constexpr const Color CursorUncheckedDisabled(Color::Transparent());
          constexpr const Color BackgroundChecked(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundCheckedDisabled(Color::Premultiply(Palette::Primary, DisabledOpacity));
          constexpr const Color BackgroundUnchecked(Color::Premultiply(Palette::RadioButtonBackground));
          constexpr const Color BackgroundUncheckedDisabled(Color::Premultiply(Palette::RadioButtonBackground, DisabledOpacity));
          constexpr const Color CheckedFont(Palette::FontOnPrimary);
          constexpr const Color UncheckedFont(Palette::FontOnPrimary);
        }

        // Switch
        namespace Switch
        {
          constexpr const Color HoverChecked(Color::PremultiplyRGB(Palette::SwitchCursorChecked, HoverAlpha, 0));
          constexpr const Color HoverUnchecked(Color::PremultiplyRGB(Palette::SwitchCursorUnchecked, HoverUnselectedAlpha, 0));
          constexpr const Color CursorChecked(Color::Premultiply(Palette::SwitchCursorChecked));
          constexpr const Color CursorCheckedDisabled(Color::PremultiplyRGB(Palette::SwitchCursorChecked, DisabledOpacity, 255u));
          constexpr const Color CursorUnchecked(Color::Premultiply(Palette::SwitchCursorUnchecked));
          constexpr const Color CursorUncheckedDisabled(Color::PremultiplyRGB(Palette::SwitchCursorUnchecked, DisabledOpacity, 255u));
          constexpr const Color BackgroundChecked(Color::Premultiply(Palette::PrimaryDark));
          constexpr const Color BackgroundCheckedDisabled(Color::Premultiply(Palette::PrimaryDark, DisabledOpacity));
          constexpr const Color BackgroundUnchecked(Color::Premultiply(Palette::SwitchBackground));
          constexpr const Color BackgroundUncheckedDisabled(Color::Premultiply(Palette::SwitchBackground, DisabledOpacity));

          constexpr const Color CheckedFont(Palette::FontOnPrimary);
          constexpr const Color UncheckedFont(Palette::FontOnPrimary);
        }

        // Slider
        namespace Slider
        {
          constexpr const Color Background(Color::Premultiply(Palette::Primary));
          constexpr const Color BackgroundDisabled(Palette::PrimaryDisabled);
          constexpr const Color Cursor(Color::Premultiply(Palette::Primary));
          constexpr const Color CursorDisabled(Palette::PrimaryDisabled);
          constexpr const Color HoverColor(Color::PremultiplyRGB(Palette::Primary, SliderHoverAlpha, 0));
        }
      }

      namespace LocalConfig
      {
        constexpr const DpPoint CheckBoxCursorChecked(24, 24);
        constexpr const DpPoint CheckBoxCursorUnchecked(24, 24);

        constexpr const DpPoint RadioButtonCursorChecked(24, 24);
        constexpr const DpPoint RadioButtonCursorUnchecked(24, 24);

        constexpr const DpPoint SwitchCursorChecked(32, 24);
        constexpr const DpPoint SwitchCursorUnchecked(16, 24);

        // 160dpi 32, 24 | 16, 24
        //  96dpi 19, 14 | 10, 14
        //  80dpi 16, 12 |  8, 12

      }

      namespace TextureName
      {
        constexpr const IO::PathView Fill("Fill");
        constexpr const IO::PathView Divider("Control/White/Divider");

        // Dialog
        constexpr const IO::PathView Dialog("Control/White/ButtonN");

        // Bar
        constexpr const IO::PathView LeftBar("Control/White/BarLeft");
        constexpr const IO::PathView TopBar("Control/White/BarTop");
        constexpr const IO::PathView RightBar("Control/White/BarRight");
        constexpr const IO::PathView BottomBar("Control/White/BarBottom");

        // Button
        constexpr const IO::PathView ButtonHover("Control/White/ButtonH");
        constexpr const IO::PathView ButtonNormal("Control/White/ButtonN");

        constexpr const IO::PathView ButtonOutlineNormal("Control/White/ButtonOutlineN");
        constexpr const IO::PathView ButtonOutlineHover("Control/White/ButtonOutlineH");

        // CheckBox
        constexpr const IO::PathView CheckBoxCursorHover("Control/White/Switch_Hover");
        constexpr const IO::PathView CheckBoxCursor("Control/White/CheckBox_Cursor");
        constexpr const IO::PathView CheckBoxBackground("Control/White/CheckBox_BG");

        // FloatingButton
        constexpr const IO::PathView FloatingButtonPressed("Control/White/FloatingRoundButtonP");
        constexpr const IO::PathView FloatingButtonNormal("Control/White/FloatingRoundButtonN");
        constexpr const IO::PathView FloatingButtonHover("Control/White/FloatingRoundButtonH");
        constexpr const IO::PathView FloatingSmallButtonPressed("Control/White/FloatingSmallRoundButtonP");
        constexpr const IO::PathView FloatingSmallButtonNormal("Control/White/FloatingSmallRoundButtonN");
        constexpr const IO::PathView FloatingSmallButtonHover("Control/White/FloatingSmallRoundButtonH");

        // RadioButton
        constexpr const IO::PathView RadioButtonCursorHover("Control/White/Switch_Hover");
        constexpr const IO::PathView RadioButtonCursor("Control/White/RadioButton_Cursor");
        constexpr const IO::PathView RadioButtonBackground("Control/White/RadioButton_BG");

        // Switch
        constexpr const IO::PathView SwitchCursorHover("Control/White/Switch_Hover");
        constexpr const IO::PathView SwitchCursor("Control/White/Switch_Cursor");
        constexpr const IO::PathView SwitchBackground("Control/White/Switch_BG");

        // Slider
        constexpr const IO::PathView SliderBackground("Control/White/SliderBG");
        constexpr const IO::PathView SliderCursor("Control/White/SliderCursor");
        constexpr const IO::PathView SliderCursorHover("Control/White/SliderCursor_Hover");

      }

      namespace
      {
        constexpr Color GetBarColor(const BarType type)
        {
          switch (type)
          {
          case BarType::Transparent:
            return ThemeColor::Palette::TransparentBarColor;
          case BarType::Normal:
          default:
            return ThemeColor::Palette::BarColor;
          }
        }
      }

      BasicThemeFactory::BasicThemeFactory(const std::shared_ptr<WindowContext>& context, ISpriteResourceManager& rResourceManager,
                                           const SpriteMaterialId& spriteMaterialId)
        : m_context(context)
      {
        if (!context)
        {
          throw std::invalid_argument("context can not be null");
        }

        m_fillSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::Fill);

        m_dividerNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::Divider);

        // bar
        m_leftBarNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::LeftBar);
        m_topBarNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::TopBar);
        m_rightBarNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::RightBar);
        m_bottomBarNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::BottomBar);

        // Dialog
        m_dialogNineSliceSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::Dialog);

        // button
        m_containedButtonNormalSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::ButtonNormal);
        m_containedButtonHoverSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::ButtonHover);
        m_outlineButtonHoverSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::ButtonOutlineHover);
        m_outlineButtonNormalSprite = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::ButtonOutlineNormal);

        // checkbox
        m_texCheckBoxHover = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::CheckBoxCursorHover);
        m_texCheckBoxChecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::CheckBoxCursor);
        m_texCheckBoxUnchecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::CheckBoxBackground);

        // Floating button
        m_floatingButtonPressedSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingButtonPressed);
        m_floatingButtonNormalSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingButtonNormal);
        m_floatingButtonHoverSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingButtonHover);

        m_floatingSmallButtonPressedSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingSmallButtonPressed);
        m_floatingSmallButtonNormalSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingSmallButtonNormal);
        m_floatingSmallButtonHoverSprite = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::FloatingSmallButtonHover);

        // radiobutton
        m_texRadioButtonHover = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::RadioButtonCursorHover);
        m_texRadioButtonChecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::RadioButtonCursor);
        m_texRadioButtonUnchecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::RadioButtonBackground);

        // switch
        m_texSwitchHover = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::SwitchCursorHover);
        m_texSwitchChecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::SwitchCursor);
        m_texSwitchUnchecked = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::SwitchBackground);

        // slider
        m_texSliderCursor = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::SliderCursor);
        m_texSliderCursorHover = rResourceManager.CreateImageSprite(spriteMaterialId, TextureName::SliderCursorHover);
        m_texSliderBG = rResourceManager.CreateNineSliceSprite(spriteMaterialId, TextureName::SliderBackground);
        m_sliderCursorSizeDp = DpSize(12, 12);
        m_sliderCursorOriginDp = DpPoint(24, 24);
      }


      BasicThemeFactory::~BasicThemeFactory() = default;


      Color BasicThemeFactory::GetThemePrimaryColor() const    // NOLINT(readability-convert-member-functions-to-static)
      {
        return ThemeColor::Palette::Primary;
      }

      Color BasicThemeFactory::GetThemePrimaryDarkColor() const    // NOLINT(readability-convert-member-functions-to-static)
      {
        return ThemeColor::Palette::PrimaryDark;
      }

      Color BasicThemeFactory::GetThemeSidebarColor() const    // NOLINT(readability-convert-member-functions-to-static)
      {
        return ThemeColor::Palette::BarColor;
      }


      Color BasicThemeFactory::GetThemeFontOnPrimaryColor() const
      {
        return ThemeColor::Palette::FontOnPrimary;
      }

      Color BasicThemeFactory::GetThemeFontOnPrimaryDisabledColor() const
      {
        return ThemeColor::Palette::FontOnPrimaryDisabled;
      }


      BasicThemeFactory::SliderThemeColors
        BasicThemeFactory::GetSliderThemeColors() const    // NOLINT(readability-convert-member-functions-to-static)
      {
        return {ThemeColor::Slider::Background, ThemeColor::Slider::BackgroundDisabled, ThemeColor::Slider::Cursor,
                ThemeColor::Slider::CursorDisabled, ThemeColor::Slider::HoverColor};
      }


      std::shared_ptr<UI::Label> BasicThemeFactory::CreateLabel(const StringViewLite& strView)
      {
        auto label = std::make_shared<UI::Label>(m_context);
        label->SetContent(strView);
        label->SetFontDisabledColor(ThemeColor::Palette::FontOnPrimaryDisabled);
        label->FinishAnimation();
        return label;
      }

      std::shared_ptr<UI::Label> BasicThemeFactory::CreateLabel(std::string&& str)
      {
        auto label = std::make_shared<UI::Label>(m_context);
        label->SetContent(std::move(str));
        label->SetFontDisabledColor(ThemeColor::Palette::FontOnPrimaryDisabled);
        label->FinishAnimation();
        return label;
      }

      // ----- CreateImageButton

      std::shared_ptr<ImageButton> BasicThemeFactory::CreateImageButton(const ImageButtonType buttonType, std::shared_ptr<ImageSprite> sprite)
      {
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }
        auto newControl = std::make_shared<ImageButton>(m_context);
        newControl->SetContent(std::move(sprite));
        newControl->SetUpColor(ThemeColor::ImageButton::ImageUp);
        newControl->SetDownColor(ThemeColor::ImageButton::ImageDown);
        newControl->SetDisabledColor(ThemeColor::ImageButton::ImageDisabled);
        newControl->SetScalePolicy(ItemScalePolicy::NoScaling);
        newControl->SetBackgroundColorHoverUp(ThemeColor::ImageButton::BackgroundHoverUp);
        newControl->SetBackgroundColorUp(ThemeColor::ImageButton::BackgroundUp);
        newControl->SetBackgroundColorDown(ThemeColor::ImageButton::BackgroundDown);
        newControl->SetBackgroundColorDisabled(ThemeColor::ImageButton::BackgroundDisabled);

        switch (buttonType)
        {
        case ImageButtonType::Normal:
          newControl->SetBackground(m_floatingButtonPressedSprite);
          newControl->SetBackgroundHover(m_floatingButtonNormalSprite);
          break;
        case ImageButtonType::Small:
          newControl->SetBackground(m_floatingSmallButtonPressedSprite);
          newControl->SetBackgroundHover(m_floatingSmallButtonNormalSprite);
          break;
        case ImageButtonType::FloatingNormal:
          newControl->SetBackground(m_floatingButtonNormalSprite);
          newControl->SetBackgroundHover(m_floatingButtonHoverSprite);
          break;
        case ImageButtonType::FloatingSmall:
          newControl->SetBackground(m_floatingSmallButtonNormalSprite);
          newControl->SetBackgroundHover(m_floatingSmallButtonHoverSprite);
          break;
        default:
          throw NotImplementedException("Unsupported button type");
        }

        newControl->FinishAnimation();
        return newControl;
      }

      // ----- CreateImageButton

      std::shared_ptr<SimpleImageButton> BasicThemeFactory::CreateImageButton(std::shared_ptr<ImageSprite> sprite)
      {
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }
        auto newControl = std::make_shared<SimpleImageButton>(m_context);
        newControl->SetContent(std::move(sprite));
        newControl->SetUpColor(ThemeColor::ImageButton::ImageUp);
        newControl->SetDownColor(ThemeColor::ImageButton::ImageDown);
        newControl->SetDisabledColor(ThemeColor::ImageButton::ImageDisabled);
        newControl->SetScalePolicy(ItemScalePolicy::NoScaling);
        newControl->FinishAnimation();
        return newControl;
      }

      // ----- CreateTextButton

      std::shared_ptr<UI::LabelNineSliceButton> BasicThemeFactory::CreateTextButton(const ButtonType buttonType)
      {
        auto newButton = std::make_shared<UI::LabelNineSliceButton>(m_context);
        newButton->SetContentAlignmentX(UI::ItemAlignment::Center);
        newButton->SetContentAlignmentY(UI::ItemAlignment::Center);
        newButton->SetFontColorUp(ThemeColor::Button::FontUp);
        newButton->SetFontColorDown(ThemeColor::Button::FontDown);
        // newButton->SetMinWidth(64);
        // newButton->SetMinHeight(36);
        switch (buttonType)
        {
        case ButtonType::Text:
          // No background
          newButton->SetBackgroundHoverOverlay(m_outlineButtonHoverSprite);
          newButton->SetBackgroundHoverOverlayColorUp(ThemeColor::Button::BackgroundHoverOverlayUp);
          newButton->SetBackgroundHoverOverlayColorDown(ThemeColor::Button::BackgroundHoverOverlayDown);
          newButton->SetFontColorDown(ThemeColor::Button::BackgroundDown);
          newButton->SetFontColorUp(ThemeColor::Button::BackgroundUp);
          newButton->SetFontColorDown(ThemeColor::Button::OutlineFontDown);
          newButton->SetFontColorUp(ThemeColor::Button::OutlineFontUp);
          newButton->SetFontColorDisabled(ThemeColor::Button::OutlineFontDisabled);
          break;
        case ButtonType::Outlined:
          newButton->SetBackground(m_outlineButtonNormalSprite);
          newButton->SetBackgroundHover(m_outlineButtonNormalSprite);
          newButton->SetBackgroundHoverOverlay(m_outlineButtonHoverSprite);
          newButton->SetBackgroundColorHoverUp(ThemeColor::Button::BackgroundHoverUp);
          newButton->SetBackgroundColorUp(ThemeColor::Button::BackgroundUp);
          newButton->SetBackgroundColorDown(ThemeColor::Button::BackgroundDown);
          newButton->SetBackgroundColorDisabled(ThemeColor::Button::BackgroundDisabled);
          newButton->SetBackgroundHoverOverlayColorUp(ThemeColor::Button::BackgroundHoverOverlayUp);
          newButton->SetBackgroundHoverOverlayColorDown(ThemeColor::Button::BackgroundHoverOverlayDown);
          newButton->SetFontColorDown(ThemeColor::Button::OutlineFontDown);
          newButton->SetFontColorUp(ThemeColor::Button::OutlineFontUp);
          newButton->SetFontColorDisabled(ThemeColor::Button::OutlineFontDisabled);
          break;
        case ButtonType::Contained:
          newButton->SetBackgroundColorHoverUp(ThemeColor::Button::BackgroundHoverUp);
          newButton->SetBackgroundColorUp(ThemeColor::Button::BackgroundUp);
          newButton->SetBackgroundColorDown(ThemeColor::Button::BackgroundDown);
          newButton->SetBackgroundColorDisabled(ThemeColor::Button::BackgroundDisabled);
          newButton->SetBackgroundHoverOverlayColorUp(ThemeColor::Button::BackgroundHoverOverlayUp);
          newButton->SetBackgroundHoverOverlayColorDown(ThemeColor::Button::BackgroundHoverOverlayDown);
          newButton->SetBackground(m_containedButtonNormalSprite);
          newButton->SetBackgroundHover(m_containedButtonHoverSprite);
          newButton->SetBackgroundHoverOverlay(m_outlineButtonHoverSprite);
          newButton->SetFontColorDisabled(ThemeColor::Palette::FontOnPrimaryDisabled);
          break;
        default:
          FSLLOG3_WARNING("Unknown button type {}, using default", static_cast<int>(buttonType));
          newButton->SetBackground(m_outlineButtonNormalSprite);
          break;
        }
        newButton->FinishAnimation();
        return newButton;
      }


      std::shared_ptr<UI::LabelNineSliceButton> BasicThemeFactory::CreateTextButton(const ButtonType buttonType, const StringViewLite& strView)
      {
        auto newControl = CreateTextButton(buttonType);
        newControl->SetContent(strView);
        newControl->FinishAnimation();
        return newControl;
      }

      std::shared_ptr<UI::LabelNineSliceButton> BasicThemeFactory::CreateTextButton(const ButtonType buttonType, std::string&& str)
      {
        auto newControl = CreateTextButton(buttonType);
        newControl->SetContent(std::move(str));
        newControl->FinishAnimation();
        return newControl;
      }

      // --- CheckBox

      std::shared_ptr<UI::CheckBox> BasicThemeFactory::CreateCheckBox(const bool checked)
      {
        auto newControl = std::make_shared<UI::CheckBox>(m_context);
        newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
        newControl->SetCursorCheckedPosition(LocalConfig::CheckBoxCursorChecked);
        newControl->SetCursorUncheckedPosition(LocalConfig::CheckBoxCursorUnchecked);
        newControl->SetHoverOverlaySprite(m_texCheckBoxHover);
        newControl->SetCheckedSprite(m_texCheckBoxChecked);
        newControl->SetUncheckedSprite(m_texCheckBoxUnchecked);
        newControl->SetImageAlignment(ItemAlignment::Far);
        newControl->SetIsChecked(checked);

        newControl->SetHoverOverlayCheckedColor(ThemeColor::CheckBox::HoverChecked);
        newControl->SetHoverOverlayUncheckedColor(ThemeColor::CheckBox::HoverUnchecked);
        newControl->SetCursorCheckedColor(ThemeColor::CheckBox::CursorChecked);
        newControl->SetCursorCheckedDisabledColor(ThemeColor::CheckBox::CursorCheckedDisabled);
        newControl->SetCursorUncheckedColor(ThemeColor::CheckBox::CursorUnchecked);
        newControl->SetCursorUncheckedDisabledColor(ThemeColor::CheckBox::CursorUncheckedDisabled);
        newControl->SetBackgroundCheckedColor(ThemeColor::CheckBox::BackgroundChecked);
        newControl->SetBackgroundCheckedDisabledColor(ThemeColor::CheckBox::BackgroundCheckedDisabled);
        newControl->SetBackgroundUncheckedColor(ThemeColor::CheckBox::BackgroundUnchecked);
        newControl->SetBackgroundUncheckedDisabledColor(ThemeColor::CheckBox::BackgroundUncheckedDisabled);
        newControl->SetFontCheckedkColor(ThemeColor::CheckBox::CheckedFont);
        newControl->SetFontUncheckColor(ThemeColor::CheckBox::UncheckedFont);
        newControl->SetFontDisabledColor(ThemeColor::Palette::FontOnPrimaryDisabled);

        newControl->FinishAnimation();
        return newControl;
      }


      std::shared_ptr<UI::CheckBox> BasicThemeFactory::CreateCheckBox(const StringViewLite& strView, const bool checked)
      {
        auto newControl = CreateCheckBox(checked);
        newControl->SetText(strView);
        newControl->FinishAnimation();
        return newControl;
      }

      std::shared_ptr<UI::CheckBox> BasicThemeFactory::CreateCheckBox(std::string&& str, const bool checked)
      {
        auto newControl = CreateCheckBox(checked);
        newControl->SetText(std::move(str));
        newControl->FinishAnimation();
        return newControl;
      }

      // --- RadioButton

      std::shared_ptr<RadioGroup> BasicThemeFactory::CreateRadioGroup(const StringViewLite& name)
      {
        return std::make_shared<RadioGroup>(name);
      }


      std::shared_ptr<UI::RadioButton> BasicThemeFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const bool checked)
      {
        auto newControl = std::make_shared<UI::RadioButton>(m_context);
        radioGroup->Add(newControl);
        newControl->SetRadioGroup(radioGroup);
        newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
        newControl->SetCursorCheckedPosition(LocalConfig::RadioButtonCursorChecked);
        newControl->SetCursorUncheckedPosition(LocalConfig::RadioButtonCursorUnchecked);
        newControl->SetHoverOverlaySprite(m_texRadioButtonHover);
        newControl->SetCheckedSprite(m_texRadioButtonChecked);
        newControl->SetUncheckedSprite(m_texRadioButtonUnchecked);
        newControl->SetImageAlignment(ItemAlignment::Far);
        newControl->SetIsChecked(checked);

        newControl->SetHoverOverlayCheckedColor(ThemeColor::RadioButton::HoverChecked);
        newControl->SetHoverOverlayUncheckedColor(ThemeColor::RadioButton::HoverUnchecked);
        newControl->SetCursorCheckedColor(ThemeColor::RadioButton::CursorChecked);
        newControl->SetCursorCheckedDisabledColor(ThemeColor::RadioButton::CursorCheckedDisabled);
        newControl->SetCursorUncheckedColor(ThemeColor::RadioButton::CursorUnchecked);
        newControl->SetCursorUncheckedDisabledColor(ThemeColor::RadioButton::CursorUncheckedDisabled);
        newControl->SetBackgroundCheckedColor(ThemeColor::RadioButton::BackgroundChecked);
        newControl->SetBackgroundCheckedDisabledColor(ThemeColor::RadioButton::BackgroundCheckedDisabled);
        newControl->SetBackgroundUncheckedColor(ThemeColor::RadioButton::BackgroundUnchecked);
        newControl->SetBackgroundUncheckedDisabledColor(ThemeColor::RadioButton::BackgroundUncheckedDisabled);
        newControl->SetFontCheckedkColor(ThemeColor::RadioButton::CheckedFont);
        newControl->SetFontUncheckColor(ThemeColor::RadioButton::UncheckedFont);
        newControl->SetFontDisabledColor(ThemeColor::Palette::FontOnPrimaryDisabled);


        newControl->FinishAnimation();
        return newControl;
      }


      std::shared_ptr<UI::RadioButton> BasicThemeFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup,
                                                                            const StringViewLite& strView, const bool checked)
      {
        auto newControl = CreateRadioButton(radioGroup, checked);
        newControl->SetText(strView);
        newControl->FinishAnimation();
        return newControl;
      }

      std::shared_ptr<UI::RadioButton> BasicThemeFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, std::string&& str,
                                                                            const bool checked)
      {
        auto newControl = CreateRadioButton(radioGroup, checked);
        newControl->SetText(std::move(str));
        newControl->FinishAnimation();
        return newControl;
      }

      // --- Switch

      std::shared_ptr<UI::Switch> BasicThemeFactory::CreateSwitch(const bool checked)
      {
        auto newControl = std::make_shared<UI::Switch>(m_context);
        newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
        newControl->SetCursorCheckedPosition(LocalConfig::SwitchCursorChecked);
        newControl->SetCursorUncheckedPosition(LocalConfig::SwitchCursorUnchecked);
        newControl->SetHoverOverlaySprite(m_texSwitchHover);
        newControl->SetCheckedSprite(m_texSwitchChecked);
        newControl->SetUncheckedSprite(m_texSwitchUnchecked);
        newControl->SetImageAlignment(ItemAlignment::Far);
        newControl->SetIsChecked(checked);

        newControl->SetHoverOverlayCheckedColor(ThemeColor::Switch::HoverChecked);
        newControl->SetHoverOverlayUncheckedColor(ThemeColor::Switch::HoverUnchecked);
        newControl->SetCursorCheckedColor(ThemeColor::Switch::CursorChecked);
        newControl->SetCursorCheckedDisabledColor(ThemeColor::Switch::CursorCheckedDisabled);
        newControl->SetCursorUncheckedColor(ThemeColor::Switch::CursorUnchecked);
        newControl->SetCursorUncheckedDisabledColor(ThemeColor::Switch::CursorUncheckedDisabled);
        newControl->SetBackgroundCheckedColor(ThemeColor::Switch::BackgroundChecked);
        newControl->SetBackgroundCheckedDisabledColor(ThemeColor::Switch::BackgroundCheckedDisabled);
        newControl->SetBackgroundUncheckedColor(ThemeColor::Switch::BackgroundUnchecked);
        newControl->SetBackgroundUncheckedDisabledColor(ThemeColor::Switch::BackgroundUncheckedDisabled);
        newControl->SetFontCheckedkColor(ThemeColor::Switch::CheckedFont);
        newControl->SetFontUncheckColor(ThemeColor::Switch::UncheckedFont);
        newControl->SetFontDisabledColor(ThemeColor::Palette::FontOnPrimaryDisabled);

        newControl->FinishAnimation();
        return newControl;
      }


      std::shared_ptr<UI::Switch> BasicThemeFactory::CreateSwitch(const StringViewLite& strView, const bool checked)
      {
        auto newControl = CreateSwitch(checked);
        newControl->SetText(strView);
        newControl->FinishAnimation();
        return newControl;
      }

      std::shared_ptr<UI::Switch> BasicThemeFactory::CreateSwitch(std::string&& str, const bool checked)
      {
        auto newControl = CreateSwitch(checked);
        newControl->SetText(std::move(str));
        newControl->FinishAnimation();
        return newControl;
      }

      std::shared_ptr<BasicImage> BasicThemeFactory::CreateImage(const std::shared_ptr<BasicImageSprite>& spriteImage)
      {
        auto imageControl = std::make_shared<BasicImage>(m_context);
        imageControl->SetContent(spriteImage);
        imageControl->FinishAnimation();
        return imageControl;
      }


      std::shared_ptr<BasicNineSliceImage> BasicThemeFactory::CreateImage(const std::shared_ptr<BasicNineSliceSprite>& spriteImage)
      {
        auto imageControl = std::make_shared<BasicNineSliceImage>(m_context);
        imageControl->SetContent(spriteImage);
        imageControl->FinishAnimation();
        return imageControl;
      }


      std::shared_ptr<Image> BasicThemeFactory::CreateImage(const std::shared_ptr<ImageSprite>& spriteImage)
      {
        auto imageControl = std::make_shared<Image>(m_context);
        imageControl->SetContent(spriteImage);
        imageControl->FinishAnimation();
        return imageControl;
      }


      std::shared_ptr<NineSliceImage> BasicThemeFactory::CreateImage(const std::shared_ptr<NineSliceSprite>& spriteImage)
      {
        auto imageControl = std::make_shared<NineSliceImage>(m_context);
        imageControl->SetContent(spriteImage);
        imageControl->FinishAnimation();
        return imageControl;
      }

      // ----- CreateLeftBar

      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateLeftBar(const BarType barType)
      {
        auto background = std::make_shared<UI::BackgroundNineSlice>(m_context);
        background->SetBackground(m_leftBarNineSliceSprite);
        background->SetAlignmentY(UI::ItemAlignment::Stretch);
        background->SetBackgroundColor(GetBarColor(barType));
        background->SetPadding(DpThickness(20, 20, 20, 20));
        background->FinishAnimation();
        return background;
      }


      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateLeftBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
      {
        auto background = CreateLeftBar(barType);
        background->SetContent(content);
        background->FinishAnimation();
        return background;
      }


      // ----- CreateTopBar

      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateTopBar(const BarType barType)
      {
        auto background = std::make_shared<UI::BackgroundNineSlice>(m_context);
        background->SetBackground(m_topBarNineSliceSprite);
        background->SetAlignmentX(UI::ItemAlignment::Stretch);
        background->SetAlignmentY(UI::ItemAlignment::Near);
        background->SetBackgroundColor(GetBarColor(barType));
        background->SetPadding(DpThickness(20, 20, 20, 20));
        background->FinishAnimation();
        return background;
      }


      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateTopBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
      {
        auto background = CreateTopBar(barType);
        background->SetContent(content);
        background->FinishAnimation();
        return background;
      }

      // ----- CreateRigthBar

      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateRightBar(const BarType barType)
      {
        auto background = std::make_shared<UI::BackgroundNineSlice>(m_context);
        background->SetBackground(m_rightBarNineSliceSprite);
        background->SetAlignmentX(UI::ItemAlignment::Far);
        background->SetAlignmentY(UI::ItemAlignment::Stretch);
        background->SetBackgroundColor(GetBarColor(barType));
        background->SetPadding(DpThickness(20, 20, 20, 20));
        background->FinishAnimation();
        return background;
      }


      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateRightBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
      {
        auto background = CreateRightBar(barType);
        background->SetContent(content);
        background->FinishAnimation();
        return background;
      }

      // ----- CreateBottomBar


      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateBottomBar(const BarType barType)
      {
        auto background = std::make_shared<UI::BackgroundNineSlice>(m_context);
        background->SetBackground(m_bottomBarNineSliceSprite);
        background->SetAlignmentX(UI::ItemAlignment::Stretch);
        background->SetAlignmentY(UI::ItemAlignment::Far);
        background->SetBackgroundColor(GetBarColor(barType));
        background->SetPadding(DpThickness(20, 20, 20, 20));
        background->FinishAnimation();
        return background;
      }


      std::shared_ptr<UI::BackgroundNineSlice> BasicThemeFactory::CreateBottomBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
      {
        auto background = CreateBottomBar(barType);
        background->SetContent(content);
        background->FinishAnimation();
        return background;
      }
    }
  }
}
