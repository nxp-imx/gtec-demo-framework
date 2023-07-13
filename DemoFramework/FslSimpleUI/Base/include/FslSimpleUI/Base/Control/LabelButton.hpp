#ifndef FSLSIMPLEUI_BASE_CONTROL_LABELBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LABELBUTTON_HPP
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

#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
#include <string>

namespace Fsl
{
  class SpriteFont;

  namespace UI
  {
    class WindowContext;

    class LabelButton : public ButtonBase
    {
      using base_type = ButtonBase;

      PxSize2D m_cachedMeasureMinimalFontSizePx;

    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      SimpleSpriteFontMesh m_fontMesh;

      DataBinding::TypedDependencyProperty<Color> m_propertyColorUp{DefaultColor::Button::Up};
      DataBinding::TypedDependencyProperty<Color> m_propertyColorDown{DefaultColor::Button::Down};
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyContent;

    public:
      static DataBinding::DependencyPropertyDefinition PropertyColorUp;
      static DataBinding::DependencyPropertyDefinition PropertyColorDown;
      static DataBinding::DependencyPropertyDefinition PropertyContent;

      explicit LabelButton(const std::shared_ptr<WindowContext>& context);

      StringViewLite GetContent() const noexcept
      {
        return m_propertyContent.Get();
      }
      bool SetContent(const char* const value)
      {
        return SetContent(StringViewLite(value));
      }
      bool SetContent(const StringViewLite value);
      bool SetContent(const std::string& value);

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_fontMesh.GetSprite();
      }
      void SetFont(const std::shared_ptr<SpriteFont>& value);


      Color GetColorUp() const noexcept
      {
        return m_propertyColorUp.Get();
      }

      bool SetColorUp(const Color value);


      Color GetColorDown() const noexcept
      {
        return m_propertyColorDown.Get();
      }

      bool SetColorDown(const Color value);

      void WinDraw(const UIDrawContext& context) override;

    protected:
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
    };
  }
}

#endif
