#ifndef FSLSIMPLEUI_BASE_PXAVAILABLESIZE_HPP
#define FSLSIMPLEUI_BASE_PXAVAILABLESIZE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl::UI
{
  //! A PxAvailableSize can not contain a negative width or height (so it clamps all math operations).
  //! Beware however it has special values like PxAvailableSizeUtil::NaNPx (which is negative) and PxAvailableSizeUtil::InfiniteSpacePx so special
  //! care has to be taken when working with it.
  struct PxAvailableSize
  {
    using value_type = int32_t;

  private:
    value_type m_width{0};
    value_type m_height{0};

  public:
    constexpr PxAvailableSize() noexcept = default;

    constexpr value_type Width() const
    {
      return m_width;
    }

    constexpr value_type Height() const
    {
      return m_height;
    }

    constexpr PxAvailableSize(const PxSize1D width, const PxSize1D height) noexcept
      : m_width(width.RawValue())
      , m_height(height.RawValue())
    {
      assert(PxAvailableSizeUtil::IsNormalValue(m_width));
      assert(PxAvailableSizeUtil::IsNormalValue(m_height));
    }

    constexpr PxAvailableSize(const value_type width, const value_type height) noexcept
      : m_width(PxAvailableSizeUtil::IsNormalValue(width) ? std::max(width, 0) : width)
      , m_height(PxAvailableSizeUtil::IsNormalValue(height) ? std::max(height, 0) : height)
    {
    }

    constexpr PxAvailableSize(const value_type width, const value_type height, const OptimizationCheckFlag /*unused*/) noexcept
      : m_width(width)
      , m_height(height)
    {
      assert(width >= 0);
      assert(height >= 0);
    }

    //! When using this constructor we expect a normal value this is not in the magic NaN or Infinity ranges!
    constexpr explicit PxAvailableSize(const PxPoint2 value) noexcept
      : m_width(std::max(value.X, 0))
      , m_height(std::max(value.Y, 0))
    {
      assert(PxAvailableSizeUtil::IsNormalValue(value.X));
      assert(PxAvailableSizeUtil::IsNormalValue(value.Y));
    }

    constexpr explicit PxAvailableSize(const PxSize2D value) noexcept
      : m_width(value.Width())
      , m_height(value.Height())
    {
      assert(PxAvailableSizeUtil::IsNormalValue(value.Width()));
      assert(PxAvailableSizeUtil::IsNormalValue(value.Height()));
    }


    constexpr void SetNormalWidth(const int32_t valuePx)
    {
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_width = std::max(valuePx, 0);
    }

    constexpr void SetNormalWidth(const int32_t valuePx, const OptimizationCheckFlag /*unused*/)
    {
      assert(valuePx >= 0);
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_width = valuePx;
    }

    constexpr void SetNormalHeight(const int32_t valuePx)
    {
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_height = std::max(valuePx, 0);
    }

    constexpr void SetNormalHeight(const int32_t valuePx, const OptimizationCheckFlag /*unused*/)
    {
      assert(valuePx >= 0);
      assert(PxAvailableSizeUtil::IsNormalValue(valuePx));
      m_height = valuePx;
    }

    constexpr PxSize2D ToPxSize2D() const
    {
      assert(IsNormal());
      return {m_width, m_height};
    }

    // constexpr PxPoint2 ToRealSizePxPoint2() const
    //{
    //  assert(IsNormal());
    //  return {m_width, m_height};
    //}


    inline constexpr bool IsInfinityWidth() const
    {
      return PxAvailableSizeUtil::IsConsideredInfiniteSpace(m_width);
    }

    inline constexpr bool IsInfinityHeight() const
    {
      return PxAvailableSizeUtil::IsConsideredInfiniteSpace(m_height);
    }

    inline constexpr bool IsNormalWidth() const
    {
      return PxAvailableSizeUtil::IsNormalValue(m_width);
    }

    inline constexpr bool IsNormalHeight() const
    {
      return PxAvailableSizeUtil::IsNormalValue(m_height);
    }

    inline constexpr bool IsNormal() const
    {
      return IsNormalWidth() && IsNormalHeight();
    }

    inline constexpr bool ContainsInfinity() const
    {
      return IsInfinityWidth() || IsInfinityHeight();
    }

    constexpr bool operator==(const PxAvailableSize& rhs) const noexcept
    {
      return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    constexpr bool operator!=(const PxAvailableSize& rhs) const noexcept
    {
      return m_width != rhs.m_width || m_height != rhs.m_height;
    }

    // @brief Returns a PxAvailableSize with all components being zero (0, 0)
    static constexpr PxAvailableSize Zero() noexcept
    {
      return {};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxAvailableSize valuePx)
    {
      return {PxAvailableSizeUtil::Add_LayoutSize_LayoutSize(sizePx.m_width, valuePx.m_width),
              PxAvailableSizeUtil::Add_LayoutSize_LayoutSize(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxAvailableSize valuePx)
    {
      return {PxAvailableSizeUtil::Sub_LayoutSize_LayoutSize(sizePx.m_width, valuePx.m_width),
              PxAvailableSizeUtil::Sub_LayoutSize_LayoutSize(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxPoint2 valuePx)
    {
      return {PxAvailableSizeUtil::Add_LayoutSize_Number(sizePx.m_width, valuePx.X),
              PxAvailableSizeUtil::Add_LayoutSize_Number(sizePx.m_height, valuePx.Y)};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxPoint2 valuePx)
    {
      return {PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_width, valuePx.X),
              PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_height, valuePx.Y)};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxSize2D valuePx)
    {
      return {PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_width, valuePx.Width()),
              PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_height, valuePx.Height())};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxSize2D valuePx)
    {
      return {PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_width, valuePx.Width()),
              PxAvailableSizeUtil::Sub_LayoutSize_Number(sizePx.m_height, valuePx.Height())};
    }
  };
}

#endif
