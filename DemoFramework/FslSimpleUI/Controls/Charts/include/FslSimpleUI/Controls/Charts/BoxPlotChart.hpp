#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_BOXPLOTCHART_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_BOXPLOTCHART_HPP
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

#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslDataBinding/Base/Property/TypedObserverDependencyProperty.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Controls/Charts/Data/ChartSortedDataChannelView.hpp>
#include <FslSimpleUI/Controls/Charts/Render/BoxPlotDrawInfo.hpp>
#include <memory>

namespace Fsl
{
  class NineSliceSprite;

  namespace UI
  {
    class AChartData;
    namespace Render
    {
      class BoxPlotDrawData;
    }

    struct ChartDataWindowRecord;
    class ChartDataView;

    class BoxPlotChart final
      : public BaseWindow
      , public DataBinding::IObjectObserver
    {
      using base_type = BaseWindow;
      using color_prop_type = UIColor;
      using spacing_prop_type = DpSize1DF;
      using size_prop_type = DpLayoutSize1D;
      using dataview_prop_type = std::shared_ptr<ChartDataView>;


      std::shared_ptr<Render::BoxPlotDrawData> m_drawData;
      std::vector<ChartSortedDataChannelView> m_channels;
      CustomBasicSpriteBasicMesh m_graphMesh;

      DataBinding::TypedDependencyProperty<color_prop_type> m_propertyMedianColor;
      DataBinding::TypedDependencyProperty<spacing_prop_type> m_propertySpacing;
      DataBinding::TypedDependencyProperty<size_prop_type> m_propertyBoxSize;
      DataBinding::TypedDependencyProperty<size_prop_type> m_propertyWhiskerSize;
      DataBinding::TypedDependencyProperty<size_prop_type> m_propertyOutlierSize;
      DataBinding::TypedObserverDependencyProperty<dataview_prop_type> m_propertyDataView;

      bool m_cacheIsReady{false};

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMedianColor;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertySpacing;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBoxSize;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyWhiskerSize;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyOutlierSize;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyDataView;


      explicit BoxPlotChart(const std::shared_ptr<BaseWindowContext>& context);
      ~BoxPlotChart() override;

      void SetOpaqueFillSprite(const std::shared_ptr<BasicImageSprite>& value);

      LayoutOrientation GetLayoutOrientation() const;
      bool SetOrientation(const LayoutOrientation value);

      color_prop_type GetMedianColor() const;
      bool SetMedianColor(const color_prop_type color);
      spacing_prop_type GetSpacing() const;
      bool SetSpacing(const spacing_prop_type sizeDp);
      size_prop_type GetWhiskerSize() const;
      bool SetWhiskerSize(const size_prop_type sizeDp);
      size_prop_type GetBoxSize() const;
      bool SetBoxSize(const size_prop_type sizeDp);
      size_prop_type GetOutlierSize() const;
      bool SetOutlierSize(const size_prop_type sizeDp);

      const std::shared_ptr<ChartDataView>& GetDataView() const;

      //! @brief Set the data view for this chart
      bool SetDataView(const std::shared_ptr<ChartDataView>& dataView);

      //! @brief Set the data view for this chart
      bool SetDataView(const std::shared_ptr<AChartData>& data);

      void WinDraw(const UIDrawContext& context) final;


      void OnChanged(const DataBinding::DataBindingInstanceHandle hInstance) final;

      void WinResolutionChanged(const ResolutionChangedInfo& info) final;

    protected:
      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) final;


      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

    private:
      bool ProcessDataViewChange();
      static void PopulateDrawDataWithBoxPlots(Render::BoxPlotDrawData& rDrawData, const ChartDataView* const pDataView,
                                               ReadOnlySpan<ChartSortedDataChannelView> channels, const UIColorConverter colorConverter);
      void RebuildCacheOnDemand();
    };
  }
}


#endif
