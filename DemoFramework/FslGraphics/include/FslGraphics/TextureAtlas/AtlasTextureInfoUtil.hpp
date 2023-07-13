#ifndef FSLGRAPHICS_TEXTUREATLAS_ATLASTEXTUREINFOUTIL_HPP
#define FSLGRAPHICS_TEXTUREATLAS_ATLASTEXTUREINFOUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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

#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <cassert>

namespace Fsl::AtlasTextureInfoUtil
{
  //! @brief Method to take a user supplied src rectangle and transform it to the actual location in the atlas and clipping it against
  //         the actual src trim rect.
  //         Since the 'untrimmed src rect on the texture can start at a negative position we need to work in signed integers.
  //         Hence the need for all the casting.
  //         BEWARE: We also assume that none of the values exceed std::numeric_limit<int32_t>::max()
  inline bool AdjustSourceRect(PxRectangleU32& rPxSrcRect, const AtlasTextureInfo& texInfo, Vector2& rOrigin)
  {
    int32_t clippedSrcRectLeftPx = static_cast<int32_t>(rPxSrcRect.X.Value) + texInfo.OffsetPx.X.Value;
    int32_t clippedSrcRectTopPx = static_cast<int32_t>(rPxSrcRect.Y.Value) + texInfo.OffsetPx.Y.Value;
    int32_t clippedSrcRectRightPx = clippedSrcRectLeftPx + static_cast<int32_t>(rPxSrcRect.Width.Value);
    int32_t clippedSrcRectBottomPx = clippedSrcRectTopPx + static_cast<int32_t>(rPxSrcRect.Height.Value);

    // Early abort
    if (clippedSrcRectLeftPx >= static_cast<int32_t>(texInfo.TrimmedRectPx.RawRight()) ||
        clippedSrcRectTopPx >= static_cast<int32_t>(texInfo.TrimmedRectPx.RawBottom()) ||
        clippedSrcRectRightPx <= static_cast<int32_t>(texInfo.TrimmedRectPx.RawLeft()) ||
        clippedSrcRectBottomPx <= static_cast<int32_t>(texInfo.TrimmedRectPx.RawTop()))
    {
      rPxSrcRect = PxRectangleU32();
      return false;
    }

    if (clippedSrcRectLeftPx < static_cast<int32_t>(texInfo.TrimmedRectPx.RawLeft()))
    {
      rOrigin.X -= static_cast<float>(static_cast<int32_t>(texInfo.TrimmedRectPx.RawLeft()) - clippedSrcRectLeftPx);
      clippedSrcRectLeftPx = static_cast<int32_t>(texInfo.TrimmedRectPx.RawLeft());
    }

    if (clippedSrcRectTopPx < static_cast<int32_t>(texInfo.TrimmedRectPx.RawTop()))
    {
      rOrigin.Y -= static_cast<float>(static_cast<int32_t>(texInfo.TrimmedRectPx.RawTop()) - clippedSrcRectTopPx);
      clippedSrcRectTopPx = static_cast<int32_t>(texInfo.TrimmedRectPx.RawTop());
    }

    if (clippedSrcRectRightPx >= static_cast<int32_t>(texInfo.TrimmedRectPx.RawRight()))
    {
      clippedSrcRectRightPx = static_cast<int32_t>(texInfo.TrimmedRectPx.RawRight());
    }

    if (clippedSrcRectBottomPx >= static_cast<int32_t>(texInfo.TrimmedRectPx.RawBottom()))
    {
      clippedSrcRectBottomPx = static_cast<int32_t>(texInfo.TrimmedRectPx.RawBottom());
    }

    assert(clippedSrcRectLeftPx >= 0);
    assert(clippedSrcRectTopPx >= 0);
    assert(clippedSrcRectRightPx >= clippedSrcRectLeftPx);
    assert(clippedSrcRectBottomPx >= clippedSrcRectTopPx);
    rPxSrcRect = PxRectangleU32::Create(static_cast<uint32_t>(clippedSrcRectLeftPx), static_cast<uint32_t>(clippedSrcRectTopPx),
                                        static_cast<uint32_t>(clippedSrcRectRightPx - clippedSrcRectLeftPx),
                                        static_cast<uint32_t>(clippedSrcRectBottomPx - clippedSrcRectTopPx));
    return true;
  }

}
#endif
