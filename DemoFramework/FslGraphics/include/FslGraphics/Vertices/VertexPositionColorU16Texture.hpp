#ifndef FSLGRAPHICS_VERTICES_VERTEXPOSITIONCOLORU16TEXTURE_HPP
#define FSLGRAPHICS_VERTICES_VERTEXPOSITIONCOLORU16TEXTURE_HPP
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
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/ColorU16.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexPositionColorU16Texture
  {
    using position_type = Vector3;
    using color_type = ColorU16;
    using texture_coordinate_type = Vector2;

    Vector3 Position;
    ColorU16 Color;
    Vector2 TextureCoordinate;

    constexpr VertexPositionColorU16Texture() noexcept = default;

    constexpr VertexPositionColorU16Texture(const Vector3& position, const ColorU16 color, const Vector2 textureCoordinate) noexcept
      : Position(position)
      , Color(color)
      , TextureCoordinate(textureCoordinate)
    {
    }

    constexpr VertexPositionColorU16Texture(const float positionX, const float positionY, const float positionZ, const ColorU16 color,
                                            const float textureCoordinateU, const float textureCoordinateV) noexcept
      : Position(positionX, positionY, positionZ)
      , Color(color)
      , TextureCoordinate(textureCoordinateU, textureCoordinateV)
    {
    }


    static constexpr VertexDeclarationArray<3> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<3> Elements = {
        VertexElement(offsetof(VertexPositionColorU16Texture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElement(offsetof(VertexPositionColorU16Texture, Color), VertexElementFormat::X16Y16Z16W16_UNORM, VertexElementUsage::Color, 0),
        VertexElement(offsetof(VertexPositionColorU16Texture, TextureCoordinate), VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate,
                      0)};
      constexpr VertexDeclarationArray<3> Span(Elements, sizeof(VertexPositionColorU16Texture));
      return Span;
    }

    // IMPROVEMENT: In C++23 this could be a constexpr since static variables are allowed in constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<3> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
    }


    constexpr bool operator==(const VertexPositionColorU16Texture& rhs) const noexcept
    {
      return Position == rhs.Position && Color == rhs.Color && TextureCoordinate == rhs.TextureCoordinate;
    }

    constexpr bool operator!=(const VertexPositionColorU16Texture& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
