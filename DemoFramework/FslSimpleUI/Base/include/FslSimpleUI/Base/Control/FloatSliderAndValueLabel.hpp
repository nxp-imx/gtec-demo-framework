#ifndef FSLSIMPLEUI_BASE_CONTROL_FLOATSLIDERANDVALUELABEL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_FLOATSLIDERANDVALUELABEL_HPP
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
#include <FslSimpleUI/Base/Control/ContentControlBase.hpp>
#include <FslSimpleUI/Base/Control/FloatSlider.hpp>
#include <FslSimpleUI/Base/Control/FloatValueLabel.hpp>

namespace Fsl
{
  namespace UI
  {

    class ComplexStackLayout;

    class FloatSliderAndValueLabel : public ContentControlBase
    {
      std::shared_ptr<ComplexStackLayout> m_layout;
      std::shared_ptr<FloatSlider> m_slider;
      std::shared_ptr<FloatValueLabel> m_label;
      float m_cachedMinValue;
      float m_cachedMaxValue;
      uint32_t m_cachedFormatDecimals;
    public:
      FloatSliderAndValueLabel(const std::shared_ptr<WindowContext>& context);

      const AtlasTexture2D& GetBackgroundTexture() const;
      void SetBackgroundTexture(const AtlasTexture2D& value);

      const AtlasTexture2D& GetCursorTexture() const;
      void SetCursorTexture(const AtlasTexture2D& value);

      float GetValue() const;
      void SetValue(const float& value);

      float GetMinValue() const;
      float GetMaxValue() const;
      bool SetValueLimits(const float& min, const float& max);

      const ThicknessF& GetBackgroundPadding() const;
      void SetBackgroundPadding(const ThicknessF& value);

      const ThicknessF& GetCursorPadding() const;
      void SetCursorPadding(const ThicknessF& value);

      const NineSlice GetNineSlice() const;
      void SetNineSlice(const NineSlice& value);

      int32_t GetFormatDecimals() const;
      void SetFormatDecimals(const uint32_t value);

    protected:
      virtual void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) override;
      virtual Vector2 MeasureOverride(const Vector2& availableSize) override;
    private:
      void UpdateLinkedContent();
      void FixLayout();
    };
  }
}

#endif
