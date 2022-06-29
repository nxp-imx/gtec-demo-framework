#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_AREACHARTGRIDLINEMANAGER_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_AREACHARTGRIDLINEMANAGER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/ChartGridConfig.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/ChartGridLineInfo.hpp>
#include <FslSimpleUI/Controls/Charts/Render/RenderAreaChartConfig.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  class NineSliceSprite;
  class SpriteFont;
  class TransitionCache;

  namespace UI
  {
    class IChartGridLines;
    class ChartDataView;

    namespace Render
    {
      struct ChartDataWindowDrawData;
    }

    class AreaChartGridLineManager
    {
      struct ViewRecord
      {
        TransitionValue m_animatedViewMin;
        TransitionValue m_animatedViewMax;
        MinMax<uint32_t> m_actualMinMax;

        ViewRecord(TransitionCache& rTransitionCache, const TimeSpan transitionTime);
        bool SetViewMinMax(const MinMax<uint32_t> minMax);
        uint32_t ViewMin() const;
        uint32_t ViewMax() const;
        MinMax<uint32_t> ViewMinMax() const;

        bool Update(const TimeSpan& timespan);
        bool IsAnimating() const;
        // void FinishAnimation();
      };

      struct ChartDataRecord
      {
        ReadOnlySpan<ChartGridLineInfo> Span;
      };

      struct GridLineRecord
      {
        bool Suggested{false};
        TransitionValue m_animatedAlpha;
        uint8_t m_actualAlpha{0};
        uint32_t RawDataPosition{0};
        std::string Label;

        GridLineRecord() = default;
        GridLineRecord(TransitionCache& rTransitionCache, const TimeSpan transitionTime);

        void SetAlpha(const uint8_t alpha);
        uint8_t GetAlpha() const;

        void Clear()
        {
          Suggested = false;
          m_animatedAlpha.SetActualValue(0.0f);
          m_actualAlpha = 0;
          RawDataPosition = 0;
        }
        bool Update(const TimeSpan& timespan);
        bool IsAnimating() const;
      };


      int32_t m_chartEntryWidthPx;
      int32_t m_chartLabelSpacingPx;

      std::vector<GridLineRecord> m_gridLineRecords;
      uint32_t m_gridLineRecordCount{0};

      ViewRecord m_viewRecord;
      bool m_isAnimating{true};

      std::shared_ptr<IChartGridLines> m_gridLines;
      std::shared_ptr<ChartDataView> m_dataView;

    public:
      AreaChartGridLineManager(TransitionCache& rTransitionCache, const TimeSpan transitionTime, const TimeSpan transitionTimespanLabels,
                               const int32_t chartEntryWidthPx, const int32_t chartLabelSpacingPx);


      const std::shared_ptr<ChartDataView>& GetDataView() const
      {
        return m_dataView;
      }

      void Reset();

      bool SetGridLines(const std::shared_ptr<IChartGridLines>& gridLines);
      bool SetDataView(const std::shared_ptr<ChartDataView>& data);

      int32_t GetChartEntryWidth() const
      {
        return m_chartEntryWidthPx;
      }

      void SetChartEntryWidth(const int32_t chartEntryWidthPx);

      int32_t GetChartLabelSpacing() const
      {
        return m_chartLabelSpacingPx;
      }

      void SetChartLabelSpacing(const int32_t chartLabelSpacingPx);

      void ExtractDrawData(Render::ChartDataWindowDrawData& rDst, const PxSize2D renderSizePx, const NineSliceSprite* const pBackgroundSprite,
                           const SpriteFont* const pFont, const bool matchDataViewEntries);

      void Update(const TimeSpan& timespan);
      bool IsAnimating() const;

    private:
      static void SelectGridLines(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount, const IChartGridLines* const pGridLines,
                                  const ViewRecord& viewRecord);

      static void DetermineVisibility(Render::ChartDataWindowDrawData& rDst, Span<GridLineRecord> gridLines, const ViewRecord& viewRecord,
                                      const PxSize2D renderSizePx, const NineSliceSprite* const pBackgroundSprite, const SpriteFont* const pFont,
                                      const int32_t chartEntryWidthPx, const int32_t chartLabelSpacingPx);


      static ChartDataRecord ExtractChartData(const IChartGridLines* const pGridLines, const MinMax<uint32_t> minMax);
      static uint32_t InsertAt(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount, const uint32_t insertAtIndex,
                               const ChartGridLineInfo& srcInfo);
      static uint32_t RemoveAt(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount, const uint32_t removeAtIndex);

      static bool AreGridLinePositionsValid(const ReadOnlySpan<GridLineRecord> span);
      static void SanityCheck(const ReadOnlySpan<GridLineRecord> span);
      bool CheckIsAnimating() const;
    };
  }
}


#endif
