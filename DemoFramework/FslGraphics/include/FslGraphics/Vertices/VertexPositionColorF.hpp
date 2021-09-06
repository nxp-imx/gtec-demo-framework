#ifndef FSLGRAPHICS_VERTICES_VERTEXPOSITIONCOLORF_HPP
#define FSLGRAPHICS_VERTICES_VERTEXPOSITIONCOLORF_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexPositionColorF
  {
    Vector3 Position;
    Vector4 Color;

    constexpr VertexPositionColorF() noexcept = default;

    constexpr VertexPositionColorF(const Vector3& position, const Vector4& color) noexcept
      : Position(position)
      , Color(color)
    {
    }

    constexpr VertexPositionColorF(const Vector3& position, const Fsl::Color& color) noexcept
      : Position(position)
      , Color(color.ToVector4())
    {
    }


    constexpr static VertexDeclarationArray<2> GetVertexDeclarationArray()
    {
      constexpr std::array<VertexElementEx, 2> elements = {
        VertexElementEx(offsetof(VertexPositionColorF, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElementEx(offsetof(VertexPositionColorF, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0)};
      return VertexDeclarationArray<2>(elements, sizeof(VertexPositionColorF));
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<2> decl = GetVertexDeclarationArray();
      return decl.AsReadOnlySpan();
    }

    constexpr bool operator==(const VertexPositionColorF& rhs) const
    {
      return Position == rhs.Position && Color == rhs.Color;
    }

    constexpr bool operator!=(const VertexPositionColorF& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
