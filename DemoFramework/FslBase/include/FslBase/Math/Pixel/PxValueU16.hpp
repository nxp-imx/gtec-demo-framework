#ifndef FSLBASE_MATH_PIXEL_PXVALUEU16_HPP
#define FSLBASE_MATH_PIXEL_PXVALUEU16_HPP
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
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>

namespace Fsl
{
  struct PxValueU16
  {
    using value_type = uint16_t;
    using raw_value_type = uint16_t;

    uint16_t Value{0};

    inline constexpr PxValueU16() noexcept = default;

    inline explicit constexpr PxValueU16(const value_type value) noexcept
      : Value(value)
    {
    }

    // @brief Returns a PxValueU16 with all components being zero (0, 0)
    inline static constexpr PxValueU16 Zero() noexcept
    {
      return {};
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator PxValue() const noexcept
    {
      return PxValue(Value);
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator PxValueU() const noexcept
    {
      return PxValueU(Value);
    }

    //! equal to std::min but its both constexpr and noexcept
    static constexpr PxValueU16 Min(const PxValueU16 val0, const PxValueU16 val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValueU16(val0.Value <= val1.Value ? val0.Value : val1.Value);
    }

    //! equal to std::max but its both constexpr and noexcept
    static constexpr PxValueU16 Max(const PxValueU16 val0, const PxValueU16 val1) noexcept
    {
      // Both values are known to be >= 0
      return PxValueU16(val0.Value >= val1.Value ? val0.Value : val1.Value);
    }

    inline static constexpr PxValueU16 Create(const value_type value) noexcept
    {
      return PxValueU16(value);
    }
  };

  // op==

  inline constexpr bool operator==(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }


  // PxValueU
  inline constexpr bool operator==(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }

  inline constexpr bool operator==(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }

  // PxValue
  inline constexpr bool operator==(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }

  inline constexpr bool operator==(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value == rhs.Value;
  }

  // PxSize1D
  inline constexpr bool operator==(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() == rhs.Value;
  }

  inline constexpr bool operator==(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value == rhs.RawValue();
  }

  // op!=

  inline constexpr bool operator!=(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }


  // PxValueU
  inline constexpr bool operator!=(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }

  inline constexpr bool operator!=(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }


  // PxValue
  inline constexpr bool operator!=(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }

  inline constexpr bool operator!=(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value != rhs.Value;
  }


  // PxSize1D
  inline constexpr bool operator!=(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() != rhs.Value;
  }

  inline constexpr bool operator!=(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value != rhs.RawValue();
  }

  // op<

  inline constexpr bool operator<(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }


  // PxValueU
  inline constexpr bool operator<(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }

  inline constexpr bool operator<(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }


  // PxValue
  inline constexpr bool operator<(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }

  inline constexpr bool operator<(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value < rhs.Value;
  }


  // PxSize1D
  inline constexpr bool operator<(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() < rhs.Value;
  }

  inline constexpr bool operator<(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value < rhs.RawValue();
  }

  // op<=

  inline constexpr bool operator<=(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }

  // PxValueU
  inline constexpr bool operator<=(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }

  inline constexpr bool operator<=(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }


  // PxValue
  inline constexpr bool operator<=(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }

  inline constexpr bool operator<=(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value <= rhs.Value;
  }


  // PxSize1D
  inline constexpr bool operator<=(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() <= rhs.Value;
  }

  inline constexpr bool operator<=(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value <= rhs.RawValue();
  }

  // op>

  inline constexpr bool operator>(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }


  // PxValueU
  inline constexpr bool operator>(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }

  inline constexpr bool operator>(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }


  // PxValue
  inline constexpr bool operator>(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }

  inline constexpr bool operator>(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value > rhs.Value;
  }


  // PxSize1D
  inline constexpr bool operator>(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() > rhs.Value;
  }

  inline constexpr bool operator>(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value > rhs.RawValue();
  }

  // op>=

  inline constexpr bool operator>=(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }

  // PxValueU
  inline constexpr bool operator>=(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }

  inline constexpr bool operator>=(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }


  // PxValue
  inline constexpr bool operator>=(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }

  inline constexpr bool operator>=(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return lhs.Value >= rhs.Value;
  }


  // PxSize1D
  inline constexpr bool operator>=(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return lhs.RawValue() >= rhs.Value;
  }

  inline constexpr bool operator>=(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return lhs.Value >= rhs.RawValue();
  }

  // op add

  inline constexpr PxValueU operator+(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value + rhs.Value);
  }


  // PxValueU
  inline constexpr PxValueU operator+(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value + rhs.Value);
  }

  inline constexpr PxValueU operator+(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return PxValueU(lhs.Value + rhs.Value);
  }


  // PxValue
  inline constexpr PxValue operator+(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return PxValue(lhs.Value + rhs.Value);
  }

  inline constexpr PxValue operator+(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value + rhs.Value);
  }


  // PxSize1D
  inline constexpr PxSize1D operator+(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return PxSize1D::UncheckedCreate(lhs.RawValue() + rhs.Value);
  }

  inline constexpr PxSize1D operator+(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::UncheckedCreate(lhs.Value + rhs.RawValue());
  }

  // op sub

  inline constexpr PxValueU operator-(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value - rhs.Value);
  }


  // PxValueU
  inline constexpr PxValueU operator-(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value - rhs.Value);
  }

  inline constexpr PxValueU operator-(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return PxValueU(lhs.Value - rhs.Value);
  }


  // PxValue
  inline constexpr PxValue operator-(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return PxValue(lhs.Value - rhs.Value);
  }

  inline constexpr PxValue operator-(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value - rhs.Value);
  }


  // PxSize1D
  inline constexpr PxValue operator-(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return PxValue(lhs.RawValue() - rhs.Value);
  }

  inline constexpr PxValue operator-(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return PxValue(lhs.Value - rhs.RawValue());
  }

  // op multiply

  inline constexpr PxValueU operator*(const PxValueU16 lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value * rhs.Value);
  }


  // PxValueU
  inline constexpr PxValueU operator*(const PxValueU lhs, const PxValueU16 rhs) noexcept
  {
    return PxValueU(lhs.Value * rhs.Value);
  }

  inline constexpr PxValueU operator*(const PxValueU16 lhs, const PxValueU rhs) noexcept
  {
    return PxValueU(lhs.Value * rhs.Value);
  }


  // PxValue
  inline constexpr PxValue operator*(const PxValue lhs, const PxValueU16 rhs) noexcept
  {
    return PxValue(lhs.Value * rhs.Value);
  }

  inline constexpr PxValue operator*(const PxValueU16 lhs, const PxValue rhs) noexcept
  {
    return PxValue(lhs.Value * rhs.Value);
  }


  // PxSize1D
  inline constexpr PxSize1D operator*(const PxSize1D lhs, const PxValueU16 rhs) noexcept
  {
    return PxSize1D::UncheckedCreate(lhs.RawValue() * rhs.Value);
  }

  inline constexpr PxSize1D operator*(const PxValueU16 lhs, const PxSize1D rhs) noexcept
  {
    return PxSize1D::UncheckedCreate(lhs.Value * rhs.RawValue());
  }

  // op divide

  inline constexpr PxValueU operator/(const PxValueU16 lhs, const PxValueU16 rhs)
  {
    return PxValueU(lhs.Value / rhs.Value);
  }


  // PxValueU
  inline constexpr PxValueU operator/(const PxValueU lhs, const PxValueU16 rhs)
  {
    return PxValueU(lhs.Value / rhs.Value);
  }

  inline constexpr PxValueU operator/(const PxValueU16 lhs, const PxValueU rhs)
  {
    return PxValueU(lhs.Value / rhs.Value);
  }


  // PxValue
  inline constexpr PxValue operator/(const PxValue lhs, const PxValueU16 rhs)
  {
    return PxValue(lhs.Value / rhs.Value);
  }

  inline constexpr PxValue operator/(const PxValueU16 lhs, const PxValue rhs)
  {
    return PxValue(lhs.Value / rhs.Value);
  }


  // PxSize1D
  inline constexpr PxSize1D operator/(const PxSize1D lhs, const PxValueU16 rhs)
  {
    return PxSize1D::UncheckedCreate(lhs.RawValue() / rhs.Value);
  }

  inline constexpr PxSize1D operator/(const PxValueU16 lhs, const PxSize1D rhs)
  {
    return PxSize1D::UncheckedCreate(lhs.Value / rhs.RawValue());
  }
}

#endif
