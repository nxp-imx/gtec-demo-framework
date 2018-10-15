#ifndef FSLBASE_MATH_VECTORHELPER_HPP
#define FSLBASE_MATH_VECTORHELPER_HPP
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

#include <algorithm>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>

namespace Fsl
{
  namespace VectorHelper
  {
    /// <summary>
    ///     B
    ///    /|
    ///  c/ |a
    ///  /  |
    /// A---C
    ///   b
    /// Calc the angle A for the vector c.
    /// </summary>
    /// <param name="vec">the vector. WARNING the length of this must not be 0</param>
    /// <returns>the angle in radians<returns>
    inline float VectorToAngle(const Vector2& vec)
    {
      Vector2 value(Vector2::Normalize(vec));
      return std::atan2(value.Y, value.X);
    }


    inline Vector2 CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount)
    {
      return Vector2(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
                     MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount));
    }

    inline Vector3 CatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3, const Vector3& value4, const float amount)
    {
      return Vector3(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
                     MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
                     MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount));
    }

    inline Vector4 CatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount)
    {
      return Vector4(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
                     MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
                     MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount),
                     MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount));
    }
  };
}

#endif
