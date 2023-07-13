#ifndef FSLSIMPLEUI_BASE_LAYOUTHELPERPXFCONVERTER_HPP
#define FSLSIMPLEUI_BASE_LAYOUTHELPERPXFCONVERTER_HPP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl::UI::LayoutHelperPxfConverter
{
  inline PxPoint2 ToPxPoint2(const PxAvailableSize value)
  {
    assert(value.IsNormal());
    return PxPoint2::Create(value.RawWidth(), value.RawHeight());
  }

  inline PxAvailableSize ToPxAvailableSize(const PxPoint2 value)
  {
    return PxAvailableSize::Create(value.X.Value, value.Y.Value);
  }

  inline PxAvailableSize ToPxAvailableSize(const PxSize2D value)
  {
    return {value.Width(), value.Height()};
  }


  inline PxRectangle PxfToPxRectangle(const Rect& value)
  {
    assert(!std::isinf(value.Left()) && !std::isnan(value.Left()));
    assert(!std::isinf(value.Top()) && !std::isnan(value.Top()));
    assert(!std::isinf(value.Right()) && !std::isnan(value.Right()));
    assert(!std::isinf(value.Bottom()) && !std::isnan(value.Bottom()));
    assert(value.Left() <= value.Right() && value.Top() <= value.Bottom());
    assert(value.Width() >= 0.0f && value.Height() >= 0.0f);
    return PxRectangle::Create(static_cast<int32_t>(std::round(value.Left())), static_cast<int32_t>(std::round(value.Top())),
                               static_cast<int32_t>(std::round(value.Width())), static_cast<int32_t>(std::round(value.Height())));
  }


  inline int32_t PxfToPxInt32(const float value)
  {
    assert(!std::isnan(value));
    if (std::isinf(value))
    {
      return PxAvailableSizeUtil::InfiniteSpacePx;
    }
    return static_cast<int32_t>(std::round(value));
  }

  inline float PxToPxfFloat(const int32_t value)
  {
    if (PxAvailableSizeUtil::IsConsideredInfiniteSpace(value))
    {
      return std::numeric_limits<float>::infinity();
    }
    return static_cast<float>(value);
  }


  //! @brief Convert a input pxf value to PxPoint with awareness of the magic inf + nan values.
  inline PxPoint2 PxfToPxPoint2(const Vector2 value)
  {
    return {PxValue(PxfToPxInt32(value.X)), PxValue(PxfToPxInt32(value.Y))};
  }

  //! @brief Convert a input pxf value to PxPoint with awareness of the magic inf + nan values.
  inline Vector2 PxToPxfVector2(const PxPoint2 value)
  {
    return {PxToPxfFloat(value.X.Value), PxToPxfFloat(value.Y.Value)};
  }
}

#endif
