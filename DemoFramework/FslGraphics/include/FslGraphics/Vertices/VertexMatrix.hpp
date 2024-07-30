#ifndef FSLGRAPHICS_VERTICES_VERTEXMATRIX_HPP
#define FSLGRAPHICS_VERTICES_VERTEXMATRIX_HPP
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
#include <FslGraphics/Vertices/VertexDeclarationArray.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <array>
#include <cstddef>

namespace Fsl
{
  struct VertexMatrix
  {
    Fsl::Matrix Matrix;

    constexpr VertexMatrix() noexcept = default;

    constexpr explicit VertexMatrix(const Fsl::Matrix& matrix) noexcept
      : Matrix(matrix)
    {
    }

    constexpr static VertexDeclarationArray<1> GetVertexDeclarationArray()
    {
      constexpr BasicVertexDeclarationArray<1> Elements = {
        VertexElement(offsetof(VertexMatrix, Matrix), VertexElementFormat::Matrix4x4, VertexElementUsage::Matrix4x4, 0),
      };
      return {Elements, sizeof(VertexMatrix)};
    }


    // IMPROVEMENT: In C++17 this could be a constexpr since array .data() becomes a constexpr
    //              At least this workaround still gives us compile time validation of the vertex element data
    static VertexDeclarationSpan AsVertexDeclarationSpan()
    {
      constexpr static VertexDeclarationArray<1> Decl = GetVertexDeclarationArray();
      return Decl.AsReadOnlySpan();
    }


    constexpr bool operator==(const VertexMatrix& rhs) const noexcept
    {
      return Matrix == rhs.Matrix;
    }

    constexpr bool operator!=(const VertexMatrix& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
