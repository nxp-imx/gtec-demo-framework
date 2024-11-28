#ifndef FSLGRAPHICS2D_PROCEDURAL_BUILDER_CLIP2DUTIL_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BUILDER_CLIP2DUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <array>
#include <cassert>

namespace Fsl::Clip2DUtil
{
  //! @brief Given a input array of coordinates where each successive coordinate in the array is >= the previous one
  //!        produce a ReadOnlySpan the encompasses the entries that fit inside the clipReangePxf (x is minimum, y is maximum).
  template <std::size_t TEntries>
  inline constexpr ReadOnlySpan<PxVector2> Clip(std::array<PxVector2, TEntries>& rCoordsPxf, const PxVector2 clipRangePxf) noexcept
  {
    static_assert(TEntries > 0, "the array must contain at least one entry");
    std::size_t startIndex = 0;
    std::size_t index = 0;
    if (clipRangePxf.X < clipRangePxf.Y && rCoordsPxf[0].X < clipRangePxf.Y && rCoordsPxf[TEntries - 1u].X > clipRangePxf.X)
    {
      index = 1;
      // find the first clip index
      const std::size_t endIndex = TEntries;
      while (index < endIndex && rCoordsPxf[index].X < clipRangePxf.X)
      {
        ++index;
      }
      assert(/*index >= 0 &&*/ index <= TEntries);
      --index;
      if (rCoordsPxf[index].X < clipRangePxf.X)
      {
        // we need to clip the coordinate
        const float clippedDeltaPxf = clipRangePxf.X.Value - rCoordsPxf[index].X.Value;
        const float deltaPxf = rCoordsPxf[index + 1u].X.Value - rCoordsPxf[index].X.Value;
        const float percentageLeft = deltaPxf > 0.0f ? clippedDeltaPxf / deltaPxf : 0.0f;
        const float newY = (rCoordsPxf[index + 1u].Y.Value - rCoordsPxf[index].Y.Value) * percentageLeft;
        rCoordsPxf[index] = PxVector2::Create(clipRangePxf.X.Value, rCoordsPxf[index].Y.Value + newY);
      }
      startIndex = index;
      ++index;

      // find the end clip index
      while (index < endIndex && rCoordsPxf[index].X <= clipRangePxf.Y)
      {
        ++index;
      }
      if (index < endIndex)
      {
        // we need to clip the end coordinate
        const std::size_t prevIndex = index - 1u;
        const float clippedDeltaPxf = clipRangePxf.Y.Value - rCoordsPxf[prevIndex].X.Value;
        const float deltaPxf = rCoordsPxf[index].X.Value - rCoordsPxf[prevIndex].X.Value;
        const float percentageLeft = deltaPxf > 0.0f ? clippedDeltaPxf / deltaPxf : 0.0f;
        const float newY = (rCoordsPxf[index].Y.Value - rCoordsPxf[prevIndex].Y.Value) * percentageLeft;
        rCoordsPxf[index] = PxVector2::Create(clipRangePxf.Y.Value, rCoordsPxf[prevIndex].Y.Value + newY);
        ++index;
      }
    }
    assert(startIndex <= rCoordsPxf.size());
    assert(index <= rCoordsPxf.size());
    assert(startIndex <= index);
    return SpanUtil::UncheckedAsReadOnlySpan(rCoordsPxf, startIndex, index - startIndex);
  }

  //! @brief Given a input array of coordinates where each successive coordinate in the array is >= the previous one
  //!        produce a ReadOnlySpan the encompasses the entries that fit inside the clipReangePxf (x is minimum, y is maximum).
  template <std::size_t TEntries>
  inline constexpr SpanRange<std::size_t> ClipToRange(std::array<PxVector2, TEntries>& rCoordsPxf, const PxVector2 clipRangePxf) noexcept
  {
    static_assert(TEntries > 0, "the array must contain at least one entry");
    std::size_t startIndex = 0;
    std::size_t index = 0;
    if (clipRangePxf.X < clipRangePxf.Y && rCoordsPxf[0].X < clipRangePxf.Y && rCoordsPxf[TEntries - 1u].X > clipRangePxf.X)
    {
      index = 1;
      // find the first clip index
      const std::size_t endIndex = TEntries;
      while (index < endIndex && rCoordsPxf[index].X < clipRangePxf.X)
      {
        ++index;
      }
      assert(/*index >= 0 &&*/ index <= TEntries);
      --index;
      if (rCoordsPxf[index].X < clipRangePxf.X)
      {
        // we need to clip the coordinate
        const float clippedDeltaPxf = clipRangePxf.X.Value - rCoordsPxf[index].X.Value;
        const float deltaPxf = rCoordsPxf[index + 1u].X.Value - rCoordsPxf[index].X.Value;
        const float percentageLeft = deltaPxf > 0.0f ? clippedDeltaPxf / deltaPxf : 0.0f;
        const float newY = (rCoordsPxf[index + 1u].Y.Value - rCoordsPxf[index].Y.Value) * percentageLeft;
        rCoordsPxf[index] = PxVector2::Create(clipRangePxf.X.Value, rCoordsPxf[index].Y.Value + newY);
      }
      startIndex = index;
      ++index;

      // find the end clip index
      while (index < endIndex && rCoordsPxf[index].X <= clipRangePxf.Y)
      {
        ++index;
      }
      if (index < endIndex)
      {
        // we need to clip the end coordinate
        const std::size_t prevIndex = index - 1u;
        const float clippedDeltaPxf = clipRangePxf.Y.Value - rCoordsPxf[prevIndex].X.Value;
        const float deltaPxf = rCoordsPxf[index].X.Value - rCoordsPxf[prevIndex].X.Value;
        const float percentageLeft = deltaPxf > 0.0f ? clippedDeltaPxf / deltaPxf : 0.0f;
        const float newY = (rCoordsPxf[index].Y.Value - rCoordsPxf[prevIndex].Y.Value) * percentageLeft;
        rCoordsPxf[index] = PxVector2::Create(clipRangePxf.Y.Value, rCoordsPxf[prevIndex].Y.Value + newY);
        ++index;
      }
    }
    assert(startIndex <= rCoordsPxf.size());
    assert(index <= rCoordsPxf.size());
    assert(startIndex <= index);
    return {startIndex, index - startIndex};
  }


  inline constexpr bool Clip(PxAreaRectangleF& rRectanglePxf, NativeTextureArea& rTextureArea, const PxAreaRectangleF& clipRectanglePxf) noexcept
  {
    // x1 is always >= x2
    // y1 is always >= y2
    if (!(rRectanglePxf.Right() <= clipRectanglePxf.Left() || rRectanglePxf.Left() >= clipRectanglePxf.Right() ||
          rRectanglePxf.Bottom() <= clipRectanglePxf.Top() || rRectanglePxf.Top() >= clipRectanglePxf.Bottom()))
    {
      const PxValueF dx = rRectanglePxf.Right() - rRectanglePxf.Left();
      const PxValueF dy = rRectanglePxf.Bottom() - rRectanglePxf.Top();
      if (dx <= PxValueF() || dy <= PxValueF())
      {
        return true;
      }

      PxValueF clippedTargetL = rRectanglePxf.Left();
      PxValueF clippedTargetT = rRectanglePxf.Top();
      PxValueF clippedTargetR = rRectanglePxf.Right();
      PxValueF clippedTargetB = rRectanglePxf.Bottom();
      float clippedU0 = rTextureArea.X0;
      float clippedU1 = rTextureArea.X1;
      float clippedV0 = rTextureArea.Y0;
      float clippedV1 = rTextureArea.Y1;

      const float du = clippedU1 - clippedU0;

      if (rRectanglePxf.Left() < clipRectanglePxf.Left())
      {
        const PxValueF dxClip = clipRectanglePxf.Left() - rRectanglePxf.Left();
        const float len = (dxClip / dx).Value;

        clippedU0 = clippedU0 + (du * len);
        clippedTargetL = clipRectanglePxf.Left();
      }
      if (rRectanglePxf.Right() > clipRectanglePxf.Right())
      {
        const PxValueF dxClip = rRectanglePxf.Right() - clipRectanglePxf.Right();
        const float len = (dxClip / dx).Value;
        clippedU1 = clippedU1 - (du * len);
        clippedTargetR = clipRectanglePxf.Right();
      }

      const float dv = clippedV1 - clippedV0;

      if (rRectanglePxf.Top() < clipRectanglePxf.Top())
      {
        const PxValueF dyClip = clipRectanglePxf.Top() - rRectanglePxf.Top();
        const float len = (dyClip / dy).Value;

        clippedV0 = clippedV0 + (dv * len);
        clippedTargetT = clipRectanglePxf.Top();
      }
      if (rRectanglePxf.Bottom() > clipRectanglePxf.Bottom())
      {
        const PxValueF dyClip = rRectanglePxf.Bottom() - clipRectanglePxf.Bottom();
        const float len = (dyClip / dy).Value;
        clippedV1 = rTextureArea.Y1 - (dv * len);
        clippedTargetB = clipRectanglePxf.Bottom();
      }
      rRectanglePxf = PxAreaRectangleF::FromLeftTopRightBottom(clippedTargetL, clippedTargetT, clippedTargetR, clippedTargetB);
      rTextureArea = NativeTextureArea(clippedU0, clippedV0, clippedU1, clippedV1);
      return false;
    }
    return true;
  }
}

#endif
