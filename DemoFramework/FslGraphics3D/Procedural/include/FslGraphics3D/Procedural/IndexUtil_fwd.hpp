#ifndef FSLGRAPHICS3D_PROCEDURAL_INDEXUTIL_FWD_HPP
#define FSLGRAPHICS3D_PROCEDURAL_INDEXUTIL_FWD_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics3D/Procedural/IndexUtilBase.hpp>
#include <FslGraphics/PrimitiveType.hpp>
#include <vector>

namespace Fsl {
namespace Procedural
{
  class IndexUtil : public IndexUtilBase
  {
  public:
    //! @brief Merge the srcIndices to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t Merge(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const TIndex mod, const PrimitiveType primitiveType);

    //! @brief Merge the srcIndices at srcStartIndex to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t Merge(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const std::size_t srcStartIndex, const std::size_t srcCount, const TIndex mod, const PrimitiveType primitiveType);

    //! @brief Merge the srcIndices to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t MergeListIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const TIndex mod);

    //! @brief Merge the the srcIndices at srcStartIndex to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t MergeListIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const std::size_t srcStartIndex, const std::size_t srcCount, const TIndex mod);

    //! @brief Merge the the srcIndices to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t MergeTriangleStripIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const TIndex mod);

    //! @brief Merge the the srcIndices at srcStartIndex to rDstIndices at dstIndex adding 'mod' to each element
    //! @return the number of indices that was copied.
    template<typename TIndex>
    static std::size_t MergeTriangleStripIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const std::size_t srcStartIndex, const std::size_t srcCount, const TIndex mod);
  };

}}

#endif
