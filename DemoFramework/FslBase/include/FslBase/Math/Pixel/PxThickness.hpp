#ifndef FSLBASE_MATH_PIXEL_PXTHICKNESS_HPP
#define FSLBASE_MATH_PIXEL_PXTHICKNESS_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>

namespace Fsl
{
  //! A PxThickness can not contain negative values
  struct PxThickness
  {
    using value_type = PxSize1D;
    using raw_value_type = value_type::raw_value_type;

  private:
    value_type m_left;
    value_type m_top;
    value_type m_right;
    value_type m_bottom;

  public:
    constexpr PxThickness() noexcept = default;

    constexpr PxThickness(const value_type left, const value_type top, const value_type right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const value_type top, const value_type right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const value_type top, const PxValue right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const value_type top, const PxValue right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }


    constexpr PxThickness(const value_type left, const PxValue top, const value_type right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const PxValue top, const value_type right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const PxValue top, const PxValue right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const value_type left, const PxValue top, const PxValue right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const value_type top, const value_type right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const value_type top, const value_type right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const value_type top, const PxValue right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const value_type top, const PxValue right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }


    constexpr PxThickness(const PxValue left, const PxValue top, const value_type right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const PxValue top, const value_type right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const PxValue top, const PxValue right, const value_type bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
    }

    constexpr PxThickness(const PxValue left, const PxValue top, const PxValue right, const PxValue bottom) noexcept
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
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
      return m_left.RawValue();
    }

    constexpr raw_value_type RawTop() const noexcept
    {
      return m_top.RawValue();
    }

    constexpr raw_value_type RawRight() const noexcept
    {
      return m_right.RawValue();
    }

    constexpr raw_value_type RawBottom() const noexcept
    {
      return m_bottom.RawValue();
    }

    constexpr PxPoint2 TopLeft() const
    {
      return {m_left, m_top};
    }

    constexpr PxPoint2 TopRight() const
    {
      return {m_right, m_top};
    }

    constexpr PxPoint2 BottomLeft() const
    {
      return {m_left, m_bottom};
    }

    constexpr PxPoint2 BottomRight() const
    {
      return {m_right, m_bottom};
    }

    constexpr PxSize2D Sum() const noexcept
    {
      return {m_left + m_right, m_top + m_bottom};
    }

    constexpr value_type SumX() const noexcept
    {
      return m_left + m_right;
    }

    constexpr value_type SumY() const noexcept
    {
      return m_top + m_bottom;
    }

    constexpr bool operator==(const PxThickness& rhs) const noexcept
    {
      return m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom;
    }
    constexpr bool operator!=(const PxThickness& rhs) const noexcept
    {
      return !(*this == rhs);
    }

    static constexpr PxThickness Min(const PxThickness& val0, const PxThickness& val1)
    {
      return {value_type::Min(val0.Left(), val1.Left()), value_type::Min(val0.Top(), val1.Top()), value_type::Min(val0.Right(), val1.Right()),
              value_type::Min(val0.Bottom(), val1.Bottom())};
    }

    static constexpr PxThickness Max(const PxThickness& val0, const PxThickness& val1)
    {
      return {value_type::Max(val0.Left(), val1.Left()), value_type::Max(val0.Top(), val1.Top()), value_type::Max(val0.Right(), val1.Right()),
              value_type::Max(val0.Bottom(), val1.Bottom())};
    }

    static constexpr PxThickness Max(const PxThickness& val0, const PxThickness& val1, const PxThickness& val2)
    {
      return {value_type::Max(val0.Left(), val1.Left(), val2.Left()), value_type::Max(val0.Top(), val1.Top(), val2.Top()),
              value_type::Max(val0.Right(), val1.Right(), val2.Right()), value_type::Max(val0.Bottom(), val1.Bottom(), val2.Bottom())};
    }

    static constexpr PxThickness Create(const raw_value_type left, const raw_value_type top, const raw_value_type right,
                                        const raw_value_type bottom) noexcept
    {
      return {value_type::Create(left), value_type::Create(top), value_type::Create(right), value_type::Create(bottom)};
    }

    static constexpr PxThickness UncheckedCreate(const raw_value_type left, const raw_value_type top, const raw_value_type right,
                                                 const raw_value_type bottom) noexcept
    {
      return {value_type::UncheckedCreate(left), value_type::UncheckedCreate(top), value_type::UncheckedCreate(right),
              value_type::UncheckedCreate(bottom)};
    }
  };
}


#endif
