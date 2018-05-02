#ifndef FSLSIMPLEUI_BASE_CONTROL_SLIDER_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SLIDER_HPP
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

#include <FslBase/Math/ThicknessF.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslBase/Math/NineSlice.hpp>

namespace Fsl
{
  namespace UI
  {
    class Slider : public BaseWindow
    {
      enum class DragState
      {
        Idle,
        Dragging
      };

      AtlasTexture2D m_texBackground;
      AtlasTexture2D m_texCursor;
      float m_percentage;
      int32_t m_value;
      int32_t m_minValue;
      int32_t m_maxValue;
      Rect m_barClickRect;
      DragState m_dragState;
      float m_barStart;
      float m_dragStartPos;
      float m_dragOffset;
      float m_renderXMin;
      float m_renderXMax;
      ThicknessF m_backgroundPadding;
      ThicknessF m_cursorPadding;
      NineSlice m_nineSlice;

    public:
      Slider(const std::shared_ptr<WindowContext>& context);

      //! @brief Check if the slider is being dragged or not (a idle slider is not being dragged)
      bool IsIdle() const;

      const AtlasTexture2D& GetBackgroundTexture() const
      {
        return m_texBackground;
      }
      void SetBackgroundTexture(const AtlasTexture2D& value);

      const AtlasTexture2D& GetCursorTexture() const
      {
        return m_texCursor;
      }
      void SetCursorTexture(const AtlasTexture2D& value);

      int32_t GetValue() const
      {
        return m_value;
      }
      void SetValue(const int32_t& value);

      int32_t GetMinValue() const
      {
        return m_minValue;
      }
      int32_t GetMaxValue() const
      {
        return m_maxValue;
      }
      bool SetValueLimits(const int32_t& min, const int32_t& max);

      const ThicknessF& GetBackgroundPadding() const
      {
        return m_backgroundPadding;
      }
      void SetBackgroundPadding(const ThicknessF& value);

      const ThicknessF& GetCursorPadding() const
      {
        return m_cursorPadding;
      }
      void SetCursorPadding(const ThicknessF& value);

      const NineSlice GetNineSlice() const
      {
        return m_nineSlice;
      }
      void SetNineSlice(const NineSlice& value);

      virtual void WinDraw(const UIDrawContext& context) override;
    protected:
      inline void DoSetValue(const int32_t& value, const int32_t reason);
      virtual void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      virtual Vector2 ArrangeOverride(const Vector2& finalSize) override;
      virtual Vector2 MeasureOverride(const Vector2& availableSize) override;
      inline void RecalcPercentage();
      inline void SetValueBasedOnPosition(const float position);
    };
  }
}

#endif
