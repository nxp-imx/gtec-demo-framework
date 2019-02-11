#ifndef FSLBASE_MATH_THICKNESS_HPP
#define FSLBASE_MATH_THICKNESS_HPP
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

namespace Fsl
{
  struct Thickness
  {
  private:
    int32_t m_left{0};
    int32_t m_top{0};
    int32_t m_right{0};
    int32_t m_bottom{0};

  public:
    constexpr Thickness() = default;

    constexpr Thickness(const int32_t left, const int32_t top, const int32_t right, const int32_t bottom)
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr int32_t Bottom() const
    {
      return m_bottom;
    }
    constexpr int32_t Left() const
    {
      return m_left;
    }
    constexpr int32_t Right() const
    {
      return m_right;
    }
    constexpr int32_t Top() const
    {
      return m_top;
    }
    constexpr int32_t SumX() const
    {
      return m_left + m_right;
    }
    constexpr int32_t SumY() const
    {
      return m_top + m_bottom;
    }

    constexpr bool operator==(const Thickness& rhs) const
    {
      return m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom;
    }
    constexpr bool operator!=(const Thickness& rhs) const
    {
      return !(*this == rhs);
    }
  };
}


#endif
