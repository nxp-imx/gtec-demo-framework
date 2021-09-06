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

#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderBasicImageInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderNineSliceInfo.hpp>
#include <FslGraphics2D/Procedural/Builder/ScopedCustomUITextMeshBuilder2D.hpp>
#include <FslGraphics2D/Procedural/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <FslGraphics2D/Procedural/Builder/UIRawMeshBuilder2D.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Controls/Charts/Data/IChartDataGridLines.hpp>
#include <FslSimpleUI/Controls/Charts/Data/IChartComplexDataWindow.hpp>
#include <cmath>
#include "Render/ChartDataWindowDrawData.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      namespace LocalConfig
      {
        // In microseconds
        constexpr int64_t ViewChangeTime = 600;
        constexpr int64_t LabelFadeTime = 400;

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
        constexpr Color GridLine(0x80505050);
        constexpr Color ToolTipBackground = Color(0xB0000000);
        constexpr Color ToolTipLabel = Color::White();
      }


      void DrawCustomGridLines(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                               const RenderBasicImageInfo& renderInfo, const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines,
                               const Color mainColor)
      {
        const float dstX1Pxf = dstPositionPxf.X + float(dstSizePx.Width());
        // const int32_t heightPx = dstSizePx.Height();
        // const int32_t maxYPx = heightPx - 1;    // -1 because we don't start the last pixel at height

        const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
        for (uint32_t i = 0; i < gridLineCount; ++i)
        {
          const Render::ChartDataWindowDrawData::GridLineRecord& record = gridLines[i];

          rBuilder.SetColor(Color::Premultiply(Color::ApplyAlpha(mainColor, record.Alpha)));

          const int32_t offsetYPx = record.LinePositionPx;
          rBuilder.AddRect(dstPositionPxf.X, dstPositionPxf.Y + float(offsetYPx), dstX1Pxf, dstPositionPxf.Y + float(offsetYPx + 1),
                           renderInfo.TextureArea);
        }
      }

      void DrawCustomGridLabelsBG(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                  const RenderNineSliceInfo& renderInfo,
                                  const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines, const Color mainColor)
      {
        const PxAreaRectangleF clipRectanglePx(dstPositionPxf.X, dstPositionPxf.Y, float(dstSizePx.Width()), float(dstSizePx.Height()));

        // const PxVector2 dstOffsetPxf(dstPositionPxf + renderInfo.ScaledTrimMarginPxf.TopLeft());
        const float dstX0Pxf = dstPositionPxf.X + (renderInfo.ScaledTrimMarginPxf.Top());
        const float dstY0Pxf = dstPositionPxf.Y + (renderInfo.ScaledTrimMarginPxf.Left());
        const float dstX1Pxf = dstPositionPxf.X + (renderInfo.ScaledTrimMarginPxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Top());
        const float dstY1Pxf = dstPositionPxf.Y + (renderInfo.ScaledTrimMarginPxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Left());

        const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
        for (uint32_t i = 0; i < gridLineCount; ++i)
        {
          const Render::ChartDataWindowDrawData::GridLineRecord& gridLineEntry = gridLines[i];
          const float renderWidthPxf = static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.Width()) - renderInfo.ScaledTrimMarginPxf.Bottom();
          const float renderHeightPxf =
            static_cast<float>(gridLineEntry.LabelBackgroundRectanglePx.Height()) - renderInfo.ScaledTrimMarginPxf.Right();
          if (renderWidthPxf > 0 && renderHeightPxf > 0)
          {
            rBuilder.SetColor(Color::Premultiply(Color::ApplyAlpha(mainColor, gridLineEntry.Alpha)));

            const auto xOffsetPxf = float(gridLineEntry.LabelBackgroundRectanglePx.Left());
            const auto yOffsetPxf = float(gridLineEntry.LabelBackgroundRectanglePx.Top());
            const float dstX2Pxf = dstPositionPxf.X + (renderWidthPxf - renderInfo.ScaledTrimmedNineSlicePxf.Bottom());
            const float dstY2Pxf = dstPositionPxf.Y + (renderHeightPxf - renderInfo.ScaledTrimmedNineSlicePxf.Right());
            const float dstX3Pxf = dstPositionPxf.X + (renderWidthPxf);
            const float dstY3Pxf = dstPositionPxf.Y + (renderHeightPxf);

            if (yOffsetPxf >= 0 && (yOffsetPxf + renderHeightPxf) <= dstSizePx.Height())
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

      void DrawCustomGridLabels(ScopedCustomUITextMeshBuilder2D& rTextBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                const ReadOnlySpan<Render::ChartDataWindowDrawData::GridLineRecord> gridLines, const Color mainColor)
      {
        const PxAreaRectangleF clipRectanglePx(dstPositionPxf.X, dstPositionPxf.Y, float(dstSizePx.Width()), float(dstSizePx.Height()));

        const float dstX0Pxf = dstPositionPxf.X;
        const float dstY0Pxf = dstPositionPxf.Y;

        const auto gridLineCount = UncheckedNumericCast<uint32_t>(gridLines.size());
        for (uint32_t i = 0; i < gridLineCount; ++i)
        {
          const Render::ChartDataWindowDrawData::GridLineRecord& record = gridLines[i];
          const auto strView = StringViewLiteUtil::AsStringViewLite(record.Label);
          const int32_t xPositionPx = record.LabelOffsetPx.X;
          const int32_t yPositionPx = record.LabelOffsetPx.Y;

          rTextBuilder.SetColor(Color::Premultiply(Color::ApplyAlpha(mainColor, record.Alpha)));

          if (yPositionPx >= 0 && (yPositionPx + record.LabelSizePx.Height()) <= dstSizePx.Height())
          {
            rTextBuilder.AddString(PxVector2(dstX0Pxf + float(xPositionPx), dstY0Pxf + float(yPositionPx)), strView);
          }
          else
          {
            rTextBuilder.AddString(PxVector2(dstX0Pxf + float(xPositionPx), dstY0Pxf + float(yPositionPx)), strView, clipRectanglePx);
          }
        }
      }

      void DrawCustomGraph(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                           const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
      {
        const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
        if (pChartWindow != nullptr && pChartWindow->Data)
        {
          const IChartComplexDataWindow* const pData = pChartWindow->Data.get();
          const ChartComplexDataInfo dataInfo = pData->DataInfo();

          // const float dstY1Pxf = dstPositionPxf.Y + float(dstSizePx.Height());

          const int32_t entryPixelWidth = pChartWindow->Chart.EntryWidthPx;

          const int32_t maxYPx = dstSizePx.Height() - 1;    // -1 because we don't start the last pixel at height
          const auto totalDataEntries = UncheckedNumericCast<int32_t>(dataInfo.Count);
          const int32_t maxPixels = std::min(totalDataEntries * entryPixelWidth, dstSizePx.Width());
          const int32_t entriesToDraw = maxPixels / entryPixelWidth;
          const int32_t leftoverPixels = maxPixels % entryPixelWidth;

          assert(pChartWindow->ChartCache.Valid);
          const Render::ChartDataWindowDrawData::ChartRecord& chart = pChartWindow->Chart;
          // const uint32_t chartViewMax = chart.ViewMax;

          int32_t dstXPos = dstSizePx.Width();
          auto entriesLeft = UncheckedNumericCast<uint32_t>(entriesToDraw);
          std::size_t lastSegmentOffset = 0;
          uint32_t segmentIndex = dataInfo.SegmentCount;
          for (; segmentIndex > 0 && entriesLeft > 0; --segmentIndex)
          {
            ReadOnlySpan<ChartComplexDataEntry> dataSpan = pData->DataAsReadOnlySpan(segmentIndex - 1);
            {
              auto spanAreaToDrawEntries = std::min(UncheckedNumericCast<std::size_t>(entriesLeft), dataSpan.size());
              lastSegmentOffset = dataSpan.size() - spanAreaToDrawEntries;
              dataSpan = dataSpan.subspan(lastSegmentOffset, spanAreaToDrawEntries);
            }
            const auto dataSpanEntries = UncheckedNumericCast<uint32_t>(dataSpan.size());
            dstXPos -= dataSpanEntries * entryPixelWidth;
            int32_t dstXPosCurrent = dstXPos + ((dataSpanEntries - 1) * entryPixelWidth);
            for (uint32_t dataSpanIndex = dataSpanEntries; dataSpanIndex > 0; --dataSpanIndex)
            {
              const auto i = dataSpanIndex - 1;
              int32_t lastPx = maxYPx + 1;
              float topPxf = 0;
              for (uint32_t entryIndex = 0; entryIndex < dataInfo.ComplexDataEntries; ++entryIndex)
              {
                const float renderData = dataSpan[i].Values[entryIndex] * chart.DataRenderScale;
                topPxf += renderData;
                const auto scaledTopDataSizePx = int32_t(MathHelper::Clamp(topPxf, 0.0f, float(0x10000000)));
                const int32_t yModPx = maxYPx - scaledTopDataSizePx;
                assert(lastPx >= yModPx);

                const int32_t drawYModPx0 = MathHelper::Clamp(yModPx, 0, maxYPx);
                const int32_t drawYModPx1 = MathHelper::Clamp(lastPx, 0, maxYPx);
                const int32_t clippedEntryHeightPx = drawYModPx1 - drawYModPx0;
                if (clippedEntryHeightPx > 0)
                {
                  rBuilder.SetColor(pChartWindow->ChartCache.Premultiplied[entryIndex]);
                  rBuilder.AddRect(dstPositionPxf.X + float(dstXPosCurrent), dstPositionPxf.Y + float(drawYModPx0),
                                   dstPositionPxf.X + float(dstXPosCurrent + entryPixelWidth), dstPositionPxf.Y + float(drawYModPx1),
                                   renderInfo.TextureArea);
                }
                lastPx = yModPx;
              }
              dstXPosCurrent -= entryPixelWidth;
            }
            entriesLeft -= dataSpanEntries;
          }
          if (entriesLeft == 0 && leftoverPixels > 0)
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
            ReadOnlySpan<ChartComplexDataEntry> dataSpan = pData->DataAsReadOnlySpan(segmentIndex).subspan(lastSegmentOffset, 1);
            if (!dataSpan.empty())
            {
              int32_t lastPx = maxYPx + 1;
              float topPxf = 0;
              for (uint32_t entryIndex = 0; entryIndex < dataInfo.ComplexDataEntries; ++entryIndex)
              {
                const float renderData = dataSpan[0].Values[entryIndex] * chart.DataRenderScale;
                topPxf += renderData;
                const int32_t scaledTopDataSizePx = TypeConverter::UncheckedChangeTo<int32_t>(topPxf);
                const int32_t yModPx = maxYPx - scaledTopDataSizePx;
                assert(lastPx >= yModPx);
                const int32_t entryHeightPx = lastPx - yModPx;
                if (entryHeightPx > 0)
                {
                  rBuilder.SetColor(pChartWindow->ChartCache.Premultiplied[entryIndex]);
                  rBuilder.AddRect(dstPositionPxf.X, dstPositionPxf.Y + float(yModPx), dstPositionPxf.X + float(entryPixelWidth),
                                   dstPositionPxf.Y + float(lastPx), renderInfo.TextureArea);
                }
                lastPx = yModPx;
              }
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
          const Color mainColor = rBuilder.GetColor();

          // Draw entries that are fading out to ensure they are behind everything
          DrawCustomGridLines(
            rBuilder, dstPositionPxf, dstSizePx, renderInfo,
            ReadOnlySpanUtil::AsSpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount, OptimizationCheckFlag::NoCheck), mainColor);

          // Draw all fading-in or fully visible entries
          DrawCustomGridLines(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                              ReadOnlySpanUtil::AsSpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount, OptimizationCheckFlag::NoCheck),
                              mainColor);
        }
      }


      void DrawCustomGridLabelsBG(UIRawMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                  const RenderNineSliceInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
      {
        const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
        if (pChartWindow != nullptr)
        {
          const Color mainColor = rBuilder.GetColor();

          // Draw entries that are fading out to ensure they are behind everything
          DrawCustomGridLabelsBG(
            rBuilder, dstPositionPxf, dstSizePx, renderInfo,
            ReadOnlySpanUtil::AsSpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount, OptimizationCheckFlag::NoCheck), mainColor);

          // Draw all fading-in or fully visible entries
          DrawCustomGridLabelsBG(rBuilder, dstPositionPxf, dstSizePx, renderInfo,
                                 ReadOnlySpanUtil::AsSpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount, OptimizationCheckFlag::NoCheck),
                                 mainColor);
        }
      }

      void DrawCustomGridLabels(ScopedCustomUITextMeshBuilder2D& rTextBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                                const ICustomDrawData* const pCustomDrawData)
      {
        const auto* pChartWindow = dynamic_cast<const Render::ChartDataWindowDrawData*>(pCustomDrawData);
        if (pChartWindow != nullptr)
        {
          const Color mainColor = rTextBuilder.GetColor();

          // Draw entries that are fading out to ensure they are behind everything
          DrawCustomGridLabels(
            rTextBuilder, dstPositionPxf, dstSizePx,
            ReadOnlySpanUtil::AsSpan(pChartWindow->FadingGridLines, 0, pChartWindow->FadingGridLineCount, OptimizationCheckFlag::NoCheck), mainColor);

          // Draw all fading-in or fully visible entries
          DrawCustomGridLabels(rTextBuilder, dstPositionPxf, dstSizePx,
                               ReadOnlySpanUtil::AsSpan(pChartWindow->GridLines, 0, pChartWindow->GridLineCount, OptimizationCheckFlag::NoCheck),
                               mainColor);
        }
      }
    }

    AreaChart::AreaChart(const std::shared_ptr<BaseWindowContext>& context)
      : BaseWindow(context)
      , m_gridLineManager(context->UITransitionCache, LocalConfig::ViewChangeTime, LocalConfig::LabelFadeTime,
                          context->UnitConverter.DpToPxInt32(LocalConfig::ChartBarWidthDp),
                          context->UnitConverter.DpToPxInt32(LocalConfig::ChartLabelSpacingDp))
      , m_chartWindowDrawData(std::make_shared<Render::ChartDataWindowDrawData>())
      , m_graphMesh(context->TheUIContext.Get()->MeshManager, 6)
      , m_gridLinesMesh(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLinesVertices, LocalConfig::MaxGridLinesIndices)
      , m_gridLabelsBackground(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLineLabelsBGVertices,
                               LocalConfig::MaxGridLineLabelsBGIndices)
      , m_gridLabelsMesh(context->TheUIContext.Get()->MeshManager, LocalConfig::MaxGridLineLabelsVertices, LocalConfig::MaxGridLineLabelsIndices)
      , m_lineColor(LocalDefaultColors::GridLine)
      , m_backgroundColor(LocalDefaultColors::ToolTipBackground)
      , m_labelColor(LocalDefaultColors::ToolTipLabel)
    {
      Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::PostLayoutEnabled));

      {
        auto& rColorArray = m_chartWindowDrawData->ChartColors;
        static_assert(4 <= std::tuple_size<std::remove_reference_t<decltype(rColorArray)>>(), "assumption failed");
        rColorArray[0] = Color::Red();
        rColorArray[1] = Color::Green();
        rColorArray[2] = Color::Blue();
        rColorArray[3] = Color::Cyan();
      }
    }


    AreaChart::~AreaChart() = default;


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


    const std::shared_ptr<IChartComplexDataWindow>& AreaChart::GetData() const
    {
      return m_gridLineManager.GetData();
    }

    void AreaChart::SetData(const std::shared_ptr<IChartComplexDataWindow>& data)
    {
      if (m_gridLineManager.SetData(data))
      {
        m_chartWindowDrawData->Data = data;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void AreaChart::SetLineColor(const Color color)
    {
      if (color != m_lineColor)
      {
        m_lineColor = color;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void AreaChart::SetBackgroundColor(const Color color)
    {
      if (color != m_backgroundColor)
      {
        m_backgroundColor = color;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void AreaChart::SetLabelColor(const Color color)
    {
      if (color != m_labelColor)
      {
        m_labelColor = color;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void AreaChart::SetEntryColor(const uint32_t index, const Color color)
    {
      if (m_chartWindowDrawData->SetEntryColor(index, color))
      {
        PropertyUpdated(PropertyType::Other);
      }
    }


    void AreaChart::WinResolutionChanged(const ResolutionChangedInfo& info)
    {
      FSL_PARAM_NOT_USED(info);

      auto context = GetContext();

      m_gridLineManager.SetChartEntryWidth(context->UnitConverter.DpToPxInt32(LocalConfig::ChartBarWidthDp));
      m_gridLineManager.SetChartLabelSpacing(context->UnitConverter.DpToPxInt32(LocalConfig::ChartLabelSpacingDp));
    }


    void AreaChart::WinPostLayout()
    {
      PxSize2D renderSizePx = RenderSizePx();
      // const uint32_t dataIOnfo = 1;
      const int32_t maxEntries =
        renderSizePx.Width() / m_gridLineManager.GetChartEntryWidth() + ((renderSizePx.Width() % m_gridLineManager.GetChartEntryWidth()) > 0 ? 1 : 0);

      auto data = m_gridLineManager.GetData();
      const uint32_t maxStackedEntries = data ? data->DataInfo().ComplexDataEntries : 1;

      const auto maxPixelsX = UncheckedNumericCast<uint32_t>(maxEntries);
      const uint32_t vertexCapacity = maxStackedEntries * maxPixelsX * 6u;
      m_graphMesh.EnsureCapacity(vertexCapacity);
    }

    void AreaChart::WinDraw(const UIDrawContext& context)
    {
      assert(m_chartWindowDrawData);
      m_gridLineManager.ExtractDrawData(*m_chartWindowDrawData, RenderSizePx(), GetLabelBackground().get(), GetFont().get());

      const Color finalBaseColor(GetFinalBaseColor());

      if (m_graphMesh.IsValid())
      {
        m_chartWindowDrawData->FillChartCache(finalBaseColor);
        context.CommandBuffer.DrawCustom(m_graphMesh.Get(), context.TargetRect.Location(), RenderSizePx(), finalBaseColor, DrawCustomGraph,
                                         m_chartWindowDrawData);
      }
      if (m_gridLinesMesh.IsValid())
      {
        context.CommandBuffer.DrawCustom(m_gridLinesMesh.Get(), context.TargetRect.Location(), RenderSizePx(), finalBaseColor * m_lineColor,
                                         DrawCustomGridLines, m_chartWindowDrawData);
      }
      if (m_gridLabelsBackground.IsValid())
      {
        context.CommandBuffer.DrawCustom(m_gridLabelsBackground.Get(), context.TargetRect.Location(), RenderSizePx(),
                                         finalBaseColor * m_backgroundColor, DrawCustomGridLabelsBG, m_chartWindowDrawData);
      }
      if (m_gridLabelsMesh.IsValid())
      {
        context.CommandBuffer.DrawCustom(m_gridLabelsMesh.Get(), context.TargetRect.Location(), RenderSizePx(), finalBaseColor * m_labelColor,
                                         DrawCustomGridLabels, m_chartWindowDrawData);
      }

      // This really only need to be called if the graph data changes, but for now this is good enough
      CheckAnimationState();
    }

    void AreaChart::UpdateAnimation(const TransitionTimeSpan& timeSpan)
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
}


// 9876543210
// 4294967295
// constexpr uint16_t BasicLog10(const uint32_t value)
//{
//  return (value >= 1000000000u)  ? 9u
//         : (value >= 100000000u) ? 8u
//         : (value >= 10000000u)  ? 7u
//         : (value >= 1000000u)   ? 6u
//         : (value >= 100000u)    ? 5u
//         : (value >= 10000u)     ? 4u
//         : (value >= 1000u)      ? 3u
//         : (value >= 100u)       ? 2u
//         : (value >= 10u)        ? 1u
//                                 : 0u;
//}

// constexpr uint32_t FindMaxBase10(const uint32_t value)
//{
//  return (value >= 1000000000u)  ? 1000000000u
//         : (value >= 100000000u) ? 100000000u
//         : (value >= 10000000u)  ? 10000000u
//         : (value >= 1000000u)   ? 1000000u
//         : (value >= 100000u)    ? 100000u
//         : (value >= 10000u)     ? 10000u
//         : (value >= 1000u)      ? 1000u
//         : (value >= 100u)       ? 100u
//         : (value >= 10u)        ? 10u
//                                 : 1u;
//}


// void ExtractDrawData2(ChartDataWindowRecord& rDst, const PxSize2D renderSizePx, const NineSliceSprite* const pBackgroundSprite,
//                      const SpriteFont* const pFont)
//{
//  if (rDst.Data)
//  {
//    IChartComplexDataWindow& rData = *rDst.Data;
//    {    // Update the 'chart data window size' to match what we can draw
//      assert(rDst.Chart.EntryWidthPx > 0);
//      const int32_t entryWidthPx = rDst.Chart.EntryWidthPx;
//      const int32_t windowMaxEntries = (renderSizePx.Width() / entryWidthPx) + ((renderSizePx.Width() % entryWidthPx) > 0 ? 1 : 0);
//      rData.SetWindowMaxSize(windowMaxEntries);
//    }
//    {    // Generate the grid line + label entries
//      const int32_t renderHeightPx = renderSizePx.Height();
//      const int32_t backgroundXOffsetPx = rDst.Chart.EntryWidthPx * 4;
//      const int32_t fontBaseLinePx = (pFont != nullptr ? pFont->GetInfo().ScaledBaseLinePx : 0);
//      const int32_t fontlineSpacingPx = (pFont != nullptr ? pFont->GetInfo().ScaledLineSpacingPx : 0);
//      const RenderNineSliceInfo backgroundRenderInfo(pBackgroundSprite != nullptr ? pBackgroundSprite->GetRenderInfo() :
//      RenderNineSliceInfo()); const PxSize2D contentMarginSumPx = backgroundRenderInfo.ScaledContentMarginPx.Sum(); const int32_t
//      maxCaptionEntryHeightPx = (fontlineSpacingPx + backgroundRenderInfo.ScaledContentMarginPx.SumY()) + rDst.LabelSpacingPx;
//      // const uint32_t maxEntries = std::min(maxCaptionEntryHeightPx > 0 ? uint32_t(renderHeightPx / maxCaptionEntryHeightPx) : uint32_t(1u),
//      LocalConfig::MaxGridLines);

//      const ChartGridInfo gridInfo = rData.GridInfo();

//      //const ReadOnlySpan<ChartGridLineInfo> gridLinePositionSpan = rData.GridLinesAsReadOnlySpan();

//      const int32_t maxYPx = renderHeightPx - 1;    // -1 because we don't start the last pixel at height
//      // Calc the render scale of the data
//      const uint32_t viewMin = gridInfo.ViewMin;
//      const uint32_t viewMax = std::max(gridInfo.ViewMax, viewMin);
//      const uint32_t delta = (viewMax - viewMin);
//      const float dataRenderScale = delta > 0 ? float(maxYPx) / float(delta) : 1.0f;
//      const float pixelsToDataScale = maxYPx > 0 ? float(delta) / float(maxYPx) : 1.0f;

//      //const auto gridLineCount = UncheckedNumericCast<uint32_t>(std::min(gridLinePositionSpan.size(), rDst.GridLines.size()));
//      uint32_t gridLineIndex = 0;


//      {
//        uint32_t maxBase10 = FindMaxBase10(viewMax);
//        uint32_t minBase10 = FindMaxBase10(viewMin);
//        uint32_t maxCaptionValue = maxBase10 * (viewMax / maxBase10);
//        uint32_t maxCaptionValue2 = maxCaptionValue + (maxBase10 / 2);
//        if (maxCaptionValue2 <= viewMax)
//          maxCaptionValue = maxCaptionValue2;

//        const uint32_t qwww = static_cast<uint32_t>(std::round(maxCaptionEntryHeightPx * pixelsToDataScale));
//        const uint32_t qwEntriesMax = qwww > 0 ? maxCaptionValue / qwww : 1u;

//        uint32_t qw = maxCaptionValue / qwEntriesMax;
//        uint32_t magic = FindMaxBase10(qw);
//        uint32_t base10EntrySub = magic * ((qw / magic) + ((qw % magic) >= (magic / 2) ? 1 : 0));
//        if (base10EntrySub < qw)
//          base10EntrySub += magic;
//        const uint32_t entryCount = std::min(base10EntrySub > 0 ? (maxCaptionValue / base10EntrySub) + 1 : 1, LocalConfig::MaxGridLines);

//        for (uint32_t i = 0; i < entryCount; ++i)
//        {
//          const uint32_t position = maxCaptionValue - (i * base10EntrySub);
//          const uint32_t dataValue = position - viewMin;
//          const int32_t gridLineYPositionPx = maxYPx - TypeConverter::UncheckedChangeTo<int32_t>(dataValue * dataRenderScale);

//          auto labelStrView = rData.GetDescriptionString(position);

//          const PxSize2D labelSizePx(
//            pFont != nullptr ? PxSize2D(pFont->MeasureString(labelStrView).Width(), pFont->GetInfo().ScaledLineSpacingPx) : PxSize2D());
//          const PxSize2D finalSizePx = PxSize2D::Max(backgroundRenderInfo.ScaledSizePx, labelSizePx + contentMarginSumPx);
//          const int32_t backgroundYOffsetPx = gridLineYPositionPx - fontBaseLinePx - backgroundRenderInfo.ScaledContentMarginPx.Top();

//          rDst.GridLines[gridLineIndex].LabelOffsetPx =
//            PxPoint2(backgroundXOffsetPx + backgroundRenderInfo.ScaledContentMarginPx.Left(), gridLineYPositionPx);
//          rDst.GridLines[gridLineIndex].LabelSizePx = labelSizePx;
//          rDst.GridLines[gridLineIndex].LabelBackgroundRectanglePx =
//            PxRectangle(PxPoint2(backgroundXOffsetPx, backgroundYOffsetPx), finalSizePx);
//          StringViewLiteUtil::Set(rDst.GridLines[gridLineIndex].Label, labelStrView);

//          ++gridLineIndex;
//        }
//      }

//      //for (uint32_t i = 0; i < gridLineCount; ++i)
//      //{
//      //  const ChartGridLineInfo& entry = gridLinePositionSpan[i];
//      //  const uint32_t position = entry.Position;
//      //  if (position >= viewMin && position < viewMax)
//      //  {
//      //    const uint32_t dataValue = position - viewMin;
//      //    const int32_t gridLineYPositionPx = maxYPx - TypeConverter::UncheckedChangeTo<int32_t>(dataValue * dataRenderScale);

//      //    const PxSize2D contentMarginSumPx = backgroundRenderInfo.ScaledContentMarginPx.Sum();
//      //    const PxSize2D labelSizePx(
//      //      pFont != nullptr ? PxSize2D(pFont->MeasureString(entry.Label).Width(), pFont->GetInfo().ScaledLineSpacingPx) : PxSize2D());
//      //    const PxSize2D finalSizePx = PxSize2D::Max(backgroundRenderInfo.ScaledSizePx, labelSizePx + contentMarginSumPx);
//      //    const int32_t backgroundYOffsetPx = gridLineYPositionPx - fontBaseLinePx - backgroundRenderInfo.ScaledContentMarginPx.Top();

//      //    rDst.GridLines[gridLineIndex].LabelOffsetPx =
//      //      PxPoint2(backgroundXOffsetPx + backgroundRenderInfo.ScaledContentMarginPx.Left(), gridLineYPositionPx);
//      //    rDst.GridLines[gridLineIndex].LabelSizePx = labelSizePx;
//      //    rDst.GridLines[gridLineIndex].LabelBackgroundRectanglePx =
//      //      PxRectangle(PxPoint2(backgroundXOffsetPx, backgroundYOffsetPx), finalSizePx);
//      //    StringViewLiteUtil::Set(rDst.GridLines[gridLineIndex].Label, entry.Label);
//      //    ++gridLineIndex;
//      //  }
//      //}

//      // Fill the grid line info
//      rDst.FontBaseLinePx = fontBaseLinePx;
//      rDst.GridLineCount = gridLineIndex;
//      rDst.Chart.ViewMin = viewMin;
//      rDst.Chart.ViewMax = viewMax;
//      rDst.Chart.DataRenderScale = dataRenderScale;
//    }
//  }
//  else
//  {
//    rDst.Clear();
//  }
//}
