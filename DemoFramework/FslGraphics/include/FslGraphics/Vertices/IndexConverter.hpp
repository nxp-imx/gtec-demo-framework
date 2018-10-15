#ifndef FSLGRAPHICS_VERTICES_INDEXCONVERTER_HPP
#define FSLGRAPHICS_VERTICES_INDEXCONVERTER_HPP
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
#include <cstdlib>

namespace Fsl
{
  class IndexConverter
  {
  public:
    //! @brief Convert from one index format to another. This method exist for convenience.
    //! @param pDst the dst index array that should be written to. This points to the start of the first element.
    //! @param dstIndexCapacity the number of indices that the dst index can hold.
    //! @param pSrc the src index array that should be read from. This points to the start of the first element.
    //! @param srcIndexCount the number of indices that the src index holds.
    static void Convert(uint8_t* const pDst, const std::size_t dstIndexCapacity, const uint8_t* const pSrc, const std::size_t srcIndexCount);

    //! @brief Convert from one index format to another. This method exist for convenience.
    //! @param pDst the dst index array that should be written to. This points to the start of the first element.
    //! @param dstIndexCapacity the number of indices that the dst index can hold.
    //! @param pSrc the src index array that should be read from. This points to the start of the first element.
    //! @param srcIndexCount the number of indices that the src index holds.
    static void Convert(uint16_t* const pDst, const std::size_t dstIndexCapacity, const uint16_t* const pSrc, const std::size_t srcIndexCount);

    //! @brief Convert from one index format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst index array that should be written to. This points to the start of the first element.
    //! @param dstIndexCapacity the number of indices that the dst index can hold.
    //! @param pSrc the src index array that should be read from. This points to the start of the first element.
    //! @param srcIndexCount the number of indices that the src index holds.
    //! @note If you use primitive restart and convert to a small index size then be sure that the primitive restart value isn't used accidentally
    //! @note Index size conversion does not do any compacting of the values, the exact index value is converted, if it doesn't fit a exception will
    //! be thrown.
    static void Convert(uint8_t* const pDst, const std::size_t dstIndexCapacity, const uint16_t* const pSrc, const std::size_t srcIndexCount);

    //! @brief Convert from one index format to another. This method exist for convenience.
    //! @param pDst the dst index array that should be written to. This points to the start of the first element.
    //! @param dstIndexCapacity the number of indices that the dst index can hold.
    //! @param pSrc the src index array that should be read from. This points to the start of the first element.
    //! @param srcIndexCount the number of indices that the src index holds.
    static void Convert(uint16_t* const pDst, const std::size_t dstIndexCapacity, const uint8_t* const pSrc, const std::size_t srcIndexCount);

    //! @brief Convert from one index format to another. This method exist for convenience and its performance is probably pretty bad!
    //! @param pDst the dst index array that should be written to. This points to the start of the first element.
    //! @param cbDst the number of bytes the pDst array can hold.
    //! @param dstIndexByteSize the byte size of one dstIndex entry.
    //! @param pSrc the src index array that should be read from. This points to the start of the first element.
    //! @param cbSrc the number of bytes the pSrc array holds.
    //! @param srcIndexByteSize the byte size of one srcIndex entry.
    //! @param srcIndexCount the number of src indices that should be copied.
    //! @note If you use primitive restart and convert to a small index size then be sure that the primitive restart value isn't used accidentally
    //! @note Index size conversion does not do any compacting of the values, the exact index value is converted, if it doesn't fit a exception will
    //! be thrown.
    static void GenericConvert(void* const pDst, const std::size_t cbDst, const std::size_t dstIndexByteSize, const void* const pSrc,
                               const std::size_t cbSrc, const std::size_t srcIndexByteSize, const std::size_t srcIndexCount);
  };
}

#endif
