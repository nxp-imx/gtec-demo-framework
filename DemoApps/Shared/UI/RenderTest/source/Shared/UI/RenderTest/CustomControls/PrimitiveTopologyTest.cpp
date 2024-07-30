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
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <FslSimpleUI/Render/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <Shared/UI/RenderTest/CustomControls/PrimitiveTopologyTest.hpp>

namespace Fsl::UI::Custom
{
  using TClass = PrimitiveTopologyTest;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyContentColor = TFactory::Create<UIColor, TClass, &TClass::GetContentColor, &TClass::SetContentColor>("Color");


  namespace
  {
    void DrawCustomLineList(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                            const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const float leftPxf = dstPositionPxf.X.Value;
      const float topPxf = dstPositionPxf.Y.Value;
      const float rightPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth());
      const float bottomPxf = topPxf + static_cast<float>(dstSizePx.RawHeight());
      // NOLINTNEXTLINE(bugprone-integer-division)
      const float middleXPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth() / 2);

      const float x0 = leftPxf;
      const float x1 = middleXPxf;
      const float x2 = rightPxf;

      const float y0 = bottomPxf;
      const float y1 = topPxf;
      const float y2 = bottomPxf;

      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Red()));
      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Green()));

      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Green()));
      rBuilder.AddVertex(x2, y2, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Blue()));

      rBuilder.AddVertex(x2, y2, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Blue()));
      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Red()));
    }

    void DrawCustomLineStrip(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                             const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const float leftPxf = dstPositionPxf.X.Value;
      const float topPxf = dstPositionPxf.Y.Value;
      const float rightPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth());
      const float bottomPxf = topPxf + static_cast<float>(dstSizePx.RawHeight());
      // NOLINTNEXTLINE(bugprone-integer-division)
      const float middleXPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth() / 2);

      const float x0 = leftPxf;
      const float x1 = middleXPxf;
      const float x2 = rightPxf;

      const float y0 = bottomPxf;
      const float y1 = topPxf;
      const float y2 = bottomPxf;

      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Red()));
      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Green()));
      rBuilder.AddVertex(x2, y2, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Blue()));
      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Red()));
    }

    void DrawCustomTriangle(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                            const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const float leftPxf = dstPositionPxf.X.Value;
      const float topPxf = dstPositionPxf.Y.Value;
      const float rightPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth());
      const float bottomPxf = topPxf + static_cast<float>(dstSizePx.RawHeight());
      // NOLINTNEXTLINE(bugprone-integer-division)
      const float middleXPxf = leftPxf + static_cast<float>(dstSizePx.RawWidth() / 2);

      const float x0 = leftPxf;
      const float x1 = middleXPxf;
      const float x2 = rightPxf;

      const float y0 = bottomPxf;
      const float y1 = topPxf;
      const float y2 = bottomPxf;

      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Red()));
      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Green()));
      rBuilder.AddVertex(x2, y2, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0, UIRenderColor(Colors::Blue()));
    }
  }


  PrimitiveTopologyTest::PrimitiveTopologyTest(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_content(context->TheUIContext.Get()->MeshManager, 6)
    , m_propertyContentColor(context->ColorConverter, UIColors::White())
  {
    Enable(WindowFlags::DrawEnabled);
  }


  PrimitiveTopologyTest::~PrimitiveTopologyTest() = default;


  void PrimitiveTopologyTest::SetContent(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      m_contentPrimitiveTopology = m_content.GetPrimitiveTopology();
      PropertyUpdated(PropertyType::Content);
    }
  }


  void PrimitiveTopologyTest::SetContent(std::shared_ptr<BasicImageSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      m_contentPrimitiveTopology = m_content.GetPrimitiveTopology();
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool PrimitiveTopologyTest::SetContentColor(const UIColor value)
  {
    const bool changed = m_propertyContentColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }


  void PrimitiveTopologyTest::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    const UIRenderColor finalBaseColor(GetFinalBaseColor());

    if (m_content.IsValid())
    {
      const auto finalContentColor = finalBaseColor * m_propertyContentColor.InternalColor;


      // Schedule the rendering
      switch (m_contentPrimitiveTopology)
      {
      case BasicPrimitiveTopology::LineList:
        context.CommandBuffer.DrawCustom(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), finalContentColor, DrawCustomLineList, {});
        break;
      case BasicPrimitiveTopology::LineStrip:
        context.CommandBuffer.DrawCustom(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), finalContentColor, DrawCustomLineStrip, {});
        break;
      case BasicPrimitiveTopology::TriangleList:
        context.CommandBuffer.DrawCustom(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), finalContentColor, DrawCustomTriangle, {});
        break;
      }
    }
  }


  PxSize2D PrimitiveTopologyTest::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return base_type::ArrangeOverride(finalSizePx);
  }


  PxSize2D PrimitiveTopologyTest::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    return base_type::MeasureOverride(availableSizePx);
  }


  DataBinding::DataBindingInstanceHandle PrimitiveTopologyTest::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                            PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult PrimitiveTopologyTest::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                                const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                     PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void PrimitiveTopologyTest::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyContentColor);
  }
}
