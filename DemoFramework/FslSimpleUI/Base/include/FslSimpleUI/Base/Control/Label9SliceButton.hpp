#ifndef FSLSIMPLEUI_BASE_CONTROL_LABEL9SLICEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LABEL9SLICEBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Math/NineSlice.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <string>

namespace Fsl
{
  class AtlasFont;

  namespace UI
  {
    class WindowContext;

    class Label9SliceButton : public ButtonBase
    {
    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      std::string m_content;
      std::shared_ptr<AtlasFont> m_font;
      Color m_colorUp;
      Color m_colorDown;

      ThicknessF m_padding;
      AtlasTexture2D m_background;
      NineSlice m_nineSlice;
      ThicknessF m_minimumBackgroundMargin;
      ItemAlignment m_contentAlignmentX{ItemAlignment::Near};
      ItemAlignment m_contentAlignmentY{ItemAlignment::Near};
      Vector2 m_labelSize;

    public:
      Label9SliceButton(const std::shared_ptr<WindowContext>& context);

      ItemAlignment GetContentAlignmentX() const
      {
        return m_contentAlignmentX;
      }

      void SetContentAlignmentX(const ItemAlignment& value);

      ItemAlignment GetContentAlignmentY() const
      {
        return m_contentAlignmentY;
      };

      void SetContentAlignmentY(const ItemAlignment& value);


      ThicknessF GetPadding() const
      {
        return m_padding;
      }

      void SetPadding(const ThicknessF& value);


      const AtlasTexture2D& GetBackground() const
      {
        return m_background;
      }

      void SetBackground(const AtlasTexture2D& value);

      const NineSlice GetNineSlice() const
      {
        return m_nineSlice;
      }

      void SetNineSlice(const NineSlice& value);

      const ThicknessF& GetMinimumBackgroundMargin() const
      {
        return m_minimumBackgroundMargin;
      }

      void SetMinimumBackgroundMargin(const ThicknessF& value);

      const std::string& GetContent() const
      {
        return m_content;
      }

      void SetContent(const std::string& value);

      const std::shared_ptr<AtlasFont>& GetFont() const
      {
        return m_font;
      }
      void SetFont(const std::shared_ptr<AtlasFont>& value);

      Color GetColorUp() const
      {
        return m_colorUp;
      }
      void SetColorUp(const Color& value);

      Color GetColorDown() const
      {
        return m_colorDown;
      }
      void SetColorDown(const Color& value);

      void WinDraw(const UIDrawContext& context) override;

    protected:
      Vector2 ArrangeOverride(const Vector2& finalSize) override;
      Vector2 MeasureOverride(const Vector2& availableSize) override;
    };
  }
}

#endif
