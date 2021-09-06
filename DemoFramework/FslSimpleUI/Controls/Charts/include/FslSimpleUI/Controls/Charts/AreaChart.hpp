#ifndef FSLSIMPLEUI_CONTROLS_CHARTS_AREACHART_HPP
#define FSLSIMPLEUI_CONTROLS_CHARTS_AREACHART_HPP
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

#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteMesh.hpp>
#include <FslSimpleUI/Base/Mesh/CustomSpriteFontMesh.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChartGridLineManager.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class NineSliceSprite;

  namespace UI
  {
    namespace Render
    {
      struct ChartDataWindowDrawData;
    }

    struct ChartDataWindowRecord;
    class IChartComplexDataWindow;

    class AreaChart final : public BaseWindow
    {
      AreaChartGridLineManager m_gridLineManager;
      std::shared_ptr<Render::ChartDataWindowDrawData> m_chartWindowDrawData;

      CustomBasicSpriteBasicMesh m_graphMesh;
      CustomBasicSpriteMesh m_gridLinesMesh;
      CustomSpriteMesh<NineSliceSprite> m_gridLabelsBackground;
      CustomSpriteFontMesh m_gridLabelsMesh;
      Color m_lineColor;
      Color m_backgroundColor;
      Color m_labelColor;

    public:
      explicit AreaChart(const std::shared_ptr<BaseWindowContext>& context);
      ~AreaChart() override;

      void SetOpaqueFillSprite(const std::shared_ptr<BasicImageSprite>& value);
      void SetTransparentFillSprite(const std::shared_ptr<BasicImageSprite>& value);

      const std::shared_ptr<NineSliceSprite>& GetLabelBackground() const;
      void SetLabelBackground(const std::shared_ptr<NineSliceSprite>& value);

      const std::shared_ptr<SpriteFont>& GetFont() const;
      void SetFont(const std::shared_ptr<SpriteFont>& value);

      const std::shared_ptr<IChartComplexDataWindow>& GetData() const;
      void SetData(const std::shared_ptr<IChartComplexDataWindow>& data);


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

      void SetEntryColor(const uint32_t index, const Color color);

      void WinResolutionChanged(const ResolutionChangedInfo& info) final;
      void WinPostLayout() final;
      void WinDraw(const UIDrawContext& context) final;

    protected:
      void UpdateAnimation(const TransitionTimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;
    };
  }
}


#endif
