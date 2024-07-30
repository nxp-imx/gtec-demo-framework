#ifndef SHARED_UI_GESTURES_MOVEABLERECTANGLEMANAGER_HPP
#define SHARED_UI_GESTURES_MOVEABLERECTANGLEMANAGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxUIRectangle2D.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>
#include <Shared/UI/Gestures/MoveableRectangleInfo.hpp>
#include <Shared/UI/Gestures/MoveableRectangleRecord.hpp>
#include <array>
#include <span>

namespace Fsl
{
  namespace UI
  {
    class UIColorConverter;
  }

  class MoveableRectangleManager final
  {
    struct DragRecord
    {
      bool IsDragging{false};
      std::size_t Index{0};
      PxPoint2 OffsetPx;
    };

    UI::UIColorConverter m_converter;
    PxSize2D m_cachedWindowSizePx;
    SpriteUnitConverter m_spriteUnitConverter;
    DpValueF m_dragFlickAcceleration;
    float m_dragEndAnimTimeMod{};

    std::array<MoveableRectangleRecord, 3> m_records;
    std::array<MoveableRectangleInfo, 3> m_drawInfo;
    DragRecord m_dragRecord;
    bool m_firstArrange{true};

  public:
    MoveableRectangleManager(const UI::UIColorSpace colorSpace, const PxSize2D windowSizePx, const uint32_t densityDpi);

    void OnConfigurationChanged(const UI::UIColorSpace colorSpace, const uint32_t densityDpi);
    void Arrange(const PxSize2D windowSizePx);

    bool IsDragging() const noexcept
    {
      return m_dragRecord.IsDragging;
    }

    bool IsAnimating() const noexcept;

    void SetDragFlickDeceleration(const DpValueF value) noexcept;

    void SetDragEndAnimTimeMod(const float value) noexcept
    {
      m_dragEndAnimTimeMod = value;
    }


    void SetToDefaults();

    bool TryBeginDrag(const PxPoint2 positionPx);
    bool TryDrag(const PxPoint2 positionPx);
    bool TryDragEnd(const PxPoint2 positionPx, const DpPoint2F flickVelocityDpf);
    bool TryCancelDrag();

    void Process(const TimeSpan elapsedTime);

    std::span<const MoveableRectangleInfo> GetDrawInfoSpan() const
    {
      return m_drawInfo;
    }

  private:
    int32_t TryFindDragableEntity(const PxPoint2 positionPx);
    PxPoint2 CalculateFinalLocation(const std::size_t index) const noexcept;
    PxUIRectangle2D CalculateFinalRectangle(const std::size_t index) const noexcept;
    void CompleteDrag();

    void ApplyCurrentFlickPositions() noexcept;

    //! If the rectangle is being flicked, stop the flick and apply the current position to the rectangle
    void ApplyCurrentFlickPosition(const std::size_t index);

    void OnAreaChanges();
    void RecalculateRenderPosition();
  };
}


#endif
