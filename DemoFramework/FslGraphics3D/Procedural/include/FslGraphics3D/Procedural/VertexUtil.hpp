#ifndef FSLGRAPHICS3D_PROCEDURAL_VERTEXUTIL_HPP
#define FSLGRAPHICS3D_PROCEDURAL_VERTEXUTIL_HPP
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

#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics3D/Procedural/VertexUtil_fwd.hpp>
#include <algorithm>
#include <vector>

namespace Fsl
{
  namespace Procedural
  {
    // The class is defined in VertexUtil_fwd.hpp and this file contains the implementation of the 'templated' functions of VertexUtil.
    // This allows us to use templates without 'spreading' dependencies to files that don't need it by allowing them to only include the _fwd file

    template <typename TVertex>
    std::size_t VertexUtil::Copy(std::vector<TVertex>& rDst, const std::size_t dstIndex, const std::vector<TVertex>& srcVertices)
    {
      if (dstIndex < 0 || (std::size_t(dstIndex) + srcVertices.size()) > rDst.size())
      {
        throw std::invalid_argument("One of the arguments is invalid");
      }

      std::copy(srcVertices.begin(), srcVertices.end(), rDst.begin() + dstIndex);
      return srcVertices.size();
    }


    template <typename TVertex>
    std::size_t VertexUtil::Copy(std::vector<TVertex>& rDst, const std::size_t dstIndex, const std::vector<TVertex>& srcVertices,
                                 const Vector3& positionMod)
    {
      if (dstIndex < 0 || (std::size_t(dstIndex) + srcVertices.size()) > rDst.size())
      {
        throw std::invalid_argument("One of the arguments is invalid");
      }

      // Copy and apply position mod
      for (std::size_t i = 0; i < srcVertices.size(); ++i)
      {
        TVertex tmp = srcVertices[i];
        tmp.Position += positionMod;
        rDst[i] = tmp;
      }
      return srcVertices.size();
    }


    template <typename TVertexDst, typename TVertexSrc>
    void VertexUtil::ExtractNormalsAsLineList(std::vector<TVertexDst>& rDst, const std::size_t dstIndex, const std::vector<TVertexSrc>& srcVertices,
                                              const float len)
    {
      // We could generate one piece of code that handles this in a generic way by using the vertex declarations and direct memory access
      // ExtractNormalsAsLineList(rDst.data(), dstIndex, rDst.size(), TVertexDst::GetVertexDeclaration(), srcVertices.data(), srcVertices.size(),
      // TVertexSrc::GetVertexDeclaration());

      if (rDst.size() < (srcVertices.size() * 2))
      {
        throw std::invalid_argument("dst array too small");
      }

      const TVertexSrc* pSrcVertices = srcVertices.data();
      for (std::size_t i = 0; i < srcVertices.size(); ++i)
      {
        const Vector3 pos = pSrcVertices[i].Position;
        rDst[i * 2 + 0].Position = pos;
        rDst[i * 2 + 1].Position = pos + (pSrcVertices[i].Normal * len);
      }
    }
  }
}

#endif
