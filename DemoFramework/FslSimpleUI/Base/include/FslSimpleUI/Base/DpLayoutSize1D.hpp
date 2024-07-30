#ifndef FSLSIMPLEUI_BASE_DPLAYOUTSIZE1D_HPP
#define FSLSIMPLEUI_BASE_DPLAYOUTSIZE1D_HPP
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
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Dp/DpValue.hpp>
#include <FslBase/Math/Dp/DpValueF.hpp>

namespace Fsl::UI
{
  struct DpLayoutSize1D
  {
    using value_type = DpValueF;

  private:
    value_type m_value{-1.0f};

  public:
    constexpr DpLayoutSize1D() noexcept = default;

    constexpr explicit DpLayoutSize1D(const float value) noexcept
      : m_value(DpValueF(value))
    {
    }

    constexpr explicit DpLayoutSize1D(const DpSize1DF value) noexcept
      : m_value(value.Value())
    {
    }

    constexpr explicit DpLayoutSize1D(const value_type value) noexcept
      : m_value(value >= value_type(0.0f) ? value : value_type(-1.0f))
    {
    }

    constexpr explicit DpLayoutSize1D(const DpValue value) noexcept
      : m_value(value.Value >= 0 ? value_type(static_cast<value_type::value_type>(value.Value)) : value_type(-1.0f))
    {
    }

    constexpr bool HasValue() const noexcept
    {
      return m_value >= value_type(0.0f);
    }

    constexpr value_type Value() const noexcept
    {
      return (m_value >= value_type(0.0f) ? m_value : value_type(0.0f));
    }

    constexpr DpSize1DF Size() const noexcept
    {
      return DpSize1DF::Create(m_value);
    }

    constexpr DpSize1DF SizeOr(const DpSize1DF defaultValue) const noexcept
    {
      return m_value.Value >= 0 ? DpSize1DF ::Create(m_value, OptimizationCheckFlag::NoCheck) : defaultValue;
    }

    //! @brief Get the unfiltered value
    constexpr value_type RawValue() const noexcept
    {
      return m_value;
    }

    static constexpr DpLayoutSize1D Create(float value) noexcept
    {
      return DpLayoutSize1D(DpValueF(value));
    }


    constexpr bool operator==(const DpLayoutSize1D rhs) const noexcept
    {
      return m_value == rhs.m_value;
    }

    constexpr bool operator!=(const DpLayoutSize1D rhs) const noexcept
    {
      return m_value != rhs.m_value;
    }
  };
}

#endif
