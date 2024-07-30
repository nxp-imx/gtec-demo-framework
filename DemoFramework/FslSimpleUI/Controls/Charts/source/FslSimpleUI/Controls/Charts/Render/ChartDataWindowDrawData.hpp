#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_CHARTDATAWINDOWDRAWDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_CHARTDATAWINDOWDRAWDATA_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Math/MinMax.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChartConfig.hpp>
#include <FslSimpleUI/Controls/Charts/Canvas/ChartCanvas1D.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataEntry.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <array>
#include <memory>
#include <string>

namespace Fsl::UI::Render
{
  namespace ChartDataWindowDrawDataConfig
  {
    constexpr uint32_t MaxStackedEntries = static_cast<uint32_t>(std::tuple_size<ChartDataEntry::array_type>());
    constexpr uint32_t MaxGridLines = AreaChartConfig::MaxGridLines * 2;
  }

  struct ChartDataWindowDrawData : public ICustomDrawData
  {
    struct GridLineRecord
    {
      PxValue LinePositionPx{0};
      PxPoint2 LabelOffsetPx;
      PxSize2D LabelSizePx;
      PxRectangle LabelBackgroundRectanglePx;
      ColorChannelValueU16 Alpha{0};
      std::string Label;
    };

    struct ChartRecord
    {
      uint32_t ViewMax{0};
      float DataRenderScale{1.0f};
      //! The width of a chart entry in pixels
      PxSize1D EntryWidthPx{PxSize1D::Create(1)};
    };

    ChartCanvas1D Canvas;
    std::shared_ptr<ChartDataView> DataView;

    // The chart data
    ChartRecord Chart;

    std::array<UIRenderColor, ChartDataWindowDrawDataConfig::MaxStackedEntries> ChartColors;
    struct ChartColorCache
    {
      bool Valid{false};
      UIRenderColor OldBaseColor;
      std::array<UIRenderColor, ChartDataWindowDrawDataConfig::MaxStackedEntries> Premultiplied;
    };
    ChartColorCache ChartCache;

    //! The grid line y-positions
    std::array<GridLineRecord, ChartDataWindowDrawDataConfig::MaxGridLines> GridLines;
    uint32_t GridLineCount{0};
    std::array<GridLineRecord, ChartDataWindowDrawDataConfig::MaxGridLines> FadingGridLines;


    uint32_t FadingGridLineCount{0};

    // int32_t FontBaseLinePx{0};

    explicit ChartDataWindowDrawData(const SpriteUnitConverter& unitConverter)
      : Canvas(unitConverter)
    {
      Canvas.SetOrientation(LayoutOrientation::Vertical);
    }

    constexpr void Clear()
    {
      GridLineCount = 0u;
      FadingGridLineCount = 0u;
    }

    bool SetEntryColor(const uint32_t index, const UIRenderColor color)
    {
      if (index >= ChartColors.size())
      {
        throw std::invalid_argument("index");
      }
      const bool changed = color != ChartColors[index];
      if (changed)
      {
        ChartColors[index] = color;
        ChartCache.Valid = false;
      }
      return changed;
    }

    void FillChartCache(const UIRenderColor baseColor)
    {
      if (!ChartCache.Valid || baseColor != ChartCache.OldBaseColor)
      {
        ChartCache.Valid = true;
        ChartCache.OldBaseColor = baseColor;
        for (std::size_t i = 0; i < ChartCache.Premultiplied.size(); ++i)
        {
          ChartCache.Premultiplied[i] = UIRenderColor::Premultiply(ChartColors[i] * baseColor);
        }
      }
    }
  };
}


#endif
