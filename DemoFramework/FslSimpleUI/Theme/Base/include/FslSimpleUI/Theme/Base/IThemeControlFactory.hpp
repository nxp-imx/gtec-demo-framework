#ifndef FSLSIMPLEUI_THEME_BASE_ITHEMECONTROLFACTORY_HPP
#define FSLSIMPLEUI_THEME_BASE_ITHEMECONTROLFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2023 NXP
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

#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/ScrollModeFlags.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/SliderAndFmtValueLabel.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Theme/Base/BarType.hpp>
#include <FslSimpleUI/Theme/Base/ButtonType.hpp>
#include <FslSimpleUI/Theme/Base/ElementType.hpp>
#include <FslSimpleUI/Theme/Base/FloatingButtonType.hpp>
#include <FslSimpleUI/Theme/Base/FontType.hpp>
#include <FslSimpleUI/Theme/Base/ImageButtonType.hpp>
#include <FslSimpleUI/Theme/Base/SliderConfig.hpp>
#include <FslSimpleUI/Theme/Base/WindowType.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class BasicImageSprite;
  class BasicNineSliceSprite;
  class ImageSprite;
  class INineSliceSprite;
  class NineSliceSprite;
  class StringViewLite;

  namespace UI
  {
    class Background;
    class BackgroundLabelButton;
    class BaseWindow;
    class ButtonBase;
    class CheckBox;
    class Image;
    class ImageButton;
    class Label;
    class RadioButton;
    class RadioGroup;
    class ScrollViewer;
    class SimpleImageButton;
    class Switch;

    namespace Theme
    {
      class IThemeResources;

      class IThemeControlFactory
      {
      public:
        virtual ~IThemeControlFactory() = default;

        virtual const std::shared_ptr<WindowContext>& GetContext() const = 0;
        virtual const IThemeResources& GetResources() const = 0;
        virtual UIColor GetThemePrimaryDarkColor() const = 0;
        virtual DpThicknessF GetDefaultMarginDp(const ElementType elementType) const = 0;

        // ----- CreateScrollViewer

        virtual std::shared_ptr<ScrollViewer> CreateScrollViewer(const std::shared_ptr<BaseWindow>& content, const ScrollModeFlags scrollMode,
                                                                 const bool clipContent) = 0;

        // ----- CreateDivider

        virtual std::shared_ptr<Image> CreateDivider(const LayoutOrientation orientation) = 0;

        // ----- CreateLabel

        virtual std::shared_ptr<Label> CreateLabel(const StringViewLite& strView, const FontType fontType = FontType::Default) = 0;
        virtual std::shared_ptr<Label> CreateLabel(std::string&& str, const FontType fontType = FontType::Default) = 0;
        virtual std::shared_ptr<Label> CreateLabel(const char* const psz, const FontType fontType = FontType::Default) = 0;
        virtual std::shared_ptr<Label> CreateLabel(const std::string& str, const FontType fontType = FontType::Default) = 0;

        // ----- CreateImageButton

        virtual std::shared_ptr<ImageButton> CreateImageButton(const ImageButtonType buttonType, std::shared_ptr<ImageSprite> sprite) = 0;
        virtual std::shared_ptr<SimpleImageButton> CreateImageButton(std::shared_ptr<ImageSprite> sprite) = 0;

        // ----- CreateTextButton

        virtual std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType) = 0;
        virtual std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const StringViewLite& strView) = 0;
        virtual std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, std::string&& str) = 0;
        virtual std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const char* const psz) = 0;
        virtual std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const std::string& str) = 0;

        // ----- CreateCheckBox

        virtual std::shared_ptr<CheckBox> CreateCheckBox(const bool checked = false) = 0;
        virtual std::shared_ptr<CheckBox> CreateCheckBox(const StringViewLite& strView, const bool checked = false) = 0;
        virtual std::shared_ptr<CheckBox> CreateCheckBox(std::string&& str, const bool checked = false) = 0;
        virtual std::shared_ptr<CheckBox> CreateCheckBox(const char* const psz, const bool checked = false) = 0;
        virtual std::shared_ptr<CheckBox> CreateCheckBox(const std::string& str, const bool checked = false) = 0;

        // ----- CreateRadioButton

        virtual std::shared_ptr<RadioGroup> CreateRadioGroup(const StringViewLite& name) = 0;

        virtual std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const bool checked = false) = 0;
        virtual std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const StringViewLite& strView,
                                                               const bool checked = false) = 0;
        virtual std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, std::string&& str,
                                                               const bool checked = false) = 0;
        virtual std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const char* const psz,
                                                               const bool checked = false) = 0;
        virtual std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const std::string& str,
                                                               const bool checked = false) = 0;

        // ----- CreateSwitch

        virtual std::shared_ptr<Switch> CreateSwitch(const bool checked = false) = 0;
        virtual std::shared_ptr<Switch> CreateSwitch(const StringViewLite& strView, const bool checked = false) = 0;
        virtual std::shared_ptr<Switch> CreateSwitch(std::string&& str, const bool checked = false) = 0;
        virtual std::shared_ptr<Switch> CreateSwitch(const char* const psz, const bool checked = false) = 0;
        virtual std::shared_ptr<Switch> CreateSwitch(const std::string& str, const bool checked = false) = 0;

        // ----- CreateImage

        virtual std::shared_ptr<Image> CreateImage(const std::shared_ptr<BasicImageSprite>& spriteImage) = 0;
        virtual std::shared_ptr<Image> CreateImage(const std::shared_ptr<BasicNineSliceSprite>& spriteImage) = 0;
        virtual std::shared_ptr<Image> CreateImage(const std::shared_ptr<ImageSprite>& spriteImage) = 0;
        virtual std::shared_ptr<Image> CreateImage(const std::shared_ptr<INineSliceSprite>& spriteImage) = 0;

        // ----- CreateLeftBar

        virtual std::shared_ptr<Background> CreateLeftBar(const BarType barType = BarType::Normal) = 0;
        virtual std::shared_ptr<Background> CreateLeftBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal) = 0;

        // ----- CreateTopBar

        virtual std::shared_ptr<Background> CreateTopBar(const BarType barType = BarType::Normal) = 0;
        virtual std::shared_ptr<Background> CreateTopBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal) = 0;


        // ----- CreateRightBar

        virtual std::shared_ptr<Background> CreateRightBar(const BarType barType = BarType::Normal) = 0;
        virtual std::shared_ptr<Background> CreateRightBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal) = 0;

        // ----- CreateBottomBar

        virtual std::shared_ptr<Background> CreateBottomBar(const BarType barType = BarType::Normal) = 0;
        virtual std::shared_ptr<Background> CreateBottomBar(const std::shared_ptr<BaseWindow>& content, const BarType barType = BarType::Normal) = 0;


        // ----- CreateBackgroundWindow

        virtual std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType) = 0;
        virtual std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content) = 0;
        virtual std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content,
                                                                   const UI::ItemAlignment alignment) = 0;
        virtual std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content,
                                                                   const UI::ItemAlignment alignmentX, const UI::ItemAlignment alignmentY) = 0;

        // ----- CreateFmtValueLabel<uint8_t>
        virtual std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const StringViewLite& strViewFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, std::string&& strFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const char* const pszFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const std::string& strFormat) = 0;

        // ----- CreateFmtValueLabel<int32_t>
        virtual std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value) = 0;
        virtual std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const StringViewLite& strViewFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, std::string&& strFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const char* const pszFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const std::string& strFormat) = 0;

        // ----- CreateFmtValueLabel<uint32_t>
        virtual std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const StringViewLite& strViewFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, std::string&& strFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const char* const pszFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const std::string& strFormat) = 0;

        // ----- CreateFmtValueLabel<uint64_t>
        virtual std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const StringViewLite& strViewFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, std::string&& strFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const char* const pszFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const std::string& strFormat) = 0;

        // ----- CreateFmtValueLabel<float>
        virtual std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value) = 0;
        virtual std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const StringViewLite& strViewFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, std::string&& strFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const char* const pszFormat) = 0;
        virtual std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const std::string& strFormat) = 0;


        // CreateFloatingButton
        virtual std::shared_ptr<ButtonBase> CreateFloatingButton(const FloatingButtonType type) = 0;

        // ----- CreateSlider<uint8_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<Slider<uint8_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<uint8_t>& value,
                                                              const SliderConfig& config = {}) = 0;

        // ----- CreateSlider<int32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<Slider<int32_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<int32_t>& value,
                                                              const SliderConfig& config = {}) = 0;

        // ----- CreateSlider<uint32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<Slider<uint32_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<uint32_t>& value,
                                                               const SliderConfig& config = {}) = 0;

        // ----- CreateSlider<float>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<Slider<float>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                            const SliderConfig& config = {}) = 0;

        // ----- CreateSliderFmtValue<uint8_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint8_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<uint8_t>& value, const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<uint8_t>& value,
                                                                                      const StringViewLite& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<uint8_t>& value, std::string&& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<uint8_t>& value,
                                                                                      const char* const pszFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<uint8_t>& value,
                                                                                      const std::string& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        // ----- CreateSliderFmtValue<int32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<SliderAndFmtValueLabel<int32_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<int32_t>& value, const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<int32_t>& value,
                                                                                      const StringViewLite& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<int32_t>& value, std::string&& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<int32_t>& value,
                                                                                      const char* const pszFormat,
                                                                                      const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                      const ConstrainedValue<int32_t>& value,
                                                                                      const std::string& strFormat,
                                                                                      const SliderConfig& config = {}) = 0;

        // ----- CreateSliderFmtValue<uint32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint32_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<uint32_t>& value, const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                       const ConstrainedValue<uint32_t>& value,
                                                                                       const StringViewLite& strFormat,
                                                                                       const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                       const ConstrainedValue<uint32_t>& value,
                                                                                       std::string&& strFormat, const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                       const ConstrainedValue<uint32_t>& value,
                                                                                       const char* const pszFormat,
                                                                                       const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                       const ConstrainedValue<uint32_t>& value,
                                                                                       const std::string& strFormat,
                                                                                       const SliderConfig& config = {}) = 0;

        // ----- CreateSliderFmtValue<float>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        virtual std::shared_ptr<SliderAndFmtValueLabel<float>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value, const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                    const ConstrainedValue<float>& value,
                                                                                    const StringViewLite& strFormat,
                                                                                    const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                    const ConstrainedValue<float>& value, std::string&& strFormat,
                                                                                    const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                    const ConstrainedValue<float>& value, const char* const pszFormat,
                                                                                    const SliderConfig& config = {}) = 0;
        virtual std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                                    const ConstrainedValue<float>& value,
                                                                                    const std::string& strFormat,
                                                                                    const SliderConfig& config = {}) = 0;
      };
    }
  }
}

#endif
