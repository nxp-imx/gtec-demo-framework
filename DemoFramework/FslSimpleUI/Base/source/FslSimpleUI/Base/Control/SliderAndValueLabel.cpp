/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslSimpleUI/Base/Control/SliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
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
    SliderAndValueLabel::SliderAndValueLabel(const std::shared_ptr<WindowContext>& context)
      : ContentControlBase(context)
      , m_layout(std::make_shared<ComplexStackLayout>(context))
      , m_slider(std::make_shared<Slider>(context))
      , m_label(std::make_shared<ValueLabel>(context))
      , m_cachedMinValue(m_slider->GetMinValue() - 1)
      , m_cachedMaxValue(0)
    {
      // Enable resolve so we can do the final layout tweaks before the layout process starts
      Enable(WindowFlags::ResolveEnabled);

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
      FixLayout();
    }


    const AtlasTexture2D& SliderAndValueLabel::GetBackgroundTexture() const
    {
      return m_slider->GetBackgroundTexture();
    }


    void SliderAndValueLabel::SetBackgroundTexture(const AtlasTexture2D& value)
    {
      m_slider->SetBackgroundTexture(value);
    }


    const AtlasTexture2D& SliderAndValueLabel::GetCursorTexture() const
    {
      return m_slider->GetCursorTexture();
    }


    void SliderAndValueLabel::SetCursorTexture(const AtlasTexture2D& value)
    {
      m_slider->SetCursorTexture(value);
    }


    int32_t SliderAndValueLabel::GetValue() const
    {
      return m_slider->GetValue();
    }


    void SliderAndValueLabel::SetValue(const int32_t& value)
    {
      m_slider->SetValue(value);
    }


    int32_t SliderAndValueLabel::GetMinValue() const
    {
      return m_slider->GetMinValue();
    }


    int32_t SliderAndValueLabel::GetMaxValue() const
    {
      return m_slider->GetMaxValue();
    }


    bool SliderAndValueLabel::SetValueLimits(const int32_t& min, const int32_t& max)
    {
      if (!m_slider->SetValueLimits(min, max))
      {
        return false;
      }
      FixLayout();
      return true;
    }


    const ThicknessF& SliderAndValueLabel::GetBackgroundPadding() const
    {
      return m_slider->GetBackgroundPadding();
    }


    void SliderAndValueLabel::SetBackgroundPadding(const ThicknessF& value)
    {
      m_slider->SetBackgroundPadding(value);
    }


    const ThicknessF& SliderAndValueLabel::GetCursorPadding() const
    {
      return m_slider->GetCursorPadding();
    }


    void SliderAndValueLabel::SetCursorPadding(const ThicknessF& value)
    {
      m_slider->SetCursorPadding(value);
    }


    const NineSlice SliderAndValueLabel::GetNineSlice() const
    {
      return m_slider->GetNineSlice();
    }


    void SliderAndValueLabel::SetNineSlice(const NineSlice& value)
    {
      m_slider->SetNineSlice(value);
    }


    void SliderAndValueLabel::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
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


    Vector2 SliderAndValueLabel::MeasureOverride(const Vector2& availableSize)
    {
      UpdateLinkedContent();
      FixLayout();
      return ContentControlBase::MeasureOverride(availableSize);
    }


    void SliderAndValueLabel::UpdateLinkedContent()
    {
      m_label->SetContent(m_slider->GetValue());
    }


    void SliderAndValueLabel::FixLayout()
    {
      const auto minValue = m_slider->GetMinValue();
      const auto maxValue = m_slider->GetMaxValue();

      if (minValue != m_cachedMinValue || maxValue != m_cachedMaxValue)
      {
        // The way we 'find' the max render size of the number is not a sure thing.
        // For example if the range is 0-9 then the numbers 1-8 could actually be larger when rendered :(
        m_cachedMinValue = minValue;
        m_cachedMaxValue = maxValue;

        Vector2 size1 = m_label->MeasureRenderedValue(minValue);
        Vector2 size2 = m_label->MeasureRenderedValue(maxValue);
        Vector2 finalSize = Vector2(std::max(size1.X, size2.X), std::max(size1.Y, size2.Y));
        m_label->SetHeight(finalSize.Y);

        m_layout->ClearLayoutLengths();
        m_layout->PushLayoutLength(LayoutLength(1.0f, LayoutUnitType::Star));
        m_layout->PushLayoutLength(LayoutLength(finalSize.X, LayoutUnitType::Fixed));
      }
    }
  }
}
