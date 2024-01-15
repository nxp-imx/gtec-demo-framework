/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/TypeConverter.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Logic/RadioGroup.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeResources.hpp>

namespace Fsl::UI::Theme
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const DpValueF PaddingLowDp(2);
      constexpr const DpValueF PaddingHighDp(10);
      constexpr const DpValueF PaddingWinDp(4);

      constexpr const DpThicknessF PaddingLR(PaddingHighDp, PaddingLowDp, PaddingHighDp, PaddingLowDp);
      constexpr const DpThicknessF PaddingTB(PaddingLowDp, PaddingHighDp, PaddingLowDp, PaddingHighDp);
      constexpr const DpThicknessF PaddingWindowDp(PaddingWinDp, PaddingWinDp, PaddingWinDp, PaddingWinDp);

      constexpr const auto CheckBoxCursorChecked = DpPoint2::Create(24, 24);
      constexpr const auto CheckBoxCursorUnchecked = DpPoint2::Create(24, 24);

      constexpr const auto RadioButtonCursorChecked = DpPoint2::Create(24, 24);
      constexpr const auto RadioButtonCursorUnchecked = DpPoint2::Create(24, 24);

      constexpr const auto SwitchCursorChecked = DpPoint2::Create(32, 24);
      constexpr const auto SwitchCursorUnchecked = DpPoint2::Create(16, 24);

      constexpr const auto DividerMargin = DpThicknessF::Create(0.0f, 4.0f, 0.0f, 4.0f);

      // 160dpi 32, 24 | 16, 24
      //  96dpi 19, 14 | 10, 14
      //  80dpi 16, 12 |  8, 12
    }

    void UpdateLabelColor(LabelBase& rLabel, const BasicThemeColors::DefaultFontRecord& fontColors)
    {
      rLabel.SetFontColor(fontColors.Primary);
      rLabel.SetFontDisabledColor(fontColors.PrimaryDisabled);
    }


    template <typename TValue>
    std::shared_ptr<FmtValueLabel<TValue>> DoCreateFmtValueLabel(const std::shared_ptr<WindowContext>& context, const TValue value,
                                                                 const BasicThemeColors::DefaultFontRecord& fontColors)
    {
      auto label = std::make_shared<FmtValueLabel<TValue>>(context);

      UpdateLabelColor(*label, fontColors);
      label->SetContent(value);
      label->FinishAnimation();
      return label;
    }

    //! @brief Create a slider.
    //! @note  It will default to stretch in the layout direction.
    template <typename TValue>
    std::shared_ptr<Slider<TValue>> DoCreateSlider(const std::shared_ptr<WindowContext>& context, const BasicThemeResources& resources,
                                                   const BasicThemeColors::SliderRecord& themeColors, const LayoutOrientation orientation,
                                                   const ConstrainedValue<TValue>& value, const SliderConfig& config)
    {
      auto newControl = std::make_shared<Slider<TValue>>(context);
      newControl->SetOrientation(orientation);
      newControl->SetRange(value.Min(), value.Max());
      newControl->SetValue(value.Get());
      newControl->SetBackgroundSprite(resources.SliderBGSprite);
      newControl->SetCursorSprite(resources.SliderCursorSprite);
      newControl->SetCursorOrigin(resources.SliderCursorOriginDp);
      newControl->SetCursorSize(resources.SliderCursorSizeDp);
      newControl->SetCursorOverlaySprite(resources.SliderCursorHoverSprite);
      newControl->SetBackgroundColor(themeColors.Background);
      newControl->SetBackgroundDisabledColor(themeColors.BackgroundDisabled);
      newControl->SetCursorColor(themeColors.Cursor);
      newControl->SetCursorDisabledColor(themeColors.CursorDisabled);
      newControl->SetCursorOverlayColor(themeColors.CursorOverlay);
      newControl->SetDirection(config.Direction);
      switch (orientation)
      {
      case LayoutOrientation::Horizontal:
        newControl->SetAlignmentX(ItemAlignment::Stretch);
        break;
      case LayoutOrientation::Vertical:
        newControl->SetAlignmentY(ItemAlignment::Stretch);
        newControl->SetEnableVerticalGraphicsRotation(true);
        break;
      default:
        FSLLOG3_INFO("Unsupported orientation: {}", int32_t(orientation));
        break;
      }

      switch (config.Direction)
      {
      case LayoutDirection::NearToFar:
        break;
      case LayoutDirection::FarToNear:
        break;
      default:
        FSLLOG3_INFO("Unsupported direction: {}", int32_t(config.Direction));
        break;
      }

      newControl->FinishAnimation();
      return newControl;
    }


    //! @brief Create a slider.
    //! @note  It will default to stretch in the layout direction.
    template <typename TValue>
    std::shared_ptr<SliderAndFmtValueLabel<TValue>>
      DoCreateSliderFmtValue(const std::shared_ptr<WindowContext>& context, const BasicThemeResources& resources, const Color fontColor,
                             const Color fontDisabledColor, const BasicThemeColors::SliderRecord& themeColors, const LayoutOrientation orientation,
                             const ConstrainedValue<TValue>& value, const SliderConfig& config)
    {
      auto newControl = std::make_shared<SliderAndFmtValueLabel<TValue>>(context);
      newControl->SetOrientation(orientation);
      newControl->SetRange(value.Min(), value.Max());
      newControl->SetValue(value.Get());
      newControl->SetBackgroundSprite(resources.SliderBGSprite);
      newControl->SetCursorSprite(resources.SliderCursorSprite);
      newControl->SetCursorOrigin(resources.SliderCursorOriginDp);
      newControl->SetCursorSize(resources.SliderCursorSizeDp);
      newControl->SetCursorOverlaySprite(resources.SliderCursorHoverSprite);
      newControl->SetBackgroundColor(themeColors.Background);
      newControl->SetBackgroundDisabledColor(themeColors.BackgroundDisabled);
      newControl->SetCursorColor(themeColors.Cursor);
      newControl->SetCursorDisabledColor(themeColors.CursorDisabled);
      newControl->SetCursorOverlayColor(themeColors.CursorOverlay);
      newControl->SetFontColor(fontColor);
      newControl->SetFontDisabledColor(fontDisabledColor);
      newControl->SetDirection(config.Direction);
      switch (orientation)
      {
      case LayoutOrientation::Horizontal:
        newControl->SetAlignmentX(ItemAlignment::Stretch);
        break;
      case LayoutOrientation::Vertical:
        newControl->SetAlignmentY(ItemAlignment::Stretch);
        newControl->SetEnableVerticalGraphicsRotation(true);
        break;
      default:
        FSLLOG3_INFO("Unsupported orientation: {}", int32_t(orientation));
        break;
      }

      switch (config.Direction)
      {
      case LayoutDirection::NearToFar:
        break;
      case LayoutDirection::FarToNear:
        break;
      default:
        FSLLOG3_INFO("Unsupported direction: {}", int32_t(config.Direction));
        break;
      }

      newControl->FinishAnimation();
      return newControl;
    }


    BasicThemeColors CreateThemeColors(const bool usePrimaryPalette, const ColorSpace colorSpace)
    {
      BasicThemeColors defaultColors(usePrimaryPalette);
      return BasicThemeColors::ApplyColorSpaceLossy(defaultColors, colorSpace);
    }
  }

  BasicThemeControlFactory::BasicThemeControlFactory(const std::shared_ptr<WindowContext>& context,
                                                     const std::shared_ptr<BasicThemeResources>& themeResources, const bool usePrimaryPalette,
                                                     const ColorSpace colorSpace)
    : m_colors(CreateThemeColors(usePrimaryPalette, colorSpace))
    , m_context(context)
    , m_resources(themeResources)
  {
    if (!context)
    {
      throw std::invalid_argument("context can not be null");
    }
    if (!themeResources)
    {
      throw std::invalid_argument("themeResources can not be null");
    }
  }


  BasicThemeControlFactory::~BasicThemeControlFactory() = default;


  const IThemeResources& BasicThemeControlFactory::GetResources() const
  {
    return *m_resources;
  }


  Color BasicThemeControlFactory::GetThemePrimaryDarkColor() const
  {
    return m_colors.Palette.PrimaryDark;
  }


  DpThicknessF BasicThemeControlFactory::GetDefaultMarginDp(const ElementType elementType) const
  {
    switch (elementType)
    {
    case ElementType::LeftBar:
    case ElementType::RightBar:
      return LocalConfig::PaddingLR;
    case ElementType::TopBar:
    case ElementType::BottomBar:
      return LocalConfig::PaddingTB;
    default:
      return {};
    }
  }


  std::shared_ptr<Image> BasicThemeControlFactory::CreateDivider(const LayoutOrientation orientation)
  {
    auto imageControl = std::make_shared<Image>(m_context);
    imageControl->SetMargin(LocalConfig::DividerMargin);
    imageControl->SetContent(m_resources->DividerNineSliceSprite);
    imageControl->SetContentColor(m_colors.Divider.Primary);
    if (orientation == LayoutOrientation::Horizontal)
    {
      imageControl->SetAlignmentX(ItemAlignment::Stretch);
      imageControl->SetAlignmentY(ItemAlignment::Center);
      imageControl->SetScalePolicy(ItemScalePolicy::Fit);
    }
    else
    {
      imageControl->SetAlignmentX(ItemAlignment::Center);
      imageControl->SetAlignmentY(ItemAlignment::Stretch);
      imageControl->SetScalePolicy(ItemScalePolicy::Fit);
      imageControl->SetRotateImageCW(true);
    }
    imageControl->FinishAnimation();
    return imageControl;
  }


  std::shared_ptr<UI::Label> BasicThemeControlFactory::CreateLabel(const StringViewLite& strView, const FontType fontType)
  {
    auto label = std::make_shared<UI::Label>(m_context);
    UpdateLabelColor(*label, m_colors.DefaultFont);
    label->SetFont(fontType == FontType::Default ? m_resources->DefaultFont : m_resources->DefaultHeaderFont);
    label->SetContent(strView);
    label->FinishAnimation();
    return label;
  }


  std::shared_ptr<UI::Label> BasicThemeControlFactory::CreateLabel(std::string&& str, const FontType fontType)
  {
    auto label = std::make_shared<UI::Label>(m_context);
    UpdateLabelColor(*label, m_colors.DefaultFont);
    label->SetFont(fontType == FontType::Default ? m_resources->DefaultFont : m_resources->DefaultHeaderFont);
    label->SetContent(std::move(str));
    label->FinishAnimation();
    return label;
  }

  // ----- CreateImageButton

  std::shared_ptr<ImageButton> BasicThemeControlFactory::CreateImageButton(const ImageButtonType buttonType, std::shared_ptr<ImageSprite> sprite)
  {
    if (!sprite)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    auto newControl = std::make_shared<ImageButton>(m_context);
    newControl->SetContent(std::move(sprite));
    newControl->SetColorUp(m_colors.ImageButton.ImageUp);
    newControl->SetColorDown(m_colors.ImageButton.ImageDown);
    newControl->SetColorDisabled(m_colors.ImageButton.ImageDisabled);
    newControl->SetScalePolicy(ItemScalePolicy::NoScaling);
    newControl->SetBackgroundColorHoverUp(m_colors.ImageButton.BackgroundHoverUp);
    newControl->SetBackgroundColorUp(m_colors.ImageButton.BackgroundUp);
    newControl->SetBackgroundColorDown(m_colors.ImageButton.BackgroundDown);
    newControl->SetBackgroundColorDisabled(m_colors.ImageButton.BackgroundDisabled);

    switch (buttonType)
    {
    case ImageButtonType::Normal:
      newControl->SetBackground(m_resources->FloatingButtonPressedSprite);
      newControl->SetBackgroundHover(m_resources->FloatingButtonNormalSprite);
      break;
    case ImageButtonType::Small:
      newControl->SetBackground(m_resources->FloatingSmallButtonPressedSprite);
      newControl->SetBackgroundHover(m_resources->FloatingSmallButtonNormalSprite);
      break;
    case ImageButtonType::FloatingNormal:
      newControl->SetBackground(m_resources->FloatingButtonNormalSprite);
      newControl->SetBackgroundHover(m_resources->FloatingButtonHoverSprite);
      break;
    case ImageButtonType::FloatingSmall:
      newControl->SetBackground(m_resources->FloatingSmallButtonNormalSprite);
      newControl->SetBackgroundHover(m_resources->FloatingSmallButtonHoverSprite);
      break;
    default:
      throw InternalErrorException("Unsupported button type");
    }

    newControl->FinishAnimation();
    return newControl;
  }

  // ----- CreateImageButton

  std::shared_ptr<SimpleImageButton> BasicThemeControlFactory::CreateImageButton(std::shared_ptr<ImageSprite> sprite)
  {
    if (!sprite)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    auto newControl = std::make_shared<SimpleImageButton>(m_context);
    newControl->SetContent(std::move(sprite));
    newControl->SetColorUp(m_colors.ImageButton.ImageUp);
    newControl->SetColorDown(m_colors.ImageButton.ImageDown);
    newControl->SetColorDisabled(m_colors.ImageButton.ImageDisabled);
    newControl->SetScalePolicy(ItemScalePolicy::NoScaling);
    newControl->FinishAnimation();
    return newControl;
  }

  // ----- CreateTextButton

  std::shared_ptr<UI::BackgroundLabelButton> BasicThemeControlFactory::CreateTextButton(const ButtonType buttonType)
  {
    auto newButton = std::make_shared<UI::BackgroundLabelButton>(m_context);
    newButton->SetContentAlignmentX(UI::ItemAlignment::Center);
    newButton->SetContentAlignmentY(UI::ItemAlignment::Center);
    newButton->SetFontColorUp(m_colors.Button.FontUp);
    newButton->SetFontColorDown(m_colors.Button.FontDown);
    newButton->SetFontColorDisabled(m_colors.Button.FontDisabled);
    // newButton->SetMinWidth(64);
    // newButton->SetMinHeight(36);
    switch (buttonType)
    {
    case ButtonType::Text:
      // No background
      newButton->SetBackgroundHoverOverlay(m_resources->OutlineButtonHoverSprite);
      newButton->SetBackgroundHoverOverlayColorUp(m_colors.Button.BackgroundHoverOverlayUp);
      newButton->SetBackgroundHoverOverlayColorDown(m_colors.Button.BackgroundHoverOverlayDown);
      newButton->SetFontColorDown(m_colors.Button.OutlineFontDown);
      newButton->SetFontColorUp(m_colors.Button.OutlineFontUp);
      newButton->SetFontColorDisabled(m_colors.Button.OutlineFontDisabled);
      break;
    case ButtonType::Outlined:
      newButton->SetBackground(m_resources->OutlineButtonNormalSprite);
      newButton->SetBackgroundHover(m_resources->OutlineButtonNormalSprite);
      newButton->SetBackgroundHoverOverlay(m_resources->OutlineButtonHoverSprite);
      newButton->SetBackgroundColorHoverUp(m_colors.Button.BackgroundHoverUp);
      newButton->SetBackgroundColorUp(m_colors.Button.BackgroundUp);
      newButton->SetBackgroundColorDown(m_colors.Button.BackgroundDown);
      newButton->SetBackgroundColorDisabled(m_colors.Button.BackgroundDisabled);
      newButton->SetBackgroundHoverOverlayColorUp(m_colors.Button.BackgroundHoverOverlayUp);
      newButton->SetBackgroundHoverOverlayColorDown(m_colors.Button.BackgroundHoverOverlayDown);
      newButton->SetFontColorDown(m_colors.Button.OutlineFontDown);
      newButton->SetFontColorUp(m_colors.Button.OutlineFontUp);
      newButton->SetFontColorDisabled(m_colors.Button.OutlineFontDisabled);
      break;
    case ButtonType::Contained:
      newButton->SetBackgroundColorHoverUp(m_colors.Button.BackgroundHoverUp);
      newButton->SetBackgroundColorUp(m_colors.Button.BackgroundUp);
      newButton->SetBackgroundColorDown(m_colors.Button.BackgroundDown);
      newButton->SetBackgroundColorDisabled(m_colors.Button.BackgroundDisabled);
      newButton->SetBackgroundHoverOverlayColorUp(m_colors.Button.BackgroundHoverOverlayUp);
      newButton->SetBackgroundHoverOverlayColorDown(m_colors.Button.BackgroundHoverOverlayDown);
      newButton->SetBackground(m_resources->ContainedButtonNormalSprite);
      newButton->SetBackgroundHover(m_resources->ContainedButtonHoverSprite);
      newButton->SetBackgroundHoverOverlay(m_resources->OutlineButtonHoverSprite);
      break;
    default:
      FSLLOG3_WARNING("Unknown button type {}, using default", static_cast<int>(buttonType));
      newButton->SetBackground(m_resources->OutlineButtonNormalSprite);
      break;
    }
    newButton->FinishAnimation();
    return newButton;
  }


  std::shared_ptr<UI::BackgroundLabelButton> BasicThemeControlFactory::CreateTextButton(const ButtonType buttonType, const StringViewLite& strView)
  {
    auto newControl = CreateTextButton(buttonType);
    newControl->SetContent(strView);
    newControl->FinishAnimation();
    return newControl;
  }

  std::shared_ptr<UI::BackgroundLabelButton> BasicThemeControlFactory::CreateTextButton(const ButtonType buttonType, std::string&& str)
  {
    auto newControl = CreateTextButton(buttonType);
    newControl->SetContent(std::move(str));
    newControl->FinishAnimation();
    return newControl;
  }

  // --- CheckBox

  std::shared_ptr<UI::CheckBox> BasicThemeControlFactory::CreateCheckBox(const bool checked)
  {
    auto newControl = std::make_shared<UI::CheckBox>(m_context);
    newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
    newControl->SetCursorCheckedPosition(LocalConfig::CheckBoxCursorChecked);
    newControl->SetCursorUncheckedPosition(LocalConfig::CheckBoxCursorUnchecked);
    newControl->SetHoverOverlaySprite(m_resources->TexCheckBoxHover);
    newControl->SetCheckedSprite(m_resources->TexCheckBoxChecked);
    newControl->SetUncheckedSprite(m_resources->TexCheckBoxUnchecked);
    newControl->SetImageAlignment(ItemAlignment::Far);
    newControl->SetIsChecked(checked);

    newControl->SetHoverOverlayCheckedColor(m_colors.CheckBox.HoverChecked);
    newControl->SetHoverOverlayUncheckedColor(m_colors.CheckBox.HoverUnchecked);
    newControl->SetCursorColorChecked(m_colors.CheckBox.CursorChecked);
    newControl->SetCursorColorCheckedDisabled(m_colors.CheckBox.CursorCheckedDisabled);
    newControl->SetCursorColorUnchecked(m_colors.CheckBox.CursorUnchecked);
    newControl->SetCursorColorUncheckedDisabled(m_colors.CheckBox.CursorUncheckedDisabled);
    newControl->SetBackgroundColorChecked(m_colors.CheckBox.BackgroundChecked);
    newControl->SetBackgroundColorCheckedDisabled(m_colors.CheckBox.BackgroundCheckedDisabled);
    newControl->SetBackgroundColorUnchecked(m_colors.CheckBox.BackgroundUnchecked);
    newControl->SetBackgroundColorUncheckedDisabled(m_colors.CheckBox.BackgroundUncheckedDisabled);
    newControl->SetFontColorChecked(m_colors.CheckBox.CheckedFont);
    newControl->SetFontColorUnchecked(m_colors.CheckBox.UncheckedFont);
    newControl->SetFontColorDisabled(m_colors.DefaultFont.PrimaryDisabled);

    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<UI::CheckBox> BasicThemeControlFactory::CreateCheckBox(const StringViewLite& strView, const bool checked)
  {
    auto newControl = CreateCheckBox(checked);
    newControl->SetText(strView);
    newControl->FinishAnimation();
    return newControl;
  }

  std::shared_ptr<UI::CheckBox> BasicThemeControlFactory::CreateCheckBox(std::string&& str, const bool checked)
  {
    auto newControl = CreateCheckBox(checked);
    newControl->SetText(std::move(str));
    newControl->FinishAnimation();
    return newControl;
  }

  // --- RadioButton

  std::shared_ptr<RadioGroup> BasicThemeControlFactory::CreateRadioGroup(const StringViewLite& name)
  {
    return std::make_shared<RadioGroup>(name);
  }


  std::shared_ptr<UI::RadioButton> BasicThemeControlFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const bool checked)
  {
    auto newControl = std::make_shared<UI::RadioButton>(m_context);
    radioGroup->Add(newControl);
    newControl->SetRadioGroup(radioGroup);
    newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
    newControl->SetCursorCheckedPosition(LocalConfig::RadioButtonCursorChecked);
    newControl->SetCursorUncheckedPosition(LocalConfig::RadioButtonCursorUnchecked);
    newControl->SetHoverOverlaySprite(m_resources->RadioButtonHoverSprite);
    newControl->SetCheckedSprite(m_resources->RadioButtonCheckedSprite);
    newControl->SetUncheckedSprite(m_resources->RadioButtonUncheckedSprite);
    newControl->SetImageAlignment(ItemAlignment::Far);
    newControl->SetIsChecked(checked);

    newControl->SetHoverOverlayCheckedColor(m_colors.RadioButton.HoverChecked);
    newControl->SetHoverOverlayUncheckedColor(m_colors.RadioButton.HoverUnchecked);
    newControl->SetCursorColorChecked(m_colors.RadioButton.CursorChecked);
    newControl->SetCursorColorCheckedDisabled(m_colors.RadioButton.CursorCheckedDisabled);
    newControl->SetCursorColorUnchecked(m_colors.RadioButton.CursorUnchecked);
    newControl->SetCursorColorUncheckedDisabled(m_colors.RadioButton.CursorUncheckedDisabled);
    newControl->SetBackgroundColorChecked(m_colors.RadioButton.BackgroundChecked);
    newControl->SetBackgroundColorCheckedDisabled(m_colors.RadioButton.BackgroundCheckedDisabled);
    newControl->SetBackgroundColorUnchecked(m_colors.RadioButton.BackgroundUnchecked);
    newControl->SetBackgroundColorUncheckedDisabled(m_colors.RadioButton.BackgroundUncheckedDisabled);
    newControl->SetFontColorChecked(m_colors.RadioButton.CheckedFont);
    newControl->SetFontColorUnchecked(m_colors.RadioButton.UncheckedFont);
    newControl->SetFontColorDisabled(m_colors.DefaultFont.PrimaryDisabled);


    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<UI::RadioButton> BasicThemeControlFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup,
                                                                               const StringViewLite& strView, const bool checked)
  {
    auto newControl = CreateRadioButton(radioGroup, checked);
    newControl->SetText(strView);
    newControl->FinishAnimation();
    return newControl;
  }

  std::shared_ptr<UI::RadioButton> BasicThemeControlFactory::CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, std::string&& str,
                                                                               const bool checked)
  {
    auto newControl = CreateRadioButton(radioGroup, checked);
    newControl->SetText(std::move(str));
    newControl->FinishAnimation();
    return newControl;
  }

  // --- Switch

  std::shared_ptr<UI::Switch> BasicThemeControlFactory::CreateSwitch(const bool checked)
  {
    auto newControl = std::make_shared<UI::Switch>(m_context);
    newControl->SetAlignmentX(UI::ItemAlignment::Stretch);
    newControl->SetCursorCheckedPosition(LocalConfig::SwitchCursorChecked);
    newControl->SetCursorUncheckedPosition(LocalConfig::SwitchCursorUnchecked);
    newControl->SetHoverOverlaySprite(m_resources->SwitchHoverSprite);
    newControl->SetCheckedSprite(m_resources->SwitchCheckedSprite);
    newControl->SetUncheckedSprite(m_resources->SwitchUncheckedSprite);
    newControl->SetImageAlignment(ItemAlignment::Far);
    newControl->SetIsChecked(checked);

    newControl->SetHoverOverlayCheckedColor(m_colors.Switch.HoverChecked);
    newControl->SetHoverOverlayUncheckedColor(m_colors.Switch.HoverUnchecked);
    newControl->SetCursorColorChecked(m_colors.Switch.CursorChecked);
    newControl->SetCursorColorCheckedDisabled(m_colors.Switch.CursorCheckedDisabled);
    newControl->SetCursorColorUnchecked(m_colors.Switch.CursorUnchecked);
    newControl->SetCursorColorUncheckedDisabled(m_colors.Switch.CursorUncheckedDisabled);
    newControl->SetBackgroundColorChecked(m_colors.Switch.BackgroundChecked);
    newControl->SetBackgroundColorCheckedDisabled(m_colors.Switch.BackgroundCheckedDisabled);
    newControl->SetBackgroundColorUnchecked(m_colors.Switch.BackgroundUnchecked);
    newControl->SetBackgroundColorUncheckedDisabled(m_colors.Switch.BackgroundUncheckedDisabled);
    newControl->SetFontColorChecked(m_colors.Switch.CheckedFont);
    newControl->SetFontColorUnchecked(m_colors.Switch.UncheckedFont);
    newControl->SetFontColorDisabled(m_colors.DefaultFont.PrimaryDisabled);

    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<UI::Switch> BasicThemeControlFactory::CreateSwitch(const StringViewLite& strView, const bool checked)
  {
    auto newControl = CreateSwitch(checked);
    newControl->SetText(strView);
    newControl->FinishAnimation();
    return newControl;
  }

  std::shared_ptr<UI::Switch> BasicThemeControlFactory::CreateSwitch(std::string&& str, const bool checked)
  {
    auto newControl = CreateSwitch(checked);
    newControl->SetText(std::move(str));
    newControl->FinishAnimation();
    return newControl;
  }

  std::shared_ptr<Image> BasicThemeControlFactory::CreateImage(const std::shared_ptr<BasicImageSprite>& spriteImage)
  {
    auto imageControl = std::make_shared<Image>(m_context);
    imageControl->SetContent(spriteImage);
    imageControl->FinishAnimation();
    return imageControl;
  }


  std::shared_ptr<Image> BasicThemeControlFactory::CreateImage(const std::shared_ptr<BasicNineSliceSprite>& spriteImage)
  {
    auto imageControl = std::make_shared<Image>(m_context);
    imageControl->SetScalePolicy(ItemScalePolicy::Fit);
    imageControl->SetContent(spriteImage);
    imageControl->FinishAnimation();
    return imageControl;
  }


  std::shared_ptr<Image> BasicThemeControlFactory::CreateImage(const std::shared_ptr<ImageSprite>& spriteImage)
  {
    auto imageControl = std::make_shared<Image>(m_context);
    imageControl->SetContent(spriteImage);
    imageControl->FinishAnimation();
    return imageControl;
  }


  std::shared_ptr<Image> BasicThemeControlFactory::CreateImage(const std::shared_ptr<INineSliceSprite>& spriteImage)
  {
    auto imageControl = std::make_shared<Image>(m_context);
    imageControl->SetScalePolicy(ItemScalePolicy::Fit);
    imageControl->SetContent(spriteImage);
    imageControl->FinishAnimation();
    return imageControl;
  }

  // ----- CreateLeftBar

  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateLeftBar(const BarType barType)
  {
    auto background = std::make_shared<UI::Background>(m_context);
    background->SetBackground(m_resources->LeftBarNineSliceSprite);
    background->SetAlignmentY(UI::ItemAlignment::Stretch);
    background->SetBackgroundColor(m_colors.GetBarColor(barType));
    background->SetPadding(LocalConfig::PaddingLR);
    background->FinishAnimation();
    return background;
  }


  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateLeftBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
  {
    auto background = CreateLeftBar(barType);
    background->SetContent(content);
    background->FinishAnimation();
    return background;
  }


  // ----- CreateTopBar

  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateTopBar(const BarType barType)
  {
    auto background = std::make_shared<UI::Background>(m_context);
    background->SetBackground(m_resources->TopBarNineSliceSprite);
    background->SetAlignmentX(UI::ItemAlignment::Stretch);
    background->SetAlignmentY(UI::ItemAlignment::Near);
    background->SetBackgroundColor(m_colors.GetBarColor(barType));
    background->SetPadding(LocalConfig::PaddingTB);
    background->FinishAnimation();
    return background;
  }


  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateTopBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
  {
    auto background = CreateTopBar(barType);
    background->SetContent(content);
    background->FinishAnimation();
    return background;
  }

  // ----- CreateRigthBar

  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateRightBar(const BarType barType)
  {
    auto background = std::make_shared<UI::Background>(m_context);
    background->SetBackground(m_resources->RightBarNineSliceSprite);
    background->SetAlignmentX(UI::ItemAlignment::Far);
    background->SetAlignmentY(UI::ItemAlignment::Stretch);
    background->SetBackgroundColor(m_colors.GetBarColor(barType));
    background->SetPadding(LocalConfig::PaddingLR);
    background->FinishAnimation();
    return background;
  }


  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateRightBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
  {
    auto background = CreateRightBar(barType);
    background->SetContent(content);
    background->FinishAnimation();
    return background;
  }

  // ----- CreateBottomBar


  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateBottomBar(const BarType barType)
  {
    auto background = std::make_shared<UI::Background>(m_context);
    background->SetBackground(m_resources->BottomBarNineSliceSprite);
    background->SetAlignmentX(UI::ItemAlignment::Stretch);
    background->SetAlignmentY(UI::ItemAlignment::Far);
    background->SetBackgroundColor(m_colors.GetBarColor(barType));
    background->SetPadding(LocalConfig::PaddingTB);
    background->FinishAnimation();
    return background;
  }


  std::shared_ptr<UI::Background> BasicThemeControlFactory::CreateBottomBar(const std::shared_ptr<BaseWindow>& content, const BarType barType)
  {
    auto background = CreateBottomBar(barType);
    background->SetContent(content);
    background->FinishAnimation();
    return background;
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateBackgroundWindow
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<Background> BasicThemeControlFactory::CreateBackgroundWindow(const WindowType windowType,
                                                                               const std::shared_ptr<BaseWindow>& content,
                                                                               const UI::ItemAlignment alignmentX, const UI::ItemAlignment alignmentY)
  {
    auto background = std::make_shared<UI::Background>(m_context);
    background->SetBackground(m_resources->GetBackground(windowType));
    background->SetBackgroundColor(m_colors.GetWindowColor(windowType));
    background->SetPadding(LocalConfig::PaddingWindowDp);
    background->SetContent(content);
    background->SetAlignmentX(alignmentX);
    background->SetAlignmentY(alignmentY);
    background->FinishAnimation();

    return background;
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateFloatingButton
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<ButtonBase> BasicThemeControlFactory::CreateFloatingButton(const FloatingButtonType type)
  {
    std::shared_ptr<ImageSprite> icon;
    switch (type)
    {
    case FloatingButtonType::Back:
      icon = m_resources->GetIconSprite(UI::Theme::IconType::NavigationArrowBack);
      break;
    default:
      throw NotSupportedException("Unsupported floating button type");
    }

    auto button = CreateImageButton(UI::Theme::ImageButtonType::FloatingNormal, icon);
    button->SetAlignmentY(UI::ItemAlignment::Far);
    button->FinishAnimation();
    return button;
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateFmtValueLabel<uint8_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<FmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint8_t value)
  {
    return DoCreateFmtValueLabel(m_context, value, m_colors.DefaultFont);
  }

  std::shared_ptr<FmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint8_t value, const StringViewLite& strViewFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strViewFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint8_t value, std::string&& strFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint8_t value, const char* const pszFormat)
  {
    return CreateFmtValueLabel(value, StringViewLite(pszFormat));
  }

  std::shared_ptr<FmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint8_t value, const std::string& strFormat)
  {
    return CreateFmtValueLabel(value, StringViewLiteUtil::AsStringViewLite(strFormat));
  }


  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateFmtValueLabel<int32_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<FmtValueLabel<int32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const int32_t value)
  {
    return DoCreateFmtValueLabel(m_context, value, m_colors.DefaultFont);
  }

  std::shared_ptr<FmtValueLabel<int32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const int32_t value, const StringViewLite& strViewFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strViewFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<int32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const int32_t value, std::string&& strFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<int32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const int32_t value, const char* const pszFormat)
  {
    return CreateFmtValueLabel(value, StringViewLite(pszFormat));
  }

  std::shared_ptr<FmtValueLabel<int32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const int32_t value, const std::string& strFormat)
  {
    return CreateFmtValueLabel(value, StringViewLiteUtil::AsStringViewLite(strFormat));
  }


  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateFmtValueLabel<uint32_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<FmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint32_t value)
  {
    return DoCreateFmtValueLabel(m_context, value, m_colors.DefaultFont);
  }

  std::shared_ptr<FmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint32_t value, const StringViewLite& strViewFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strViewFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint32_t value, std::string&& strFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint32_t value, const char* const pszFormat)
  {
    return CreateFmtValueLabel(value, StringViewLite(pszFormat));
  }

  std::shared_ptr<FmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateFmtValueLabel(const uint32_t value, const std::string& strFormat)
  {
    return CreateFmtValueLabel(value, StringViewLiteUtil::AsStringViewLite(strFormat));
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateFmtValueLabel<float>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<FmtValueLabel<float>> BasicThemeControlFactory::CreateFmtValueLabel(const float value)
  {
    return DoCreateFmtValueLabel(m_context, value, m_colors.DefaultFont);
  }

  std::shared_ptr<FmtValueLabel<float>> BasicThemeControlFactory::CreateFmtValueLabel(const float value, const StringViewLite& strViewFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strViewFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<float>> BasicThemeControlFactory::CreateFmtValueLabel(const float value, std::string&& strFormat)
  {
    auto label = CreateFmtValueLabel(value);
    label->SetFormatString(strFormat);
    label->FinishAnimation();
    return label;
  }

  std::shared_ptr<FmtValueLabel<float>> BasicThemeControlFactory::CreateFmtValueLabel(const float value, const char* const pszFormat)
  {
    return CreateFmtValueLabel(value, StringViewLite(pszFormat));
  }

  std::shared_ptr<FmtValueLabel<float>> BasicThemeControlFactory::CreateFmtValueLabel(const float value, const std::string& strFormat)
  {
    return CreateFmtValueLabel(value, StringViewLiteUtil::AsStringViewLite(strFormat));
  }

  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSlider<uint8_t>
  // ----------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<Slider<uint8_t>> BasicThemeControlFactory::CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<uint8_t>& value,
                                                                          const SliderConfig& config)
  {
    return DoCreateSlider(m_context, *m_resources, m_colors.Slider, orientation, value, config);
  }

  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSlider<int32_t>
  // ----------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<Slider<int32_t>> BasicThemeControlFactory::CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<int32_t>& value,
                                                                          const SliderConfig& config)
  {
    return DoCreateSlider(m_context, *m_resources, m_colors.Slider, orientation, value, config);
  }

  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSlider<uint32_t>
  // ----------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<Slider<uint32_t>> BasicThemeControlFactory::CreateSlider(const LayoutOrientation orientation,
                                                                           const ConstrainedValue<uint32_t>& value, const SliderConfig& config)
  {
    return DoCreateSlider(m_context, *m_resources, m_colors.Slider, orientation, value, config);
  }

  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSlider<float>
  // ----------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<Slider<float>> BasicThemeControlFactory::CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                        const SliderConfig& config)
  {
    return DoCreateSlider(m_context, *m_resources, m_colors.Slider, orientation, value, config);
  }


  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSliderFmtValue<uint8_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<uint8_t>& value,
                                                                                                  const SliderConfig& config)
  {
    const Color fontColor = m_colors.DefaultFont.Primary;
    const Color fontDisabledColor = m_colors.DefaultFont.PrimaryDisabled;
    return DoCreateSliderFmtValue(m_context, *m_resources, fontColor, fontDisabledColor, m_colors.Slider, orientation, value, config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<uint8_t>& value,
                                                                                                  const StringViewLite& strFormat,
                                                                                                  const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<uint8_t>& value,
                                                                                                  std::string&& strFormat, const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<uint8_t>& value,
                                                                                                  const char* const pszFormat,
                                                                                                  const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLite(pszFormat), config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<uint8_t>& value,
                                                                                                  const std::string& strFormat,
                                                                                                  const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLiteUtil::AsStringViewLite(strFormat), config);
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSliderFmtValue<int32_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<SliderAndFmtValueLabel<int32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<int32_t>& value,
                                                                                                  const SliderConfig& config)
  {
    const Color fontColor = m_colors.DefaultFont.Primary;
    const Color fontDisabledColor = m_colors.DefaultFont.PrimaryDisabled;
    return DoCreateSliderFmtValue(m_context, *m_resources, fontColor, fontDisabledColor, m_colors.Slider, orientation, value, config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<int32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<int32_t>& value,
                                                                                                  const StringViewLite& strFormat,
                                                                                                  const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<int32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<int32_t>& value,
                                                                                                  std::string&& strFormat, const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<int32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<int32_t>& value,
                                                                                                  const char* const pszFormat,
                                                                                                  const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLite(pszFormat), config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<int32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                  const ConstrainedValue<int32_t>& value,
                                                                                                  const std::string& strFormat,
                                                                                                  const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLiteUtil::AsStringViewLite(strFormat), config);
  }


  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSliderFmtValue<uint32_t>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                   const ConstrainedValue<uint32_t>& value,
                                                                                                   const SliderConfig& config)
  {
    const Color fontColor = m_colors.DefaultFont.Primary;
    const Color fontDisabledColor = m_colors.DefaultFont.PrimaryDisabled;
    return DoCreateSliderFmtValue(m_context, *m_resources, fontColor, fontDisabledColor, m_colors.Slider, orientation, value, config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                   const ConstrainedValue<uint32_t>& value,
                                                                                                   const StringViewLite& strFormat,
                                                                                                   const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                   const ConstrainedValue<uint32_t>& value,
                                                                                                   std::string&& strFormat,
                                                                                                   const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                   const ConstrainedValue<uint32_t>& value,
                                                                                                   const char* const pszFormat,
                                                                                                   const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLite(pszFormat), config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                   const ConstrainedValue<uint32_t>& value,
                                                                                                   const std::string& strFormat,
                                                                                                   const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLiteUtil::AsStringViewLite(strFormat), config);
  }

  // ---------------------------------------------------------------------------------------------------------------------------------------------
  // CreateSliderFmtValue<float>
  // ---------------------------------------------------------------------------------------------------------------------------------------------

  std::shared_ptr<SliderAndFmtValueLabel<float>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                const ConstrainedValue<float>& value,
                                                                                                const SliderConfig& config)
  {
    const Color fontColor = m_colors.DefaultFont.Primary;
    const Color fontDisabledColor = m_colors.DefaultFont.PrimaryDisabled;
    return DoCreateSliderFmtValue(m_context, *m_resources, fontColor, fontDisabledColor, m_colors.Slider, orientation, value, config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<float>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                const ConstrainedValue<float>& value,
                                                                                                const StringViewLite& strFormat,
                                                                                                const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<float>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                const ConstrainedValue<float>& value,
                                                                                                std::string&& strFormat, const SliderConfig& config)
  {
    auto newControl = CreateSliderFmtValue(orientation, value, config);
    newControl->SetFormatString(strFormat);
    newControl->FinishAnimation();
    return newControl;
  }


  std::shared_ptr<SliderAndFmtValueLabel<float>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                const ConstrainedValue<float>& value,
                                                                                                const char* const pszFormat,
                                                                                                const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLite(pszFormat), config);
  }


  std::shared_ptr<SliderAndFmtValueLabel<float>> BasicThemeControlFactory::CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                                const ConstrainedValue<float>& value,
                                                                                                const std::string& strFormat,
                                                                                                const SliderConfig& config)
  {
    return CreateSliderFmtValue(orientation, value, StringViewLiteUtil::AsStringViewLite(strFormat), config);
  }

}
