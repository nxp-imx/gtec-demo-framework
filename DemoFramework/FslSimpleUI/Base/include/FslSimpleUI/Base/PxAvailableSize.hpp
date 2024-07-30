#ifndef FSLSIMPLEUI_BASE_PXAVAILABLESIZE_HPP
#define FSLSIMPLEUI_BASE_PXAVAILABLESIZE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslSimpleUI/Base/PxAvailableSize1D.hpp>
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
    using value_type = PxAvailableSize1D;
    using raw_value_type = value_type::raw_value_type;

  private:
    value_type m_width;
    value_type m_height;

  public:
    constexpr PxAvailableSize() noexcept = default;

    constexpr value_type Width() const noexcept
    {
      return m_width;
    }

    constexpr value_type Height() const noexcept
    {
      return m_height;
    }

    constexpr raw_value_type RawWidth() const noexcept
    {
      return m_width.Value();
    }

    constexpr raw_value_type RawHeight() const noexcept
    {
      return m_height.Value();
    }

    constexpr PxAvailableSize(const PxAvailableSize1D width, const PxAvailableSize1D height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxAvailableSize(const PxSize1D width, const PxAvailableSize1D height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    constexpr PxAvailableSize(const PxAvailableSize1D width, const PxSize1D height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }


    constexpr PxAvailableSize(const PxSize1D width, const PxSize1D height) noexcept
      : m_width(width)
      , m_height(height)
    {
    }

    //! When using this constructor we expect a normal value this is not in the magic NaN or Infinity ranges!
    constexpr explicit PxAvailableSize(const PxPoint2 value) noexcept
      : m_width(value.X)
      , m_height(value.Y)
    {
    }

    constexpr explicit PxAvailableSize(const PxSize2D value) noexcept
      : m_width(value.Width())
      , m_height(value.Height())
    {
    }


    constexpr void SetNormalWidth(const int32_t valuePx) noexcept
    {
      m_width.SetNormalValue(valuePx);
    }

    constexpr void SetNormalHeight(const int32_t valuePx) noexcept
    {
      m_height.SetNormalValue(valuePx);
    }

    constexpr void UncheckedSetNormalWidth(const int32_t valuePx) noexcept
    {
      m_height.UncheckedSetNormalValue(valuePx);
    }

    constexpr void UncheckedSetNormalHeight(const int32_t valuePx) noexcept
    {
      m_height.UncheckedSetNormalValue(valuePx);
    }

    constexpr PxSize2D ToPxSize2D() const noexcept
    {
      return {m_width.ToPxSize1D(), m_height.ToPxSize1D()};
    }


    constexpr PxSize1D ToPxWidth() const noexcept
    {
      return m_width.ToPxSize1D();
    }


    constexpr PxSize1D ToPxHeight() const noexcept
    {
      return m_height.ToPxSize1D();
    }


    inline constexpr bool IsInfinityWidth() const noexcept
    {
      return m_width.IsInfinity();
    }

    inline constexpr bool IsInfinityHeight() const
    {
      return m_height.IsInfinity();
    }

    inline constexpr bool IsNormalWidth() const
    {
      return m_width.IsNormal();
    }

    inline constexpr bool IsNormalHeight() const
    {
      return m_height.IsNormal();
    }

    inline constexpr bool IsNormal() const
    {
      return IsNormalWidth() && IsNormalHeight();
    }

    inline constexpr bool ContainsInfinity() const noexcept
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

    inline static constexpr PxAvailableSize Create(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {PxAvailableSize1D::Create(width), PxAvailableSize1D::Create(height)};
    }

    inline static constexpr PxAvailableSize UncheckedCreate(const raw_value_type width, const raw_value_type height) noexcept
    {
      return {PxAvailableSize1D::UncheckedCreate(width), PxAvailableSize1D::UncheckedCreate(height)};
    }


    // @brief Returns a PxAvailableSize with all components being zero (0, 0)
    static constexpr PxAvailableSize Zero() noexcept
    {
      return {};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxAvailableSize valuePx) noexcept
    {
      return {PxAvailableSize1D::Add(sizePx.m_width, valuePx.m_width), PxAvailableSize1D::Add(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxAvailableSize valuePx) noexcept
    {
      return {PxAvailableSize1D::Subtract(sizePx.m_width, valuePx.m_width), PxAvailableSize1D::Subtract(sizePx.m_height, valuePx.m_height)};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxPoint2 valuePx) noexcept
    {
      return {PxAvailableSize1D::Add(sizePx.m_width, PxValue(valuePx.X)), PxAvailableSize1D::Add(sizePx.m_height, valuePx.Y)};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxPoint2 valuePx) noexcept
    {
      return {PxAvailableSize1D::Subtract(sizePx.m_width, PxValue(valuePx.X)), PxAvailableSize1D::Subtract(sizePx.m_height, valuePx.Y)};
    }

    static constexpr PxAvailableSize Add(const PxAvailableSize sizePx, const PxSize2D valuePx) noexcept
    {
      return {PxAvailableSize1D::Add(sizePx.m_width, valuePx.Width()), PxAvailableSize1D::Add(sizePx.m_height, valuePx.Height())};
    }

    static constexpr PxAvailableSize Subtract(const PxAvailableSize sizePx, const PxSize2D valuePx) noexcept
    {
      return {PxAvailableSize1D::Subtract(sizePx.m_width, valuePx.Width()), PxAvailableSize1D::Subtract(sizePx.m_height, valuePx.Height())};
    }


    static constexpr PxSize2D MinPxSize2D(const PxAvailableSize val0, const PxSize2D val1) noexcept
    {
      return {PxSize1D::Create(MathHelper::Min(val0.RawWidth(), val1.RawWidth())),
              PxSize1D::Create(MathHelper::Min(val0.RawHeight(), val1.RawHeight()))};
    }


    static constexpr PxSize2D MinPxSize2D(const PxSize2D val0, const PxAvailableSize val1) noexcept
    {
      return {PxSize1D::Create(MathHelper::Min(val0.RawWidth(), val1.RawWidth())),
              PxSize1D::Create(MathHelper::Min(val0.RawHeight(), val1.RawHeight()))};
    }
  };
}

#endif
