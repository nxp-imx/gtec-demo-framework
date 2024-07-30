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
#include <FslBase/Log/Time/FmtTimeSpan.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/ScrollGestureAnimationConfig.hpp>
#include <FslSimpleUI/Base/Control/ScrollGestureHandler.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBeginBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragEndBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureFlickBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureTapBasicEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <array>

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr DpValueF MinimumVelocity(50.0f);
      constexpr TimeSpan MaxFlickAnimationTime(TimeSpan::FromSeconds(5));
    }

    constexpr DpValueF CalculateSpringDisplacement(const float force, const float springStiffness) noexcept
    {
      if (springStiffness >= 1.0f)
      {
        // Hooke's Law: F = k * x -> x = F / k
        return DpValueF(force / springStiffness);
      }
      // When the spring stiffness is too small we ignore this calculation
      // This basically disables the 'bounce'
      return {};
    }

    constexpr PxValue ApplySpringCollision(const SpriteUnitConverter& unitConverter, const PxValue locationPx, const PxSize1D maxScrollPx,
                                           const float springStiffness)
    {
      if (locationPx >= PxValue(0))
      {
        const float force = unitConverter.ToDpValueF(locationPx).Value;
        return unitConverter.ToPxValue(CalculateSpringDisplacement(force, springStiffness));
      }
      if (-locationPx > maxScrollPx)
      {
        const float force = unitConverter.ToDpValueF(-locationPx - maxScrollPx).Value;
        return -(maxScrollPx.Value() + unitConverter.ToPxValue(CalculateSpringDisplacement(force, springStiffness)));
      }
      return locationPx;
    }

    constexpr PxPoint2 ApplySpringCollision(const SpriteUnitConverter& unitConverter, const PxPoint2 locationPx, const PxSize2D maxScrollPx,
                                            const Vector2 springStiffness)
    {
      return {ApplySpringCollision(unitConverter, locationPx.X, maxScrollPx.Width(), springStiffness.X),
              ApplySpringCollision(unitConverter, locationPx.Y, maxScrollPx.Height(), springStiffness.Y)};
    }

    constexpr float TimeToRest(const DpPoint2F velocityDpf, const DpPoint2F accelerationDp)
    {
      // time = (finalVelocity - initialVelocity)/Acceleration

      // Calculate the magnitude of the acceleration
      const DpValueF a = accelerationDp.Length();

      // Ensure the acceleration is not zero to avoid division by zero
      if (a.Value == 0.0f)
      {
        FSLLOG3_ERROR("Acceleration cannot be zero.");
        return -1;
      }

      // Calculate the magnitude of the initial velocity
      const DpValueF v0 = velocityDpf.Length();

      // Calculate the time to rest
      return v0.Value / a.Value;
    }

    // Function to calculate final position
    constexpr DpPoint2F FinalPosition(const DpPoint2F velocityDpf, const DpPoint2F accelerationDpf, const float time)
    {
      // Using the kinematic equation: delta-x = (v0 * t) + (0.5 * a * t^2)
      const DpValueF tmpDpfX(0.5f * accelerationDpf.X * time * time);
      const DpValueF tmpDpfY(0.5f * accelerationDpf.Y * time * time);
      return {(velocityDpf.X * time) + tmpDpfX, (velocityDpf.Y * time) + tmpDpfY};
    }

    PxPoint2 ApplyScrollMode(const ScrollModeFlags scrollMode, const PxPoint2 value)
    {
      PxPoint2 res = value;
      if (!ScrollModeFlagsUtil::IsEnabled(scrollMode, ScrollModeFlags::TranslateX))
      {
        res.X = PxValue();
      }
      if (!ScrollModeFlagsUtil::IsEnabled(scrollMode, ScrollModeFlags::TranslateY))
      {
        res.Y = PxValue();
      }
      return res;
    }

    DpPoint2F ApplyScrollMode(const ScrollModeFlags scrollMode, const DpPoint2F value)
    {
      float resX = value.X.Value;
      float resY = value.Y.Value;
      if (!ScrollModeFlagsUtil::IsEnabled(scrollMode, ScrollModeFlags::TranslateX))
      {
        resX = 0.0f;
      }
      if (!ScrollModeFlagsUtil::IsEnabled(scrollMode, ScrollModeFlags::TranslateY))
      {
        resY = 0.0f;
      }
      return DpPoint2F::Create(resX, resY);
    }

    DpPoint2F ApplyScrollModeToVelocity(const ScrollModeFlags scrollMode, const DpPoint2F value, const DpValueF minimumVelocityDpf)
    {
      DpPoint2F velocityDpf = ApplyScrollMode(scrollMode, value);
      return velocityDpf.LengthSquared() >= (minimumVelocityDpf * minimumVelocityDpf) ? velocityDpf : DpPoint2F();
    }
  }

  ScrollGestureHandler::ScrollGestureHandler(const uint16_t densityDpi)
    : m_unitConverter(densityDpi)
    , m_gestureManager(UI::GestureFlags::Drag, UI::GestureDetector(), densityDpi)
  {
  }


  void ScrollGestureHandler::ConfigurationChanged(const uint16_t densityDpi)
  {
    // Cancel any ongoing drag operation
    TryCancelDrag();

    m_unitConverter.SetDensityDpi(densityDpi);
  }


  void ScrollGestureHandler::AddMovement(const MillisecondTickCount32 eventTimestamp, const PxPoint2 screenPositionPx, bool isDown)
  {
    if (m_scrollEnabled)
    {
      m_gestureManager.AddMovement(eventTimestamp, screenPositionPx, isDown);
    }
  }


  void ScrollGestureHandler::SetScrollMode(const ScrollModeFlags value) noexcept
  {
    if (value != m_scrollMode)
    {
      // Cancel any ongoing drag
      TryCancelDrag();

      // Cancel any ongoing animation
      m_animRecord.Status = AnimStatus::Idle;
      m_animRecord.Anim.SetActualValue({});

      // Set the scroll mode and apply it to the active scroll offset
      m_scrollMode = value;
      m_scrollOffsetPx = ApplyScrollMode(m_scrollMode, m_scrollOffsetPx);
    }
  }


  bool ScrollGestureHandler::UpdateAnimation(const TimeSpan timeSpan, const ScrollGestureAnimationConfig& config)
  {
    m_config = config;
    bool layoutDirty = m_gestureManager.Process();
    if (m_animRecord.Status != AnimStatus::Idle && !m_animRecord.Anim.IsCompleted())
    {
      m_animRecord.Anim.Update(timeSpan);
      layoutDirty = true;
    }
    return layoutDirty;
  }


  bool ScrollGestureHandler::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    return !m_gestureManager.IsIdle() || m_animRecord.Status != AnimStatus::Idle;
  }


  PxPoint2 ScrollGestureHandler::Arrange(const PxSize2D finalAreaRenderSizePx, const PxSize2D contentRenderSizePx)
  {
    if (!IsScrollingRequired(finalAreaRenderSizePx, contentRenderSizePx))
    {
      m_scrollEnabled = false;
      TryCancelDrag();
      m_animRecord.Status = AnimStatus::Idle;
      m_animRecord.Anim.SetActualValue({});
      m_scrollOffsetPx = {};
      // Clear all recorded movement, any pending gestures and all in progress gestures
      m_gestureManager.Clear();
      return {};
    }
    m_scrollEnabled = true;

    const PxSize2D scrollSizePx(contentRenderSizePx - finalAreaRenderSizePx);
    PxPoint2 locationPx;

    bool arranged = false;

    // Process all pending gestures
    UI::GestureBasicEvent currentEvent{};
    while (m_gestureManager.TryGet(currentEvent))
    {
      switch (currentEvent.Type)
      {
      case UI::GestureEventType::GestureDragBegin:
        {
          const auto typedEvent = UI::GestureDragBeginBasicEvent::Decode(currentEvent);
          BeginDrag(typedEvent.PositionPx);
        }
        break;
      case UI::GestureEventType::GestureDrag:
        {
          const auto typedEvent = UI::GestureDragBasicEvent::Decode(currentEvent);
          TryDrag(typedEvent.PositionPx);
          break;
        }
      case UI::GestureEventType::GestureDragEnd:
        {
          const auto typedEvent = UI::GestureDragEndBasicEvent::Decode(currentEvent);
          TryDragEnd(typedEvent.PositionPx, typedEvent.FlickVelocityDpf);
          break;
        }
      default:
        FSLLOG3_WARNING("Unexpected event type: {}", fmt::underlying(currentEvent.Type));
        break;
      }

      locationPx = ArrangeNow(scrollSizePx);
      arranged = true;
    }

    // We call arrange after each gesture just to give it a chance to react to the new state
    if (!arranged)
    {
      locationPx = ArrangeNow(scrollSizePx);
    }

    // FSLLOG3_INFO("m_scrollOffsetPx {} locationPx {} IsDragging {} Flicking {}", m_scrollOffsetPx, locationPx, fmt::underlying(m_dragRecord.Status),
    //              fmt::underlying(m_animRecord.Status));
    return locationPx;
  }


  bool ScrollGestureHandler::IsScrollingRequired(const PxSize2D finalAreaRenderSizePx, const PxSize2D contentRenderSizePx) noexcept
  {
    switch (m_scrollMode)
    {
    case ScrollModeFlags::TranslateX:
      return contentRenderSizePx.Width() > finalAreaRenderSizePx.Width();
    case ScrollModeFlags::TranslateY:
      return contentRenderSizePx.Height() > finalAreaRenderSizePx.Height();
    case ScrollModeFlags::Translate:
      return contentRenderSizePx.Width() > finalAreaRenderSizePx.Width() || contentRenderSizePx.Height() > finalAreaRenderSizePx.Height();
    case ScrollModeFlags::NotDefined:
      return false;
    }
    return false;
  }


  void ScrollGestureHandler::BeginDrag(const PxPoint2 positionPx)
  {
    if (m_dragRecord.Status != DragStatus::Idle)
    {
      FSLLOG3_VERBOSE3("TryBeginDrag: Cancelling existing drag cancelled due to starting a new drag");
      TryCancelDrag();
    }
    assert(m_dragRecord.Status == DragStatus::Idle);

    FSLLOG3_VERBOSE3("BeginDrag: starting drag at position {}px", positionPx);
    // Apply the current flick position
    if (m_animRecord.Status != AnimStatus::Idle)
    {
      FSLLOG3_VERBOSE3("- Applying animation position");
      m_scrollOffsetPx += TypeConverter::UncheckedChangeTo<PxPoint2>(m_animRecord.Anim.GetValue());
      m_animRecord.Status = AnimStatus::Idle;
      m_animRecord.Anim.SetActualValue({});
    }
    m_dragRecord.Status = DragStatus::Dragging;
  }


  bool ScrollGestureHandler::TryDrag(const PxPoint2 positionPx)
  {
    const bool allowDrag = m_dragRecord.Status == DragStatus::Dragging;
    if (allowDrag)
    {
      m_dragRecord.OffsetPx = ApplyScrollMode(m_scrollMode, positionPx);
      FSLLOG3_VERBOSE3("TryDrag: position {}", m_dragRecord.OffsetPx);
    }
    return allowDrag;
  }


  bool ScrollGestureHandler::TryDragEnd(const PxPoint2 positionPx, const DpPoint2F flickVelocityDpf)
  {
    if (m_dragRecord.Status != DragStatus::Dragging)
    {
      return false;
    }

    m_dragRecord.Status = DragStatus::Completed;
    m_dragRecord.OffsetPx = ApplyScrollMode(m_scrollMode, positionPx);
    m_dragRecord.DragEndFlickVelocityDpf = ApplyScrollModeToVelocity(m_scrollMode, flickVelocityDpf, LocalConfig::MinimumVelocity);
    FSLLOG3_VERBOSE3("TryDragEnd: positionPx {}px velocity: {}dp unclampedVelocity: {}dp", m_dragRecord.OffsetPx,
                     m_dragRecord.DragEndFlickVelocityDpf, flickVelocityDpf);
    return true;
  }


  bool ScrollGestureHandler::TryCancelDrag()
  {
    const bool wasDragging = m_dragRecord.Status != DragStatus::Idle;
    if (wasDragging)
    {
      m_dragRecord = {};
    }
    return wasDragging;
  }


  PxPoint2 ScrollGestureHandler::ArrangeNow(const PxSize2D scrollSizePx)
  {
    if (m_dragRecord.Status == DragStatus::Completed)
    {
      assert(m_animRecord.Status == AnimStatus::Idle);
      const DpPoint2F dragEndFlickVelocityDpf =
        ApplyScrollModeToVelocity(m_scrollMode, m_dragRecord.DragEndFlickVelocityDpf, LocalConfig::MinimumVelocity);
      const PxPoint2 finalPositionPx = ApplyScrollMode(m_scrollMode, m_scrollOffsetPx + m_dragRecord.OffsetPx);

      if (dragEndFlickVelocityDpf != DpPoint2F())
      {
        const auto dragFlickAcceleration(DpValueF::Create(-m_config.DragFlickDeceleration));
        const DpPoint2F dragFlickAccelerationDpf(ApplyScrollMode(m_scrollMode, DpPoint2F(dragFlickAcceleration, dragFlickAcceleration)));

        // A completed drag, turns into either a flick, bounce or no-animation
        const float timeToRest = TimeToRest(dragEndFlickVelocityDpf, dragFlickAccelerationDpf);
        if (timeToRest > 0.0f)
        {
          SelectFlickAnimation(timeToRest, dragFlickAccelerationDpf, finalPositionPx, dragEndFlickVelocityDpf);
        }
        else
        {
          SelectAnimation(finalPositionPx, scrollSizePx);
        }
      }
      else
      {
        // Examine the final drag position and determine if we need to execute a bounce or we are at a resting state (no animation)
        SelectAnimation(finalPositionPx, scrollSizePx);
      }
      m_dragRecord = {};
    }
    else if (m_animRecord.Status == AnimStatus::Flick && m_animRecord.Anim.IsCompleted())
    {
      // Look at the final animation position
      const PxPoint2 finalPositionPx = m_scrollOffsetPx + TypeConverter::UncheckedChangeTo<PxPoint2>(m_animRecord.Anim.GetValue());
      SelectAnimation(finalPositionPx, scrollSizePx);
    }
    else if (m_animRecord.Status == AnimStatus::Bounce && m_animRecord.Anim.IsCompleted())
    {
      // Apply the final animation position
      m_scrollOffsetPx += TypeConverter::UncheckedChangeTo<PxPoint2>(m_animRecord.Anim.GetValue());
      m_animRecord.Status = AnimStatus::Idle;
      m_animRecord.Anim.SetActualValue({});
    }

    // finally calculate the current location
    PxPoint2 locationPx = m_scrollOffsetPx;
    if (m_dragRecord.Status == DragStatus::Dragging)
    {
      locationPx += m_dragRecord.OffsetPx;
    }
    else if (m_animRecord.Status != AnimStatus::Idle)
    {
      locationPx += TypeConverter::UncheckedChangeTo<PxPoint2>(m_animRecord.Anim.GetValue());
    }
    else
    {
      // Handle the case where m_scrollOffsetPx has gone out of bounds.
      // Which can occur if a animation is running and new drag is initiated and then canceled
      SelectAnimation(m_scrollOffsetPx, scrollSizePx);
    }
    return ApplySpringCollision(m_unitConverter, locationPx, scrollSizePx, m_config.BounceSpringStiffness);
  }


  void ScrollGestureHandler::SelectAnimation(const PxPoint2 finalPositionPx, const PxSize2D scrollSizePx)
  {
    const PxValue minX(-scrollSizePx.Width().Value());
    const PxValue minY(-scrollSizePx.Height().Value());
    constexpr PxValue MaxX(0);
    constexpr PxValue MaxY(0);

    // Apply the final position right away
    if (finalPositionPx.X > MaxX || finalPositionPx.Y > MaxY || finalPositionPx.X < minX || finalPositionPx.Y < minY)
    {
      FSLLOG3_VERBOSE3("- Bounce");

      // We need a bounce animation
      PxPoint2 clippedFinalPositionPx = finalPositionPx;
      PxPoint2 startOffsetPx;

      if (finalPositionPx.X > MaxX)
      {
        startOffsetPx.X = finalPositionPx.X - MaxX;
        clippedFinalPositionPx.X = MaxX;
      }
      if (finalPositionPx.Y > MaxY)
      {
        startOffsetPx.Y = finalPositionPx.Y - MaxY;
        clippedFinalPositionPx.Y = MaxY;
      }
      if (finalPositionPx.X < minX)
      {
        startOffsetPx.X = finalPositionPx.X - minX;
        clippedFinalPositionPx.X = minX;
      }
      if (finalPositionPx.Y < minY)
      {
        startOffsetPx.Y = finalPositionPx.Y - minY;
        clippedFinalPositionPx.Y = minY;
      }

      m_animRecord.Status = AnimStatus::Bounce;
      m_animRecord.Anim.SetActualValue(TypeConverter::UncheckedTo<PxVector2>(startOffsetPx));
      m_animRecord.Anim.SetTransitionTime(m_config.BounceAnimationTime, m_config.BounceTransitionType);
      m_animRecord.Anim.SetValue(PxVector2());
      m_scrollOffsetPx = clippedFinalPositionPx;
    }
    else
    {
      FSLLOG3_VERBOSE3("- Done");

      // No animation is need so just apply the final position
      m_animRecord.Status = AnimStatus::Idle;
      m_animRecord.Anim.SetActualValue({});
      m_scrollOffsetPx = finalPositionPx;
    }
  }


  void ScrollGestureHandler::SelectFlickAnimation(const float timeToRest, const DpPoint2F dragFlickAccelerationDpf, const PxPoint2 finalPositionPx,
                                                  const DpPoint2F dragEndFlickVelocityDpf)
  {
    // Execute a flick animation
    const DpPoint2F finalOffsetDpf = FinalPosition(dragEndFlickVelocityDpf, dragFlickAccelerationDpf, timeToRest);
    PxVector2 finalOffsetPxf = m_unitConverter.ToPxVector2(finalOffsetDpf);

    TimeSpan animationTime(static_cast<int64_t>(std::round(timeToRest * TimeSpan::TicksPerSecond * m_config.DragEndAnimTimeMultiplier)));

    if (animationTime >= LocalConfig::MaxFlickAnimationTime)
    {
      animationTime = LocalConfig::MaxFlickAnimationTime;
    }

    FSLLOG3_VERBOSE3(
      "- FlickAnim AnimTime: {} ({:2f}ms) FinalOffsetDp: {} FinalOffsetPx: {}, DragFlickDeceleration: {} "
      "DragEndAnimTimeMultiplier: {} Time to rest: {}s",
      animationTime, animationTime.TotalMilliseconds(), finalOffsetDpf, finalOffsetPxf, m_config.DragFlickDeceleration,
      m_config.DragEndAnimTimeMultiplier, timeToRest);

    m_animRecord.Status = AnimStatus::Flick;
    m_animRecord.Anim.SetActualValue({});
    m_animRecord.Anim.SetTransitionTime(animationTime, m_config.DragFlickTransitionType);
    m_animRecord.Anim.SetValue(finalOffsetPxf);
    m_scrollOffsetPx = finalPositionPx;
  }
}
