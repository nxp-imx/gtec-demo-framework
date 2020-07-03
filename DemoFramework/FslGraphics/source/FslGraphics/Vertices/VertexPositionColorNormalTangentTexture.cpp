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

#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics/Color.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  VertexPositionColorNormalTangentTexture::VertexPositionColorNormalTangentTexture(const Vector3& position, const Fsl::Color& color,
                                                                                   const Vector3& normal, const Vector3& tangent,
                                                                                   const Vector2& textureCoordinate)
    : Position(position)
    , Color(color.ToVector4())
    , Normal(normal)
    , Tangent(tangent)
    , TextureCoordinate(textureCoordinate)
  {
  }

  VertexDeclaration VertexPositionColorNormalTangentTexture::GetVertexDeclaration()
  {
    static constexpr std::array<VertexElementEx, 5> g_elements = {
      VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
      VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0),
      VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0),
      VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Tangent), VertexElementFormat::Vector3, VertexElementUsage::Tangent, 0),
      VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, TextureCoordinate), VertexElementFormat::Vector2,
                      VertexElementUsage::TextureCoordinate, 0)};
    return VertexDeclaration(g_elements.data(), g_elements.size(), sizeof(VertexPositionColorNormalTangentTexture));
  }
}
