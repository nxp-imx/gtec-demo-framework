#ifndef FSLBASE_MATH_NINESLICE_HPP
#define FSLBASE_MATH_NINESLICE_HPP
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

namespace Fsl
{
  struct NineSlice
  {
  private:
    int32_t m_sliceFromTopLeftX{0};
    int32_t m_sliceFromTopLeftY{0};
    int32_t m_sliceFromBottomRightX{0};
    int32_t m_sliceFromBottomRightY{0};

  public:
    constexpr NineSlice() = default;

    //! @brief Create the slice points based on a rectangle
    NineSlice(const int32_t sliceFromTopLeftX, const int32_t sliceFromTopLeftY, const int32_t sliceFromBottomRightX,
              const int32_t sliceFromBottomRightY);

    constexpr int32_t SliceFromTopLeftX() const
    {
      return m_sliceFromTopLeftX;
    }

    constexpr int32_t SliceFromTopLeftY() const
    {
      return m_sliceFromTopLeftY;
    }

    constexpr int32_t SliceFromBottomRightX() const
    {
      return m_sliceFromBottomRightX;
    }

    constexpr int32_t SliceFromBottomRightY() const
    {
      return m_sliceFromBottomRightY;
    }

    constexpr bool operator==(const NineSlice& rhs) const
    {
      return m_sliceFromTopLeftX == rhs.m_sliceFromTopLeftX && m_sliceFromTopLeftY == rhs.m_sliceFromTopLeftY &&
             m_sliceFromBottomRightX == rhs.m_sliceFromBottomRightX && m_sliceFromBottomRightY == rhs.m_sliceFromBottomRightY;
    }

    constexpr bool operator!=(const NineSlice& rhs) const
    {
      return !(*this == rhs);
    }

    constexpr int32_t SumX() const
    {
      return m_sliceFromTopLeftX + m_sliceFromBottomRightX;
    }

    constexpr int32_t SumY() const
    {
      return m_sliceFromTopLeftY + m_sliceFromBottomRightY;
    }

    constexpr bool IsEmpty() const
    {
      return (m_sliceFromTopLeftX + m_sliceFromTopLeftY + m_sliceFromBottomRightX + m_sliceFromBottomRightY) == 0;
    }
  };
}


#endif
