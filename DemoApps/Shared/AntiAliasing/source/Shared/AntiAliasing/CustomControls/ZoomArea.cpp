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
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <FslSimpleUI/Render/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <Shared/AntiAliasing/CustomControls/ZoomArea.hpp>

namespace Fsl::UI::Custom
{
  using TClass = ZoomArea;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyContentColor = TFactory::Create<UIColor, TClass, &TClass::GetContentColor, &TClass::SetContentColor>("Color");
  TDef TClass::PropertyAreaWidth = TFactory::Create<DpSize1DF, TClass, &TClass::GetAreaWidth, &TClass::SetAreaWidth>("AreaWidth");
  TDef TClass::PropertyAreaHeight = TFactory::Create<DpSize1DF, TClass, &TClass::GetAreaHeight, &TClass::SetAreaHeight>("AreaHeight");
  TDef TClass::PropertyZoomFactor = TFactory::Create<float, TClass, &TClass::GetZoomFactor, &TClass::SetZoomFactor>("ZoomFactor");

  namespace
  {
    void DrawCustomLineList(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx,
                            const DrawClipContext& clipContext, const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const float x0 = dstPositionPxf.X.Value;
      const float x1 = x0 + static_cast<float>(dstSizePx.RawWidth() - 1);

      const float y0 = dstPositionPxf.Y.Value;
      const float y1 = y0 + static_cast<float>(dstSizePx.RawHeight() - 1);

      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
      rBuilder.AddVertex(x1, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);

      rBuilder.AddVertex(x1, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);

      rBuilder.AddVertex(x1, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
      rBuilder.AddVertex(x0, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);

      rBuilder.AddVertex(x0, y1, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
      rBuilder.AddVertex(x0, y0, renderInfo.TextureArea.X0, renderInfo.TextureArea.Y0);
    }
  }


  ZoomArea::ZoomArea(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_content(context->TheUIContext.Get()->MeshManager, 8)
    , m_propertyContentColor(context->ColorConverter, UIColors::White())
    , m_propertyZoomFactor(1.0f)
    , m_zoomFactorAnim(TimeSpan::FromMilliseconds(100), TransitionType::EaseOutSine)
    , m_zoomBorderColor(TimeSpan::FromMilliseconds(100), TransitionType::EaseOutSine)
  {
    m_zoomFactorAnim.SetActualValue(1.0f);
    m_zoomBorderColor.SetActualValue(context->ColorConverter.Convert(UIColors::Transparent()));
    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput | WindowFlags::ResolveEnabled));
  }


  ZoomArea::~ZoomArea() = default;


  void ZoomArea::SetContent(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      if (m_content.GetPrimitiveTopology() != BasicPrimitiveTopology::LineList)
      {
        throw UsageErrorException("BasicPrimitiveTopology must be a LineList");
      }
      PropertyUpdated(PropertyType::Content);
    }
  }


  void ZoomArea::SetContent(std::shared_ptr<BasicImageSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      if (m_content.GetPrimitiveTopology() != BasicPrimitiveTopology::LineList)
      {
        throw UsageErrorException("BasicPrimitiveTopology must be a LineList");
      }
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool ZoomArea::SetContentColor(const UIColor value)
  {
    const bool changed = m_propertyContentColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool ZoomArea::SetAreaWidth(const DpSize1DF value)
  {
    const bool changed = m_propertyAreaWidth.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ZoomArea::SetAreaHeight(const DpSize1DF value)
  {
    const bool changed = m_propertyAreaHeight.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ZoomArea::SetZoomFactor(const float value)
  {
    const bool changed = m_propertyZoomFactor.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  bool ZoomArea::IsZoomEnabled() const
  {
    return m_isDown || !m_zoomFactorAnim.IsCompleted() || !m_zoomBorderColor.IsCompleted();
  }

  PxRectangle ZoomArea::GetResolvedArea() const
  {
    return {m_cachedClampedOffsetPositionPx, m_cachedAreaSizePx};
  }


  PxRectangle ZoomArea::GetResolvedAreaInScreenSpace() const
  {
    return {PointToScreen(m_cachedClampedOffsetPositionPx), m_cachedAreaSizePx};
  }


  ZoomInfo ZoomArea::GetResolvedZoomInfo() const
  {
    auto area = GetResolvedArea();
    return {IsZoomEnabled(), GetResolvedAreaInScreenSpace(), area, m_cachedClampedMousePositionPx, area.GetSize(), m_zoomFactorAnim.GetValue()};
  }

  void ZoomArea::WinResolve(const TimeSpan& timespan)
  {
    base_type::WinResolve(timespan);

    const auto renderSizePx = RenderSizePx();

    const PxSize2D halfAreaSizePx = m_cachedAreaSizePx / PxSize1D::Create(2);
    const PxPoint2 nearPositionPx = m_cachedMousePositionPx - halfAreaSizePx;
    const PxPoint2 farPositionPx = nearPositionPx + m_cachedAreaSizePx;

    {    // Clamp the mouse position to fit inside the control
      m_cachedClampedMousePositionPx = m_cachedMousePositionPx;
      if (m_cachedClampedMousePositionPx.X > renderSizePx.Width())
      {
        m_cachedClampedMousePositionPx.X = renderSizePx.Width().Value();
      }
      if (m_cachedClampedMousePositionPx.Y > renderSizePx.Height())
      {
        m_cachedClampedMousePositionPx.Y = renderSizePx.Height().Value();
      }
      if (m_cachedClampedMousePositionPx.X < PxValue(0))
      {
        m_cachedClampedMousePositionPx.X = PxValue(0);
      }
      if (m_cachedClampedMousePositionPx.Y < PxValue(0))
      {
        m_cachedClampedMousePositionPx.Y = PxValue(0);
      }
    }
    {    // Clamp the zoom area to fit inside the control
      m_cachedClampedOffsetPositionPx = nearPositionPx;
      if (farPositionPx.X > renderSizePx.Width())
      {
        m_cachedClampedOffsetPositionPx.X = renderSizePx.Width() - m_cachedAreaSizePx.Width();
      }
      if (farPositionPx.Y > renderSizePx.Height())
      {
        m_cachedClampedOffsetPositionPx.Y = renderSizePx.Height() - m_cachedAreaSizePx.Height();
      }
      if (nearPositionPx.X < PxValue(0))
      {
        m_cachedClampedOffsetPositionPx.X = PxValue(0);
      }
      if (nearPositionPx.Y < PxValue(0))
      {
        m_cachedClampedOffsetPositionPx.Y = PxValue(0);
      }
    }
  }


  void ZoomArea::WinDraw(const UIDrawContext& context)
  {
    base_type::WinDraw(context);

    const UIRenderColor finalBaseColor(GetFinalBaseColor());

    if (m_content.IsValid() && IsZoomEnabled())
    {
      const auto finalContentColor = finalBaseColor * m_zoomBorderColor.GetValue();

      // Schedule the rendering
      PxVector2 positionPxf = context.TargetRect.Location() + TypeConverter::To<PxVector2>(m_cachedClampedOffsetPositionPx);
      context.CommandBuffer.DrawCustom(m_content.Get(), positionPxf, m_cachedAreaSizePx, finalContentColor, context.ClipContext, DrawCustomLineList,
                                       {});
    }
  }


  void ZoomArea::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    if (!theEvent->IsHandled())
    {
      theEvent->Handled();
      m_isDown = theEvent->IsBegin();
      m_cachedMousePositionPx = PointFromScreen(theEvent->GetScreenPosition());
    }
    base_type::OnClickInput(theEvent);
  }


  PxSize2D ZoomArea::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    m_cachedAreaSizePx = GetContext()->UnitConverter.ToPxSize2D(DpSize2DF(m_propertyAreaWidth.Get(), m_propertyAreaHeight.Get()));
    m_cachedAreaSizePx = PxSize2D::Min(m_cachedAreaSizePx, finalSizePx);
    return base_type::ArrangeOverride(finalSizePx);
  }


  PxSize2D ZoomArea::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    return base_type::MeasureOverride(availableSizePx);
  }

  void ZoomArea::UpdateAnimation(const TimeSpan& timeSpan)
  {
    base_type::UpdateAnimation(timeSpan);
    m_zoomFactorAnim.Update(timeSpan);
    m_zoomBorderColor.Update(timeSpan);
  }


  bool ZoomArea::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    bool isAnimating = base_type::UpdateAnimationState(forceCompleteAnimation);
    m_zoomFactorAnim.SetValue(m_isDown ? m_propertyZoomFactor.Get() : 1.0f);
    m_zoomBorderColor.SetValue(m_isDown ? m_propertyContentColor.InternalColor : GetContext()->ColorConverter.Convert(UIColors::Transparent()));

    if (forceCompleteAnimation)
    {
      m_zoomFactorAnim.ForceComplete();
      m_zoomBorderColor.ForceComplete();
    }
    return isAnimating || !m_zoomFactorAnim.IsCompleted();
  }


  DataBinding::DataBindingInstanceHandle ZoomArea::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyAreaWidth, m_propertyAreaWidth), DataBinding::PropLinkRefs(PropertyAreaHeight, m_propertyAreaHeight),
      DataBinding::PropLinkRefs(PropertyZoomFactor, m_propertyZoomFactor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ZoomArea::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                   const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor),
      DataBinding::PropLinkRefs(PropertyAreaWidth, m_propertyAreaWidth), DataBinding::PropLinkRefs(PropertyAreaHeight, m_propertyAreaHeight),
      DataBinding::PropLinkRefs(PropertyZoomFactor, m_propertyZoomFactor));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ZoomArea::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyContentColor);
    rProperties.push_back(PropertyAreaWidth);
    rProperties.push_back(PropertyAreaHeight);
    rProperties.push_back(PropertyZoomFactor);
  }
}
