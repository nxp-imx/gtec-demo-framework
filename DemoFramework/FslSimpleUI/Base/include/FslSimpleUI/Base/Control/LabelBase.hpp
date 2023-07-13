#ifndef FSLSIMPLEUI_BASE_CONTROL_LABELBASE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LABELBASE_HPP
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

#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
#include <string>

namespace Fsl
{
  class SpriteFont;

  namespace UI
  {
    class WindowContext;
    class LabelBase : public BaseWindow
    {
      using base_type = BaseWindow;

      PxSize2D m_cachedMeasureMinimalFontSizePx;

    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      SimpleSpriteFontMesh m_fontMesh;
      DataBinding::TypedDependencyProperty<bool> m_propertyIsEnabled{true};
      DataBinding::TypedDependencyProperty<Color> m_propertyFontColor{DefaultColor::Palette::Font};
      DataBinding::TypedDependencyProperty<Color> m_propertyFontDisabledColor{DefaultColor::Palette::FontDisabled};
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentX;
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentY;

    public:
      static DataBinding::DependencyPropertyDefinition PropertyIsEnabled;
      static DataBinding::DependencyPropertyDefinition PropertyFontColor;
      static DataBinding::DependencyPropertyDefinition PropertyFontDisabledColor;
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentX;
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentY;

      explicit LabelBase(const std::shared_ptr<WindowContext>& context);

      bool IsEnabled() const noexcept
      {
        return m_propertyIsEnabled.Get();
      }

      bool SetEnabled(const bool value);


      ItemAlignment GetContentAlignmentX() const noexcept
      {
        return m_propertyContentAlignmentX.Get();
      };

      bool SetContentAlignmentX(const ItemAlignment value);

      ItemAlignment GetContentAlignmentY() const noexcept
      {
        return m_propertyContentAlignmentY.Get();
      };

      bool SetContentAlignmentY(const ItemAlignment value);


      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_fontMesh.GetSprite();
      }

      void SetFont(const std::shared_ptr<SpriteFont>& value);

      Color GetFontColor() const noexcept
      {
        return m_propertyFontColor.Get();
      }

      bool SetFontColor(const Color value);

      Color GetFontDisabledColor() const noexcept
      {
        return m_propertyFontDisabledColor.Get();
      }

      bool SetFontDisabledColor(const Color value);

      void WinDraw(const UIDrawContext& context) override;

    protected:
      virtual StringViewLite DoGetContent() const = 0;

      void DoSetContent(const StringViewLite value);

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      PxSize2D DoMeasureRenderedString(const std::string& value) const
      {
        return m_fontMesh.Measure(StringViewLiteUtil::AsStringViewLite(value));
      }

      PxSize2D DoMeasureRenderedString(const StringViewLite& value) const
      {
        return m_fontMesh.Measure(value);
      }

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
    };
  }
}

#endif
