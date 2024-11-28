#ifndef FSLSIMPLEUI_THEME_BASIC_BASICTHEMECONTROLFACTORY_HPP
#define FSLSIMPLEUI_THEME_BASIC_BASICTHEMECONTROLFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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
#include <FslBase/Math/ConstrainedValue.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslGraphics/Render/NineSliceAtlasTexture2D.hpp>
#include <FslGraphics/Sprite/IContentSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/UIColorSpace.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeColors.hpp>
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
    class Background;
    class BackgroundImageButton;
    class BackgroundLabelButton;
    class CheckBox;
    class ImageButton;
    class RadioButton;
    class RadioGroup;
    class Switch;
    class Image;
    class Label;
    class SimpleImageButton;
    class WindowContext;

    namespace Theme
    {
      class BasicThemeResources;
      class IThemeResources;

      class BasicThemeControlFactory final : public IThemeControlFactory
      {
        BasicThemeColors m_colors;
        std::shared_ptr<WindowContext> m_context;
        std::shared_ptr<BasicThemeResources> m_resources;

      public:
        BasicThemeControlFactory(const std::shared_ptr<WindowContext>& context, const std::shared_ptr<BasicThemeResources>& themeResources,
                                 const bool usePrimaryPalette, const UI::UIColorSpace colorSpace);
        ~BasicThemeControlFactory() override;

        const std::shared_ptr<WindowContext>& GetContext() const final
        {
          return m_context;
        }

        const IThemeResources& GetResources() const final;
        UIColor GetThemePrimaryDarkColor() const final;
        DpThicknessF GetDefaultMarginDp(const ElementType elementType) const final;


        // ----- CreateScrollViewer
        std::shared_ptr<ScrollViewer> CreateScrollViewer(const std::shared_ptr<BaseWindow>& content, const ScrollModeFlags scrollMode,
                                                         const bool clipContent) final;

        // ----- CreateDivider
        std::shared_ptr<Image> CreateDivider(const LayoutOrientation orientation) final;

        // ----- CreateLabel

        std::shared_ptr<Label> CreateLabel(const StringViewLite& strView, const FontType fontType) final;
        std::shared_ptr<Label> CreateLabel(std::string&& str, const FontType fontType) final;

        std::shared_ptr<Label> CreateLabel(const char* const psz, const FontType fontType) final
        {
          return CreateLabel(StringViewLite(psz), fontType);
        }

        std::shared_ptr<Label> CreateLabel(const std::string& str, const FontType fontType) final
        {
          return CreateLabel(StringViewLite(str), fontType);
        }


        // ----- CreateImageButton

        std::shared_ptr<ImageButton> CreateImageButton(const ImageButtonType buttonType, std::shared_ptr<ImageSprite> sprite) final;
        std::shared_ptr<SimpleImageButton> CreateImageButton(std::shared_ptr<ImageSprite> sprite) final;

        // ----- CreateTextButton

        std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType) final;
        std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const StringViewLite& strView) final;
        std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, std::string&& str) final;
        std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const char* const psz) final
        {
          return CreateTextButton(buttonType, StringViewLite(psz));
        }
        std::shared_ptr<BackgroundLabelButton> CreateTextButton(const ButtonType buttonType, const std::string& str) final
        {
          return CreateTextButton(buttonType, StringViewLite(str));
        }

        // ----- CreateCheckBox

        std::shared_ptr<CheckBox> CreateCheckBox(const bool checked) final;
        std::shared_ptr<CheckBox> CreateCheckBox(const StringViewLite& strView, const bool checked) final;
        std::shared_ptr<CheckBox> CreateCheckBox(std::string&& str, const bool checked) final;
        std::shared_ptr<CheckBox> CreateCheckBox(const char* const psz, const bool checked) final
        {
          return CreateCheckBox(StringViewLite(psz), checked);
        }
        std::shared_ptr<CheckBox> CreateCheckBox(const std::string& str, const bool checked) final
        {
          return CreateCheckBox(StringViewLite(str), checked);
        }

        // ----- CreateRadioButton

        std::shared_ptr<RadioGroup> CreateRadioGroup(const StringViewLite& name) final;

        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const bool checked) final;
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const StringViewLite& strView,
                                                       const bool checked) final;
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, std::string&& str, const bool checked) final;
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const char* const psz, const bool checked) final
        {
          return CreateRadioButton(radioGroup, StringViewLite(psz), checked);
        }
        std::shared_ptr<RadioButton> CreateRadioButton(const std::shared_ptr<RadioGroup>& radioGroup, const std::string& str,
                                                       const bool checked) final
        {
          return CreateRadioButton(radioGroup, StringViewLite(str), checked);
        }

        // ----- CreateSwitch

        std::shared_ptr<Switch> CreateSwitch(const bool checked) final;
        std::shared_ptr<Switch> CreateSwitch(const StringViewLite& strView, const bool checked) final;
        std::shared_ptr<Switch> CreateSwitch(std::string&& str, const bool checked) final;
        std::shared_ptr<Switch> CreateSwitch(const char* const psz, const bool checked) final
        {
          return CreateSwitch(StringViewLite(psz), checked);
        }
        std::shared_ptr<Switch> CreateSwitch(const std::string& str, const bool checked) final
        {
          return CreateSwitch(StringViewLite(str), checked);
        }

        // ----- CreateImage

        std::shared_ptr<Image> CreateImage(const std::shared_ptr<BasicImageSprite>& spriteImage) final;
        std::shared_ptr<Image> CreateImage(const std::shared_ptr<BasicNineSliceSprite>& spriteImage) final;
        std::shared_ptr<Image> CreateImage(const std::shared_ptr<ImageSprite>& spriteImage) final;
        std::shared_ptr<Image> CreateImage(const std::shared_ptr<INineSliceSprite>& spriteImage) final;

        // ----- CreateLeftBar

        std::shared_ptr<Background> CreateLeftBar(const BarType barType) final;
        std::shared_ptr<Background> CreateLeftBar(const std::shared_ptr<BaseWindow>& content, const BarType barType) final;

        // ----- CreateTopBar

        std::shared_ptr<Background> CreateTopBar(const BarType barType) final;
        std::shared_ptr<Background> CreateTopBar(const std::shared_ptr<BaseWindow>& content, const BarType barType) final;


        // ----- CreateRightBar

        std::shared_ptr<Background> CreateRightBar(const BarType barType) final;
        std::shared_ptr<Background> CreateRightBar(const std::shared_ptr<BaseWindow>& content, const BarType barType) final;

        // ----- CreateBottomBar

        std::shared_ptr<Background> CreateBottomBar(const BarType barType) final;
        std::shared_ptr<Background> CreateBottomBar(const std::shared_ptr<BaseWindow>& content, const BarType barType) final;

        // ----- CreateBackgroundWindow

        std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType) final
        {
          return CreateBackgroundWindow(windowType, {}, UI::ItemAlignment::Near);
        }

        std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content) final
        {
          return CreateBackgroundWindow(windowType, content, UI::ItemAlignment::Near);
        }

        std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content,
                                                           const UI::ItemAlignment alignment) final
        {
          return CreateBackgroundWindow(windowType, content, alignment, alignment);
        }

        std::shared_ptr<Background> CreateBackgroundWindow(const WindowType windowType, const std::shared_ptr<BaseWindow>& content,
                                                           const UI::ItemAlignment alignmentX, const UI::ItemAlignment alignmentY) final;

        // ----- CreateFloatingButton
        std::shared_ptr<ButtonBase> CreateFloatingButton(const FloatingButtonType type) final;

        // ----- CreateFmtValueLabel<uint8_t>
        std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value) final;
        std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const StringViewLite& strViewFormat) final;
        std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, std::string&& strFormat) final;
        std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const char* const pszFormat) final;
        std::shared_ptr<FmtValueLabel<uint8_t>> CreateFmtValueLabel(const uint8_t value, const std::string& strFormat) final;

        // ----- CreateFmtValueLabel<int32_t>
        std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value) final;
        std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const StringViewLite& strViewFormat) final;
        std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, std::string&& strFormat) final;
        std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const char* const pszFormat) final;
        std::shared_ptr<FmtValueLabel<int32_t>> CreateFmtValueLabel(const int32_t value, const std::string& strFormat) final;

        // ----- CreateFmtValueLabel<uint32_t>
        std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value) final;
        std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const StringViewLite& strViewFormat) final;
        std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, std::string&& strFormat) final;
        std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const char* const pszFormat) final;
        std::shared_ptr<FmtValueLabel<uint32_t>> CreateFmtValueLabel(const uint32_t value, const std::string& strFormat) final;

        // ----- CreateFmtValueLabel<uint64_t>
        std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value) final;
        std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const StringViewLite& strViewFormat) final;
        std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, std::string&& strFormat) final;
        std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const char* const pszFormat) final;
        std::shared_ptr<FmtValueLabel<uint64_t>> CreateFmtValueLabel(const uint64_t value, const std::string& strFormat) final;

        // ----- CreateFmtValueLabel<float>
        std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value) final;
        std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const StringViewLite& strViewFormat) final;
        std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, std::string&& strFormat) final;
        std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const char* const pszFormat) final;
        std::shared_ptr<FmtValueLabel<float>> CreateFmtValueLabel(const float value, const std::string& strFormat) final;


        // ----- CreateSlider<uint8_t>

        std::shared_ptr<Slider<uint8_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<uint8_t>& value,
                                                      const SliderConfig& config = {}) final;

        // ----- CreateSlider<int32_t>

        std::shared_ptr<Slider<int32_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<int32_t>& value,
                                                      const SliderConfig& config) final;

        // ----- CreateSlider<uint32_t>

        std::shared_ptr<Slider<uint32_t>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<uint32_t>& value,
                                                       const SliderConfig& config) final;

        // ----- CreateSlider<float>

        std::shared_ptr<Slider<float>> CreateSlider(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                    const SliderConfig& config) final;


        // ----- CreateSliderFmtValue<uint8_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        std::shared_ptr<SliderAndFmtValueLabel<uint8_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<uint8_t>& value, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<uint8_t>& value, const StringViewLite& strFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<uint8_t>& value, std::string&& strFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<uint8_t>& value, const char* const pszFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint8_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<uint8_t>& value, const std::string& strFormat,
                                                                              const SliderConfig& config) final;

        // ----- CreateSliderFmtValue<int32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        std::shared_ptr<SliderAndFmtValueLabel<int32_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<int32_t>& value, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<int32_t>& value, const StringViewLite& strFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<int32_t>& value, std::string&& strFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<int32_t>& value, const char* const pszFormat,
                                                                              const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<int32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                              const ConstrainedValue<int32_t>& value, const std::string& strFormat,
                                                                              const SliderConfig& config) final;

        // ----- CreateSliderFmtValue<uint32_t>

        //! @brief Create a slider.
        //! @note  It will default to stretch in the layout direction.
        std::shared_ptr<SliderAndFmtValueLabel<uint32_t>>
          CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<uint32_t>& value, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                               const ConstrainedValue<uint32_t>& value,
                                                                               const StringViewLite& strFormat, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                               const ConstrainedValue<uint32_t>& value, std::string&& strFormat,
                                                                               const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                               const ConstrainedValue<uint32_t>& value, const char* const pszFormat,
                                                                               const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<uint32_t>> CreateSliderFmtValue(const LayoutOrientation orientation,
                                                                               const ConstrainedValue<uint32_t>& value, const std::string& strFormat,
                                                                               const SliderConfig& config) final;

        // ----- CreateSliderFmtValue<float>

        std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                            const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                            const StringViewLite& strFormat, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                            std::string&& strFormat, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                            const char* const pszFormat, const SliderConfig& config) final;
        std::shared_ptr<SliderAndFmtValueLabel<float>> CreateSliderFmtValue(const LayoutOrientation orientation, const ConstrainedValue<float>& value,
                                                                            const std::string& strFormat, const SliderConfig& config) final;
      };
    }
  }
}

#endif
