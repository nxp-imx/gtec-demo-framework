#ifndef FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERPIXELSPANINFO_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LOGIC_SLIDERPIXELSPANINFO_HPP
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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <cmath>

namespace Fsl::UI
{
  struct SliderPixelSpanInfo
  {
  private:
    PxValue m_startPx;
    PxSize1D m_lengthPx;
    bool m_reverseDirection{false};

  public:
    constexpr SliderPixelSpanInfo() noexcept = default;

    //! @brief Create a pixel span info
    constexpr explicit SliderPixelSpanInfo(const PxValue startPx, const PxSize1D lengthPx, const bool reverseDirection = false) noexcept
      : m_startPx(startPx)
      , m_lengthPx(lengthPx)
      , m_reverseDirection(reverseDirection)
    {
    }

    constexpr PxValue GetStartPx() const
    {
      return m_startPx;
    }

    constexpr PxSize1D GetLengthPx() const
    {
      return m_lengthPx;
    }

    constexpr bool IsReversedDirection() const
    {
      return m_reverseDirection;
    }

    //! @brief Calculate the percentage of the span the position represents
    constexpr float CalculatePercentage(const PxValue positionPx) const
    {
      // const auto clampedTickInterval(MathHelper::Clamp(pxTickInterval, 1, std::max(m_lengthPx, 1)));

      const auto includedEndPx = GetIncludedEndPx();
      // clamp the position to be inside
      auto relativePositionPx = MathHelper::Clamp(positionPx, m_startPx, m_startPx + includedEndPx) - m_startPx;
      // Calculate the percentage
      auto percentage = (includedEndPx.Value > 0 ? (static_cast<float>(relativePositionPx.Value) / static_cast<float>(includedEndPx.Value)) : 0.0f);

      return static_cast<float>(!m_reverseDirection ? percentage : (1.0 - percentage));
    }

    PxValue CalcPercentageToPxPosition(const float percentage) const
    {
      float cappedPercentage = MathHelper::Clamp(percentage, 0.0f, 1.0f);
      cappedPercentage = !m_reverseDirection ? cappedPercentage : (1.0f - cappedPercentage);

      const auto includedEndPx = GetIncludedEndPx();
      const auto positionPx = PxValue(static_cast<int32_t>(std::round(cappedPercentage * static_cast<float>(includedEndPx.Value))) + m_startPx.Value);
      return MathHelper::Clamp(positionPx, m_startPx, m_startPx + includedEndPx);
    }

    constexpr bool operator==(const SliderPixelSpanInfo& rhs) const noexcept
    {
      return m_startPx == rhs.m_startPx && m_lengthPx == rhs.m_lengthPx && m_reverseDirection == rhs.m_reverseDirection;
    }

    constexpr bool operator!=(const SliderPixelSpanInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }

  private:
    // The last included pixel in the span range (this is what we need to ensure that all slider values can be selected)
    // beware this is relative to zero not m_startPx
    constexpr PxValue GetIncludedEndPx() const
    {
      return m_lengthPx > PxValue(0) ? (m_lengthPx - PxValue(1)) : PxValue(0);
    }
  };
}

#endif
