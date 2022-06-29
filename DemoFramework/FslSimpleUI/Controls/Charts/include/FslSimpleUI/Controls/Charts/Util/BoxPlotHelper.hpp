#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_UTIL_BOXPLOTHELPER_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_UTIL_BOXPLOTHELPER_HPP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslSimpleUI/Controls/Charts/Data/BoxPlotData.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI::BoxPlotHelper
{
  /// <summary>
  /// The minimum number of entries that need to be in a span to calculate a valid box plot
  /// </summary>
  constexpr uint32_t MinimumEntries = 5;

  //! @brief Calculate the median in a sorted span (low to high).
  //! @param span a span of sorted values (sorting is expected to be low to high).
  template <typename T>
  inline double CalculateMedian(const ReadOnlySpan<T> span)
  {
    // A median is found by arranging the numbers in the list in numerical order from low to high (this is what we expect to be in span)
    // if the span.size() is odd -> take the two middle members and add together then divide by two
    //

    assert(span.size() >= 2);
    const auto size = span.size();

    // 2 | 0+1 | 2/2=1
    // 3 | 1   | 3/2=1
    // 4 | 1+2 | 4/2=2
    // 5 | 2   | 5/2=2
    // 6 | 2+3 | 6/2=3
    // 7 | 3   | 7/2=3
    const auto halfSize = size / 2;
    //                         even                                        : odd
    return ((size & 1) == 0) ? (static_cast<double>((span[halfSize - 1] + span[halfSize])) / 2.0) : static_cast<double>(span[halfSize]);
  }

  //! @brief Calculate the box plot data from a sorted span. (low to high)
  //! @param span a span of sorted values (sorting is expected to be low to high).
  template <typename T>
  inline BoxPlotData Calculate(const ReadOnlySpan<T> span)
  {
    const auto size = span.size();
    if (size < MinimumEntries)
      throw NotSupportedException("we expect at least five entries");

    // Find the median of the span
    const auto q2 = CalculateMedian(span);

    // create two equally size spans (split on the middle), if the original size was uneven we skip the middle number.
    // since we calc on integers this will be equal to half rounded down.
    const auto halfSize = size / 2;

    // find the median of the first half
    const double q1 = CalculateMedian(span.subspan(0, halfSize));
    // find the median of the second half
    const double q3 = CalculateMedian(span.subspan(size - halfSize, halfSize));

    assert(q1 <= q3);
    const double iqr = q3 - q1;
    const double lowerLimit = q1 - (1.5 * iqr);
    const double upperLimit = q3 + (1.5 * iqr);

    // find the first element that is greater or equal from the beginning of the span
    // We expect the search to always find a element in the list (due to the way the value we search for is calculated).
    assert(span.back() >= lowerLimit);
    auto min = lowerLimit;    // just to have a default value
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      min = span[i];
      if (min >= lowerLimit)
      {
        break;
      }
    }

    // find the last element that is less or equal from the end of the span
    // We expect the search to always find a element in the list (due to the way the value we search for is calculated).
    assert(span.front() <= upperLimit);
    auto max = upperLimit;    // just to have a default value
    for (std::size_t i = span.size(); i > 0; --i)
    {
      max = span[i - 1];
      if (max <= upperLimit)
      {
        break;
      }
    }
    return {float(span.front()), float(min), float(q1), float(q2), float(q3), float(max), float(span.back())};
  }
}

#endif
