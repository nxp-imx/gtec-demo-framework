#ifndef FSLGRAPHICS_VERTICES_VERTEXPOSITIONNORMALTEXTURE_HPP
#define FSLGRAPHICS_VERTICES_VERTEXPOSITIONNORMALTEXTURE_HPP
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
  struct VertexPositionNormalTexture
  {
    Vector3 Position;
    Vector3 Normal;
    Vector2 TextureCoordinate;

    constexpr VertexPositionNormalTexture() noexcept = default;

    constexpr VertexPositionNormalTexture(const Vector3& position, const Vector3& normal, const Vector2& textureCoordinate) noexcept
      : Position(position)
      , Normal(normal)
      , TextureCoordinate(textureCoordinate)
    {
    }

    constexpr static VertexDeclarationArray<3> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<3> elements = {
        VertexElement(offsetof(VertexPositionNormalTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
        VertexElement(offsetof(VertexPositionNormalTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0),
        VertexElement(offsetof(VertexPositionNormalTexture, TextureCoordinate), VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate,
                      0)};

      return {elements, sizeof(VertexPositionNormalTexture)};
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<3> decl = GetVertexDeclarationArray();
      return decl.AsReadOnlySpan();
    }

    constexpr bool operator==(const VertexPositionNormalTexture& rhs) const noexcept
    {
      return Position == rhs.Position && Normal == rhs.Normal && TextureCoordinate == rhs.TextureCoordinate;
    }

    constexpr bool operator!=(const VertexPositionNormalTexture& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
