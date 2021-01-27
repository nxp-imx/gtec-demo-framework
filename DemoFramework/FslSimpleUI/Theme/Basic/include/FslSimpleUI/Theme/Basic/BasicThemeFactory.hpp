#ifndef FSLSIMPLEUI_THEME_BASIC_BASICTHEMEFACTORY_HPP
#define FSLSIMPLEUI_THEME_BASIC_BASICTHEMEFACTORY_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/DpSize.hpp>
#include <FslBase/Math/ConstrainedValue.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslGraphics/Render/NineSliceAtlasTexture2D.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class BasicImageSprite;
  class BasicNineSliceSprite;
  class ISpriteResourceManager;
  class ImageSprite;
  class NineSliceSprite;
  struct SpriteMaterialId;

  namespace UI
  {
    class BackgroundNineSlice;
    class BasicImage;
    class BasicNineSliceImage;
    class BackgroundImageButton;
    class CheckBox;
    class ImageButton;
    class RadioButton;
    class RadioGroup;
    class Switch;
    class Image;
    class Label;
    class LabelNineSliceButton;
    class NineSliceImage;
    class SimpleImageButton;
    class WindowContext;

    namespace Theme
    {
      enum class BarType
      {
        Normal,
        Transparent
      };

      enum class ButtonType
      {
        Text,
        Outlined,
        Contained
      };

      enum class ImageButtonType
      {
        Normal,
        Small,
        FloatingNormal,
        FloatingSmall,
      };

      struct SliderConfig
      {
        LayoutDirection Direction{LayoutDirection::NearToFar};

        constexpr SliderConfig() = default;

        constexpr explicit SliderConfig(const LayoutDirection direction)
          : Direction(direction)
        {
        }
      };


      class BasicThemeFactory
      {
        std::shared_ptr<WindowContext> m_context;

        std::shared_ptr<ImageSprite> m_fillSprite;
        std::shared_ptr<NineSliceSprite> m_dividerNineSliceSprite;

        std::shared_ptr<NineSliceSprite> m_leftBarNineSliceSprite;
        std::shared_ptr<NineSliceSprite> m_topBarNineSliceSprite;
        std::shared_ptr<NineSliceSprite> m_rightBarNineSliceSprite;
        std::shared_ptr<NineSliceSprite> m_bottomBarNineSliceSprite;

        // Dialog
        std::shared_ptr<NineSliceSprite> m_dialogNineSliceSprite;

        // button
        std::shared_ptr<NineSliceSprite> m_containedButtonNormalSprite;
        std::shared_ptr<NineSliceSprite> m_containedButtonHoverSprite;
        std::shared_ptr<NineSliceSprite> m_outlineButtonNormalSprite;
        std::shared_ptr<NineSliceSprite> m_outlineButtonHoverSprite;

        // CheckBox
        std::shared_ptr<ImageSprite> m_texCheckBoxHover;
        std::shared_ptr<ImageSprite> m_texCheckBoxChecked;
        std::shared_ptr<ImageSprite> m_texCheckBoxUnchecked;

        // floating button
        std::shared_ptr<ImageSprite> m_floatingButtonPressedSprite;
        std::shared_ptr<ImageSprite> m_floatingButtonNormalSprite;
        std::shared_ptr<ImageSprite> m_floatingButtonHoverSprite;
        std::shared_ptr<ImageSprite> m_floatingSmallButtonPressedSprite;
        std::shared_ptr<ImageSprite> m_floatingSmallButtonNormalSprite;
        std::shared_ptr<ImageSprite> m_floatingSmallButtonHoverSprite;

        // RadioButton
        std::shared_ptr<ImageSprite> m_texRadioButtonHover;
        std::shared_ptr<ImageSprite> m_texRadioButtonChecked;
        std::shared_ptr<ImageSprite> m_texRadioButtonUnchecked;
        // Switch
        std::shared_ptr<ImageSprite> m_texSwitchHover;
        std::shared_ptr<ImageSprite> m_texSwitchChecked;
        std::shared_ptr<ImageSprite> m_texSwitchUnchecked;
        // slider
        std::shared_ptr<ImageSprite> m_texSliderCursor;
        std::shared_ptr<ImageSprite> m_texSliderCursorHover;
        std::shared_ptr<NineSliceSprite> m_texSliderBG;
        DpSize m_sliderCursorSizeDp;
        DpPoint m_sliderCursorOriginDp;

      public:
        struct SliderThemeColors
        {
          Color Background;
          Color BackgroundDisabled;
          Color Cursor;
          Color CursorDisabled;
          Color CursorOverlay;
        };


        BasicThemeFactory(const std::shared_ptr<WindowContext>& context, ISpriteResourceManager& rResourceManager,
                          const SpriteMaterialId& spriteMaterialId);
        ~BasicThemeFactory();


        const std::shared_ptr<NineSliceSprite>& GetDialogNineSliceSprite() const
        {
          return m_dialogNineSliceSprite;
        }

        std::shared_ptr<ImageSprite> GetFillSprite() const
        {
          return m_fillSprite;
        }

        std::shared_ptr<NineSliceSprite> GetDividerNineSliceSprite() const
        {
          return m_dividerNineSliceSprite;
        }

        const std::shared_ptr<NineSliceSprite>& GetLeftBarNineSliceSprite() const
        {
          return m_leftBarNineSliceSprite;
        }

        const std::shared_ptr<NineSliceSprite>& GetBottomBarNineSliceSprite() const
        {
          return m_bottomBarNineSliceSprite;
        }


        Color GetThemePrimaryColor() const;
        Color GetThemePrimaryDarkColor() const;
        Color GetThemeSidebarColor() const;
        Color GetThemeFontOnPrimaryColor() const;
        Color GetThemeFontOnPrimaryDisabledColor() const;

        SliderThemeColors GetSliderThemeColors() const;

        // ----- CreateLabel

        std::shared_ptr<Label> CreateLabel(const StringViewLite& strView);
        std::shared_ptr<Label> CreateLabel(std::string&& str);

        std::shared_ptr<Label> CreateLabel(const char* const psz)
        {
          return CreateLabel(StringViewLite(psz));
        }

        std::shared_ptr<Label> CreateLabel(const std::string& str)
        {
          return CreateLabel(StringViewLiteUtil::AsStringViewLite(str));
        }


        // ----- CreateFmtValueLabel

        template <typename TValue>
        std::shared_ptr<FmtValueLabel<TValue>> CreateFmtValueLabel(const TValue value)
        {
          auto label = std::make_shared<FmtValueLabel<TValue>>(m_context);
          UpdateLabelColor(*label);
          label->SetContent(value);
          label->FinishAnimation();
          return label;
        }

        template <typename TValue>
        std::shared_ptr<FmtValueLabel<TValue>> CreateFmtValueLabel(const TValue value, const StringViewLite& strViewFormat)
        {
          auto label = CreateFmtValueLabel(value);
          label->SetFormatString(strViewFormat);
          label->FinishAnimation();
          return label;
        }

        template <typename TValue>
        std::shared_ptr<FmtValueLabel<TValue>> CreateFmtValueLabel(const TValue value, std::string&& strFormat)
        {
          auto label = CreateFmtValueLabel(value);
          label->SetFormatString(std::move(strFormat));
          label->FinishAnimation();
          return label;
        }

        template <typename TValue>
        std::shared_ptr<FmtValueLabel<TValue>> CreateFmtValueLabel(const TValue value, const char* const pszFormat)
        {
          return CreateFmtValueLabel(value, StringViewLite(pszFormat));
        }

        template <typename TValue>
        std::shared_ptr<FmtValueLabel<TValue>> CreateFmtValueLabel(const TValue value, const std::string& strFormat)
        {
          return CreateFmtValueLabel(value, StringViewLiteUtil::AsStringViewLite(strFormat));
        }

        // ----- CreateImageButton

        std::shared_ptr<ImageButton> CreateImageButton(const ImageButtonType buttonType, std::shared_ptr<ImageSprite> sprite);
        std::shared_ptr<SimpleImageButton> CreateImageButton(std::shared_ptr<ImageSprite> sprite);

        // ----- CreateTextButton

        std::shared_ptr<LabelNineSliceButton> CreateTextButton(const ButtonType buttonType);
        std::shared_ptr<LabelNineSliceButton> CreateTextButton(const ButtonType buttonType, const StringViewLite& strView);
        std::shared_ptr<LabelNineSliceButton> CreateTextButton(const ButtonType buttonType, std::string&& str);
        std::shared_ptr<LabelNineSliceButton> CreateTextButton(const ButtonType buttonType, const char* const psz)
        {
          return CreateTextButton(buttonType, StringViewLite(psz));
        }
        std::shared_ptr<LabelNineSliceButton> CreateTextButton(const ButtonType buttonType, const std::string& str)
        {
          return CreateTextButton(buttonType, StringViewLiteUtil::AsStringViewLite(str));
        }

        // ----- CreateCheckBox

        std::shared_ptr<CheckBox> CreateCheckBox(const bool checked = false);
        std::shared_ptr<CheckBox> CreateCheckBox(const StringViewLite& strView, const bool checked = false);
        std::shared_ptr<CheckBox> CreateCheckBox(std::string&& str, const bool checked = false);
        std::shared_ptr<CheckBox> CreateCheckBox(const char* const psz, const bool checked = false)
        {
          return CreateCheckBox(StringViewLite(psz), checked);
        }
        std::shared_ptr<CheckBox> CreateCheckBox(const std::string& str, const bool checked = false)
        {
          return CreateCheckBox(StringViewLiteUtil::AsStringViewLite(str), checked);
        }

        // ----- CreateRadioButton

        std::shared_ptr<RadioGroup> CreateRadioGroup(const StringViewLite& name);

        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const bool checked = false);
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const StringViewLite& strView,
                                                       const bool checked = false);
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, std::string&& str, const bool checked = false);
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const char* const psz,
                                                       const bool checked = false)
        {
          return CreateRadioButton(radioGroup, StringViewLite(psz), checked);
        }
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const std::string& str,
                                                       const bool checked = false)
        {
          return CreateRadioButton(radioGroup, StringViewLiteUtil::AsStringViewLite(str), checked);
        }

        // ----- CreateSwitch

        std::shared_ptr<Switch> CreateSwitch(const bool checked = false);
        std::shared_ptr<Switch> CreateSwitch(const StringViewLite& strView, const bool checked = false);
        std::shared_ptr<Switch> CreateSwitch(std::string&& str, const bool checked = false);
        std::shared_ptr<Switch> CreateSwitch(const char* const psz, const bool checked = false)
        {
          return CreateSwitch(StringViewLite(psz), checked);
        }
        std::shared_ptr<Switch> CreateSwitch(const std::string& str, const bool checked = false)
        {
          return CreateSwitch(StringViewLiteUtil::AsStringViewLite(str), checked);
        }


        // ----- CreateSlider

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        template <typename TValue>
        std::shared_ptr<Slider<TValue>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<TValue>& value,
                                                     const SliderConfig& config = {})
        {
          SliderThemeColors themeColors = GetSliderThemeColors();
          auto newControl = std::make_shared<Slider<TValue>>(m_context);
          newControl->SetOrientation(orientation);
          newControl->SetRange(value.Min(), value.Max());
          newControl->SetValue(value.Get());
          newControl->SetBackgroundSprite(m_texSliderBG);
          newControl->SetCursorSprite(m_texSliderCursor);
          newControl->SetCursorOrigin(m_sliderCursorOriginDp);
          newControl->SetCursorSize(m_sliderCursorSizeDp);
          newControl->SetCursorOverlaySprite(m_texSliderCursorHover);
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

        template <typename TValue>
        std::shared_ptr<Slider<TValue>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<TValue>& value,
                                                     const StringViewLite& strFormat, const SliderConfig& config = {})
        {
          auto newControl = CreateSlider(orientation, value, config);
          newControl->SetFormatString(strFormat);
          newControl->FinishAnimation();
          return newControl;
        }

        template <typename TValue>
        std::shared_ptr<Slider<TValue>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<TValue>& value,
                                                     std::string&& strFormat, const SliderConfig& config = {})
        {
          auto newControl = CreateSlider(orientation, value, config);
          newControl->SetFormatString(std::move(strFormat));
          newControl->FinishAnimation();
          return newControl;
        }

        template <typename TValue>
        std::shared_ptr<Slider<TValue>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<TValue>& value,
                                                     const char* const psz, const SliderConfig& config = {})
        {
          return CreateSlider(orientation, value, StringViewLite(psz), config);
        }

        template <typename TValue>
        std::shared_ptr<Slider<TValue>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<TValue>& value,
                                                     const std::string& str, const SliderConfig& config = {})
        {
          return CreateSlider(orientation, value, StringViewLiteUtil::AsStringViewLite(str), config);
        }

        // ----- CreateSliderFmtValue

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        template <typename TValue>
        std::shared_ptr<SliderAndFmtValueLabel<TValue>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                             const ConstrainedValue<TValue>& value, const SliderConfig& config = {})
        {
          SliderThemeColors themeColors = GetSliderThemeColors();
          auto newControl = std::make_shared<SliderAndFmtValueLabel<TValue>>(m_context);
          newControl->SetOrientation(orientation);
          newControl->SetRange(value.Min(), value.Max());
          newControl->SetValue(value.Get());
          newControl->SetBackgroundSprite(m_texSliderBG);
          newControl->SetCursorSprite(m_texSliderCursor);
          newControl->SetCursorOrigin(m_sliderCursorOriginDp);
          newControl->SetCursorSize(m_sliderCursorSizeDp);
          newControl->SetCursorOverlaySprite(m_texSliderCursorHover);
          newControl->SetBackgroundColor(themeColors.Background);
          newControl->SetBackgroundDisabledColor(themeColors.BackgroundDisabled);
          newControl->SetCursorColor(themeColors.Cursor);
          newControl->SetCursorDisabledColor(themeColors.CursorDisabled);
          newControl->SetCursorOverlayColor(themeColors.CursorOverlay);
          newControl->SetFontColor(GetThemeFontOnPrimaryColor());
          newControl->SetFontDisabledColor(GetThemeFontOnPrimaryDisabledColor());
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

        template <typename TValue>
        std::shared_ptr<SliderAndFmtValueLabel<TValue>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                             const ConstrainedValue<TValue>& value, const StringViewLite& strFormat,
                                                                             const SliderConfig& config = {})
        {
          auto newControl = CreateSliderFmtValue(orientation, value, config);
          newControl->SetFormatString(strFormat);
          newControl->FinishAnimation();
          return newControl;
        }

        template <typename TValue>
        std::shared_ptr<SliderAndFmtValueLabel<TValue>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                             const ConstrainedValue<TValue>& value, std::string&& strFormat,
                                                                             const SliderConfig& config = {})
        {
          auto newControl = CreateSliderFmtValue(orientation, value, config);
          newControl->SetFormatString(std::move(strFormat));
          newControl->FinishAnimation();
          return newControl;
        }

        template <typename TValue>
        std::shared_ptr<SliderAndFmtValueLabel<TValue>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                             const ConstrainedValue<TValue>& value, const char* const psz,
                                                                             const SliderConfig& config = {})
        {
          return CreateSliderFmtValue(orientation, value, StringViewLite(psz), config);
        }

        template <typename TValue>
        std::shared_ptr<SliderAndFmtValueLabel<TValue>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                             const ConstrainedValue<TValue>& value, const std::string& str,
                                                                             const SliderConfig& config = {})
        {
          return CreateSliderFmtValue(orientation, value, StringViewLiteUtil::AsStringViewLite(str), config);
        }

        // ----- CreateImage

        std::shared_ptr<BasicImage> CreateImage(const std::shared_ptr<BasicImageSprite>& spriteImage);
        std::shared_ptr<BasicNineSliceImage> CreateImage(const std::shared_ptr<BasicNineSliceSprite>& spriteImage);
        std::shared_ptr<Image> CreateImage(const std::shared_ptr<ImageSprite>& spriteImage);
        std::shared_ptr<NineSliceImage> CreateImage(const std::shared_ptr<NineSliceSprite>& spriteImage);

        // ----- CreateLeftBar

        std::shared_ptr<BackgroundNineSlice> CreateLeftBar(const BarType barType = BarType::Normal);
        std::shared_ptr<BackgroundNineSlice> CreateLeftBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal);

        // ----- CreateTopBar

        std::shared_ptr<BackgroundNineSlice> CreateTopBar(const BarType barType = BarType::Normal);
        std::shared_ptr<BackgroundNineSlice> CreateTopBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal);


        // ----- CreateRightBar

        std::shared_ptr<BackgroundNineSlice> CreateRightBar(const BarType barType = BarType::Normal);
        std::shared_ptr<BackgroundNineSlice> CreateRightBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal);

        // ----- CreateBottomBar

        std::shared_ptr<BackgroundNineSlice> CreateBottomBar(const BarType barType = BarType::Normal);
        std::shared_ptr<BackgroundNineSlice> CreateBottomBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal);

      private:
        static void UpdateLabelColor(LabelBase& rLabel);
      };
    }
  }
}

#endif
