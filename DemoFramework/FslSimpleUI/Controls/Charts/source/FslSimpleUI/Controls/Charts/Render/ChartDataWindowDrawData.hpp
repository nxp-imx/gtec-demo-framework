#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_CHARTDATAWINDOWDRAWDATA_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_RENDER_CHARTDATAWINDOWDRAWDATA_HPP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChartConfig.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartComplexDataEntry.hpp>
#include <FslSimpleUI/Controls/Charts/Data/IChartComplexDataWindow.hpp>
#include <array>
#include <memory>
#include <string>

namespace Fsl
{
  namespace UI
  {
    namespace Render
    {
      namespace ChartDataWindowDrawDataConfig
      {
        constexpr uint32_t MaxStackedEntries = static_cast<uint32_t>(std::tuple_size<ChartComplexDataEntry::array_type>());
        constexpr uint32_t MaxGridLines = AreaChartConfig::MaxGridLines * 2;
      }

      struct ChartDataWindowDrawData : public ICustomDrawData
      {
        struct GridLineRecord
        {
          int32_t LinePositionPx{0};
          PxPoint2 LabelOffsetPx;
          PxSize2D LabelSizePx;
          PxRectangle LabelBackgroundRectanglePx;
          uint8_t Alpha{0};
          std::string Label;
        };

        struct ChartRecord
        {
          uint32_t ViewMax{0};
          float DataRenderScale{1.0f};
          //! The width of a chart entry in pixels
          int32_t EntryWidthPx{1};
        };

        std::shared_ptr<IChartComplexDataWindow> Data;

        // The chart data
        ChartRecord Chart;

        std::array<Color, ChartDataWindowDrawDataConfig::MaxStackedEntries> ChartColors;
        struct ChartColorCache
        {
          bool Valid{false};
          Color OldBaseColor;
          std::array<Color, ChartDataWindowDrawDataConfig::MaxStackedEntries> Premultiplied;
        };
        ChartColorCache ChartCache;

        //! The grid line y-positions
        std::array<GridLineRecord, ChartDataWindowDrawDataConfig::MaxGridLines> GridLines;
        uint32_t GridLineCount{0};
        std::array<GridLineRecord, ChartDataWindowDrawDataConfig::MaxGridLines> FadingGridLines;


        uint32_t FadingGridLineCount{0};

        // int32_t FontBaseLinePx{0};

        constexpr void Clear()
        {
          GridLineCount = 0u;
          FadingGridLineCount = 0u;
        }

        bool SetEntryColor(const uint32_t index, const Color color)
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

        void FillChartCache(const Color baseColor)
        {
          if (!ChartCache.Valid || baseColor != ChartCache.OldBaseColor)
          {
            ChartCache.Valid = true;
            ChartCache.OldBaseColor = baseColor;
            for (std::size_t i = 0; i < ChartCache.Premultiplied.size(); ++i)
            {
              ChartCache.Premultiplied[i] = Color::Premultiply(ChartColors[i] * baseColor);
            }
          }
        }
      };
    }
  }
}


#endif
