#ifndef FSLBASE_MATH_OFFSET3D_HPP
#define FSLBASE_MATH_OFFSET3D_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
  struct Offset3D
  {
  public:
    int32_t X{0};
    int32_t Y{0};
    int32_t Z{0};

    Offset3D() = default;

    Offset3D(const int32_t x, const int32_t y, const int32_t z)
      : X(x)
      , Y(y)
      , Z(z)
    {
    }

    Offset3D& operator+=(const Offset3D& arg)
    {
      X += arg.X;
      Y += arg.Y;
      Z += arg.Z;
      return *this;
    }

    Offset3D& operator-=(const Offset3D& arg)
    {
      X -= arg.X;
      Y -= arg.Y;
      Z -= arg.Z;
      return *this;
    }

    Offset3D& operator*=(const Offset3D& arg)
    {
      X *= arg.X;
      Y *= arg.Y;
      Z *= arg.Z;
      return *this;
    }

    Offset3D& operator*=(const int arg)
    {
      X *= arg;
      Y *= arg;
      Z *= arg;
      return *this;
    }

    bool operator==(const Offset3D& rhs) const
    {
      return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
    }

    bool operator!=(const Offset3D& rhs) const
    {
      return X != rhs.X || Y != rhs.Y || Z != rhs.Z;
    }

    // @brief Returns a Offset3D with all components being zero (0, 0)
    static Offset3D Zero()
    {
      return {};
    }
  };
}

inline Fsl::Offset3D operator+(const Fsl::Offset3D& lhs, const Fsl::Offset3D& rhs)
{
  return Fsl::Offset3D(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
}

inline Fsl::Offset3D operator-(const Fsl::Offset3D& lhs, const Fsl::Offset3D& rhs)
{
  return Fsl::Offset3D(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
}

inline Fsl::Offset3D operator*(const Fsl::Offset3D& lhs, const Fsl::Offset3D& rhs)
{
  return Fsl::Offset3D(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
}

inline Fsl::Offset3D operator*(const Fsl::Offset3D& lhs, const int rhs)
{
  return Fsl::Offset3D(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
}

inline Fsl::Offset3D operator*(const int lhs, const Fsl::Offset3D& rhs)
{
  return rhs * lhs;
}

#endif
