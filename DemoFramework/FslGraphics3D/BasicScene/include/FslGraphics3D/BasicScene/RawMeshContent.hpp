#ifndef FSLGRAPHICS3D_BASICSCENE_RAWMESHCONTENT_HPP
#define FSLGRAPHICS3D_BASICSCENE_RAWMESHCONTENT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/PrimitiveType.hpp>
#include <cstddef>

namespace Fsl
{
  namespace Graphics3D
  {
    struct RawMeshContent
    {
      const void* pVertices;
      const void* pIndices;
      std::size_t IndexCount;
      std::size_t IndexStride;
      std::size_t VertexCount;
      std::size_t VertexStride;
      PrimitiveType ThePrimitiveType;

      RawMeshContent();
      RawMeshContent(const void* const pTheVertices, const std::size_t vertexCount, const std::size_t vertexStride, const void* const pTheIndices,
                     const std::size_t indexCount, const std::size_t indexStride, const PrimitiveType primitiveType);

      std::size_t VertexArrayByteSize() const
      {
        return VertexStride * VertexCount;
      }

      std::size_t IndexArrayByteSize() const
      {
        return IndexStride * IndexCount;
      }
    };
  }
}

#endif
