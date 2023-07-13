/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics2D/Procedural/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Controls/Charts/BoxPlotChart.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartData.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartDataView.hpp>
#include <FslSimpleUI/Controls/Charts/Util/BoxPlotHelper.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>
#include "Render/BoxPlotDrawData.hpp"

namespace Fsl::UI
{
  using TClass = BoxPlotChart;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyMedianColor =
    TFactory::Create<TClass::color_prop_type, TClass, &TClass::GetMedianColor, &TClass::SetMedianColor>("MedianColor");
  TDef TClass::PropertySpacing = TFactory::Create<TClass::spacing_prop_type, TClass, &TClass::GetSpacing, &TClass::SetSpacing>("Spacing");
  TDef TClass::PropertyBoxSize = TFactory::Create<TClass::size_prop_type, TClass, &TClass::GetBoxSize, &TClass::SetBoxSize>("BoxSize");
  TDef TClass::PropertyWhiskerSize =
    TFactory::Create<TClass::size_prop_type, TClass, &TClass::GetWhiskerSize, &TClass::SetWhiskerSize>("WhiskerSize");
  TDef TClass::PropertyOutlierSize =
    TFactory::Create<TClass::size_prop_type, TClass, &TClass::GetOutlierSize, &TClass::SetOutlierSize>("OutlierSize");
  TDef TClass::PropertyDataView = TFactory::Create<TClass::dataview_prop_type, TClass, &TClass::GetDataView, &TClass::SetDataView>("DataView");
}

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr Color DefaultMedianColor = Color::White();

      // constexpr TimeSpan ViewChangeTime(TimeSpan::FromMilliseconds(600));

      // 1   3345555   7
      // 122233455556667
      // 1   3345555   7

      constexpr uint32_t BoxPlotTriangles = (7 * 2);
      constexpr uint32_t BoxPlotVertices = BoxPlotTriangles * (3 * 2);

      constexpr DpSize1DF DefaultWhiskerSizeDpf = DpSize1DF::Create(5);
      constexpr DpSize1DF DefaultBoxSizeDpf = DpSize1DF::Create(7);
      constexpr DpSize1DF DefaultOutlierSizeDpf = DpSize1DF::Create(1);

      constexpr PxSize1D MinLineSizePx = PxSize1D::Create(1);
      constexpr PxSize1D MinWhiskerSizePx = PxSize1D::Create(3);
      constexpr PxSize1D MinBoxSizePx = PxSize1D::Create(3);
      constexpr PxSize1D MinOutlierSizePx = PxSize1D::Create(1);
    }

    //! Take the size and make it a en even value
    PxSize1D MakeUnevenRoundDoun(const PxSize1D sizePx)
    {
      const auto rawValuePx = sizePx.RawValue();
      return (rawValuePx & 1) == 1 ? sizePx : (PxSize1D::Create(std::max(rawValuePx - 1, 1), OptimizationCheckFlag::NoCheck));
    }


    void DrawCustomBoxPlots(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                            const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      if (dstSizePx.Height() < PxSize1D::Create(2))
      {
        return;
      }

      const auto* pDrawData = dynamic_cast<const Render::BoxPlotDrawData*>(pCustomDrawData);
      const ChartCanvas1D& canvas = pDrawData->Canvas;

      assert((pDrawData->DrawInfo.LineSizePx.RawValue() & 1) == 1);
      assert((pDrawData->DrawInfo.WhiskerSizePx.RawValue() & 1) == 1);
      assert((pDrawData->DrawInfo.BoxSizePx.RawValue() & 1) == 1);
      assert((pDrawData->DrawInfo.OutlierSizePx.RawValue() & 1) == 1);
      assert((pDrawData->DrawInfo.MaxSizePx.RawValue() & 1) == 1);

      const auto sizeOfOneDpInPxf = static_cast<float>(canvas.SizeOfOneDpInPixels().RawValue());
      const auto span = pDrawData->AsReadOnlySpan();

      if (!span.empty())
      {
        // 01   2  34   5   67
        //
        // AA   CCCDDEEEE   GG
        // ABBBBBCCDDEEEFFFFFG
        // ABBBBBCCDDEEEFFFFFG
        // AA   CCCDDEEEE   GG
        PxValue startOffsetPx;
        auto startPositionPxf = dstPositionPxf;
        const auto baseColor = rBuilder.GetColor();
        const auto medianColor = Color::Premultiply(pDrawData->MedianColor * baseColor);
        for (std::size_t spanIndex = 0; spanIndex < span.size(); ++spanIndex)
        {
          const auto& spanEntry = span[spanIndex];
          const auto primaryColor = Color::Premultiply(spanEntry.PrimaryColor * baseColor);
          rBuilder.SetColor(primaryColor);

          const auto barMiddlePx = (pDrawData->DrawInfo.MaxSizePx.RawValue() + 1) / 2;

          startOffsetPx += pDrawData->DrawInfo.NextBarPx;


          const auto outlierMinPx = canvas.CanvasAxisToPx(spanEntry.BoxPlot.OutlierMin);
          const auto outlierMaxPx = canvas.CanvasAxisToPx(spanEntry.BoxPlot.OutlierMax);

          const auto minPx = canvas.CanvasAxisToPx(spanEntry.BoxPlot.Min).Value;
          const auto q1Px = canvas.CanvasAxisToPx(spanEntry.BoxPlot.Q1).Value;
          const auto q2Px = canvas.CanvasAxisToPx(spanEntry.BoxPlot.Q2).Value;
          auto q3Px = canvas.CanvasAxisToPx(spanEntry.BoxPlot.Q3).Value;
          const auto maxPx = canvas.CanvasAxisToPx(spanEntry.BoxPlot.Max).Value;
          if (q3Px <= q1Px && (q3Px + 1) < maxPx)
          {
            ++q3Px;
          }


          if (canvas.GetLayoutOrientation() == LayoutOrientation::Horizontal)
          {
            const float dstX0Pxf = startPositionPxf.X.Value + static_cast<float>(minPx);
            const float dstX1Pxf = dstX0Pxf + sizeOfOneDpInPxf;
            const float dstX2Pxf = startPositionPxf.X.Value + static_cast<float>(q1Px);
            const float dstX3Pxf = startPositionPxf.X.Value + static_cast<float>(q2Px);
            const float dstX4Pxf = dstX3Pxf + sizeOfOneDpInPxf;
            const float dstX5Pxf = startPositionPxf.X.Value + static_cast<float>(q3Px);
            const float dstX6Pxf = startPositionPxf.X.Value + static_cast<float>(maxPx);
            const float dstX7Pxf = dstX6Pxf + sizeOfOneDpInPxf;

            {
              const float dstX0OutlerMinPxf = startPositionPxf.X.Value + static_cast<float>(outlierMinPx.Value);
              const float dstX1OutlerMinPxf = dstX0OutlerMinPxf + sizeOfOneDpInPxf;
              const float dstX0OutlerMaxPxf = startPositionPxf.X.Value + static_cast<float>(outlierMaxPx.Value);
              const float dstX1OutlerMaxPxf = dstX0OutlerMaxPxf + sizeOfOneDpInPxf;

              const int32_t halfHeightPx = pDrawData->DrawInfo.OutlierSizePx.RawValue() / 2;
              const float dstY0Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstY1Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // Min outlier box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0OutlerMinPxf, dstY0Pxf, dstX1OutlerMinPxf, dstY1Pxf),
                               renderInfo.TextureArea);
              // Max outlier box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0OutlerMaxPxf, dstY0Pxf, dstX1OutlerMaxPxf, dstY1Pxf),
                               renderInfo.TextureArea);
            }
            {
              const int32_t halfHeightPx = pDrawData->DrawInfo.LineSizePx.RawValue() / 2;
              const float dstY0Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstY1Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // left line
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX1Pxf, dstY0Pxf, dstX2Pxf, dstY1Pxf), renderInfo.TextureArea);
              // right line
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX5Pxf, dstY0Pxf, dstX6Pxf, dstY1Pxf), renderInfo.TextureArea);
            }
            {
              const int32_t halfHeightPx = pDrawData->DrawInfo.BoxSizePx.RawValue() / 2;
              const float dstY0Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstY1Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // q1 box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX2Pxf, dstY0Pxf, dstX3Pxf, dstY1Pxf), renderInfo.TextureArea);
              // q3 box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX4Pxf, dstY0Pxf, dstX5Pxf, dstY1Pxf), renderInfo.TextureArea);

              // q2 box
              rBuilder.SetColor(medianColor);
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX3Pxf, dstY0Pxf, dstX4Pxf, dstY1Pxf), renderInfo.TextureArea);
            }

            {    // left and right whisker
              const int32_t halfHeightPx = pDrawData->DrawInfo.WhiskerSizePx.RawValue() / 2;
              const float dstY0Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstY1Pxf = startPositionPxf.Y.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              rBuilder.SetColor(primaryColor);
              // min box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf), renderInfo.TextureArea);
              // max box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX6Pxf, dstY0Pxf, dstX7Pxf, dstY1Pxf), renderInfo.TextureArea);
            }
            startPositionPxf.Y = dstPositionPxf.Y + PxValueF(startOffsetPx);
          }
          else
          {
            const int32_t yOffsetPx = dstSizePx.RawHeight() - 1;
            const float dstY0Pxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - minPx);
            const float dstY1Pxf = dstY0Pxf - sizeOfOneDpInPxf;
            const float dstY2Pxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - q1Px);
            const float dstY3Pxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - q2Px);
            const float dstY4Pxf = dstY3Pxf - sizeOfOneDpInPxf;
            const float dstY5Pxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - q3Px);
            const float dstY6Pxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - maxPx);
            const float dstY7Pxf = dstY6Pxf - sizeOfOneDpInPxf;


            {
              const float dstY0OutlerMinPxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - outlierMinPx.Value);
              const float dstY1OutlerMinPxf = dstY0OutlerMinPxf - sizeOfOneDpInPxf;
              const float dstY0OutlerMaxPxf = startPositionPxf.Y.Value + static_cast<float>(yOffsetPx - outlierMaxPx.Value);
              const float dstY1OutlerMaxPxf = dstY0OutlerMaxPxf - sizeOfOneDpInPxf;

              const int32_t halfHeightPx = pDrawData->DrawInfo.OutlierSizePx.RawValue() / 2;
              const float dstX0Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstX1Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // Min outlier box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY1OutlerMinPxf, dstX1Pxf, dstY0OutlerMinPxf),
                               renderInfo.TextureArea);
              // Max outlier box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY1OutlerMaxPxf, dstX1Pxf, dstY0OutlerMaxPxf),
                               renderInfo.TextureArea);
            }
            {
              const int32_t halfHeightPx = pDrawData->DrawInfo.LineSizePx.RawValue() / 2;
              const float dstX0Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstX1Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // left line
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY2Pxf, dstX1Pxf, dstY1Pxf), renderInfo.TextureArea);
              // right line
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY6Pxf, dstX1Pxf, dstY5Pxf), renderInfo.TextureArea);
            }
            {
              const int32_t halfHeightPx = pDrawData->DrawInfo.BoxSizePx.RawValue() / 2;
              const float dstX0Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstX1Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              // q1 box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY3Pxf, dstX1Pxf, dstY2Pxf), renderInfo.TextureArea);
              // q3 box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY5Pxf, dstX1Pxf, dstY4Pxf), renderInfo.TextureArea);

              // q2 box
              rBuilder.SetColor(medianColor);
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY4Pxf, dstX1Pxf, dstY3Pxf), renderInfo.TextureArea);
            }
            {    // left and right whisker
              const int32_t halfHeightPx = pDrawData->DrawInfo.WhiskerSizePx.RawValue() / 2;
              const float dstX0Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx - halfHeightPx);
              const float dstX1Pxf = startPositionPxf.X.Value + static_cast<float>(barMiddlePx + halfHeightPx + 1);

              rBuilder.SetColor(primaryColor);
              // min box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY1Pxf, dstX1Pxf, dstY0Pxf), renderInfo.TextureArea);
              // max box
              rBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(dstX0Pxf, dstY7Pxf, dstX1Pxf, dstY6Pxf), renderInfo.TextureArea);
            }
            startPositionPxf.X = dstPositionPxf.X + PxValueF(startOffsetPx);
          }
        }
      }
    }
  }

  BoxPlotChart::BoxPlotChart(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_drawData(std::make_shared<Render::BoxPlotDrawData>(context->UnitConverter))
    , m_graphMesh(context->TheUIContext.Get()->MeshManager, 6)
    , m_propertyMedianColor(LocalConfig::DefaultMedianColor)
  {
    // Default orientation of a box-plot is Horizontal
    m_drawData->Canvas.SetOrientation(LayoutOrientation::Horizontal);

    m_propertySpacing.Set(ThisDependencyObject(), DpSize1DF::Create(4.0f));

    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::PostLayoutEnabled));
  }


  BoxPlotChart::~BoxPlotChart() = default;


  void BoxPlotChart::SetOpaqueFillSprite(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_graphMesh.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  LayoutOrientation BoxPlotChart::GetLayoutOrientation() const
  {
    return m_drawData->Canvas.GetLayoutOrientation();
  }

  bool BoxPlotChart::SetOrientation(const LayoutOrientation value)
  {
    const bool changed = m_drawData->Canvas.SetOrientation(value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  BoxPlotChart::color_prop_type BoxPlotChart::GetMedianColor() const
  {
    return m_propertyMedianColor.Get();
  }

  bool BoxPlotChart::SetMedianColor(const color_prop_type color)
  {
    const bool changed = m_propertyMedianColor.Set(ThisDependencyObject(), color);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  BoxPlotChart::spacing_prop_type BoxPlotChart::GetSpacing() const
  {
    return m_propertySpacing.Get();
  }

  bool BoxPlotChart::SetSpacing(const spacing_prop_type sizeDp)
  {
    const bool changed = m_propertySpacing.Set(ThisDependencyObject(), sizeDp);
    if (changed)
    {
      m_cacheIsReady = false;
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  BoxPlotChart::size_prop_type BoxPlotChart::GetWhiskerSize() const
  {
    return m_propertyWhiskerSize.Get();
  }

  bool BoxPlotChart::SetWhiskerSize(const size_prop_type sizeDp)
  {
    const bool changed = m_propertyWhiskerSize.Set(ThisDependencyObject(), sizeDp);
    if (changed)
    {
      m_cacheIsReady = false;
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  BoxPlotChart::size_prop_type BoxPlotChart::GetBoxSize() const
  {
    return m_propertyBoxSize.Get();
  }

  bool BoxPlotChart::SetBoxSize(const size_prop_type sizeDp)
  {
    const bool changed = m_propertyBoxSize.Set(ThisDependencyObject(), sizeDp);
    if (changed)
    {
      m_cacheIsReady = false;
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  BoxPlotChart::size_prop_type BoxPlotChart::GetOutlierSize() const
  {
    return m_propertyOutlierSize.Get();
  }

  bool BoxPlotChart::SetOutlierSize(const size_prop_type sizeDp)
  {
    const bool changed = m_propertyOutlierSize.Set(ThisDependencyObject(), sizeDp);
    if (changed)
    {
      m_cacheIsReady = false;
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  const std::shared_ptr<ChartDataView>& BoxPlotChart::GetDataView() const
  {
    return m_propertyDataView.Get();
  }

  bool BoxPlotChart::SetDataView(const std::shared_ptr<ChartDataView>& dataView)
  {
    const bool changed = m_propertyDataView.Set(this, this, ThisDependencyObject(), PropertyDataView, dataView);
    if (changed)
    {
      m_channels.clear();
      m_drawData->ChangeId = 0;

      const auto& dataView = m_propertyDataView.Get();
      if (dataView)
      {
        const uint32_t channelCount = dataView->ChannelCount();
        for (uint32_t i = 0; i < channelCount; ++i)
        {
          m_channels.emplace_back(dataView, i);
        }
        // Ensure that we can hold triangles for a box-plot per channel
        m_graphMesh.EnsureCapacity(channelCount * LocalConfig::BoxPlotVertices);
        // Ensure that we re-cache the data
        m_drawData->ChangeId = dataView->ChangeId() - 1;
      }
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }

  bool BoxPlotChart::SetDataView(const std::shared_ptr<AChartData>& data)
  {
    return SetDataView(std::make_shared<ChartDataView>(data));
  }


  void BoxPlotChart::WinDraw(const UIDrawContext& context)
  {
    assert(m_drawData);

    const Color finalBaseColor(GetFinalBaseColor());

    if (m_graphMesh.IsValid() && !m_drawData->IsEmpty())
    {
      m_drawData->MedianColor = m_propertyMedianColor.Get();

      // Schedule the rendering
      context.CommandBuffer.DrawCustom(m_graphMesh.Get(), context.TargetRect.Location(), RenderSizePx(), finalBaseColor, DrawCustomBoxPlots,
                                       m_drawData);
    }
  }


  void BoxPlotChart::OnChanged(const DataBinding::DataBindingInstanceHandle hInstance)
  {
    if (m_propertyDataView.IsChangeFromSource(hInstance))
    {
      const bool requireLayout = ProcessDataViewChange();
      PropertyUpdated(requireLayout ? PropertyType::Content : PropertyType::Other);
    }
  }


  void BoxPlotChart::WinResolutionChanged(const ResolutionChangedInfo& info)
  {
    BaseWindow::WinResolutionChanged(info);

    assert(m_drawData);
    m_drawData->Canvas.OnResolutionChanged(GetContext()->UnitConverter);

    m_cacheIsReady = false;
  }

  DataBinding::DataBindingInstanceHandle BoxPlotChart::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, PropLinkRefs(PropertyMedianColor, m_propertyMedianColor),
      PropLinkRefs(PropertySpacing, m_propertySpacing), PropLinkRefs(PropertyBoxSize, m_propertyBoxSize),
      PropLinkRefs(PropertyWhiskerSize, m_propertyWhiskerSize), PropLinkRefs(PropertyOutlierSize, m_propertyOutlierSize),
      PropLinkRefs(PropertyDataView, m_propertyDataView));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult BoxPlotChart::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                       const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(PropertyMedianColor, m_propertyMedianColor),
      PropLinkRefs(PropertySpacing, m_propertySpacing), PropLinkRefs(PropertyBoxSize, m_propertyBoxSize),
      PropLinkRefs(PropertyWhiskerSize, m_propertyWhiskerSize), PropLinkRefs(PropertyOutlierSize, m_propertyOutlierSize),
      PropLinkRefs(PropertyDataView, m_propertyDataView));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  PxSize2D BoxPlotChart::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    assert(m_cacheIsReady);

    BaseWindow::ArrangeOverride(finalSizePx);
    return m_drawData->Canvas.Arrange(finalSizePx, m_drawData->DrawInfo.MaxSizePx, m_drawData->DrawInfo.SpacingPx,
                                      UncheckedNumericCast<uint8_t>(m_channels.size()));
  }


  PxSize2D BoxPlotChart::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    RebuildCacheOnDemand();

    BaseWindow::MeasureOverride(availableSizePx);
    return m_drawData->Canvas.Measure(availableSizePx, m_drawData->DrawInfo.MaxSizePx, m_drawData->DrawInfo.SpacingPx,
                                      UncheckedNumericCast<uint8_t>(m_channels.size()));
  }


  bool BoxPlotChart::ProcessDataViewChange()
  {
    bool requireRelayout = false;
    assert(m_drawData);
    Render::BoxPlotDrawData& rDrawData = *m_drawData;

    const auto* pDataViewElement = m_propertyDataView.TryGetElement();

    if (pDataViewElement != nullptr)
    {    // Extract axis range
      auto dataStats = pDataViewElement->CalculateDataStats();
      requireRelayout = rDrawData.Canvas.SetAxisRange(dataStats.ValueMinMax);
    }

    // Recalculate the box plot data
    PopulateDrawDataWithBoxPlots(rDrawData, pDataViewElement, ReadOnlySpanUtil::AsSpan(m_channels));
    return requireRelayout;
  }

  // Update the draw data with BoxPlot calculations per channel (if available)
  void BoxPlotChart::PopulateDrawDataWithBoxPlots(Render::BoxPlotDrawData& rDrawData, const ChartDataView* const pDataView,
                                                  ReadOnlySpan<ChartSortedDataChannelView> channels)
  {
    if (pDataView == nullptr || pDataView->Count() < BoxPlotHelper::MinimumEntries)
    {
      rDrawData.ChangeId = {};
      rDrawData.Clear();
    }
    else
    {
      const auto currentViewChangeId = pDataView->ChangeId();
      if (currentViewChangeId != rDrawData.ChangeId)
      {
        rDrawData.ChangeId = currentViewChangeId;
        rDrawData.Clear();
        for (uint32_t i = 0; i < channels.size(); ++i)
        {
          auto viewSpan = channels[i].GetChannelViewSpan();
          assert(viewSpan.size() >= BoxPlotHelper::MinimumEntries);
          auto boxPlot = BoxPlotHelper::Calculate(viewSpan);
          rDrawData.Add(boxPlot, pDataView->GetChannelMetaDataInfo(i).PrimaryColor);
        }
      }
    }
  }


  void BoxPlotChart::RebuildCacheOnDemand()
  {
    if (!m_cacheIsReady)
    {
      const auto& unitConverter = GetContext()->UnitConverter;

      const auto spacingDpf = GetSpacing();
      const auto lineSizeDpf = DpSize1DF::Create(1);
      const auto whiskerSizeDpf = GetWhiskerSize().SizeOr(LocalConfig::DefaultWhiskerSizeDpf);
      const auto boxSizeDpf = GetBoxSize().SizeOr(LocalConfig::DefaultBoxSizeDpf);
      const auto outlierSizeDpf = GetOutlierSize().SizeOr(LocalConfig::DefaultOutlierSizeDpf);

      const auto spacingPx = unitConverter.ToPxSize1D(spacingDpf);

      const auto requestedlineSizePx = MakeUnevenRoundDoun(unitConverter.ToPxSize1D(lineSizeDpf));
      const auto requestedWhiskerSizePx = MakeUnevenRoundDoun(unitConverter.ToPxSize1D(whiskerSizeDpf));
      const auto requestedBoxSizePx = MakeUnevenRoundDoun(unitConverter.ToPxSize1D(boxSizeDpf));
      const auto requestedOutlierSizePx = MakeUnevenRoundDoun(unitConverter.ToPxSize1D(outlierSizeDpf));

      const auto lineSizePx = PxSize1D::Max(requestedlineSizePx, LocalConfig::MinLineSizePx);
      const auto whiskerSizePx = PxSize1D::Max(requestedWhiskerSizePx, LocalConfig::MinWhiskerSizePx);
      const auto boxSizePx = PxSize1D::Max(requestedBoxSizePx, LocalConfig::MinBoxSizePx);
      const auto outlierSizePx = PxSize1D::Max(requestedOutlierSizePx, LocalConfig::MinOutlierSizePx);


      m_drawData->DrawInfo = Render::BoxPlotDrawInfo(spacingPx, lineSizePx, whiskerSizePx, boxSizePx, outlierSizePx);
      m_cacheIsReady = true;
    }
  }
}
