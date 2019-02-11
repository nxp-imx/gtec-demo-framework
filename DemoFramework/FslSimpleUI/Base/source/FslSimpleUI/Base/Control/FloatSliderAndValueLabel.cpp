/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslSimpleUI/Base/Control/FloatSliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/EqualHelper.hpp>

namespace Fsl
{
  namespace UI
  {
    FloatSliderAndValueLabel::FloatSliderAndValueLabel(const std::shared_ptr<WindowContext>& context)
      : ContentControlBase(context)
      , m_layout(std::make_shared<ComplexStackLayout>(context))
      , m_slider(std::make_shared<FloatSlider>(context))
      , m_label(std::make_shared<FloatValueLabel>(context))
      , m_cachedMinValue(m_slider->GetMinValue() - 1.0f)    // - 1.0f to force a recache
      , m_cachedMaxValue(0.0f)
      , m_cachedFormatDecimals(0)
    {
      // Enable the resolve call to allow this control to detect changes to our internal child controls
      Enable(WindowFlags(WindowFlags::ResolveEnabled));

      m_slider->SetAlignmentX(ItemAlignment::Stretch);
      m_slider->SetAlignmentY(ItemAlignment::Center);

      m_label->SetContent(m_slider->GetValue());
      m_label->SetAlignmentX(ItemAlignment::Far);
      m_label->SetAlignmentY(ItemAlignment::Center);


      m_layout->SetAlignmentX(ItemAlignment::Stretch);
      m_layout->SetAlignmentY(ItemAlignment::Center);
      m_layout->SetLayoutOrientation(LayoutOrientation::Horizontal);
      m_layout->AddChild(m_slider);
      m_layout->AddChild(m_label);

      DoSetContent(m_layout);
      UpdateLinkedContent();
    }


    void FloatSliderAndValueLabel::WinResolve(const DemoTime& demoTime)
    {
      if (!IsLayoutDirty())
      {
        return;
      }
      ContentControlBase::WinResolve(demoTime);
      UpdateLinkedContent();
      FixLayout();
    }


    const AtlasTexture2D& FloatSliderAndValueLabel::GetBackgroundTexture() const
    {
      return m_slider->GetBackgroundTexture();
    }


    void FloatSliderAndValueLabel::SetBackgroundTexture(const AtlasTexture2D& value)
    {
      m_slider->SetBackgroundTexture(value);
    }


    const AtlasTexture2D& FloatSliderAndValueLabel::GetCursorTexture() const
    {
      return m_slider->GetCursorTexture();
    }


    void FloatSliderAndValueLabel::SetCursorTexture(const AtlasTexture2D& value)
    {
      m_slider->SetCursorTexture(value);
    }


    float FloatSliderAndValueLabel::GetValue() const
    {
      return m_slider->GetValue();
    }


    void FloatSliderAndValueLabel::SetValue(const float& value)
    {
      m_slider->SetValue(value);
    }


    float FloatSliderAndValueLabel::GetMinValue() const
    {
      return m_slider->GetMinValue();
    }


    float FloatSliderAndValueLabel::GetMaxValue() const
    {
      return m_slider->GetMaxValue();
    }


    bool FloatSliderAndValueLabel::SetValueLimits(const float& min, const float& max)
    {
      return m_slider->SetValueLimits(min, max);
    }


    const ThicknessF& FloatSliderAndValueLabel::GetBackgroundPadding() const
    {
      return m_slider->GetBackgroundPadding();
    }


    void FloatSliderAndValueLabel::SetBackgroundPadding(const ThicknessF& value)
    {
      m_slider->SetBackgroundPadding(value);
    }


    const ThicknessF& FloatSliderAndValueLabel::GetCursorPadding() const
    {
      return m_slider->GetCursorPadding();
    }


    void FloatSliderAndValueLabel::SetCursorPadding(const ThicknessF& value)
    {
      m_slider->SetCursorPadding(value);
    }


    const NineSlice FloatSliderAndValueLabel::GetNineSlice() const
    {
      return m_slider->GetNineSlice();
    }


    void FloatSliderAndValueLabel::SetNineSlice(const NineSlice& value)
    {
      m_slider->SetNineSlice(value);
    }

    int32_t FloatSliderAndValueLabel::GetFormatDecimals() const
    {
      return m_label->GetFormatDecimals();
    }

    void FloatSliderAndValueLabel::SetFormatDecimals(const uint32_t value)
    {
      m_label->SetFormatDecimals(value);
    }


    void FloatSliderAndValueLabel::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
    {
      UpdateLinkedContent();
      if (!theEvent->IsHandled())
      {
        // A bit nasty to have to ask the window manager to do it but at least we dont have to use the shared_from_this pattern then
        // It is done this way until we get time to do a cleaner interface
        auto uiContext = GetContext()->TheUIContext.Get();
        uiContext->WindowManager->SYS_SetEventSource(theEvent.get(), this);
      }
    }


    void FloatSliderAndValueLabel::UpdateLinkedContent()
    {
      m_label->SetContent(m_slider->GetValue());
    }


    void FloatSliderAndValueLabel::FixLayout()
    {
      const auto minValue = m_slider->GetMinValue();
      const auto maxValue = m_slider->GetMaxValue();
      const auto formatDecimals = m_label->GetFormatDecimals();

      if (minValue != m_cachedMinValue || maxValue != m_cachedMaxValue || formatDecimals != m_cachedFormatDecimals)
      {
        // The way we 'find' the max render size of the number is not a sure thing.
        // For example if the range is 0-9 then the numbers 1-8 could actually be larger when rendered :(
        // so the +5.0f to x is a bit of a minor compensation code (ugly yes)
        m_cachedMinValue = minValue;
        m_cachedMaxValue = maxValue;
        m_cachedFormatDecimals = formatDecimals;

        Vector2 size1 = m_label->MeasureRenderedValue(minValue);
        Vector2 size2 = m_label->MeasureRenderedValue(maxValue);
        Vector2 finalSize = Vector2(std::max(size1.X, size2.X) + 5.0f, std::max(size1.Y, size2.Y));
        m_label->SetHeight(finalSize.Y);
        m_layout->ClearLayoutLengths();
        m_layout->PushLayoutLength(LayoutLength(1.0f, LayoutUnitType::Star));
        m_layout->PushLayoutLength(LayoutLength(finalSize.X, LayoutUnitType::Fixed));
      }
    }
  }
}
