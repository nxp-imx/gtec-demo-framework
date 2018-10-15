#ifndef FSLGRAPHICS3D_PROCEDURAL_VERTEXUTIL_FWD_HPP
#define FSLGRAPHICS3D_PROCEDURAL_VERTEXUTIL_FWD_HPP
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

#include <vector>
#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  struct Vector3;

  namespace Procedural
  {
    class VertexUtil
    {
    public:
      //! @brief Copy the srcVertices to the rDst vector at the given index.
      template <typename TVertex>
      static std::size_t Copy(std::vector<TVertex>& rDst, const std::size_t dstIndex, const std::vector<TVertex>& srcVertices);

      //! @brief Copy the srcVertices to the rDst vector at the given index applying positionMod to the position of each vertex.
      template <typename TVertex>
      static std::size_t Copy(std::vector<TVertex>& rDst, const std::size_t dstIndex, const std::vector<TVertex>& srcVertices,
                              const Vector3& positionMod);


      //! @brief Calculate the number of instances that can fit inside the maxVertices count.
      static std::size_t CalcMaxInstancesThatFit(const std::size_t instanceVertexCount, const std::size_t maxVertices);

      //! @brief Calculate the number of instances that can fit inside the maxVertices count.
      static std::size_t CalcMaxInstancesThatFit(const std::size_t instanceVertexCount, const std::size_t vertexCapacity, const bool shareVertices);


      //! @brief Extract the normals as a line list.
      template <typename TVertexDst, typename TVertexSrc>
      static void ExtractNormalsAsLineList(std::vector<TVertexDst>& rDst, const std::size_t dstIndex, const std::vector<TVertexSrc>& srcVertices,
                                           const float len);
    };
  }
}

#endif
