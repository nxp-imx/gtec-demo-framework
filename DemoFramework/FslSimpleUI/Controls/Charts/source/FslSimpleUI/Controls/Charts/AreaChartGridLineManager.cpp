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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChartGridLineManager.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/IChartGridLines.hpp>
#include <cassert>
#include "Render/ChartDataWindowDrawData.hpp"

//#define LOCAL_SANITY_CHECK
#ifdef LOCAL_SANITY_CHECK
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK(X) SanityCheck(X)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK(X) \
  {                     \
  }
#endif

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      const uint32_t GrowBy = 32;
    }

    inline void SetEntry(Render::ChartDataWindowDrawData::GridLineRecord& rRec, const uint8_t alpha, const int32_t gridLineYPositionPx,
                         const PxPoint2 backgroundOffsetPx, const int32_t labelBackgroundXOffsetPx, const int32_t fontBaseLinePx,
                         const PxSize2D labelSizePx, const PxSize2D finalSizePx, const StringViewLite& labelStrView)
    {
      rRec.Alpha = alpha;
      rRec.LinePositionPx = gridLineYPositionPx;
      rRec.LabelOffsetPx = PxPoint2(labelBackgroundXOffsetPx, gridLineYPositionPx - fontBaseLinePx);
      rRec.LabelSizePx = labelSizePx;
      rRec.LabelBackgroundRectanglePx = PxRectangle(backgroundOffsetPx, finalSizePx);
      StringViewLiteUtil::Set(rRec.Label, labelStrView);
    }
  }

  AreaChartGridLineManager::ViewRecord::ViewRecord(TransitionCache& rTransitionCache, const TimeSpan transitionTime)
    : m_animatedViewMin(rTransitionCache, TimeSpan(transitionTime))
    , m_animatedViewMax(rTransitionCache, TimeSpan(transitionTime))
  {
    m_animatedViewMax.SetActualValue(5000);
  }


  bool AreaChartGridLineManager::ViewRecord::SetViewMinMax(const MinMax<uint32_t> minMax)
  {
    bool wasChanged = minMax != m_actualMinMax;
    if (wasChanged)
    {
      m_actualMinMax = minMax;
      m_animatedViewMin.SetValue(static_cast<float>(minMax.Min()));
      m_animatedViewMax.SetValue(static_cast<float>(minMax.Max()));
    }
    return wasChanged;
  }


  uint32_t AreaChartGridLineManager::ViewRecord::ViewMin() const
  {
    return m_animatedViewMin.IsCompleted() ? m_actualMinMax.Min() : static_cast<uint32_t>(m_animatedViewMin.GetValue());
  }


  uint32_t AreaChartGridLineManager::ViewRecord::ViewMax() const
  {
    return m_animatedViewMax.IsCompleted() ? m_actualMinMax.Max() : static_cast<uint32_t>(m_animatedViewMax.GetValue());
  }

  MinMax<uint32_t> AreaChartGridLineManager::ViewRecord::ViewMinMax() const
  {
    return MinMax<uint32_t>(ViewMin(), ViewMax());
  }

  bool AreaChartGridLineManager::ViewRecord::Update(const TimeSpan& timespan)
  {
    m_animatedViewMin.Update(timespan);
    m_animatedViewMax.Update(timespan);
    return IsAnimating();
  }

  bool AreaChartGridLineManager::ViewRecord::IsAnimating() const
  {
    return (!m_animatedViewMin.IsCompleted()) || (!m_animatedViewMax.IsCompleted());
  }


  // void AreaChartGridLineManager::ViewRecord::FinishAnimation()
  //{
  //  m_animatedViewMin.ForceComplete();
  //  m_animatedViewMax.ForceComplete();
  //}


  // -----------------------------------------------------------------------------------------------------------------------------------------------

  AreaChartGridLineManager::GridLineRecord::GridLineRecord(TransitionCache& rTransitionCache, const TimeSpan transitionTime)
    : m_animatedAlpha(rTransitionCache, TimeSpan(transitionTime))
  {
  }


  void AreaChartGridLineManager::GridLineRecord::SetAlpha(const uint8_t alpha)
  {
    if (alpha != m_actualAlpha)
    {
      m_actualAlpha = alpha;
      m_animatedAlpha.SetValue(static_cast<float>(alpha));
    }
  }


  uint8_t AreaChartGridLineManager::GridLineRecord::GetAlpha() const
  {
    return m_animatedAlpha.IsCompleted() ? m_actualAlpha
                                         : static_cast<uint8_t>(MathHelper::Clamp(static_cast<int32_t>(m_animatedAlpha.GetValue()), 0, 255));
  }


  bool AreaChartGridLineManager::GridLineRecord::Update(const TimeSpan& timespan)
  {
    m_animatedAlpha.Update(timespan);
    return IsAnimating();
  }


  bool AreaChartGridLineManager::GridLineRecord::IsAnimating() const
  {
    return !m_animatedAlpha.IsCompleted();
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------

  AreaChartGridLineManager::AreaChartGridLineManager(TransitionCache& rTransitionCache, const TimeSpan transitionTime,
                                                     const TimeSpan transitionTimespanLabels, const int32_t chartEntryWidthPx,
                                                     const int32_t chartLabelSpacingPx)
    : m_chartEntryWidthPx(chartEntryWidthPx)
    , m_chartLabelSpacingPx(chartLabelSpacingPx)
    , m_gridLineRecords(RenderAreaChartConfig::MaxGridLines)
    , m_viewRecord(rTransitionCache, transitionTime)
  {
    for (std::size_t i = 0; i < m_gridLineRecords.size(); ++i)
    {
      m_gridLineRecords[i] = GridLineRecord(rTransitionCache, transitionTimespanLabels);
    }
  }

  void AreaChartGridLineManager::Reset()
  {
    m_gridLineRecordCount = 0;
  }

  bool AreaChartGridLineManager::SetGridLines(const std::shared_ptr<IChartGridLines>& gridLines)
  {
    const bool changed = gridLines.get() != m_gridLines.get();
    if (changed)
    {
      m_gridLines = gridLines;
      Reset();
    }
    return changed;
  }


  bool AreaChartGridLineManager::SetDataView(const std::shared_ptr<ChartDataView>& data)
  {
    const bool changed = data != m_dataView;
    if (changed)
    {
      m_dataView = data;
      Reset();
    }
    return changed;
  }


  void AreaChartGridLineManager::SetChartEntryWidth(const int32_t chartEntryWidthPx)
  {
    m_chartEntryWidthPx = chartEntryWidthPx;
  }

  void AreaChartGridLineManager::SetChartLabelSpacing(const int32_t chartLabelSpacingPx)
  {
    m_chartLabelSpacingPx = chartLabelSpacingPx;
  }


  void AreaChartGridLineManager::ExtractDrawData(Render::ChartDataWindowDrawData& rDst, const PxSize2D renderSizePx,
                                                 const NineSliceSprite* const pBackgroundSprite, const SpriteFont* const pFont,
                                                 const bool matchDataViewEntries)
  {
    if (m_dataView)
    {
      {    // Update the 'chart data window size' to match what we can draw
        if (matchDataViewEntries)
        {
          assert(rDst.Chart.EntryWidthPx > 0);
          const int32_t windowMaxEntries = (renderSizePx.Width() / m_chartEntryWidthPx) + ((renderSizePx.Width() % m_chartEntryWidthPx) > 0 ? 1 : 0);
          m_dataView->SetMaxViewEntries(windowMaxEntries);
        }

        // Update the min max
        const auto dataStats = m_dataView->CalculateDataStats();
        m_viewRecord.SetViewMinMax(dataStats.ValueMinMax);
      }
    }
    else
    {
      m_viewRecord.SetViewMinMax(MinMax<uint32_t>());
    }

    rDst.Clear();
    SelectGridLines(m_gridLineRecords, m_gridLineRecordCount, m_gridLines.get(), m_viewRecord);
    DetermineVisibility(rDst, SpanUtil::AsSubSpan(m_gridLineRecords, 0, m_gridLineRecordCount), m_viewRecord, renderSizePx, pBackgroundSprite, pFont,
                        m_chartEntryWidthPx, m_chartLabelSpacingPx);
    if (!m_isAnimating)
    {
      m_isAnimating = CheckIsAnimating();
    }
  }


  void AreaChartGridLineManager::Update(const TimeSpan& timespan)
  {
    if (m_isAnimating)
    {
      m_isAnimating = false;
      for (std::size_t i = 0; i < m_gridLineRecordCount; ++i)
      {
        m_isAnimating |= m_gridLineRecords[i].Update(timespan);
      }

      m_isAnimating |= m_viewRecord.Update(timespan);
    }
  }

  bool AreaChartGridLineManager::IsAnimating() const
  {
    return m_isAnimating;
  }

  void AreaChartGridLineManager::SelectGridLines(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount,
                                                 const IChartGridLines* const pGridLines, const ViewRecord& viewRecord)
  {
    ChartDataRecord chartRecord = ExtractChartData(pGridLines, viewRecord.ViewMinMax());

    const std::size_t srcSpanSize = chartRecord.Span.size();
    uint32_t dstIndex = 0;

    // Process entries that might overlap
    if (srcSpanSize > 0)
    {
      uint32_t srcIndex = 0;
      while (dstIndex < rGridLineCount && srcIndex < srcSpanSize)
      {
        const ChartGridLineInfo& src = chartRecord.Span[srcIndex];
        const auto dstPosition = rGridLines[dstIndex].RawDataPosition;
        if (dstPosition < src.Position)
        {
          rGridLines[dstIndex].Suggested = false;
          ++dstIndex;
        }
        else if (dstPosition == src.Position)
        {
          rGridLines[dstIndex].Suggested = true;
          ++srcIndex;
          ++dstIndex;
        }
        else    // dstPosition > srcPosition
        {
          // we need to insert a new grid line record
          dstIndex = InsertAt(rGridLines, rGridLineCount, dstIndex, src);
          ++srcIndex;
        }
      }
      // Insert remaining entries
      while (srcIndex < srcSpanSize)
      {
        // we need to insert a new grid line record
        dstIndex = InsertAt(rGridLines, rGridLineCount, dstIndex, chartRecord.Span[srcIndex]);
        ++srcIndex;
      }
    }
    else
    {
      // Process all 'leftover' entries not part of the new data
      while (dstIndex < rGridLineCount)
      {
        rGridLines[dstIndex].Suggested = false;
        ++dstIndex;
      }
    }

    // ReclaimEntries (which are invisible and not marked as suggested)
    {
      while (dstIndex > 0)
      {
        const uint32_t currentDstIndex = dstIndex - 1u;

        if (!rGridLines[currentDstIndex].Suggested && rGridLines[currentDstIndex].GetAlpha() == 0)
        {
          RemoveAt(rGridLines, rGridLineCount, currentDstIndex);
        }
        --dstIndex;
      }
    }
  }


  void AreaChartGridLineManager::DetermineVisibility(Render::ChartDataWindowDrawData& rDst, Span<GridLineRecord> gridLines,
                                                     const ViewRecord& viewRecord, const PxSize2D renderSizePx,
                                                     const NineSliceSprite* const pBackgroundSprite, const SpriteFont* const pFont,
                                                     const int32_t chartEntryWidthPx, const int32_t chartLabelSpacingPx)
  {
    // FIX: supply this from outside
    const int32_t backgroundXOffsetPx = chartEntryWidthPx * 4;

    const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());

    const int32_t renderHeightPx = renderSizePx.Height();
    const int32_t maxYPx = renderHeightPx - 1;    // -1 because we don't start the last pixel at height

    const MinMax<uint32_t> viewMinMax = viewRecord.ViewMinMax();
    const uint32_t viewMin = viewMinMax.Min();
    const uint32_t viewMax = viewMinMax.Max();
    const uint32_t delta = viewMax - viewMin;
    const float dataRenderScale = delta > 0 ? static_cast<float>(maxYPx) / static_cast<float>(delta) : 1.0f;

    uint32_t dstGridLineIndex = 0;
    uint32_t dstFadingGridLineIndex = 0;
    if (gridLineCount > 0)
    {    // Generate the grid line + label entries
      assert(gridLineCount <= gridLines.size());
      const int32_t fontBaseLinePx = (pFont != nullptr ? pFont->GetInfo().ScaledBaseLinePx : 0);
      const int32_t fontlineSpacingPx = (pFont != nullptr ? pFont->GetInfo().ScaledLineSpacingPx : 0);
      const RenderNineSliceInfo backgroundRenderInfo(pBackgroundSprite != nullptr ? pBackgroundSprite->GetRenderInfo() : RenderNineSliceInfo());
      const PxSize2D contentMarginSumPx = backgroundRenderInfo.ScaledContentMarginPx.Sum();
      const int32_t labelBackgroundXOffsetPx = backgroundXOffsetPx + backgroundRenderInfo.ScaledContentMarginPx.Left();

      const int32_t maxCaptionEntryHeightPx = (fontlineSpacingPx + backgroundRenderInfo.ScaledContentMarginPx.SumY()) + chartLabelSpacingPx;


      int32_t lastGridLineYPositionPx = maxYPx + fontlineSpacingPx + backgroundRenderInfo.ScaledContentMarginPx.Top();
      const auto maxDstxGridLines = UncheckedNumericCast<uint32_t>(rDst.GridLines.size());
      const auto maxFadingDstxGridLines = UncheckedNumericCast<uint32_t>(rDst.FadingGridLines.size());
      for (uint32_t i = 0; i < gridLineCount && dstGridLineIndex < maxDstxGridLines; ++i)
      {
        GridLineRecord& rSrcGridLineEntry = gridLines[i];
        const uint32_t position = rSrcGridLineEntry.RawDataPosition;
        if (position >= viewMin && position <= viewMax)
        {
          const StringViewLite labelStrView = StringViewLiteUtil::AsStringViewLite(rSrcGridLineEntry.Label);
          const int32_t gridLineYPositionPx =
            maxYPx - TypeConverter::UncheckedChangeTo<int32_t>(static_cast<float>(position - viewMin) * dataRenderScale);

          const PxSize2D labelSizePx(pFont != nullptr ? PxSize2D(pFont->MeasureString(labelStrView).Width(), pFont->GetInfo().ScaledLineSpacingPx)
                                                      : PxSize2D());
          const PxSize2D finalSizePx = PxSize2D::Max(backgroundRenderInfo.ScaledSizePx, labelSizePx + contentMarginSumPx);
          const int32_t backgroundYOffsetPx = gridLineYPositionPx - fontBaseLinePx - backgroundRenderInfo.ScaledContentMarginPx.Top();

          {    // Update grid lines
            const bool canFitsAndShouldBeShown =
              ((lastGridLineYPositionPx - gridLineYPositionPx) >= maxCaptionEntryHeightPx) && rSrcGridLineEntry.Suggested;
            if (canFitsAndShouldBeShown)
            {
              rSrcGridLineEntry.SetAlpha(255);
              lastGridLineYPositionPx = gridLineYPositionPx;

              SetEntry(rDst.GridLines[dstGridLineIndex], rSrcGridLineEntry.GetAlpha(), gridLineYPositionPx,
                       PxPoint2(backgroundXOffsetPx, backgroundYOffsetPx), labelBackgroundXOffsetPx, fontBaseLinePx, labelSizePx, finalSizePx,
                       labelStrView);
              ++dstGridLineIndex;
            }
            else
            {
              rSrcGridLineEntry.SetAlpha(0);
              if (dstFadingGridLineIndex < maxFadingDstxGridLines)
              {
                SetEntry(rDst.FadingGridLines[dstFadingGridLineIndex], rSrcGridLineEntry.GetAlpha(), gridLineYPositionPx,
                         PxPoint2(backgroundXOffsetPx, backgroundYOffsetPx), labelBackgroundXOffsetPx, fontBaseLinePx, labelSizePx, finalSizePx,
                         labelStrView);
                ++dstFadingGridLineIndex;
              }
            }
          }
        }
      }
    }
    // Fill the grid line info
    rDst.GridLineCount = dstGridLineIndex;
    rDst.FadingGridLineCount = dstFadingGridLineIndex;
    rDst.Chart.DataRenderScale = dataRenderScale;
    rDst.Chart.ViewMax = viewMax;
    rDst.Chart.EntryWidthPx = chartEntryWidthPx;
  }


  AreaChartGridLineManager::ChartDataRecord AreaChartGridLineManager::ExtractChartData(const IChartGridLines* const pGridLines,
                                                                                       const MinMax<uint32_t> minMax)
  {
    if (pGridLines == nullptr)
    {
      return {};
    }
    auto span = pGridLines->GetSuggestedGridLines(minMax);
    if (span.size() > AreaChartConfig::MaxGridLines)
    {
      // Clip to max allowed grid lines
      span = span.subspan(span.size() - AreaChartConfig::MaxGridLines, AreaChartConfig::MaxGridLines);
    }
    return {span};
  }


  uint32_t AreaChartGridLineManager::InsertAt(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount, const uint32_t insertAtIndex,
                                              const ChartGridLineInfo& srcInfo)
  {
    if ((rGridLineCount + 1) >= rGridLines.size())
    {
      const std::size_t oldSize = rGridLines.size();
      rGridLines.resize(rGridLines.size() + LocalConfig::GrowBy);
      for (std::size_t i = oldSize; i < rGridLines.size(); ++i)
      {
        // Ensure the animation structures are initialized
        rGridLines[i] = rGridLines[0];
        rGridLines[i].Clear();
      }
    }
    SANITY_CHECK(ReadOnlySpanUtil::AsSpan(rGridLines, 0, rGridLineCount));

    if (insertAtIndex >= rGridLineCount)
    {
      // Insert at end
      rGridLines[rGridLineCount].Clear();
      rGridLines[rGridLineCount].Suggested = true;
      rGridLines[rGridLineCount].RawDataPosition = srcInfo.Position;
      StringViewLiteUtil::Set(rGridLines[rGridLineCount].Label, srcInfo.Label);
      ++rGridLineCount;
      SANITY_CHECK(ReadOnlySpanUtil::AsSpan(rGridLines, 0, rGridLineCount));
      return rGridLineCount;
    }

    // Insert in the middle
    assert(insertAtIndex < rGridLineCount);
    assert(rGridLineCount > 0);
    for (std::size_t i = rGridLineCount; i > insertAtIndex; --i)
    {
      rGridLines[rGridLineCount] = rGridLines[rGridLineCount - 1];
    }
    ++rGridLineCount;

    // Insert at position
    rGridLines[insertAtIndex].Clear();
    rGridLines[insertAtIndex].Suggested = true;
    rGridLines[insertAtIndex].RawDataPosition = srcInfo.Position;
    StringViewLiteUtil::Set(rGridLines[insertAtIndex].Label, srcInfo.Label);
    SANITY_CHECK(ReadOnlySpanUtil::AsSpan(rGridLines, 0, rGridLineCount));

    return insertAtIndex + 1;
  }


  uint32_t AreaChartGridLineManager::RemoveAt(std::vector<GridLineRecord>& rGridLines, uint32_t& rGridLineCount, const uint32_t removeAtIndex)
  {
    assert(removeAtIndex < rGridLineCount);
    if (removeAtIndex >= rGridLineCount)
    {
      return rGridLineCount;
    }
    assert(rGridLineCount >= 1u);

    SANITY_CHECK(ReadOnlySpanUtil::AsSpan(rGridLines, 0, rGridLineCount));

    --rGridLineCount;
    for (std::size_t i = removeAtIndex; i < rGridLineCount; ++i)
    {
      rGridLines[i] = rGridLines[i + 1];
    }
    rGridLines[rGridLineCount].Clear();

    SANITY_CHECK(ReadOnlySpanUtil::AsSpan(rGridLines, 0, rGridLineCount));

    return removeAtIndex;
  }


  bool AreaChartGridLineManager::AreGridLinePositionsValid(const ReadOnlySpan<GridLineRecord> span)
  {
    if (!span.empty())
    {
      uint32_t previousPosition = span.front().RawDataPosition;
      for (std::size_t i = 1; i < span.size(); ++i)
      {
        if (span[i].RawDataPosition < previousPosition)
        {
          return false;
        }
        previousPosition = span[i].RawDataPosition;
      }
    }
    return true;
  }

  void AreaChartGridLineManager::SanityCheck(const ReadOnlySpan<GridLineRecord> span)
  {
    if (!AreGridLinePositionsValid(span))
    {
      throw InternalErrorException("array invalid");
    }
  }

  bool AreaChartGridLineManager::CheckIsAnimating() const
  {
    if (m_viewRecord.IsAnimating())
    {
      return true;
    }
    for (std::size_t i = 0; i < m_gridLineRecordCount; ++i)
    {
      if (m_gridLineRecords[i].IsAnimating())
      {
        return true;
      }
    }
    return false;
  }
}
