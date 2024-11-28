#ifndef FSLBASE_MATH_PIXEL_PXTHICKNESSU_HPP
#define FSLBASE_MATH_PIXEL_PXTHICKNESSU_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>

namespace Fsl
{
  struct PxThicknessU
  {
    using value_type = PxValueU;
    using raw_value_type = value_type::raw_value_type;

    PxValueU Left{0};
    PxValueU Top{0};
    PxValueU Right{0};
    PxValueU Bottom{0};

    constexpr PxThicknessU() noexcept = default;

    constexpr PxThicknessU(const value_type left, const value_type top, const value_type right, const value_type bottom) noexcept
      : Left(left)
      , Top(top)
      , Right(right)
      , Bottom(bottom)
    {
    }

    constexpr value_type SumX() const noexcept
    {
      return Left + Right;
    }

    constexpr value_type SumY() const noexcept
    {
      return Top + Bottom;
    }

    constexpr PxExtent2D Sum() const noexcept
    {
      return {Left + Right, Top + Bottom};
    }

    constexpr bool operator==(const PxThicknessU& rhs) const noexcept
    {
      return Left == rhs.Left && Top == rhs.Top && Right == rhs.Right && Bottom == rhs.Bottom;
    }
    constexpr bool operator!=(const PxThicknessU& rhs) const noexcept
    {
      return !(*this == rhs);
    }

    static constexpr PxThicknessU Create(const raw_value_type left, const raw_value_type top, const raw_value_type right,
                                         const raw_value_type bottom) noexcept
    {
      return {value_type(left), value_type(top), value_type(right), value_type(bottom)};
    }
  };
}


#endif
