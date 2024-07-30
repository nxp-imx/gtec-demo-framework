// #define LOCAL_SANITY_CHECK
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxValue.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxVector2.hpp>
#include <FslBase/Log/Time/FmtTimeSpan.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <Shared/UI/Gestures/MoveableRectangleManager.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <span>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr DpValueF DefaultDragEndFlickAccelerationDpf(-20000.0f);    // dp per second
      constexpr int16_t DefaultDragEndAnimTimeMod = 4;
    }

    constexpr PxPoint2 ApplyCollision(const PxSize2D collisionAreaSizePx, PxPoint2 positionPx, const PxSize2D sizePx) noexcept
    {
      if ((positionPx.X + sizePx.Width()) > collisionAreaSizePx.Width())
      {
        positionPx.X = collisionAreaSizePx.Width() - sizePx.Width();
      }
      if ((positionPx.Y + sizePx.Height()) > collisionAreaSizePx.Height())
      {
        positionPx.Y = collisionAreaSizePx.Height() - sizePx.Height();
      }
      if (positionPx.X.Value < 0)
      {
        positionPx.X = PxValue::Create(0);
      }
      if (positionPx.Y.Value < 0)
      {
        positionPx.Y = PxValue::Create(0);
      }
      return positionPx;
    }

    constexpr PxVector2 ApplyCollision(const PxSize2D collisionAreaSizePx, PxVector2 positionPxf, const PxSize2D sizePx) noexcept
    {
      if ((positionPxf.X + TypeConverter::To<PxValueF>(sizePx.Width())) > TypeConverter::To<PxValueF>(collisionAreaSizePx.Width()))
      {
        positionPxf.X = TypeConverter::To<PxValueF>(collisionAreaSizePx.Width() - sizePx.Width());
      }
      if ((positionPxf.Y + TypeConverter::To<PxValueF>(sizePx.Height())) > TypeConverter::To<PxValueF>(collisionAreaSizePx.Height()))
      {
        positionPxf.Y = TypeConverter::To<PxValueF>(collisionAreaSizePx.Height() - sizePx.Height());
      }
      if (positionPxf.X.Value < 0.0f)
      {
        positionPxf.X = PxValueF::Create(0);
      }
      if (positionPxf.Y.Value < 0.0f)
      {
        positionPxf.Y = PxValueF::Create(0);
      }
      return positionPxf;
    }

    void ApplyDropStartEffect(std::span<MoveableRectangleRecord> records) noexcept
    {
      for (MoveableRectangleRecord& rRecord : records)
      {
        const auto finalLocationPx = rRecord.LocationPx;
        rRecord.LocationPx = PxPoint2(finalLocationPx.X, -rRecord.SizePx.Height().Value() * PxValue(2));
        rRecord.Flicking = true;
        rRecord.FlickAnim.SetTransitionTime(TimeSpan::FromMilliseconds(500));
        rRecord.FlickAnim.SetValue(PxVector2(PxValueF(), TypeConverter::UncheckedTo<PxValueF>(finalLocationPx.Y - rRecord.LocationPx.Y)));
      }
    }

    std::array<MoveableRectangleRecord, 3> CreateRectangleRecords(const PxSize2D windowSizePx,
                                                                  const SpriteUnitConverter& spriteUnitConverter) noexcept
    {
      const PxSize1D size0Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(100)));
      const PxSize1D size1Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(92)));
      const PxSize1D size2Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(84)));

      const PxSize1D px2 = PxSize1D::Create(2);

      const auto location0Px = PxPoint2((windowSizePx.Width() - size0Px) / px2, (windowSizePx.Height() - size0Px) / px2);
      const auto location1Px = PxPoint2((windowSizePx.Width() - size1Px) / px2, (windowSizePx.Height() - size1Px) / px2);
      const auto location2Px = PxPoint2((windowSizePx.Width() - size2Px) / px2, (windowSizePx.Height() - size2Px) / px2);
      const auto rectSize0Px = PxSize2D(size0Px, size0Px);
      const auto rectSize1Px = PxSize2D(size1Px, size1Px);
      const auto rectSize2Px = PxSize2D(size2Px, size2Px);

      return {MoveableRectangleRecord(location0Px, rectSize0Px, UI::UIColors::Red()),
              MoveableRectangleRecord(location1Px, rectSize1Px, UI::UIColors::Green()),
              MoveableRectangleRecord(location2Px, rectSize2Px, UI::UIColors::Blue())};
    }


    void UpdateRectangleRecords(std::array<MoveableRectangleRecord, 3>& rRecords, const PxSize2D windowSizePx,
                                const SpriteUnitConverter& spriteUnitConverter) noexcept
    {
      const PxSize1D size0Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(100)));
      const PxSize1D size1Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(92)));
      const PxSize1D size2Px(spriteUnitConverter.ToPxSize1D(DpSize1D::Create(84)));
      const auto rectSize0Px = PxSize2D(size0Px, size0Px);
      const auto rectSize1Px = PxSize2D(size1Px, size1Px);
      const auto rectSize2Px = PxSize2D(size2Px, size2Px);

      rRecords[0].SizePx = rectSize0Px;
      rRecords[1].SizePx = rectSize1Px;
      rRecords[2].SizePx = rectSize2Px;

      // Ensure the collision is applied
      for (MoveableRectangleRecord& rEntry : rRecords)
      {
        rEntry.LocationPx = ApplyCollision(windowSizePx, rEntry.LocationPx, rEntry.SizePx);
      }
    }

    // Function to calculate time to rest
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
    DpPoint2F FinalPosition(const DpPoint2F velocityDpf, const DpValueF accelerationDpf, const float time)
    {
      // Using the kinematic equation: delta-x = (v0 * t) + (0.5 * a * t^2)
      const DpValueF tmpDpf(0.5f * accelerationDpf * time * time);
      return {(velocityDpf.X * time) + tmpDpf, (velocityDpf.Y * time) + tmpDpf};
    }

#ifdef LOCAL_SANITY_CHECK
    void SanityCheckPixelPerfect(const PxVector2 position)
    {
      float intPart;
      float fracPart = std::modff(position.X.Value, &intPart);
      if (std::abs(fracPart) >= 0.000000001f)
      {
        throw InternalErrorException(fmt::format("the final resting state is not pixel perfect: {}", position.X.Value));
      }
      fracPart = std::modff(position.Y.Value, &intPart);
      if (std::abs(fracPart) >= 0.000000001f)
      {
        throw InternalErrorException(fmt::format("the final resting state is not pixel perfect: {}", position.Y.Value));
      }
    }
#endif

  }


  MoveableRectangleManager::MoveableRectangleManager(const UI::UIColorSpace colorSpace, const PxSize2D windowSizePx, const uint32_t densityDpi)
    : m_converter(colorSpace)
    , m_cachedWindowSizePx(windowSizePx)
    , m_spriteUnitConverter(densityDpi)
    , m_dragFlickAcceleration(LocalConfig::DefaultDragEndFlickAccelerationDpf)
    , m_dragEndAnimTimeMod(LocalConfig::DefaultDragEndAnimTimeMod)
    , m_records(CreateRectangleRecords(windowSizePx, m_spriteUnitConverter))
  {
    ApplyDropStartEffect(m_records);
  }


  void MoveableRectangleManager::OnConfigurationChanged(const UI::UIColorSpace colorSpace, const uint32_t densityDpi)
  {
    bool changed = false;
    if (colorSpace != m_converter.GetColorSpace())
    {
      changed = true;
      m_converter = UI::UIColorConverter(colorSpace);
    }
    if (densityDpi == m_spriteUnitConverter.GetDensityDpi())
    {
      changed = true;
      m_spriteUnitConverter.SetDensityDpi(densityDpi);
    }
    if (changed)
    {
      OnAreaChanges();
    }
  }


  void MoveableRectangleManager::Arrange(const PxSize2D windowSizePx)
  {
    if (windowSizePx == m_cachedWindowSizePx)
    {
      return;
    }
    m_cachedWindowSizePx = windowSizePx;

    OnAreaChanges();
  }


  bool MoveableRectangleManager::IsAnimating() const noexcept
  {
    for (const auto& entry : m_records)
    {
      if (!entry.FlickAnim.IsCompleted())
      {
        return true;
      }
    }
    return false;
  }


  void MoveableRectangleManager::SetDragFlickDeceleration(const DpValueF value) noexcept
  {
    m_dragFlickAcceleration = DpValueF::Create(-std::abs(value.Value));
  }

  void MoveableRectangleManager::SetToDefaults()
  {
    m_drawInfo = {};
    m_dragRecord = {};
    m_records = CreateRectangleRecords(m_cachedWindowSizePx, m_spriteUnitConverter);
    ApplyDropStartEffect(m_records);
    RecalculateRenderPosition();
  }


  bool MoveableRectangleManager::TryBeginDrag(const PxPoint2 positionPx)
  {
    if (m_dragRecord.IsDragging)
    {
      TryCancelDrag();
      FSLLOG3_VERBOSE3("TryBeginDrag: Existing drag cancelled due to starting a new drag");
    }

    int32_t foundIndex = TryFindDragableEntity(positionPx);
    if (foundIndex >= 0)
    {
      FSLLOG3_VERBOSE3("TryBeginDrag: starting drag of index {} at position {}px", foundIndex, positionPx);
      ApplyCurrentFlickPosition(foundIndex);

      m_dragRecord.IsDragging = true;
      m_dragRecord.Index = UncheckedNumericCast<std::size_t>(foundIndex);
    }
    FSLLOG3_VERBOSE3_IF(foundIndex < 0, "TryBeginDrag: No drag target found at {}px", positionPx);
    return foundIndex >= 0;
  }


  bool MoveableRectangleManager::TryDrag(const PxPoint2 positionPx)
  {
    if (m_dragRecord.IsDragging)
    {
      FSLLOG3_VERBOSE3("TryDrag: position {}", positionPx);
      m_dragRecord.OffsetPx = positionPx;
    }
    return m_dragRecord.IsDragging;
  }


  bool MoveableRectangleManager::TryDragEnd(const PxPoint2 positionPx, const DpPoint2F flickVelocityDpf)
  {
    if (!m_dragRecord.IsDragging)
    {
      return false;
    }
    FSLLOG3_VERBOSE3("TryDragEnd: positionPx {}px velocity: {}dp", positionPx, flickVelocityDpf);

    if (flickVelocityDpf != DpPoint2F())
    {
      const float timeToRest = TimeToRest(flickVelocityDpf, DpPoint2F(m_dragFlickAcceleration, m_dragFlickAcceleration));
      if (timeToRest > 0.0f)
      {
        const DpPoint2F finalOffsetDpf = FinalPosition(flickVelocityDpf, m_dragFlickAcceleration, timeToRest);
        const PxVector2 finalOffsetPxf = m_spriteUnitConverter.ToPxVector2(finalOffsetDpf);

        const TimeSpan animationTime(static_cast<int64_t>(std::round(timeToRest * TimeSpan::TicksPerSecond * m_dragEndAnimTimeMod)));
        FSLLOG3_VERBOSE("- Time to rest: {}s FinalOffsetDp: {} FinalOffsetPx: {}, AnimTime: {} ({:2f}ms)", timeToRest, finalOffsetDpf, finalOffsetPxf,
                        animationTime, animationTime.TotalMilliseconds());

        MoveableRectangleRecord& rRecord = m_records[m_dragRecord.Index];
        rRecord.Flicking = true;
        rRecord.FlickAnim.SetTransitionTime(animationTime);
        rRecord.FlickAnim.SetActualValue(PxVector2());
        rRecord.FlickAnim.SetValue(finalOffsetPxf);
      }
    }

    CompleteDrag();
    return true;
  }


  bool MoveableRectangleManager::TryCancelDrag()
  {
    const bool wasDragging = m_dragRecord.IsDragging;
    if (wasDragging)
    {
      m_dragRecord = {};
    }
    return wasDragging;
  }


  void MoveableRectangleManager::Process(const TimeSpan elapsedTime)
  {
    for (auto& rEntry : m_records)
    {
      if (rEntry.Flicking)
      {
        rEntry.FlickAnim.Update(elapsedTime);
        if (rEntry.FlickAnim.IsCompleted())
        {
          const auto newLocationPx = rEntry.LocationPx + TypeConverter::UncheckedChangeTo<PxPoint2>(rEntry.FlickAnim.GetValue());
          rEntry.Flicking = false;
          rEntry.LocationPx = ApplyCollision(m_cachedWindowSizePx, newLocationPx, rEntry.SizePx);
          rEntry.FlickAnim.SetActualValue(PxVector2());
        }
      }
    }
    RecalculateRenderPosition();
  }

  int32_t MoveableRectangleManager::TryFindDragableEntity(const PxPoint2 positionPx)
  {
    for (int32_t index = UncheckedNumericCast<int32_t>(m_records.size()) - 1; index >= 0; --index)
    {
      const PxUIRectangle2D entityRect = CalculateFinalRectangle(index);
      if (entityRect.Contains(positionPx))
      {
        return index;
      }
    }
    return -1;
  }


  PxPoint2 MoveableRectangleManager::CalculateFinalLocation(const std::size_t index) const noexcept
  {
    const MoveableRectangleRecord& entry = m_records[index];

    PxPoint2 locationPx = entry.LocationPx;
    if (m_dragRecord.IsDragging && m_dragRecord.Index == index)
    {
      locationPx += m_dragRecord.OffsetPx;
    }
    else if (entry.Flicking)
    {
      locationPx += TypeConverter::UncheckedChangeTo<PxPoint2>(entry.FlickAnim.GetValue());
    }
    return ApplyCollision(m_cachedWindowSizePx, locationPx, entry.SizePx);
  }


  PxUIRectangle2D MoveableRectangleManager::CalculateFinalRectangle(const std::size_t index) const noexcept
  {
    const MoveableRectangleRecord& entry = m_records[index];

    PxVector2 locationPxf;
    if (m_dragRecord.IsDragging && m_dragRecord.Index == index)
    {
      locationPxf = TypeConverter::To<PxVector2>(entry.LocationPx + m_dragRecord.OffsetPx);
    }
    else if (entry.Flicking)
    {
      locationPxf = TypeConverter::To<PxVector2>(entry.LocationPx) + entry.FlickAnim.GetValue();
    }
    else
    {
      locationPxf = TypeConverter::To<PxVector2>(entry.LocationPx);
    }
    return {ApplyCollision(m_cachedWindowSizePx, locationPxf, entry.SizePx), entry.SizePx};
  }


  void MoveableRectangleManager::CompleteDrag()
  {
    assert(m_dragRecord.IsDragging);

    m_records[m_dragRecord.Index].LocationPx =
      ApplyCollision(m_cachedWindowSizePx, m_records[m_dragRecord.Index].LocationPx + m_dragRecord.OffsetPx, m_records[m_dragRecord.Index].SizePx);
    m_dragRecord = {};
  }


  void MoveableRectangleManager::ApplyCurrentFlickPositions() noexcept
  {
    for (std::size_t i = 0; i < m_records.size(); ++i)
    {
      ApplyCurrentFlickPosition(i);
    }
  }


  void MoveableRectangleManager::ApplyCurrentFlickPosition(const std::size_t index)
  {
    MoveableRectangleRecord& rEntry = m_records[index];
    if (rEntry.Flicking)
    {
      rEntry.LocationPx = CalculateFinalLocation(index);
      rEntry.Flicking = false;
      rEntry.FlickAnim.SetActualValue(PxVector2());
    }
  }


  void MoveableRectangleManager::OnAreaChanges()
  {
    if (m_dragRecord.IsDragging)
    {
      // Configuration changes -> cancel the drag
      TryCancelDrag();
      FSLLOG3_VERBOSE3("Drag cancelled, due to area size changes");
    }


    if (m_firstArrange)
    {
      m_records = CreateRectangleRecords(m_cachedWindowSizePx, m_spriteUnitConverter);
      ApplyDropStartEffect(m_records);
      m_firstArrange = false;
    }
    else
    {
      // Ensure that the rectangles has the right size for the current configuration
      UpdateRectangleRecords(m_records, m_cachedWindowSizePx, m_spriteUnitConverter);
    }
    RecalculateRenderPosition();
  }


  void MoveableRectangleManager::RecalculateRenderPosition()
  {
    // Calculate the render position
    for (std::size_t index = 0; index < m_records.size(); ++index)
    {
      m_drawInfo[index] = MoveableRectangleInfo(CalculateFinalRectangle(index), m_converter.Convert(m_records[index].RenderColor));
    }
#ifdef LOCAL_SANITY_CHECK
    {
      for (std::size_t index = 0; index < m_records.size(); ++index)
      {
        if (m_dragRecord.IsDragging && m_dragRecord.Index == index)
        {
          if (m_records[index].Flicking)
          {
            // A record being dragged so not be in the flicking state
            throw InternalErrorException("Internal state is corrupt");
          }
          // While dragging we expect pixel perfect coordinates
          SanityCheckPixelPerfect(m_drawInfo[index].RectanglePxf.Offset);
        }
        else if (!m_records[index].Flicking)
        {
          // While stationary we expect pixel perfect coordinates
          SanityCheckPixelPerfect(m_drawInfo[index].RectanglePxf.Offset);
        }
      }
    }
#endif
  }


}
