
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

#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Controls/Charts/Canvas/ChartCanvas1D.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>

namespace Fsl::UI
{
  bool ChartCanvas1D::LayoutCache::SetDensityDpi(const uint32_t densityDpi)
  {
    const bool changed = densityDpi != UnitConverter.GetDensityDpi();
    if (changed)
    {
      UnitConverter.SetDensityDpi(densityDpi);
      // Cache the size of one DP in pixels
      SizeOfOneDpInPixels = UnitConverter.ToPxSize1D(DpSize1D::Create(1, OptimizationCheckFlag::NoCheck));
    }
    return changed;
  }


  ChartCanvas1D::ChartCanvas1D(const SpriteUnitConverter& unitConverter)
    : m_layoutCache(unitConverter.GetDensityDpi() + 1)
  {
  }


  bool ChartCanvas1D::SetOrientation(const LayoutOrientation value)
  {
    bool changed = value != m_orientation;
    if (changed)
    {
      m_orientation = value;
    }
    return changed;
  }


  PxSize2D ChartCanvas1D::Arrange(const PxSize2D finalSizePx, const PxSize1D itemHeightPx)
  {
    return Arrange(finalSizePx, itemHeightPx, PxSize1D(), 1);
  }

  PxSize2D ChartCanvas1D::Arrange(const PxSize2D finalSizePx, const PxSize1D itemHeightPx, const PxSize1D spacingPx, const uint8_t channels)
  {
    FSL_PARAM_NOT_USED(itemHeightPx);
    FSL_PARAM_NOT_USED(spacingPx);
    FSL_PARAM_NOT_USED(channels);
    // The data view is not allowed to change after we enter after we enter the measure/arrange and draw cycle
    // assert(IsCacheUnchanged());

    Vector2 chartScale{1.0f, 1.0f};
    // PxSize2D desiredSizePx;
    // const PxSize2D desiredSizePx = PxSize2D(CalculateAxisMinimumPixelSize(), itemHeightPx);
    m_layoutCache.Arrange = ArrangeCache(chartScale);
    return finalSizePx;
  }


  PxSize2D ChartCanvas1D::Measure(const PxAvailableSize availableSizePx, const PxSize1D itemHeightPx)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    const auto axisMinimumSizePx = CalculateAxisMinimumPixelSize();
    return m_orientation == LayoutOrientation::Horizontal ? PxSize2D(axisMinimumSizePx, itemHeightPx) : PxSize2D(itemHeightPx, axisMinimumSizePx);
  }


  PxSize2D ChartCanvas1D::Measure(const PxAvailableSize availableSizePx, const PxSize1D itemHeightPx, const PxSize1D spacingPx,
                                  const uint16_t entries)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    const auto axisMinimumSizePx = CalculateAxisMinimumPixelSize();
    const auto finalHeightPx = itemHeightPx * PxValue(entries) + (spacingPx * PxSize1D::Create(entries - 1));

    return m_orientation == LayoutOrientation::Horizontal ? PxSize2D(axisMinimumSizePx, finalHeightPx) : PxSize2D(finalHeightPx, axisMinimumSizePx);
  }

  void ChartCanvas1D::OnResolutionChanged(const SpriteUnitConverter& unitConverter)
  {
    m_layoutCache.SetDensityDpi(unitConverter.GetDensityDpi());
  }


  PxSize1D ChartCanvas1D::CalculateAxisMinimumPixelSize() const
  {
    // assert(IsCacheUnchanged());

    const auto desiredSizeDpf = DpSize1DF::Create(static_cast<float>(m_axisRange.Delta()));
    return m_layoutCache.UnitConverter.ToPxSize1D(desiredSizeDpf);
  }
}
