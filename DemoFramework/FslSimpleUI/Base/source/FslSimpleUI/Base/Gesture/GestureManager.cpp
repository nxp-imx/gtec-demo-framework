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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxVector2.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBeginBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragEndBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureFlickBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureTapBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/GestureManager.hpp>
#include <utility>

namespace Fsl::UI
{
  GestureManager::GestureManager(const GestureFlags enabledGestures, GestureDetector gestureDetection, const uint16_t densityDpi)
    : m_enabledGestures(enabledGestures)
    , m_gestureDetection(std::move(gestureDetection))
    , m_unitConverter(densityDpi)
  {
  }


  void GestureManager::SetDpi(const uint16_t densityDpi) noexcept
  {
    if (densityDpi != m_unitConverter.GetDensityDpi())
    {
      m_unitConverter.SetDensityDpi(densityDpi);
      m_gestureDetection.Clear();
    }
  }


  bool GestureManager::IsIdle() const noexcept
  {
    return m_eventQueue.empty() && !m_gestureDetection.IsGestureAvailable();
  }


  bool GestureManager::TryGet(GestureBasicEvent& rEvent) noexcept
  {
    if (m_eventQueue.empty())
    {
      rEvent = {};
      return false;
    }
    rEvent = m_eventQueue.front();
    m_eventQueue.pop_front();
    return true;
  }

  void GestureManager::AddMovement(const MillisecondTickCount32 timestamp, const PxPoint2 screenPositionPx, const bool isPressed)
  {
    const DpPoint2F screenPositionDpf = m_unitConverter.ToDpPoint2F(screenPositionPx);
    m_gestureDetection.AddMovement(timestamp, screenPositionDpf, isPressed);
  }


  void GestureManager::Clear()
  {
    m_gestureDetection.Clear();
    m_eventQueue.clear();
    m_dragInfo = {};
  }


  bool GestureManager::Process()
  {
    // get any gestures that are ready.
    GestureRecord gs;
    while (m_gestureDetection.TryReadGesture(gs))
    {
      switch (gs.Gesture)
      {
      case GestureType::Tap:
        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Tap))
        {
          FSLLOG3_VERBOSE5("Tap {0}, {1}", gs.PositionDpf, gs.VelocityDpf);
          Tap(gs.PositionDpf);
        }
        break;
      case GestureType::Flick:
        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Flick))
        {
          FSLLOG3_VERBOSE5("Flick {0}, {1}", gs.PositionDpf, gs.VelocityDpf);
          Flick(gs.PositionDpf, gs.VelocityDpf);
        }
        break;
      case GestureType::FreeDrag:
        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
        {
          if (!m_dragInfo.IsDragging)
          {
            BeginDrag(gs.PositionDpf);
          }
          else
          {
            Drag(gs.PositionDpf);
          }
        }
        break;
      case GestureType::DragComplete:
        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Drag))
        {
          if (m_dragInfo.IsDragging)
          {
            EndDrag(gs.PositionDpf, gs.VelocityDpf);
          }
        }
        break;
      case GestureType::Hold:
        if (GestureFlagsUtil::IsEnabled(m_enabledGestures, GestureFlags::Hold))
        {
          FSLLOG3_VERBOSE5("Hold {0}, {1}", gs.PositionDpf, gs.VelocityDpf);
        }
        break;
      case GestureType::Nop:
        break;
      }
    }
    return !m_eventQueue.empty();
  }


  void GestureManager::Tap(const DpPoint2F positionDpf)
  {
    FSLLOG3_VERBOSE5("Tap {}", positionDpf);
    const auto positionPx = m_unitConverter.ToPxPoint2(positionDpf);
    m_eventQueue.push_back(GestureTapBasicEvent(positionPx));
  }


  void GestureManager::BeginDrag(const DpPoint2F startPositionDpf)
  {
    m_dragInfo.IsDragging = true;
    m_dragInfo.StartPositionPx = m_unitConverter.ToPxPoint2(startPositionDpf);

    FSLLOG3_VERBOSE5("BeginDrag {}", startPositionDpf);
    m_eventQueue.push_back(GestureDragBeginBasicEvent(m_dragInfo.StartPositionPx));
  }


  void GestureManager::Drag(const DpPoint2F positionDpf)
  {
    if (!m_dragInfo.IsDragging)
    {
      return;
    }

    FSLLOG3_VERBOSE5("Drag {}", positionDpf);

    const auto deltaPositionPx = m_unitConverter.ToPxPoint2(positionDpf) - m_dragInfo.StartPositionPx;
    m_eventQueue.push_back(GestureDragBasicEvent(deltaPositionPx));
  }


  void GestureManager::EndDrag(const DpPoint2F positionDpf, const DpPoint2F velocityDpf)
  {
    if (!m_dragInfo.IsDragging)
    {
      return;
    }

    m_dragInfo.Clear();
    const auto positionPx = m_unitConverter.ToPxPoint2(positionDpf);

    FSLLOG3_VERBOSE5("EndDrag position: {} velocity: {} DensityDpi: {}", positionDpf, velocityDpf, m_unitConverter.GetDensityDpi());

    m_eventQueue.push_back(GestureDragEndBasicEvent(positionPx, velocityDpf));
  }


  void GestureManager::Flick(const DpPoint2F positionDpf, const DpPoint2F velocityDpf)
  {
    const auto positionPx = m_unitConverter.ToPxPoint2(positionDpf);
    FSLLOG3_VERBOSE5("Flick position: {} velocity: {} DensityDpi: {}", positionDpf, velocityDpf, m_unitConverter.GetDensityDpi());
    m_eventQueue.push_back(GestureFlickBasicEvent(positionPx, velocityDpf));
  }
}
