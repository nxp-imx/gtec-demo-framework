#ifndef FSLGRAPHICS_VERTICES_VERTEXMATRIXCOLOR_HPP
#define FSLGRAPHICS_VERTICES_VERTEXMATRIXCOLOR_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexMatrixColor
  {
    Fsl::Matrix Matrix;
    Vector4 Color;

    constexpr VertexMatrixColor() noexcept = default;

    constexpr VertexMatrixColor(const Fsl::Matrix& matrix, const Vector4& color) noexcept
      : Matrix(matrix)
      , Color(color)
    {
    }

    constexpr static VertexDeclarationArray<2> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<2> Elements = {
        VertexElement(offsetof(VertexMatrixColor, Matrix), VertexElementFormat::Matrix4x4, VertexElementUsage::Matrix4x4, 0),
        VertexElement(offsetof(VertexMatrixColor, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0)};
      return {Elements, sizeof(VertexMatrixColor)};
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<2> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
    }


    constexpr bool operator==(const VertexMatrixColor& rhs) const noexcept
    {
      return Matrix == rhs.Matrix && Color == rhs.Color;
    }

    constexpr bool operator!=(const VertexMatrixColor& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
