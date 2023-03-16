#ifndef FSLSIMPLEUI_BASE_CONTROL_SLIDERANDFMTVALUELABEL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SLIDERANDFMTVALUELABEL_HPP
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

#include <FslSimpleUI/Base/Control/ContentControlBase.hpp>
#include <FslSimpleUI/Base/Control/FmtValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>

namespace Fsl::UI
{
  class ComplexStackLayout;

  template <typename TValue>
  class SliderAndFmtValueLabel final : public ContentControlBase
  {
  public:
    using value_type = TValue;

  private:
    std::shared_ptr<ComplexStackLayout> m_layout;
    std::shared_ptr<Slider<value_type>> m_slider;
    std::shared_ptr<FmtValueLabel<value_type>> m_label;
    bool m_cacheValid{false};
    value_type m_cachedMinValue{0};
    value_type m_cachedMaxValue{0};
    Color m_fontColor{DefaultColor::Palette::Font};
    Color m_fontDisabledColor{DefaultColor::Palette::FontDisabled};

  public:
    explicit SliderAndFmtValueLabel(const std::shared_ptr<WindowContext>& context)
      : ContentControlBase(context)
      , m_layout(std::make_shared<ComplexStackLayout>(context))
      , m_slider(std::make_shared<Slider<value_type>>(context))
      , m_label(std::make_shared<FmtValueLabel<value_type>>(context))
    {
      // Enable resolve so we can do the final layout tweaks before the layout process starts
      Enable(WindowFlags::ResolveEnabled);

      m_slider->SetAlignmentX(ItemAlignment::Stretch);
      m_slider->SetAlignmentY(ItemAlignment::Center);

      m_label->SetContent(m_slider->GetValue());
      m_label->SetAlignmentX(ItemAlignment::Stretch);
      m_label->SetAlignmentY(ItemAlignment::Stretch);
      m_label->SetContentAlignmentX(ItemAlignment::Far);
      m_label->SetContentAlignmentY(ItemAlignment::Center);

      m_layout->SetAlignmentX(ItemAlignment::Stretch);
      m_layout->SetAlignmentY(ItemAlignment::Stretch);
      m_layout->SetOrientation(LayoutOrientation::Horizontal);
      m_layout->AddChild(m_slider);
      m_layout->AddChild(m_label);

      DoSetContent(m_layout);
      UpdateLinkedContent();
    }

    void WinResolutionChanged(const ResolutionChangedInfo& info) final
    {
      ContentControlBase::WinResolutionChanged(info);
      m_cacheValid = false;
    }


    void WinResolve(const TimeSpan& timespan) final
    {
      ContentControlBase::WinResolve(timespan);
      if (IsLayoutDirty() || !m_cacheValid)
      {
        UpdateLinkedContent();
        FixLayout();
      }
    }

    // --- Forward to SliderBase

    //! @brief Check if the slider is being dragged or not
    bool IsDragging() const
    {
      return m_slider->IsDragging();
    }

    LayoutOrientation GetOrientation() const
    {
      return m_slider->GetOrientation();
    }

    LayoutDirection GetDirection() const
    {
      return m_slider->GetDirection();
    }

    bool IsEnabled() const
    {
      return m_slider->IsEnabled();
    }

    value_type GetValue() const
    {
      return m_slider->GetValue();
    }

    value_type GetMinValue() const
    {
      return m_slider->GetMinValue();
    }

    value_type GetMaxValue() const
    {
      return m_slider->GetMaxValue();
    }

    value_type GetTickFrequency() const
    {
      return m_slider->GetTickFrequency();
    }

    bool SetOrientation(const LayoutOrientation orientation)
    {
      if (m_slider->SetOrientation(orientation))
      {
        m_layout->SetOrientation(orientation);
        m_cacheValid = false;
        switch (orientation)
        {
        case LayoutOrientation::Horizontal:
          m_slider->SetAlignmentX(ItemAlignment::Stretch);
          m_slider->SetAlignmentY(ItemAlignment::Center);
          m_label->SetContentAlignmentX(ItemAlignment::Far);
          m_label->SetContentAlignmentY(ItemAlignment::Center);
          break;
        case LayoutOrientation::Vertical:
          m_slider->SetAlignmentX(ItemAlignment::Center);
          m_slider->SetAlignmentY(ItemAlignment::Stretch);
          m_label->SetContentAlignmentX(ItemAlignment::Center);
          m_label->SetContentAlignmentY(ItemAlignment::Far);
          break;
        }
        return true;
      }
      return false;
    }

    void SetDirection(const LayoutDirection direction)
    {
      m_slider->SetDirection(direction);
    }

    bool GetEnableVerticalGraphicsRotation() const
    {
      return m_slider->GetEnableVerticalGraphicsRotation();
    }

    void SetEnableVerticalGraphicsRotation(const bool enabled)
    {
      m_slider->SetEnableVerticalGraphicsRotation(enabled);
    }

    //! @brief When disabled the slider can not be edited by the user
    bool SetEnabled(const bool enabled)
    {
      auto res = m_slider->SetEnabled(enabled);
      PatchLabelFontColor();
      return res;
    }

    bool AddValue(const value_type value)
    {
      return m_slider->AddValue(value);
    }

    bool SubValue(const value_type value)
    {
      return m_slider->SubValue(value);
    }

    bool SetValue(const value_type value)
    {
      return m_slider->SetValue(value);
    }

    bool SetRange(const value_type min, const value_type max)
    {
      return m_slider->SetRange(min, max);
    }

    bool SetTickFrequency(const value_type tickFrequency)
    {
      return m_slider->SetTickFrequency(tickFrequency);
    }

    // -----


    const Color& GetFontColor() const
    {
      return m_fontColor;
    }

    void SetFontColor(const Color& value)
    {
      if (value != m_fontColor)
      {
        m_fontColor = value;
        PatchLabelFontColor();
        PropertyUpdated(PropertyType::Other);
      }
    }

    const Color& GetFontDisabledColor() const
    {
      return m_fontDisabledColor;
    }

    void SetFontDisabledColor(const Color& value)
    {
      if (value != m_fontDisabledColor)
      {
        m_fontDisabledColor = value;
        PatchLabelFontColor();
        PropertyUpdated(PropertyType::Other);
      }
    }

    // ---

    const std::shared_ptr<ISizedSprite>& GetCursorSprite() const
    {
      return m_slider->GetCursorSprite();
    }

    void SetCursorSprite(const std::shared_ptr<ISizedSprite>& value)
    {
      m_slider->SetCursorSprite(value);
    }

    const DpPoint2& GetCursorOrigin() const
    {
      return m_slider->GetCursorOrigin();
    }

    void SetCursorOrigin(const DpPoint2& value)
    {
      m_slider->SetCursorOrigin(value);
    }

    DpSize2D GetCursorSize() const
    {
      return m_slider->GetCursorSize();
    }

    void SetCursorSize(const DpSize2D value)
    {
      m_slider->SetCursorSize(value);
    }

    const Color& GetCursorColor() const
    {
      return m_slider->GetCursorColor();
    }

    void SetCursorColor(const Color& value)
    {
      m_slider->SetCursorColor(value);
    }

    const Color& GetCursorDisabledColor() const
    {
      return m_slider->GetCursorDisabledColor();
    }

    void SetCursorDisabledColor(const Color& value)
    {
      m_slider->SetCursorDisabledColor(value);
    }

    // ---


    const std::shared_ptr<ISizedSprite>& GetCursorOverlaySprite() const
    {
      return m_slider->GetCursorOverlaySprite();
    }

    void SetCursorOverlaySprite(const std::shared_ptr<ISizedSprite>& value)
    {
      m_slider->SetCursorOverlaySprite(value);
    }

    const Color& GetCursorOverlayColor() const
    {
      return m_slider->GetCursorOverlayColor();
    }

    void SetCursorOverlayColor(const Color& value)
    {
      m_slider->SetCursorOverlayColor(value);
    }


    // ---

    const std::shared_ptr<IContentSprite>& GetBackgroundSprite() const
    {
      return m_slider->GetBackgroundSprite();
    }

    void SetBackgroundSprite(const std::shared_ptr<IContentSprite>& value)
    {
      m_slider->SetBackgroundSprite(value);
    }


    const Color& GetBackgroundColor() const
    {
      return m_slider->GetBackgroundColor();
    }

    void SetBackgroundColor(const Color& value)
    {
      m_slider->SetBackgroundColor(value);
    }

    const Color& GetBackgroundDisabledColor() const
    {
      return m_slider->GetBackgroundDisabledColor();
    }

    void SetBackgroundDisabledColor(const Color& value)
    {
      m_slider->SetBackgroundDisabledColor(value);
    }

    const DpThickness& GetBackgroundPadding() const
    {
      return m_slider->GetBackgroundPadding();
    }

    void SetBackgroundPadding(const DpThickness& value)
    {
      m_slider->SetBackgroundPadding(value);
    }

    // ---

    const std::string& GetFormatString() const
    {
      return m_label->GetFormatString();
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const char* const psz)
    {
      auto modified = m_label->SetFormatString(psz);
      if (modified)
      {
        m_cacheValid = false;
      }
      return modified;
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const StringViewLite& value)
    {
      auto modified = m_label->SetFormatString(value);
      if (modified)
      {
        m_cacheValid = false;
      }
      return modified;
    }

    //! @return true if the format string was changed (false if the format string was already equal to the value being set)
    bool SetFormatString(const std::string& strFormat)
    {
      auto modified = m_label->SetFormatString(strFormat);
      if (modified)
      {
        m_cacheValid = false;
      }
      return modified;
    }

    inline static typename DataBinding::DependencyPropertyDefinition PropertyValue =
      DataBinding::DependencyPropertyDefinitionFactory::Create<value_type, SliderAndFmtValueLabel, &SliderAndFmtValueLabel::GetValue,
                                                               &SliderAndFmtValueLabel::SetValue>("Value");

  protected:
    void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) final
    {
      FSL_PARAM_NOT_USED(args);

      UpdateLinkedContent();
      if (!theEvent->IsHandled())
      {
        // A bit nasty to have to ask the window manager to do it but at least we dont have to use the shared_from_this pattern then
        // It is done this way until we get time to do a cleaner interface
        auto uiContext = GetContext()->TheUIContext.Get();
        uiContext->WindowManager->SYS_SetEventSource(theEvent.get(), this);
      }
    }

    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override
    {
      if (sourceDef == PropertyValue)
      {
        return m_slider->GetPropertyHandle(Slider<value_type>::PropertyValue);
      }
      return ContentControlBase::TryGetPropertyHandleNow(sourceDef);
    }

    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override
    {
      if (targetDef == PropertyValue)
      {
        return m_slider->TrySetBinding(Slider<value_type>::PropertyValue, binding);
      }
      return ContentControlBase::TrySetBindingNow(targetDef, binding);
    }

    void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override
    {
      rProperties.push_back(PropertyValue);
      ContentControlBase::ExtractAllProperties(rProperties);
    }

  private:
    void UpdateLinkedContent()
    {
      m_label->SetContent(m_slider->GetValue());
    }

    void FixLayout()
    {
      const auto minValue = m_slider->GetMinValue();
      const auto maxValue = m_slider->GetMaxValue();
      if (!m_cacheValid || minValue != m_cachedMinValue || maxValue != m_cachedMaxValue)
      {
        // const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

        m_cacheValid = true;
        // The way we 'find' the max render size of the number is not a sure thing.
        // For example if the range is 0-9 then the numbers 1-8 could actually be larger when rendered :(
        m_cachedMinValue = minValue;
        m_cachedMaxValue = maxValue;

        const PxSize2D size1Px = m_label->MeasureRenderedValue(minValue);
        const PxSize2D size2Px = m_label->MeasureRenderedValue(maxValue);
        const PxSize2D finalSizePx = PxSize2D::Max(size1Px, size2Px);
        // const DpSize2D finalSizeDp = unitConverter.ToDpSize2D(finalSizePx);


        m_layout->ClearLayoutLengths();
        m_layout->PushLayoutLength(LayoutLength(LayoutUnitType::Star, 1.0f));
        if (m_slider->GetOrientation() == LayoutOrientation::Horizontal)
        {
          // Since we recalc this on resize, we might as well just use the final px sizes
          m_layout->PushLayoutLength(LayoutLength(LayoutUnitType::Px, static_cast<float>(finalSizePx.Width())));
          // m_label->SetHeight(DpLayoutSize1D(finalSizeDp.Height()));
        }
        else
        {
          // Since we recalc this on resize, we might as well just use the final px sizes
          m_layout->PushLayoutLength(LayoutLength(LayoutUnitType::Px, static_cast<float>(finalSizePx.Height())));
          // m_label->SetWidth(DpLayoutSize1D(finalSizeDp.Width()));
        }
      }
    }

    void PatchLabelFontColor()
    {
      m_label->SetFontColor(m_slider->IsEnabled() ? m_fontColor : m_fontDisabledColor);
    }
  };
}

#endif
