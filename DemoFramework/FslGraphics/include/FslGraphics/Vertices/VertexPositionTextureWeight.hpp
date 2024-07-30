#ifndef FSLGRAPHICS_VERTICES_VERTEXPOSITIONTEXTUREWEIGHT_HPP
#define FSLGRAPHICS_VERTICES_VERTEXPOSITIONTEXTUREWEIGHT_HPP
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

#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexPositionTextureWeight
  {
    using position_type = Vector3;
    using texture_coordinate_type = Vector2;
    using weight_type = float;

    Vector3 Position;
    Vector2 TextureCoordinate;
    float Weight{0.0f};

    constexpr VertexPositionTextureWeight() noexcept = default;

    constexpr VertexPositionTextureWeight(const Vector3& position, const Vector2& texture, const float weight) noexcept
      : Position(position)
      , TextureCoordinate(texture)
      , Weight(weight)
    {
    }

    constexpr static VertexDeclarationArray<3> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<3> Elements = {
        VertexElement(offsetof(VertexPositionTextureWeight, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElement(offsetof(VertexPositionTextureWeight, TextureCoordinate), VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate,
                      0),
        VertexElement(offsetof(VertexPositionTextureWeight, Weight), VertexElementFormat::Single, VertexElementUsage::BlendWeight, 0)};
      return {Elements, sizeof(VertexPositionTextureWeight)};
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<3> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
    }

    constexpr bool operator==(const VertexPositionTextureWeight& rhs) const noexcept
    {
      return Position == rhs.Position && TextureCoordinate == rhs.TextureCoordinate && Weight == rhs.Weight;
    }

    constexpr bool operator!=(const VertexPositionTextureWeight& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
