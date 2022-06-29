#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_DATA_BOXPLOTDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_DATA_BOXPLOTDATA_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

namespace Fsl::UI
{
  //! Key values necessary to render a box plot.
  struct BoxPlotData
  {
    using value_type = float;

    value_type OutlierMin{};
    value_type Min{};
    value_type Q1{};
    value_type Q2{};
    value_type Q3{};
    value_type Max{};
    value_type OutlierMax{};

    constexpr BoxPlotData() noexcept = default;
    constexpr BoxPlotData(const value_type outlierMinValue, const value_type minValue, const value_type q1, const value_type q2, const value_type q3,
                          const value_type maxValue, const value_type outlierMaxValue) noexcept
      : OutlierMin(outlierMinValue)
      , Min(minValue)
      , Q1(q1)
      , Q2(q2)
      , Q3(q3)
      , Max(maxValue)
      , OutlierMax(outlierMaxValue)
    {
    }

    constexpr bool operator==(const BoxPlotData& rhs) const noexcept
    {
      return OutlierMin == rhs.OutlierMin && Min == rhs.Min && Q1 == rhs.Q1 && Q2 == rhs.Q2 && Q3 == rhs.Q3 && Max == rhs.Max &&
             OutlierMax == rhs.OutlierMax;
    }

    constexpr bool operator!=(const BoxPlotData& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
