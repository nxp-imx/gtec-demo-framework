#ifndef FSLBASE_COMPRESSION_VALUECOMPRESSION_HPP
#define FSLBASE_COMPRESSION_VALUECOMPRESSION_HPP
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
#include <FslBase/Noncopyable.hpp>
#include <cstdlib>

namespace Fsl
{
  class ValueCompression : private Noncopyable
  {
  public:
    //! @brief Read a int32 from pSrc at starting at index
    static int32_t ReadSimpleInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);

    //! @brief Read a uint32 from pSrc at starting at index
    static uint32_t ReadSimpleUInt32(const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);

    //! @brief Read a int32 from pSrc at starting at index
    //! @return the number of bytes that was read
    static std::size_t ReadSimple(int32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);

    //! @brief Read a uint32 from pSrc at starting at index
    //! @return the number of bytes that was read
    static std::size_t ReadSimple(uint32_t& rResult, const uint8_t* const pSrc, const std::size_t srcLength, const std::size_t index);

    //! @brief Encodes a integer into a variable length encoding where the length can be determined from the first byte.
    //         The encoding favors small values.
    /// @return the number of bytes written
    static std::size_t WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const int32_t value);

    //! @brief Encodes a integer into a variable length encoding where the length can be determined from the first byte.
    //         The encoding favors small values.
    /// @return the number of bytes written
    static std::size_t WriteSimple(uint8_t* const pDst, const std::size_t dstLength, const std::size_t index, const uint32_t value);
  };
}

#endif
