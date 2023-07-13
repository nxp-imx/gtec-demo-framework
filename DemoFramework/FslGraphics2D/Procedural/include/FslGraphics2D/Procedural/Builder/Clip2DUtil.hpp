#ifndef FSLGRAPHICS2D_PROCEDURAL_BUILDER_CLIP2DUTIL_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BUILDER_CLIP2DUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <array>

namespace Fsl::Clip2DUtil
{
  //! @brief Given a input array of coordinates where each successive coordinate in the array is >= the previous one
  //!        produce a ReadOnlySpan the encompasses the entries that fit inside the clipReangePxf (x is minimum, y is maximum).
  template <std::size_t TEntries>
  constexpr ReadOnlySpan<PxVector2> Clip(std::array<PxVector2, TEntries>& rCoordsPxf, const PxVector2 clipRangePxf) noexcept
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
    return ReadOnlySpan<PxVector2>(rCoordsPxf.data() + startIndex, index - startIndex, OptimizationCheckFlag::NoCheck);
  }
}

#endif
