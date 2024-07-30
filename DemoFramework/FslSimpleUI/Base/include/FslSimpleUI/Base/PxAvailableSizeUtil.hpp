#ifndef FSLSIMPLEUI_BASE_PXAVAILABLESIZEUTIL_HPP
#define FSLSIMPLEUI_BASE_PXAVAILABLESIZEUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <cassert>
#include <limits>

namespace Fsl::UI
{
  namespace PxAvailableSizeUtil
  {
    using layout_value_t = int32_t;
    using normal_value_t = PxValue;

    // We use a range of positive numbers to simulate infinity and the infinity value we supply will be in the middle of this range.
    // As the range is fairly big and the expected numbers we work with are fairly small it should allow for code with slight errors
    // in the infinity handling to still produce a 'infinity' value when adding or subtracting from infinity.
    // (unless they subtract infinity)
    constexpr const layout_value_t InfiniteSpaceBeginPx = std::numeric_limits<layout_value_t>::max() / 2;
    constexpr const layout_value_t InfiniteSpaceEndPx = std::numeric_limits<layout_value_t>::max();
    constexpr const layout_value_t InfiniteSpacePx = InfiniteSpaceBeginPx + ((InfiniteSpaceEndPx - InfiniteSpaceBeginPx) / 2);


    //! lowest valid size value
    constexpr const layout_value_t MinPx = 0;
    //! highest value that is not considered infinite
    constexpr const layout_value_t MaxPx = InfiniteSpaceBeginPx - 1;

    //! @brief Check if the given layout available space value is considered to be infinite space.
    constexpr inline bool IsConsideredInfiniteSpace(const layout_value_t x) noexcept
    {
      return x >= InfiniteSpaceBeginPx;
    }

    //! @brief Check that this value is not inf
    constexpr inline bool IsNormalValue(const layout_value_t x) noexcept
    {
      return x >= MinPx && x <= MaxPx;
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    inline constexpr layout_value_t Add_LayoutSize_Number(const layout_value_t sizePx, const normal_value_t valuePx) noexcept
    {
      assert(!IsNormalValue(sizePx) || ((sizePx + valuePx.Value) < InfiniteSpaceBeginPx));
      return IsNormalValue(sizePx) ? MathHelper::Max(sizePx + valuePx.Value, static_cast<layout_value_t>(0)) : sizePx;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    inline constexpr layout_value_t Sub_LayoutSize_Number(const layout_value_t sizePx, const normal_value_t valuePx) noexcept
    {
      assert(!IsNormalValue(sizePx) || ((sizePx + valuePx.Value) < InfiniteSpaceBeginPx));
      return IsNormalValue(sizePx) ? MathHelper::Max(sizePx - valuePx.Value, static_cast<layout_value_t>(0)) : sizePx;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    inline constexpr layout_value_t Add_LayoutSize_LayoutSize(const layout_value_t sizePx, const layout_value_t valuePx) noexcept
    {
      if (IsNormalValue(sizePx))
      {
        if (IsNormalValue(valuePx))
        {
          assert((sizePx + valuePx) < InfiniteSpaceBeginPx);
          return MathHelper::Max(sizePx + valuePx, 0);
        }
        return valuePx;
      }
      // Size must be infinity, so it does not matter what the other value is
      return sizePx;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    inline constexpr layout_value_t Sub_LayoutSize_LayoutSize(const layout_value_t sizePx, const layout_value_t valuePx) noexcept
    {
      if (IsNormalValue(sizePx))
      {
        if (IsNormalValue(valuePx))
        {
          assert((sizePx + valuePx) < InfiniteSpaceBeginPx);
          return MathHelper::Max(sizePx - valuePx, 0);
        }
        return valuePx;
      }
      // Size must be infinity, so it does not matter what the other value is
      return sizePx;
    }
  };
}

#endif
