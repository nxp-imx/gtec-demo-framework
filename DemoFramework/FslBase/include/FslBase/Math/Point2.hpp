#ifndef FSLBASE_MATH_POINT2_HPP
#define FSLBASE_MATH_POINT2_HPP
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
  struct Point2
  {
  public:
    using element_type = int32_t;

    int32_t X{0};
    int32_t Y{0};

    constexpr Point2() = default;

    constexpr Point2(const int32_t x, const int32_t y)
      : X(x)
      , Y(y)
    {
    }

    Point2& operator+=(const Point2& arg)
    {
      X += arg.X;
      Y += arg.Y;
      return *this;
    }

    Point2& operator-=(const Point2& arg)
    {
      X -= arg.X;
      Y -= arg.Y;
      return *this;
    }

    Point2& operator*=(const Point2& arg)
    {
      X *= arg.X;
      Y *= arg.Y;
      return *this;
    }

    Point2& operator*=(const int arg)
    {
      X *= arg;
      Y *= arg;
      return *this;
    }

    constexpr bool operator==(const Point2& rhs) const
    {
      return X == rhs.X && Y == rhs.Y;
    }

    constexpr bool operator!=(const Point2& rhs) const
    {
      return X != rhs.X || Y != rhs.Y;
    }

    // @brief Returns a Point2 with all components being zero (0, 0)
    static constexpr Point2 Zero()
    {
      return {};
    }
  };
}

inline Fsl::Point2 operator+(const Fsl::Point2& lhs, const Fsl::Point2& rhs)
{
  return Fsl::Point2(lhs.X + rhs.X, lhs.Y + rhs.Y);
}

inline Fsl::Point2 operator-(const Fsl::Point2& lhs, const Fsl::Point2& rhs)
{
  return Fsl::Point2(lhs.X - rhs.X, lhs.Y - rhs.Y);
}

inline Fsl::Point2 operator*(const Fsl::Point2& lhs, const Fsl::Point2& rhs)
{
  return Fsl::Point2(lhs.X * rhs.X, lhs.Y * rhs.Y);
}

inline Fsl::Point2 operator*(const Fsl::Point2& lhs, const int rhs)
{
  return Fsl::Point2(lhs.X * rhs, lhs.Y * rhs);
}

inline Fsl::Point2 operator*(const int lhs, const Fsl::Point2& rhs)
{
  return rhs * lhs;
}

#endif
