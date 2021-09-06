#ifndef FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTQUAD_HPP
#define FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTQUAD_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Color.hpp>

namespace Fsl
{
  struct TestQuad
  {
    Vector2 Vec0;
    Vector2 Vec1;
    Vector2 Vec2;
    Vector2 Vec3;
    Vector2 TexCoords0;
    Vector2 TexCoords1;
    Fsl::Color Color;

    constexpr TestQuad() = default;

    constexpr TestQuad(const Vector2& vec0, const Vector2& vec1, const Vector2& vec2, const Vector2& vec3, const Vector2& texCoords0,
                       const Vector2& texCoords1, const Fsl::Color& color)
      : Vec0(vec0)
      , Vec1(vec1)
      , Vec2(vec2)
      , Vec3(vec3)
      , TexCoords0(texCoords0)
      , TexCoords1(texCoords1)
      , Color(color)
    {
    }


    constexpr bool operator==(const TestQuad& rhs) const
    {
      return Vec0 == rhs.Vec0 && Vec1 == rhs.Vec1 && Vec2 == rhs.Vec2 && Vec3 == rhs.Vec3 && TexCoords0 == rhs.TexCoords0 &&
             TexCoords1 == rhs.TexCoords1 && Color == rhs.Color;
    }

    constexpr bool operator!=(const TestQuad& rhs) const
    {
      return !(*this == rhs);
    }
  };


}

#endif
