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

#include <FslGraphics3D/BasicScene/MeshConverter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Vertices/IndexConverter.hpp>
#include <FslGraphics/Vertices/VertexConverter.hpp>
#include <cassert>

namespace Fsl
{
  namespace Graphics3D
  {
    void MeshConverter::GenericConvert(Mesh& rDst, const Mesh& src, const void* const pDstDefaultValues, const int32_t cbDstDefaultValues)
    {
      if (!src.IsValid())
      {
        throw std::invalid_argument("src mesh is not valid");
      }

      // Reset dst to correct size
      const RawMeshContent rawSrc = src.GenericDirectAccess();
      rDst.Reset(rawSrc.VertexCount, rawSrc.IndexCount, rawSrc.ThePrimitiveType);

      RawMeshContentEx rawDst = rDst.GenericDirectAccess();
      if (static_cast<std::size_t>(cbDstDefaultValues) != rawDst.VertexStride)
      {
        throw std::invalid_argument("default value not of the expected size");
      }

      assert(rawSrc.IndexCount == rawDst.IndexCount);
      assert(rawSrc.VertexCount == rawDst.VertexCount);

      VertexConverter::GenericConvert(rawDst.pVertices, rawDst.VertexStride * rawDst.VertexCount, rDst.GetVertexDeclaration(), rawSrc.pVertices,
                                      rawSrc.VertexStride * rawSrc.VertexCount, src.GetVertexDeclaration(), rawSrc.VertexCount, pDstDefaultValues,
                                      cbDstDefaultValues);
      IndexConverter::GenericConvert(rawDst.pIndices, rawDst.IndexStride * rawDst.IndexCount, rawDst.IndexStride, rawSrc.pIndices,
                                     rawSrc.IndexStride * rawSrc.IndexCount, rawSrc.IndexStride, rawSrc.IndexCount);
    }
  }
}
