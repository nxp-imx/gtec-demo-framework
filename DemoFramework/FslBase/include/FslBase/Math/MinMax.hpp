#ifndef FSLBASE_MATH_MINMAX_HPP
#define FSLBASE_MATH_MINMAX_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

namespace Fsl
{
  //! @brief Contains all logic for a dealing with the value of a slider
  template <typename TValue>
  struct MinMax
  {
    using value_type = TValue;

  private:
    //! Get the minimum value (inclusive)
    value_type m_min{};

    //! Get the maximum value (inclusive)
    value_type m_max{};

  public:
    constexpr MinMax() noexcept = default;
    constexpr explicit MinMax(value_type min, value_type max) noexcept
      : m_min(min)
      , m_max(max >= min ? max : min)
    {
    }

    constexpr bool Set(value_type min, value_type max)
    {
      max = (max >= min ? max : min);
      const bool changed = min != m_min || max != m_max;
      if (changed)
      {
        m_min = min;
        m_max = max;
      }
      return changed;
    }

    constexpr value_type Min() const
    {
      return m_min;
    }

    constexpr value_type Max() const
    {
      return m_max;
    }

    constexpr value_type Center() const
    {
      return ((m_max - m_min) / 2) + m_min;
    }

    constexpr value_type Delta() const
    {
      return m_max - m_min;
    }

    constexpr bool operator==(const MinMax& rhs) const noexcept
    {
      return m_min == rhs.m_min && m_max == rhs.m_max;
    }

    constexpr bool operator!=(const MinMax& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
