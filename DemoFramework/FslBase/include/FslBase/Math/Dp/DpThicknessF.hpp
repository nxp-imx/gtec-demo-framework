#ifndef FSLBASE_MATH_DP_DPTHICKNESSF_HPP
#define FSLBASE_MATH_DP_DPTHICKNESSF_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
 *    * Neither the name of the NXP. nor the names of
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
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Dp/DpSize2DF.hpp>
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>

namespace Fsl
{
  //! While a DpThickness could contain negative values its considered invalid if it does.
  struct DpThicknessF
  {
    using value_type = DpValueF;
    using raw_value_type = value_type::raw_value_type;

  private:
    DpValueF m_left;
    DpValueF m_top;
    DpValueF m_right;
    DpValueF m_bottom;

  public:
    constexpr DpThicknessF() noexcept = default;

    constexpr DpThicknessF(const value_type left, const value_type top, const value_type right, const value_type bottom) noexcept
      : m_left(left >= value_type(0) ? left : value_type(0))
      , m_top(top >= value_type(0) ? top : value_type(0))
      , m_right(right >= value_type(0) ? right : value_type(0))
      , m_bottom(bottom >= value_type(0) ? bottom : value_type(0))
    {
      assert(IsValid());
    }

    constexpr DpThicknessF(const value_type left, const value_type top, const value_type right, const value_type bottom,
                           const OptimizationCheckFlag /*unused*/) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
      assert(IsValid());
    }

    constexpr value_type Left() const noexcept
    {
      return m_left;
    }

    constexpr value_type Top() const noexcept
    {
      return m_top;
    }

    constexpr value_type Right() const noexcept
    {
      return m_right;
    }

    constexpr value_type Bottom() const noexcept
    {
      return m_bottom;
    }

    constexpr raw_value_type RawLeft() const noexcept
    {
      return m_left.Value;
    }

    constexpr raw_value_type RawTop() const noexcept
    {
      return m_top.Value;
    }

    constexpr raw_value_type RawRight() const noexcept
    {
      return m_right.Value;
    }

    constexpr raw_value_type RawBottom() const noexcept
    {
      return m_bottom.Value;
    }

    constexpr DpPoint2F TopLeft() const
    {
      return {m_left, m_top};
    }

    constexpr DpPoint2F TopRight() const
    {
      return {m_right, m_top};
    }

    constexpr DpPoint2F BottomLeft() const
    {
      return {m_left, m_bottom};
    }

    constexpr DpPoint2F BottomRight() const
    {
      return {m_right, m_bottom};
    }

    constexpr DpSize2DF Sum() const noexcept
    {
      assert(m_left >= value_type(0) && m_right >= value_type(0));
      assert(m_top >= value_type(0) && m_bottom >= value_type(0));
      return {m_left + m_right, m_top + m_bottom, OptimizationCheckFlag::NoCheck};
    }

    constexpr value_type SumX() const noexcept
    {
      assert(m_left >= value_type(0) && m_right >= value_type(0));
      return m_left + m_right;
    }

    constexpr value_type SumY() const noexcept
    {
      assert(m_top >= value_type(0) && m_bottom >= value_type(0));
      return m_top + m_bottom;
    }

    constexpr bool IsValid() const
    {
      return m_left >= value_type(0) && m_right >= value_type(0) && m_top >= value_type(0) && m_bottom >= value_type(0);
    }

    constexpr bool operator==(const DpThicknessF& rhs) const noexcept
    {
      return m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom;
    }
    constexpr bool operator!=(const DpThicknessF& rhs) const noexcept
    {
      return !(*this == rhs);
    }


    inline static constexpr DpThicknessF Create(const value_type::value_type left, const value_type::value_type top,
                                                const value_type::value_type right, const value_type::value_type bottom) noexcept
    {
      return {value_type(left), value_type(top), value_type(right), value_type(bottom)};
    }
  };
}


#endif
