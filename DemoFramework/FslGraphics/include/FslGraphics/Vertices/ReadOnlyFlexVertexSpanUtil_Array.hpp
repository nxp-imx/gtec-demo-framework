#ifndef FSLGRAPHICS_VERTICES_READONLYFLEXVERTEXSPANUTIL_ARRAY_HPP
#define FSLGRAPHICS_VERTICES_READONLYFLEXVERTEXSPANUTIL_ARRAY_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/OptimizationFlag.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpan.hpp>
#include <array>
#include <cassert>
#include <exception>

namespace Fsl::ReadOnlyFlexVertexSpanUtil
{
  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // std::array<T>
  // -----------------------------------------------------------------------------------------------------------------------------------------------


  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlyFlexVertexSpan AsSpan(const std::array<T, TSize>& value, VertexDeclarationSpan vertexDeclaration)
  {
    if (sizeof(T) != vertexDeclaration.VertexStride())
    {
      throw std::invalid_argument("elements are not compatible with the vertex declaration");
    }
    return ReadOnlyFlexVertexSpan(value.data(), value.size(), vertexDeclaration);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlyFlexVertexSpan AsSpan(const std::array<T, TSize>& value, VertexDeclarationSpan vertexDeclaration,
                                                 const OptimizationCheckFlag flag) noexcept
  {
    assert(sizeof(T) == vertexDeclaration.VertexStride());
    return ReadOnlyFlexVertexSpan(value.data(), value.size(), vertexDeclaration, flag);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlyFlexVertexSpan AsSpan(const std::array<T, TSize>& value)
  {
    const VertexDeclarationSpan vertexDeclaration(T::AsVertexDeclarationSpan());
    if (sizeof(T) != vertexDeclaration.VertexStride())
    {
      throw std::invalid_argument("elements are not compatible with the vertex declaration");
    }
    return ReadOnlyFlexVertexSpan(value.data(), value.size(), vertexDeclaration);
  }

  template <typename T, std::size_t TSize>
  constexpr inline ReadOnlyFlexVertexSpan AsSpan(const std::array<T, TSize>& value, const OptimizationCheckFlag flag)
  {
    const VertexDeclarationSpan vertexDeclaration(T::AsVertexDeclarationSpan());
    assert(sizeof(T) == vertexDeclaration.VertexStride());
    return ReadOnlyFlexVertexSpan(value.data(), value.size(), vertexDeclaration, flag);
  }
}

#endif
