#ifndef FSLBASE_MATH_BOXF_HPP
#define FSLBASE_MATH_BOXF_HPP
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

#include <FslBase/Math/Vector2.hpp>

namespace Fsl
{
  struct BoxF
  {
    float X1{0.0f};
    float Y1{0.0f};
    float X2{0.0f};
    float Y2{0.0f};

    constexpr BoxF() = default;

    constexpr BoxF(const float x1, const float y1, const float x2, const float y2)
      : X1(x1)
      , Y1(y1)
      , X2(x2)
      , Y2(y2)
    {
    }

    constexpr BoxF(const float x, const float y, const float width, const float height, const bool reserved)
      : X1(x)
      , Y1(y)
      , X2(x + width)
      , Y2(y + height)
    {
    }


    static constexpr BoxF Empty()
    {
      return BoxF(0.0f, 0.0f, 0.0f, 0.0f);
    }

    //! @brief Get the center of this box
    Vector2 GetCenter() const
    {
      return Vector2(X1 + ((X2 - X1) * 0.5f), Y1 + ((Y2 - Y1) * 0.5f));
    }

    constexpr bool operator==(const BoxF& rhs) const
    {
      return ((X1 == rhs.X1) && (Y1 == rhs.Y1) && (X2 == rhs.X2) && (Y2 == rhs.Y2));
    }

    constexpr bool operator!=(const BoxF& rhs) const
    {
      return ((X1 != rhs.X1) || (Y1 != rhs.Y1) || (X2 != rhs.X2) || (Y2 != rhs.Y2));
    }
  };
}


#endif
