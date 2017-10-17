#ifndef GLES3_TESSELLATIONSAMPLE_SLIDERCONTROL_HPP
#define GLES3_TESSELLATIONSAMPLE_SLIDERCONTROL_HPP
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

#include <FslSimpleUI/Base/Control/ContentControlBase.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/ValueLabel.hpp>

namespace Fsl
{

  class SliderControl : public UI::ContentControlBase
  {
    std::shared_ptr<UI::Label> m_labelText;
    std::shared_ptr<UI::Slider> m_slider;
    std::shared_ptr<UI::ValueLabel> m_labelValue;
  public:
    SliderControl(const std::shared_ptr<UI::WindowContext>& context, const std::string& name, const int32_t min, const int32_t max, const float widthF1, const float widthF2, const float widthF3);

    int32_t GetValue() const { return m_slider->GetValue(); }
    void SetValue(const int32_t value) { m_slider->SetValue(value); }

    int32_t GetMinValue() const { return m_slider->GetMinValue(); }
    int32_t GetMaxValue() const { return m_slider->GetMaxValue(); }
    void SetValueLimits(const int32_t& min, const int32_t& max) { m_slider->SetValueLimits(min, max);  }

    std::shared_ptr<UI::Slider> GetSlider() const { return m_slider; }
  protected:
    virtual void OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent) override;
  };
}

#endif
