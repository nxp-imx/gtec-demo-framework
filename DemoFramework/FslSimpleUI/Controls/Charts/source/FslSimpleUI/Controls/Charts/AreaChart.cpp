/****************************************************************************************************************************************************
 * Copyright 2021-2024 NXP
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

#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderBasicImageInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderNineSliceInfo.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Controls/Charts/Grid/IChartGridLines.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <FslSimpleUI/Render/Builder/ScopedCustomUITextMeshBuilder2D.hpp>
#include <FslSimpleUI/Render/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <FslSimpleUI/Render/Builder/UIRawMeshBuilder2D.hpp>
#include <cmath>
#include "Render/ChartDataWindowDrawData.hpp"


namespace Fsl::UI
{
  using TClass = AreaChart;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyMatchDataViewEntries =
    TFactory::Create<bool, TClass, &TClass::GetMatchDataViewEntries, &TClass::SetMatchDataViewEntries>("MatchDataViewEntries");
  TDef TClass::PropertyDataView = TFactory::Create<TClass::dataview_prop_type, TClass, &TClass::GetDataView, &TClass::SetDataView>("DataView");
}


namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan LabelFadeTime(TimeSpan::FromMilliseconds(400));
      constexpr TimeSpan ViewChangeTime(TimeSpan::FromMilliseconds(600));

      constexpr int32_t ChartBarWidthDp = 1;
      constexpr int32_t ChartLabelSpacingDp = 35;

      constexpr uint32_t MaxRenderGridLines = Render::ChartDataWindowDrawDataConfig::MaxGridLines * 2;
      constexpr uint32_t MaxGridLineLabelChars = 32;

      constexpr uint32_t MaxGridLinesVertices = 4 * MaxRenderGridLines;
      constexpr uint32_t MaxGridLinesIndices = 6 * MaxRenderGridLines;
      constexpr uint32_t MaxGridLineLabelsVertices = 4 * MaxRenderGridLines * MaxGridLineLabelChars;
      constexpr uint32_t MaxGridLineLabelsIndices = 6 * MaxRenderGridLines * MaxGridLineLabelChars;
      constexpr uint32_t MaxGridLineLabelsBGVertices = (4 * 4) * MaxRenderGridLines * MaxGridLineLabelChars;
      constexpr uint32_t MaxGridLineLabelsBGIndices = (6 * 9) * MaxRenderGridLines * MaxGridLineLabelChars;
    }

    namespace LocalDefaultColors
    {
      constexpr UIColor GridLine(PackedColor32(0x80505050));
      constexpr UIColor ToolTipBackground(PackedColor32(0xB0000000));
      constexpr UIColor ToolTipLabel = UIColors::White();
    }


    void DrawCustomGridLinesNow(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                const RenderBasicImageInfo& renderInfo, const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines,
                                const UIRenderColor mainColor)
    {
      const float dstX1Pxf = dstPositionPxf.X.Value + static_cast<float>(dstSizePx.RawWidth());
      // const int32_t heightPx = dstSizePx.Height();
      // const int32_t maxYPx = heightPx - 1;    // -1 because we don't start the last pixel at height

      const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
      for (uint32_t i = 0; i < gridLineCount; ++i)
      {
        const Render::ChartDataWindowDrawData::GridLineRecord& record = gridLines[i];

        rBuilder.SetColor(UIRenderColor::Premultiply(UIRenderColor::MultiplyA(mainColor, record.Alpha)));

        const int32_t offsetYPx = record.LinePositionPx.Value;
        rBuilder.AddRect(dstPositionPxf.X.Value, dstPositionPxf.Y.Value + static_cast<float>(offsetYPx), dstX1Pxf,
                         dstPositionPxf.Y.Value + static_cast<float>(offsetYPx + 1), renderInfo.TextureArea);
      }
    }

    void DrawCustomGridLabelsBGNow(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                   const RenderNineSliceInfo& renderInfo,
                                   const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines, const UIRenderColor mainColor)
    {
      const PxAreaRectangleF clipRectanglePx(dstPositionPxf.X, dstPositionPxf.Y, PxSize1DF(dstSizePx.Width()), PxSize1DF(dstSizePx.Height()));

      // const PxVector2 dstOffsetPxf(dstPositionPxf + renderInfo.ScaledTrimMarginPxf.TopLeft());
      const float dstX0Pxf = dstPositionPxf.X.Value + (renderInfo.ScaledTrimMarginPxf.RawTop());
      const float dstY0Pxf = dstPositionPxf.Y.Value + (renderInfo.ScaledTrimMarginPxf.RawLeft());
      const float dstX1Pxf = dstPositionPxf.X.Value + (renderInfo.ScaledTrimMarginPxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop());
      const float dstY1Pxf = dstPositionPxf.Y.Value + (renderInfo.ScaledTrimMarginPxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft());

      const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
      for (uint32_t i = 0; i < gridLineCount; ++i)
      {
        const Render::ChartDataWindowDrawData::GridLineRecord& gridLineEntry = gridLines[i];
        const float renderWidthPxf =
          static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.RawWidth()) - renderInfo.ScaledTrimMarginPxf.RawBottom();
        const float renderHeightPxf =
          static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.RawHeight()) - renderInfo.ScaledTrimMarginPxf.RawRight();
        if (renderWidthPxf > 0 && renderHeightPxf > 0)
        {
          rBuilder.SetColor(UIRenderColor::Premultiply(UIRenderColor::MultiplyA(mainColor, gridLineEntry.Alpha)));

          const auto xOffsetPxf = static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.RawLeft());
          const auto yOffsetPxf = static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.RawTop());
          const float dstX2Pxf = dstPositionPxf.X.Value + (renderWidthPxf - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom());
          const float dstY2Pxf = dstPositionPxf.Y.Value + (renderHeightPxf - renderInfo.ScaledTrimmedNineSlicePxf.RawRight());
          const float dstX3Pxf = dstPositionPxf.X.Value + (renderWidthPxf);
          const float dstY3Pxf = dstPositionPxf.Y.Value + (renderHeightPxf);

          if (yOffsetPxf >= 0 && (yOffsetPxf + renderHeightPxf) <= static_cast<float>(dstSizePx.RawHeight()))
          {
            rBuilder.AddNineSlice(dstX0Pxf + xOffsetPxf, dstY0Pxf + yOffsetPxf, dstX1Pxf + xOffsetPxf, dstY1Pxf + yOffsetPxf, dstX2Pxf + xOffsetPxf,
                                  dstY2Pxf + yOffsetPxf, dstX3Pxf + xOffsetPxf, dstY3Pxf + yOffsetPxf, renderInfo.TextureArea);
          }
          else
          {
            rBuilder.AddNineSlice(dstX0Pxf + xOffsetPxf, dstY0Pxf + yOffsetPxf, dstX1Pxf + xOffsetPxf, dstY1Pxf + yOffsetPxf, dstX2Pxf + xOffsetPxf,
                                  dstY2Pxf + yOffsetPxf, dstX3Pxf + xOffsetPxf, dstY3Pxf + yOffsetPxf, renderInfo.TextureArea, clipRectanglePx);
          }
        }
      }
    }

    void DrawCustomGridLabelsNow(ScopedCustomUITextMeshBuilder2D& rTextBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                 const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines, const UIRenderColor mainColor)
    {
      const PxAreaRectangleF clipRectanglePx(dstPositionPxf.X, dstPositionPxf.Y, PxSize1DF(dstSizePx.Width()), PxSize1DF(dstSizePx.Height()));

      const PxValueF dstX0Pxf = dstPositionPxf.X;
      const PxValueF dstY0Pxf = dstPositionPxf.Y;

      const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
      for (uint32_t i = 0; i < gridLineCount; ++i)
      {
        const Render::ChartDataWindowDrawData::GridLineRecord& record = gridLines[i];
        const StringViewLite strView(record.Label);
        const PxValue xPositionPx = record.LabelOffsetPx.X;
        const PxValue yPositionPx = record.LabelOffsetPx.Y;

        rTextBuilder.SetColor(UIRenderColor::Premultiply(UIRenderColor::MultiplyA(mainColor, record.Alpha)));

        if (yPositionPx.Value >= 0 && (yPositionPx + record.LabelSizePx.Height()) <= dstSizePx.Height())
        {
          rTextBuilder.AddString(PxVector2(dstX0Pxf + PxValueF(xPositionPx), dstY0Pxf + PxValueF(yPositionPx)), strView);
        }
        else
        {
          rTextBuilder.AddString(PxVector2(dstX0Pxf + PxValueF(xPositionPx), dstY0Pxf + PxValueF(yPositionPx)), strView, clipRectanglePx);
        }
      }
    }

    void DrawGraphSegmentNow(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxValue dstXPosCurrent, const PxValue maxYPx,
                             const uint32_t channelCount, const ChartDataEntry& entry, const float dataRenderScalePxf, const PxSize1D entryPixelWidth,
                             std::array<UIRenderColor, UI::Render::ChartDataWindowDrawDataConfig::MaxStackedEntries> premultipliedColors,
                             const NativeTextureArea& textureArea)
    {
      PxValue lastPx = maxYPx + PxValue(1);
      float topPxf = 0;
      for (uint32_t entryIndex = 0; entryIndex < channelCount; ++entryIndex)
      {
        const float renderData = static_cast<float>(entry.Values[entryIndex]) * dataRenderScalePxf;
        topPxf += renderData;
        const auto scaledTopDataSizePx = PxValue(static_cast<int32_t>(MathHelper::Clamp(topPxf, 0.0f, static_cast<float>(0x10000000))));
        const PxValue yModPx = maxYPx - scaledTopDataSizePx;
        assert(lastPx >= yModPx);

        const PxValue drawYModPx0 = MathHelper::Clamp(yModPx, PxValue(0), maxYPx);
        const PxValue drawYModPx1 = MathHelper::Clamp(lastPx, PxValue(0), maxYPx);
        const PxValue clippedEntryHeightPx = drawYModPx1 - drawYModPx0;
        if (clippedEntryHeightPx.Value > 0)
        {
          rBuilder.SetColor(premultipliedColors[entryIndex]);
          rBuilder.AddRect(dstPositionPxf.X.Value + static_cast<float>(dstXPosCurrent.Value),
                           dstPositionPxf.Y.Value + static_cast<float>(drawYModPx0.Value),
                           dstPositionPxf.X.Value + static_cast<float>(dstXPosCurrent.Value + entryPixelWidth.RawValue()),
                           dstPositionPxf.Y.Value + static_cast<float>(drawYModPx1.Value), textureArea);
        }
        lastPx = yModPx;
      }
    }

    void DrawCustomGraph(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                         const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
      if (pChartWindow != nullptr && pChartWindow->DataView)
      {
        const ChartDataView* const pData = pChartWindow->DataView.get();
        const ChartDataInfo dataInfo = pData->DataInfo();

        // const float dstY1Pxf = dstPositionPxf.Y + float(dstSizePx.Height());

        const PxSize1D entryPixelWidth = pChartWindow->Chart.EntryWidthPx;
        const PxValue maxYPx = PxValue(dstSizePx.RawHeight() - 1);    // -1 because we don't start the last pixel at height
        const auto totalElementCount = PxSize1D::UncheckedCreate(UncheckedNumericCast<int32_t>(dataInfo.TotalElementCount));
        const PxSize1D maxPixels = std::min(totalElementCount * entryPixelWidth, dstSizePx.Width());
        const int32_t entriesToDraw = maxPixels.RawValue() / entryPixelWidth.RawValue();
        const PxSize1D leftoverPixels = maxPixels % entryPixelWidth;

        assert(pChartWindow->ChartCache.Valid);
        const Render::ChartDataWindowDrawData::ChartRecord& chart = pChartWindow->Chart;
        // const uint32_t chartViewMax = chart.ViewMax;

        PxValue dstXPos = dstSizePx.Width().Value();
        auto entriesLeft = UncheckedNumericCast<uint32_t>(entriesToDraw);
        std::size_t lastSegmentOffset = 0;
        uint32_t segmentIndex = dataInfo.SegmentCount;
        for (; segmentIndex > 0 && entriesLeft > 0; --segmentIndex)
        {
          ReadOnlySpan<ChartDataEntry> dataSpan = pData->SegmentDataAsReadOnlySpan(segmentIndex - 1);
          {
            auto spanAreaToDrawEntries = std::min(UncheckedNumericCast<std::size_t>(entriesLeft), dataSpan.size());
            lastSegmentOffset = dataSpan.size() - spanAreaToDrawEntries;
            dataSpan = dataSpan.subspan(lastSegmentOffset, spanAreaToDrawEntries);
          }
          const auto dataSpanEntries = UncheckedNumericCast<uint32_t>(dataSpan.size());
          dstXPos -= PxValue(UncheckedNumericCast<int32_t>(dataSpanEntries)) * entryPixelWidth;
          PxValue dstXPosCurrent = dstXPos + (PxValue(UncheckedNumericCast<int32_t>(dataSpanEntries) - 1) * entryPixelWidth);
          for (uint32_t dataSpanIndex = dataSpanEntries; dataSpanIndex > 0; --dataSpanIndex)
          {
            const auto i = dataSpanIndex - 1;
            DrawGraphSegmentNow(rBuilder, dstPositionPxf, dstXPosCurrent, maxYPx, dataInfo.ChannelCount, dataSpan[i], chart.DataRenderScale,
                                entryPixelWidth, pChartWindow->ChartCache.Premultiplied, renderInfo.TextureArea);
            dstXPosCurrent -= entryPixelWidth.Value();
          }
          entriesLeft -= dataSpanEntries;
        }
        if (entriesLeft == 0 && leftoverPixels.RawValue() > 0)
        {
          if (lastSegmentOffset <= 0)
          {
            if (segmentIndex > 0)
            {
              --segmentIndex;
              lastSegmentOffset = 0;
            }
          }
          else
          {
            --lastSegmentOffset;
          }
          ReadOnlySpan<ChartDataEntry> dataSpan = pData->SegmentDataAsReadOnlySpan(segmentIndex).subspan(lastSegmentOffset, 1);
          if (!dataSpan.empty())
          {
            DrawGraphSegmentNow(rBuilder, dstPositionPxf, PxValue(0), maxYPx, dataInfo.ChannelCount, dataSpan[0], chart.DataRenderScale,
                                leftoverPixels, pChartWindow->ChartCache.Premultiplied, renderInfo.TextureArea);
          }
        }
      }
    }

    void DrawCustomGridLines(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                             const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
      if (pChartWindow != nullptr)
      {
        const UIRenderColor mainColor(rBuilder.GetColor());

        // Draw entries that are fading out to ensure they are behind everything
        DrawCustomGridLinesNow(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                               SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount), mainColor);

        // Draw all fading-in or fully visible entries
        DrawCustomGridLinesNow(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                               SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount), mainColor);
      }
    }


    void DrawCustomGridLabelsBG(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                const RenderNineSliceInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
      if (pChartWindow != nullptr)
      {
        const UIRenderColor mainColor(rBuilder.GetColor());

        // Draw entries that are fading out to ensure they are behind everything
        DrawCustomGridLabelsBGNow(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                                  SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount), mainColor);

        // Draw all fading-in or fully visible entries
        DrawCustomGridLabelsBGNow(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                                  SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount), mainColor);
      }
    }

    void DrawCustomGridLabels(ScopedCustomUITextMeshBuilder2D& rTextBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                              const ICustomDrawData* const pCustomDrawData)
    {
      const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
      if (pChartWindow != nullptr)
      {
        const UIRenderColor mainColor(rTextBuilder.GetColor());

        // Draw entries that are fading out to ensure they are behind everything
        DrawCustomGridLabelsNow(rTextBuilder, dstPositionPxf, dstSizePx,
                                SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount), mainColor);

        // Draw all fading-in or fully visible entries
        DrawCustomGridLabelsNow(rTextBuilder, dstPositionPxf, dstSizePx,
                                SpanUtil::UncheckedAsReadOnlySpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount), mainColor);
      }
    }
  }

  AreaChart::AreaChart(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_gridLineManager(LocalConfig::ViewChangeTime, LocalConfig::LabelFadeTime, context->UnitConverter.DpToPxSize1D(LocalConfig::ChartBarWidthDp),
                        context->UnitConverter.DpToPxSize1D(LocalConfig::ChartLabelSpacingDp))
    , m_chartWindowDrawData(std::make_shared<Render::ChartDataWindowDrawData>(context->UnitConverter))
    , m_graphMesh(context->TheUIContext.Get()->MeshManager, 6)
    , m_gridLinesMesh(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLinesVertices, LocalConfig::MaxGridLinesIndices)
    , m_gridLabelsBackground(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLineLabelsBGVertices,
                             LocalConfig::MaxGridLineLabelsBGIndices)
    , m_gridLabelsMesh(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLineLabelsVertices, LocalConfig::MaxGridLineLabelsIndices)
    , m_lineColor(context->ColorConverter, LocalDefaultColors::GridLine)
    , m_backgroundColor(context->ColorConverter, LocalDefaultColors::ToolTipBackground)
    , m_labelColor(context->ColorConverter, LocalDefaultColors::ToolTipLabel)
    , m_renderPolicy(ChartRenderPolicy::Measure)
    , m_propertyMatchDataViewEntries(true)
  {
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::PostLayoutEnabled));

    {
      auto& rColorArray = m_chartWindowDrawData->ChartColors;
      static_assert(4 <= std::tuple_size<std::remove_reference_t<decltype(rColorArray)>>(), "assumption failed");
      rColorArray[0] = context->ColorConverter.Convert(UIColors::Red());
      rColorArray[1] = context->ColorConverter.Convert(UIColors::Green());
      rColorArray[2] = context->ColorConverter.Convert(UIColors::Blue());
      rColorArray[3] = context->ColorConverter.Convert(UIColors::Cyan());
    }
  }


  AreaChart::~AreaChart() = default;


  ChartRenderPolicy AreaChart::GetRenderPolicy() const
  {
    return m_renderPolicy;
  }

  void AreaChart::SetRenderPolicy(const ChartRenderPolicy value)
  {
    if (value != m_renderPolicy)
    {
      m_renderPolicy = value;
      PropertyUpdated(PropertyType::Layout);
    }
  }

  void AreaChart::SetOpaqueFillSprite(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_graphMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  void AreaChart::SetTransparentFillSprite(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_gridLinesMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  const std::shared_ptr<NineSliceSprite>& AreaChart::GetLabelBackground() const
  {
    return m_gridLabelsBackground.GetSprite();
  }


  void AreaChart::SetLabelBackground(const std::shared_ptr<NineSliceSprite>& value)
  {
    if (m_gridLabelsBackground.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  const std::shared_ptr<SpriteFont>& AreaChart::GetFont() const
  {
    return m_gridLabelsMesh.GetSprite();
  }


  void AreaChart::SetFont(const std::shared_ptr<SpriteFont>& value)
  {
    if (m_gridLabelsMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  void AreaChart::SetGridLines(const std::shared_ptr<IChartGridLines>& gridLines)
  {
    if (m_gridLineManager.SetGridLines(gridLines))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  bool AreaChart::GetMatchDataViewEntries() const
  {
    return m_propertyMatchDataViewEntries.Get();
  }

  bool AreaChart::SetMatchDataViewEntries(const bool enabled)
  {
    const bool changed = m_propertyMatchDataViewEntries.Set(ThisDependencyObject(), enabled);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }

  const std::shared_ptr<ChartDataView>& AreaChart::GetDataView() const
  {
    return m_propertyDataView.Get();
  }

  bool AreaChart::SetDataView(const std::shared_ptr<ChartDataView>& dataView)
  {
    const bool changed = m_propertyDataView.Set(this, this, ThisDependencyObject(), PropertyDataView, dataView);
    if (changed)
    {
      m_gridLineManager.SetDataView(dataView);
      m_chartWindowDrawData->DataView = dataView;
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }

  bool AreaChart::SetDataView(const std::shared_ptr<AChartData>& data)
  {
    return SetDataView(std::make_shared<ChartDataView>(data));
  }

  void AreaChart::SetLineColor(const UIColor color)
  {
    if (m_lineColor.Set(GetContext()->ColorConverter, color))
    {
      PropertyUpdated(PropertyType::Other);
    }
  }


  void AreaChart::SetBackgroundColor(const UIColor color)
  {
    if (m_backgroundColor.Set(GetContext()->ColorConverter, color))
    {
      PropertyUpdated(PropertyType::Other);
    }
  }


  void AreaChart::SetLabelColor(const UIColor color)
  {
    if (m_labelColor.Set(GetContext()->ColorConverter, color))
    {
      PropertyUpdated(PropertyType::Other);
    }
  }


  // void AreaChart::SetEntryColor(const uint32_t index, const Color color)
  //{
  //   if (m_chartWindowDrawData->SetEntryColor(index, color))
  //   {
  //     PropertyUpdated(PropertyType::Other);
  //   }
  // }


  void AreaChart::WinResolutionChanged(const ResolutionChangedInfo& info)
  {
    FSL_PARAM_NOT_USED(info);

    auto context = GetContext();

    m_chartWindowDrawData->Canvas.OnResolutionChanged(context->UnitConverter);

    m_gridLineManager.SetChartEntryWidth(context->UnitConverter.DpToPxSize1D(LocalConfig::ChartBarWidthDp));
    m_gridLineManager.SetChartLabelSpacing(context->UnitConverter.DpToPxSize1D(LocalConfig::ChartLabelSpacingDp));
  }


  void AreaChart::WinPostLayout()
  {
    PxSize2D renderSizePx = RenderSizePx();
    // const uint32_t dataIOnfo = 1;
    const int32_t maxEntries = renderSizePx.RawWidth() / m_gridLineManager.GetChartEntryWidth().RawValue() +
                               ((renderSizePx.RawWidth() % m_gridLineManager.GetChartEntryWidth().RawValue()) > 0 ? 1 : 0);

    auto dataView = m_gridLineManager.GetDataView();
    const uint32_t maxStackedEntries = dataView ? dataView->DataInfo().ChannelCount : 1;

    const auto maxPixelsX = UncheckedNumericCast<uint32_t>(maxEntries);
    const uint32_t vertexCapacity = maxStackedEntries * maxPixelsX * 6u;
    m_graphMesh.EnsureCapacity(vertexCapacity);
  }

  void AreaChart::WinDraw(const UIDrawContext& context)
  {
    assert(m_chartWindowDrawData);

    m_gridLineManager.ExtractDrawData(*m_chartWindowDrawData, RenderSizePx(), GetLabelBackground().get(), GetFont().get(),
                                      m_propertyMatchDataViewEntries.Get());

    const UIRenderColor finalBaseColor(GetFinalBaseColor());

    if (m_graphMesh.IsValid())
    {
      m_chartWindowDrawData->FillChartCache(finalBaseColor);
      context.CommandBuffer.DrawCustom(m_graphMesh.Get(), context.TargetRect.Location(), RenderSizePx(), finalBaseColor, DrawCustomGraph,
                                       m_chartWindowDrawData);
    }
    if (m_gridLinesMesh.IsValid())
    {
      context.CommandBuffer.DrawCustom(m_gridLinesMesh.Get(), context.TargetRect.Location(), RenderSizePx(),
                                       finalBaseColor * m_lineColor.GetInternalColor(), DrawCustomGridLines, m_chartWindowDrawData);
    }
    if (m_gridLabelsBackground.IsValid())
    {
      context.CommandBuffer.DrawCustom(m_gridLabelsBackground.Get(), context.TargetRect.Location(), RenderSizePx(),
                                       finalBaseColor * m_backgroundColor.GetInternalColor(), DrawCustomGridLabelsBG, m_chartWindowDrawData);
    }
    if (m_gridLabelsMesh.IsValid())
    {
      context.CommandBuffer.DrawCustom(m_gridLabelsMesh.Get(), context.TargetRect.Location(), RenderSizePx(),
                                       finalBaseColor * m_labelColor.GetInternalColor(), DrawCustomGridLabels, m_chartWindowDrawData);
    }

    if (m_gridLineManager.IsAnimating())
    {
      CheckAnimationState();
    }
  }


  void AreaChart::OnChanged(const DataBinding::DataBindingInstanceHandle hInstance)
  {
    if (m_propertyDataView.IsChangeFromSource(hInstance))
    {
      const bool requireLayout = ProcessDataViewChange();
      PropertyUpdated(requireLayout ? PropertyType::Layout : PropertyType::Other);
    }
  }


  DataBinding::DataBindingInstanceHandle AreaChart::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                            PropLinkRefs(PropertyMatchDataViewEntries, m_propertyMatchDataViewEntries),
                                                            PropLinkRefs(PropertyDataView, m_propertyDataView));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult AreaChart::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                    const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                     PropLinkRefs(PropertyMatchDataViewEntries, m_propertyMatchDataViewEntries),
                                                     PropLinkRefs(PropertyDataView, m_propertyDataView));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  PxSize2D AreaChart::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    PxSize2D resultPx = BaseWindow::ArrangeOverride(finalSizePx);
    const auto* pDataViewElement = m_propertyDataView.TryGetElement();
    if (pDataViewElement != nullptr)
    {
      Render::ChartDataWindowDrawData& rDrawData = *m_chartWindowDrawData;
      const auto numEntriesInPx = PxSize1D::Create(UncheckedNumericCast<int32_t>(pDataViewElement->Count()), OptimizationCheckFlag::NoCheck);
      const auto dimensionInPx = numEntriesInPx * rDrawData.Canvas.SizeOfOneDpInPixels();
      resultPx = rDrawData.Canvas.Arrange(finalSizePx, dimensionInPx);
    }
    return resultPx;
  }


  PxSize2D AreaChart::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    assert(m_chartWindowDrawData);

    PxSize2D desiredSizePx;
    const auto* pDataViewElement = m_propertyDataView.TryGetElement();
    if (pDataViewElement != nullptr)
    {
      Render::ChartDataWindowDrawData& rDrawData = *m_chartWindowDrawData;
      const auto numEntriesInPx = PxSize1D::Create(UncheckedNumericCast<int32_t>(pDataViewElement->Count()), OptimizationCheckFlag::NoCheck);
      const auto dimensionInPx = numEntriesInPx * rDrawData.Canvas.SizeOfOneDpInPixels();
      desiredSizePx = rDrawData.Canvas.Measure(availableSizePx, dimensionInPx);
    }

    if (pDataViewElement == nullptr || m_renderPolicy == ChartRenderPolicy::FillAvailable)
    {
      desiredSizePx = BaseWindow::MeasureOverride(availableSizePx);
    }
    return desiredSizePx;
  }


  bool AreaChart::ProcessDataViewChange()
  {
    bool requireRelayout = false;
    assert(m_chartWindowDrawData);
    Render::ChartDataWindowDrawData& rDrawData = *m_chartWindowDrawData;

    const auto* pDataViewElement = m_propertyDataView.TryGetElement();

    if (pDataViewElement != nullptr)
    {
      const auto colorConverter = GetContext()->ColorConverter;

      {    // Extract colors
        const auto channelCount = pDataViewElement->ChannelCount();
        for (uint32_t i = 0; i < channelCount; ++i)
        {
          auto metaData = pDataViewElement->GetChannelMetaDataInfo(i);
          rDrawData.SetEntryColor(i, colorConverter.Convert(metaData.PrimaryColor));
        }
      }

      {    // Extract axis range
        auto dataStats = pDataViewElement->CalculateDataStats();
        requireRelayout = rDrawData.Canvas.SetAxisRange(dataStats.ValueMinMax);
      }

      const uint32_t currentCount = pDataViewElement->Count();
      const uint32_t channelCount = pDataViewElement->ChannelCount();
      if (m_dataViewCache.Count != currentCount || m_dataViewCache.ChannelCount != channelCount)
      {
        m_dataViewCache.Count = currentCount;
        m_dataViewCache.ChannelCount = channelCount;
        requireRelayout = true;
      }
    }
    // requireRelayout = true;
    return requireRelayout;
  }

  void AreaChart::UpdateAnimation(const TimeSpan& timeSpan)
  {
    BaseWindow::UpdateAnimation(timeSpan);
    m_gridLineManager.Update(timeSpan);
  }


  bool AreaChart::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    const bool isAnimating = BaseWindow::UpdateAnimationState(forceCompleteAnimation);
    return isAnimating || m_gridLineManager.IsAnimating();
  }
}
