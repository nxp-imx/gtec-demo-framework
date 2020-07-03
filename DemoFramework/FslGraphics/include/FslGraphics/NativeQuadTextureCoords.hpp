#ifndef FSLGRAPHICS_NATIVEQUADTEXTURECOORDS_HPP
#define FSLGRAPHICS_NATIVEQUADTEXTURECOORDS_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/Vector2.hpp>

namespace Fsl
{
  //! Describes a area on the texture in the native texture coordinate system
  struct NativeQuadTextureCoords
  {
    Vector2 TopLeft;
    Vector2 TopRight;
    Vector2 BottomLeft;
    Vector2 BottomRight;

    constexpr NativeQuadTextureCoords() noexcept = default;

    constexpr NativeQuadTextureCoords(const float topLeftU, const float topLeftV, const float topRightU, const float topRightV,
                                      const float bottomLeftU, const float bottomLeftV, const float bottomRightU, const float bottomRightV) noexcept
      : TopLeft(topLeftU, topLeftV)
      , TopRight(topRightU, topRightV)
      , BottomLeft(bottomLeftU, bottomLeftV)
      , BottomRight(bottomRightU, bottomRightV)
    {
    }

    constexpr NativeQuadTextureCoords(const Vector2& topLeft, const Vector2& topRight, const Vector2& bottomLeft, const Vector2& bottomRight) noexcept
      : TopLeft(topLeft)
      , TopRight(topRight)
      , BottomLeft(bottomLeft)
      , BottomRight(bottomRight)
    {
    }

    constexpr bool operator==(const NativeQuadTextureCoords& rhs) const noexcept
    {
      return TopLeft == rhs.TopLeft && TopRight == rhs.TopRight && BottomLeft == rhs.BottomLeft && BottomRight == rhs.BottomRight;
    }

    constexpr bool operator!=(const NativeQuadTextureCoords& rhs) const noexcept
    {
      return !((*this) == rhs);
    }
  };
}

#endif
