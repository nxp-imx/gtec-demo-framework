#ifndef FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERCONSTRAINEDVALUE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERCONSTRAINEDVALUE_HPP
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

#include <FslBase/Math/ConstrainedValue.hpp>

namespace Fsl
{
  namespace UI
  {
    namespace SliderConstrainedValueUtil
    {
      inline float SnapToTick(float currentValue, const float tickFrequency)
      {
        assert(tickFrequency > 1.0f);
        return std::floor((currentValue + (tickFrequency / 2.0f)) / tickFrequency) * tickFrequency;
      }

      inline double SnapToTick(double currentValue, const double tickFrequency)
      {
        assert(tickFrequency > 1.0);
        return std::floor((currentValue + (tickFrequency / 2.0)) / tickFrequency) * tickFrequency;
      }

      template <typename T>
      constexpr inline T SnapToTick(T currentValue, const T tickFrequency)
      {
        assert(tickFrequency > 1);
        return ((currentValue + (tickFrequency / 2)) / tickFrequency) * tickFrequency;
      }
    }

    //! @brief Contains all logic for a dealing with the value of a slider
    template <typename TValue>
    struct SliderConstrainedValue
    {
      using value_type = TValue;

    private:
      value_type m_min{};
      value_type m_max{};
      value_type m_value{};
      value_type m_tickFrequency{0};

    public:
      SliderConstrainedValue() noexcept = default;
      explicit SliderConstrainedValue(value_type min, value_type max) noexcept
        : m_min(min)
        , m_max(max >= min ? max : min)
        , m_value(min)
      {
      }

      explicit SliderConstrainedValue(value_type value, value_type min, value_type max) noexcept
        : m_min(min)
        , m_max(max >= min ? max : min)
        , m_value(MathHelper::Clamp(value, m_min, m_max))
      {
      }

      explicit SliderConstrainedValue(value_type value, value_type min, value_type max, value_type tickFrequency) noexcept
        : m_min(min)
        , m_max(max >= min ? max : min)
        , m_value(MathHelper::Clamp(value, m_min, m_max))
      {
        SetTickFrequency(tickFrequency);
      }

      bool SetRange(value_type min, value_type max)
      {
        if (min == m_min && max == m_max)
        {
          return false;
        }
        m_min = min;
        m_max = (max >= min ? max : min);
        m_value = MathHelper::Clamp(m_value, m_min, m_max);
        return true;
      }

      value_type Get() const
      {
        return m_value;
      }

      value_type Min() const
      {
        return m_min;
      }

      value_type Max() const
      {
        return m_max;
      }

      value_type TickFrequency() const
      {
        return m_tickFrequency;
      }

      value_type Center() const
      {
        return ((m_max - m_min) / 2) + m_min;
      }


      //! @brief Get the percentage
      float GetPercentage() const
      {
        assert(m_min <= m_max);
        const auto delta = m_max - m_min;
        return delta > 0 ? MathHelper::Clamp((m_value - m_min) / static_cast<float>(delta), 0.0f, 1.0f) : 0.0f;
      }


      //! @brief set the value based on percentage
      //! @return true if the m_value was modified
      bool SetPercentage(const float percentage)
      {
        assert(m_min <= m_max);
        const auto clampedPercentage = MathHelper::Clamp(percentage, 0.0f, 1.0f);
        const value_type finalValue =
          !std::is_floating_point<value_type>::value
            ? MathHelper::Clamp(static_cast<value_type>(std::round((m_max - m_min) * clampedPercentage)) + m_min, m_min, m_max)
            : MathHelper::Clamp(value_type((m_max - m_min) * clampedPercentage) + m_min, m_min, m_max);
        return Set(finalValue);
      }

      //! @brief set the value b
      //! @return true if the m_value was modified
      bool Set(const value_type value)
      {
        auto cappedValue = MathHelper::Clamp(value, m_min, m_max);
        if (m_tickFrequency > 1)
        {
          cappedValue -= m_min;
          cappedValue = SliderConstrainedValueUtil::SnapToTick(cappedValue, m_tickFrequency);
          cappedValue += m_min;
        }
        if (cappedValue != m_value)
        {
          m_value = cappedValue;
          return true;
        }
        return false;
      }

      bool SetTickFrequency(const value_type tickFrequency)
      {
        auto clampedTickFrequency = MathHelper::Clamp(tickFrequency, value_type(0), m_max);
        if (clampedTickFrequency != m_tickFrequency)
        {
          m_tickFrequency = clampedTickFrequency;
          if (clampedTickFrequency > 1)
          {
            // Ensure that the value is snapped
            Set(m_value);
          }
          return true;
        }
        return false;
      }

      bool operator==(const SliderConstrainedValue& rhs) const noexcept
      {
        return m_value == rhs.m_value && m_min == rhs.m_min && m_max == rhs.m_max && m_tickFrequency == rhs.m_tickFrequency;
      }

      bool operator!=(const SliderConstrainedValue& rhs) const noexcept
      {
        return !(*this == rhs);
      }
    };
  }
}

#endif
