/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslGraphics3D/BasicScene/RawMeshContent.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>

namespace Fsl
{
  namespace Graphics3D
  {
    RawMeshContent::RawMeshContent()
      : pVertices(nullptr)
      , pIndices(nullptr)
      , IndexCount(0)
      , IndexStride(0)
      , VertexCount(0)
      , VertexStride(0)
      , ThePrimitiveType(PrimitiveType::LineList)
    {
    }


    RawMeshContent::RawMeshContent(const void* const pTheVertices, const std::size_t vertexCount, const std::size_t vertexStride,
                                   const void* const pTheIndices, const std::size_t indexCount, const std::size_t indexStride,
                                   const PrimitiveType primitiveType)
      : pVertices(pTheVertices)
      , pIndices(pTheIndices)
      , IndexCount(indexCount)
      , IndexStride(indexStride)
      , VertexCount(vertexCount)
      , VertexStride(vertexStride)
      , ThePrimitiveType(primitiveType)
    {
      if (pVertices == nullptr || pIndices == nullptr)
      {
        throw std::invalid_argument("null pointers not valid");
      }
    }
  }
}
