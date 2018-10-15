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

#include "SliderControl.hpp"
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
  using namespace UI;

  SliderControl::SliderControl(const std::shared_ptr<WindowContext>& context, const std::string& name, const int32_t min, const int32_t max,
                               const float widthF1, const float widthF2, const float widthF3)
    : ContentControlBase(context)
    , m_labelText(new Label(context))
    , m_slider(new Slider(context))
    , m_labelValue(new ValueLabel(context))
  {
    m_labelText->SetContent(name);
    m_labelText->SetAlignmentX(ItemAlignment::Near);
    m_labelText->SetAlignmentY(ItemAlignment::Center);

    m_slider->SetAlignmentX(ItemAlignment::Stretch);
    m_slider->SetAlignmentY(ItemAlignment::Center);
    m_slider->SetValueLimits(min, max);

    m_labelValue->SetContent(m_slider->GetValue());
    m_labelValue->SetAlignmentX(ItemAlignment::Far);
    m_labelValue->SetAlignmentY(ItemAlignment::Center);


    std::shared_ptr<ComplexStackLayout> layout(new ComplexStackLayout(context));
    layout->SetAlignmentX(ItemAlignment::Stretch);
    layout->SetAlignmentY(ItemAlignment::Center);
    layout->SetLayoutOrientation(LayoutOrientation::Horizontal);
    layout->AddChild(m_labelText);
    layout->AddChild(m_slider);
    layout->AddChild(m_labelValue);

    layout->PushLayoutLength(LayoutLength(widthF1, LayoutUnitType::Fixed));
    layout->PushLayoutLength(LayoutLength(widthF2, LayoutUnitType::Fixed));
    layout->PushLayoutLength(LayoutLength(widthF3, LayoutUnitType::Fixed));

    DoSetContent(layout);
  }


  void SliderControl::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
  {
    if (!theEvent->IsHandled())
    {
      // A bit nasty to have to ask the window manager to do it but at least we dont have to use the shared_from_this pattern then
      // It is done this way until we get time to do a cleaner interface
      auto uiContext = GetContext()->TheUIContext.Get();
      uiContext->WindowManager->SYS_SetEventSource(theEvent.get(), this);
    }

    m_labelValue->SetContent(m_slider->GetValue());
  }
}
