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
    using value_type = int32_t;

  private:
    value_type m_sliceFromTopLeftX{0};
    value_type m_sliceFromTopLeftY{0};
    value_type m_sliceFromBottomRightX{0};
    value_type m_sliceFromBottomRightY{0};

  public:
    constexpr NineSlice() noexcept = default;

    //! @brief Create the slice points based on a rectangle
    NineSlice(const value_type sliceFromTopLeftX, const value_type sliceFromTopLeftY, const value_type sliceFromBottomRightX,
              const value_type sliceFromBottomRightY);

    constexpr value_type SliceFromTopLeftX() const noexcept
    {
      return m_sliceFromTopLeftX;
    }

    constexpr value_type SliceFromTopLeftY() const noexcept
    {
      return m_sliceFromTopLeftY;
    }

    constexpr value_type SliceFromBottomRightX() const noexcept
    {
      return m_sliceFromBottomRightX;
    }

    constexpr value_type SliceFromBottomRightY() const noexcept
    {
      return m_sliceFromBottomRightY;
    }

    constexpr bool operator==(const NineSlice& rhs) const noexcept
    {
      return m_sliceFromTopLeftX == rhs.m_sliceFromTopLeftX && m_sliceFromTopLeftY == rhs.m_sliceFromTopLeftY &&
             m_sliceFromBottomRightX == rhs.m_sliceFromBottomRightX && m_sliceFromBottomRightY == rhs.m_sliceFromBottomRightY;
    }

    constexpr bool operator!=(const NineSlice& rhs) const noexcept
    {
      return !(*this == rhs);
    }

    constexpr value_type SumX() const noexcept
    {
      return m_sliceFromTopLeftX + m_sliceFromBottomRightX;
    }

    constexpr value_type SumY() const noexcept
    {
      return m_sliceFromTopLeftY + m_sliceFromBottomRightY;
    }

    constexpr bool IsEmpty() const noexcept
    {
      return (m_sliceFromTopLeftX + m_sliceFromTopLeftY + m_sliceFromBottomRightX + m_sliceFromBottomRightY) == 0;
    }
  };
}


#endif
