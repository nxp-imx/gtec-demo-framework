#ifndef FSLGRAPHICS_VERTICES_VERTEXPOSITION2_HPP
#define FSLGRAPHICS_VERTICES_VERTEXPOSITION2_HPP
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

#include <FslBase/Math/Vector2.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexPosition2
  {
    using position_type = Vector2;

    Vector2 Position;

    constexpr VertexPosition2() noexcept = default;

    constexpr explicit VertexPosition2(const Vector2& position) noexcept
      : Position(position)
    {
    }

    constexpr VertexPosition2(const float x, const float y) noexcept
      : Position(x, y)
    {
    }

    constexpr static VertexDeclarationArray<1> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<1> Elements = {
        VertexElement(offsetof(VertexPosition2, Position), VertexElementFormat::Vector2, VertexElementUsage::Position, 0),
      };
      return {Elements, sizeof(VertexPosition2)};
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<1> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
    }

    constexpr bool operator==(const VertexPosition2& rhs) const noexcept
    {
      return Position == rhs.Position;
    }

    constexpr bool operator!=(const VertexPosition2& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
