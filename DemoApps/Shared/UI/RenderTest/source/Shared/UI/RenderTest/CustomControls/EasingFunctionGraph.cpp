/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterialUtil.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Render/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <Shared/UI/RenderTest/CustomControls/EasingFunctionGraph.hpp>
#include <algorithm>

namespace Fsl::UI::Custom
{
  using TClass = EasingFunctionGraph;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyContentColor = TFactory::Create<UIColor, TClass, &TClass::GetContentColor, &TClass::SetContentColor>("Color");
  TDef TClass::PropertyTransitionType =
    TFactory::Create<TransitionType, TClass, &TClass::GetTransitionType, &TClass::SetTransitionType>("TransitionType");


  namespace
  {
    namespace LocalConfig
    {
      const int16_t g_maxSamplingPoints = 512;
    }

    void DrawCustomLineStrip(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                             const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const int32_t widthPx = dstSizePx.RawWidth();
      const auto heightPxf = static_cast<float>(dstSizePx.RawHeight());
      const int32_t areaBorderPx = TypeConverter::ChangeTo<int32_t>(heightPxf * 0.25f);
      const auto areaHeightPxf = static_cast<float>(dstSizePx.RawHeight() - (2 * areaBorderPx));
      const auto* const pDrawData = dynamic_cast<const EasingFunctionGraphInfo*>(pCustomDrawData);
      if (widthPx >= 2 && heightPxf > 0.0f && pDrawData != nullptr)
      {
        auto fnEasingFunction = pDrawData->TryGet();
        if (fnEasingFunction != nullptr)
        {
          const auto widthPxf = static_cast<float>(widthPx);
          {    // Render background
            const auto color = rBuilder.GetColor();
            rBuilder.SetColor(UIRenderColor(Colors::Black()));
            rBuilder.AddVertex(dstPositionPxf.X.Value, dstPositionPxf.Y.Value, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, dstPositionPxf.Y.Value, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);

            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, dstPositionPxf.Y.Value, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, dstPositionPxf.Y.Value + heightPxf, renderInfo.TextureArea.X0,
                               renderInfo.TextureArea.Y0);

            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, dstPositionPxf.Y.Value + heightPxf, renderInfo.TextureArea.X0,
                               renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value, dstPositionPxf.Y.Value + heightPxf, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);

            rBuilder.AddVertex(dstPositionPxf.X.Value, dstPositionPxf.Y.Value + heightPxf, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value, dstPositionPxf.Y.Value, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.SetColor(color);
          }
          {    // Render grid lines
            const auto color = rBuilder.GetColor();
            const auto areaBorderPxf = static_cast<float>(areaBorderPx);
            const auto y0 = dstPositionPxf.Y.Value + heightPxf - areaBorderPxf;
            const auto y1 = dstPositionPxf.Y.Value + areaBorderPxf;
            rBuilder.SetColor(UIRenderColor(PackedColor32(0xFF808080)));
            rBuilder.AddVertex(dstPositionPxf.X.Value, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.AddVertex(dstPositionPxf.X.Value + widthPxf, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            rBuilder.SetColor(color);
          }

          {    // render graph
            const int32_t samplingPoints = std::min(widthPx, UncheckedNumericCast<int32_t>(LocalConfig::g_maxSamplingPoints));
            const float sampleAdd = 1.0f / static_cast<float>(samplingPoints);
            const float pixelAdd = widthPxf / static_cast<float>(samplingPoints);

            float offsetXPxf = dstPositionPxf.X.Value;
            float offsetYPxf = dstPositionPxf.Y.Value + static_cast<float>(areaBorderPx);
            float transitionValue = 0.0f;
            float lastXPxf = offsetXPxf;
            float lastYPxf = offsetYPxf + (areaHeightPxf - (fnEasingFunction(0.0f) * areaHeightPxf));
            transitionValue += sampleAdd;
            offsetXPxf += pixelAdd;
            for (int32_t i = 1; i < samplingPoints - 1; ++i)
            {
              const float easingValue = fnEasingFunction(transitionValue);
              const float y = offsetYPxf + (areaHeightPxf - (easingValue * areaHeightPxf));
              rBuilder.AddVertex(lastXPxf, lastYPxf, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
              rBuilder.AddVertex(offsetXPxf, y, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
              lastXPxf = offsetXPxf;
              lastYPxf = y;
              transitionValue += sampleAdd;
              offsetXPxf += pixelAdd;
            }
            {
              const float easingValue = fnEasingFunction(1.0f);
              const float y = offsetYPxf + (areaHeightPxf - (easingValue * areaHeightPxf));
              rBuilder.AddVertex(lastXPxf, lastYPxf, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
              rBuilder.AddVertex(offsetXPxf, y, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
            }
          }
        }
      }
    }
  }


  EasingFunctionGraph::EasingFunctionGraph(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_drawInfo(std::make_shared<EasingFunctionGraphInfo>())
    , m_content(context->TheUIContext.Get()->MeshManager, (6 + LocalConfig::g_maxSamplingPoints) * 2)
    , m_propertyContentColor(context->ColorConverter, UIColors::White())
  {
    Enable(WindowFlags::DrawEnabled);
  }


  EasingFunctionGraph::~EasingFunctionGraph() = default;


  void EasingFunctionGraph::SetContent(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      m_contentPrimitiveTopology = m_content.GetPrimitiveTopology();
      PropertyUpdated(PropertyType::Content);
    }
  }


  void EasingFunctionGraph::SetContent(std::shared_ptr<BasicImageSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      m_contentPrimitiveTopology = m_content.GetPrimitiveTopology();
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool EasingFunctionGraph::SetContentColor(const UIColor value)
  {
    const bool changed = m_propertyContentColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  bool EasingFunctionGraph::SetTransitionType(const TransitionType value)
  {
    const bool changed = m_propertyTransitionType.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_drawInfo->SetTransitionType(value);
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void EasingFunctionGraph::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    const UIRenderColor finalBaseColor(GetFinalBaseColor());

    if (m_content.IsValid() && m_contentPrimitiveTopology == BasicPrimitiveTopology::LineList)
    {
      const auto finalContentColor = finalBaseColor * m_propertyContentColor.InternalColor;
      context.CommandBuffer.DrawCustom(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), finalContentColor, DrawCustomLineStrip,
                                       m_drawInfo);
    }
  }


  PxSize2D EasingFunctionGraph::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return base_type::ArrangeOverride(finalSizePx);
  }


  PxSize2D EasingFunctionGraph::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    return base_type::MeasureOverride(availableSizePx);
  }


  DataBinding::DataBindingInstanceHandle EasingFunctionGraph::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                            PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult EasingFunctionGraph::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                              const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                     PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor),
                                                     PropLinkRefs(PropertyTransitionType, m_propertyTransitionType));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void EasingFunctionGraph::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyContentColor);
    rProperties.push_back(PropertyTransitionType);
  }
}
