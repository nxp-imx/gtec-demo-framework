#ifndef FSLSIMPLEUI_BASE_GESTURE_GESTUREMANAGER_HPP
#define FSLSIMPLEUI_BASE_GESTURE_GESTUREMANAGER_HPP
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
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/Event/EventTransactionState.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/GestureDetector.hpp>
#include <FslSimpleUI/Base/Gesture/GestureFlags.hpp>
#include <FslSimpleUI/Base/Gesture/GestureRecord.hpp>
#include <FslSimpleUI/Base/MovementOwnership.hpp>
#include <FslSimpleUI/Base/MovementTransactionAction.hpp>
#include <deque>
#include <memory>
#include <span>

namespace Fsl::UI
{

  class GestureManager final
  {
    struct DragInfo
    {
      bool IsDragging{false};
      PxPoint2 StartPositionPx;

      void Clear()
      {
        IsDragging = false;
        StartPositionPx = {};
      }
    };

    GestureFlags m_enabledGestures{};
    DragInfo m_dragInfo;
    GestureDetector m_gestureDetection;
    std::deque<GestureBasicEvent> m_eventQueue;
    SpriteUnitConverter m_unitConverter;
    bool m_enabled{false};

  public:
    GestureManager(const GestureFlags enabledGestures, GestureDetector gestureDetection, const uint16_t densityDpi);

    bool IsEnabled() const noexcept;
    void SetEnabled(const bool value);

    GestureAxis GetGestureAxis() const noexcept;
    void SetGestureAxis(const GestureAxis value);

    void SetDpi(const uint16_t densityDpi) noexcept;

    bool IsIdle() const noexcept;

    bool TryGet(GestureBasicEvent& rEvent) noexcept;

    MovementTransactionAction AddMovement(const MillisecondTickCount32 timestamp, const PxPoint2 screenPositionPx, const EventTransactionState state,
                                          const bool isRepeat, const MovementOwnership movementOwnership);

    // Clear all movement and all gestures
    void Clear();

    //! returns true if there are events to be processed
    bool Process();

  private:
    void Tap(const DpPoint2F positionDpf);
    void BeginDrag(const DpPoint2F startPositionDpf);
    void Drag(const DpPoint2F positionDpf);
    void CancelDrag(const DpPoint2F positionDpf);
    void EndDrag(const DpPoint2F positionDpf, const DpPoint2F velocityDpf);
    void Flick(const DpPoint2F positionDp, const DpPoint2F velocityDp);
  };
}

#endif
