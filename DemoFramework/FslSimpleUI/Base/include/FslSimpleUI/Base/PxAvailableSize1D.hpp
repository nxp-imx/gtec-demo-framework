#ifndef FSLSIMPLEUI_BASE_PXAVAILABLESIZE1D_HPP
#define FSLSIMPLEUI_BASE_PXAVAILABLESIZE1D_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <cassert>

namespace Fsl::UI
{
  //! A PxAvailableSize can not contain a negative width or height (so it clamps all math operations).
  //! Beware however it has special values like PxAvailableSizeUtil::InfiniteSpacePx so special care has to be taken when working with it.
  struct PxAvailableSize1D
  {
    using value_type = PxAvailableSizeUtil::layout_value_t;
    using raw_value_type = value_type;

  private:
    value_type m_value{0};

    explicit constexpr PxAvailableSize1D(const value_type value) noexcept
      : m_value(value)
    {
    }

  public:
    constexpr PxAvailableSize1D() noexcept = default;

    constexpr value_type Value() const
    {
      return m_value;
    }

    explicit constexpr PxAvailableSize1D(const PxSize1D value) noexcept
      : m_value(value.RawValue())
    {
      assert(PxAvailableSizeUtil::IsNormalValue(m_value));
    }

    //! When using this constructor we expect a normal value this is not in the magic NaN or Infinity ranges!
    constexpr explicit PxAvailableSize1D(const PxValue value) noexcept
      : m_value(MathHelper::Max(value.Value, 0))
    {
      assert(PxAvailableSizeUtil::IsNormalValue(value.Value));
    }


    constexpr void SetNormalValue(const int32_t valuePx) noexcept
    {
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_value = MathHelper::Max(valuePx, 0);
    }


    constexpr void UncheckedSetNormalValue(const int32_t valuePx) noexcept
    {
      assert(valuePx >= 0);
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_value = valuePx;
    }

    constexpr PxSize1D ToPxSize1D() const noexcept
    {
      assert(IsNormal());
      return PxSize1D::UncheckedCreate(m_value);
    }


    inline constexpr bool IsInfinity() const noexcept
    {
      return PxAvailableSizeUtil::IsConsideredInfiniteSpace(m_value);
    }

    inline constexpr bool IsNormal() const noexcept
    {
      return PxAvailableSizeUtil::IsNormalValue(m_value);
    }

    inline constexpr const PxAvailableSize1D& operator++() noexcept
    {
      if (PxAvailableSizeUtil::IsNormalValue(m_value))
      {
        ++m_value;
      }
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxAvailableSize1D operator++(int) noexcept
    {
      PxAvailableSize1D temp(*this);
      if (PxAvailableSizeUtil::IsNormalValue(m_value))
      {
        ++m_value;
      }
      return temp;
    }

    inline constexpr const PxAvailableSize1D& operator--() noexcept
    {
      if (PxAvailableSizeUtil::IsNormalValue(m_value))
      {
        --m_value;
      }
      return *this;
    }

    // NOLINTNEXTLINE(cert-dcl21-cpp)
    inline constexpr PxAvailableSize1D operator--(int) noexcept
    {
      PxAvailableSize1D temp(*this);
      if (PxAvailableSizeUtil::IsNormalValue(m_value))
      {
        --m_value;
      }
      return temp;
    }


    static constexpr PxAvailableSize1D Create(const value_type value) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::IsNormalValue(value) ? MathHelper::Max(value, 0) : value);
    }

    static constexpr PxAvailableSize1D UncheckedCreate(const value_type value) noexcept
    {
      assert(PxAvailableSizeUtil::IsNormalValue(value));
      return PxAvailableSize1D(value);
    }

    //! @brief Create a PxAvailableSize1D from the value_type allowing all special values
    static constexpr PxAvailableSize1D UncheckedCreateRAW(const value_type value) noexcept
    {
      return PxAvailableSize1D(value);
    }

    // @brief Returns a PxAvailableSize1D with all components being zero (0, 0)
    static constexpr PxAvailableSize1D Zero() noexcept
    {
      return {};
    }

    static constexpr PxAvailableSize1D Add(const PxAvailableSize1D sizePx, const PxAvailableSize1D valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Add_LayoutSize_LayoutSize(sizePx.m_value, valuePx.m_value));
    }

    static constexpr PxAvailableSize1D Subtract(const PxAvailableSize1D sizePx, const PxAvailableSize1D valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Sub_LayoutSize_LayoutSize(sizePx.m_value, valuePx.m_value));
    }

    static constexpr PxAvailableSize1D Add(const PxAvailableSize1D sizePx, const PxValue valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Add_LayoutSize_Number(sizePx.m_value, valuePx));
    }

    static constexpr PxAvailableSize1D Subtract(const PxAvailableSize1D sizePx, const PxValue valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_value, valuePx));
    }

    static constexpr PxAvailableSize1D Add(const PxAvailableSize1D sizePx, const PxSize1D valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Add_LayoutSize_Number(sizePx.m_value, valuePx.Value()));
    }

    static constexpr PxAvailableSize1D Subtract(const PxAvailableSize1D sizePx, const PxSize1D valuePx) noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_value, valuePx.Value()));
    }

    static constexpr PxAvailableSize1D InfiniteSpacePx() noexcept
    {
      return PxAvailableSize1D(PxAvailableSizeUtil::InfiniteSpacePx);
    }

    //! Special compare used during layout to compare against max values that might contain infinity
    static constexpr PxSize1D MinPxSize1D(PxSize1D lhs, PxAvailableSize1D rhs) noexcept
    {
      return PxSize1D::UncheckedCreate(MathHelper::Min(lhs.RawValue(), rhs.m_value));
    }
  };


  // op==

  inline constexpr bool operator==(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() == rhs.Value();
  }

  inline constexpr bool operator==(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() == rhs.Value();
  }

  inline constexpr bool operator==(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() == rhs.RawValue();
  }

  inline constexpr bool operator==(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value == rhs.Value();
  }

  inline constexpr bool operator==(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() == rhs.Value;
  }


  // op!=

  inline constexpr bool operator!=(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() != rhs.Value();
  }

  inline constexpr bool operator!=(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() != rhs.Value();
  }

  inline constexpr bool operator!=(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() != rhs.RawValue();
  }

  inline constexpr bool operator!=(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value != rhs.Value();
  }

  inline constexpr bool operator!=(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() != rhs.Value;
  }

  // op<

  inline constexpr bool operator<(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() < rhs.Value();
  }

  inline constexpr bool operator<(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() < rhs.Value();
  }

  inline constexpr bool operator<(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() < rhs.RawValue();
  }

  inline constexpr bool operator<(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value < rhs.Value();
  }

  inline constexpr bool operator<(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() < rhs.Value;
  }

  // op<=

  inline constexpr bool operator<=(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() <= rhs.Value();
  }

  inline constexpr bool operator<=(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() <= rhs.Value();
  }

  inline constexpr bool operator<=(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() <= rhs.RawValue();
  }

  inline constexpr bool operator<=(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value <= rhs.Value();
  }

  inline constexpr bool operator<=(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() <= rhs.Value;
  }


  // op>

  inline constexpr bool operator>(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() > rhs.Value();
  }

  inline constexpr bool operator>(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() > rhs.Value();
  }

  inline constexpr bool operator>(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() > rhs.RawValue();
  }

  inline constexpr bool operator>(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value > rhs.Value();
  }

  inline constexpr bool operator>(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() > rhs.Value;
  }

  // op>=

  inline constexpr bool operator>=(const PxAvailableSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value() >= rhs.Value();
  }

  inline constexpr bool operator>=(const PxSize1D lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.RawValue() >= rhs.Value();
  }

  inline constexpr bool operator>=(const PxAvailableSize1D lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value() >= rhs.RawValue();
  }

  inline constexpr bool operator>=(const PxValue lhs, const PxAvailableSize1D rhs) noexcept
  {
    return lhs.Value >= rhs.Value();
  }

  inline constexpr bool operator>=(const PxAvailableSize1D lhs, const PxValue rhs) noexcept
  {
    return lhs.Value() >= rhs.Value;
  }
}

#endif
