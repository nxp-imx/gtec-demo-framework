#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_AREACHART_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_AREACHART_HPP
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

#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslDataBinding/Base/Property/TypedObserverDependencyProperty.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteMesh.hpp>
#include <FslSimpleUI/Base/Mesh/CustomSpriteFontMesh.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChartGridLineManager.hpp>
#include <FslSimpleUI/Controls/Charts/ChartRenderPolicy.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class NineSliceSprite;

  namespace UI
  {
    class AChartData;
    namespace Render
    {
      struct ChartDataWindowDrawData;
    }

    struct ChartDataWindowRecord;
    class ChartDataView;

    class AreaChart final
      : public BaseWindow
      , public DataBinding::IObjectObserver
    {
      using dataview_prop_type = std::shared_ptr<ChartDataView>;

      struct DataViewCache
      {
        uint32_t Count{0};
        uint32_t ChannelCount{0};
      };


      AreaChartGridLineManager m_gridLineManager;
      std::shared_ptr<Render::ChartDataWindowDrawData> m_chartWindowDrawData;

      CustomBasicSpriteBasicMesh m_graphMesh;
      CustomBasicSpriteMesh m_gridLinesMesh;
      CustomSpriteMesh<NineSliceSprite> m_gridLabelsBackground;
      CustomSpriteFontMesh m_gridLabelsMesh;
      Color m_lineColor;
      Color m_backgroundColor;
      Color m_labelColor;
      ChartRenderPolicy m_renderPolicy;
      DataViewCache m_dataViewCache;

      DataBinding::TypedDependencyProperty<bool> m_propertyMatchDataViewEntries;
      DataBinding::TypedObserverDependencyProperty<dataview_prop_type> m_propertyDataView;


    public:
      static DataBinding::DependencyPropertyDefinition PropertyMatchDataViewEntries;
      static DataBinding::DependencyPropertyDefinition PropertyDataView;

      explicit AreaChart(const std::shared_ptr<BaseWindowContext>& context);
      ~AreaChart() override;

      ChartRenderPolicy GetRenderPolicy() const;
      void SetRenderPolicy(const ChartRenderPolicy value);

      void SetOpaqueFillSprite(const std::shared_ptr<BasicImageSprite>& value);
      void SetTransparentFillSprite(const std::shared_ptr<BasicImageSprite>& value);

      const std::shared_ptr<NineSliceSprite>& GetLabelBackground() const;
      void SetLabelBackground(const std::shared_ptr<NineSliceSprite>& value);

      const std::shared_ptr<SpriteFont>& GetFont() const;
      void SetFont(const std::shared_ptr<SpriteFont>& value);

      void SetGridLines(const std::shared_ptr<IChartGridLines>& gridLines);


      bool GetMatchDataViewEntries() const;

      //! @param enabled if true the view size will be forced to match what can be displayed
      bool SetMatchDataViewEntries(const bool enabled);


      const std::shared_ptr<ChartDataView>& GetDataView() const;

      //! @brief Set the data view for this chart
      bool SetDataView(const std::shared_ptr<ChartDataView>& dataView);

      //! @brief Set the data view for this chart
      bool SetDataView(const std::shared_ptr<AChartData>& data);


      Color GetLineColor() const
      {
        return m_lineColor;
      }

      void SetLineColor(const Color color);

      Color GetBackgroundColor() const
      {
        return m_backgroundColor;
      }

      void SetBackgroundColor(const Color color);

      Color GetLabelColor() const
      {
        return m_labelColor;
      }

      void SetLabelColor(const Color color);

      // void SetEntryColor(const uint32_t index, const Color color);

      void WinResolutionChanged(const ResolutionChangedInfo& info) final;
      void WinPostLayout() final;
      void WinDraw(const UIDrawContext& context) final;

      void OnChanged(const DataBinding::DataBindingInstanceHandle hInstance) final;

    protected:
      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) final;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) final;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      bool ProcessDataViewChange();
      void UpdateAnimation(const TimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;
    };
  }
}


#endif
