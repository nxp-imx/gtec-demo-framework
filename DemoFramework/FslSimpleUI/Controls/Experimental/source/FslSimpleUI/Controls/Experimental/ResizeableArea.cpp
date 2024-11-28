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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Controls/Experimental/ResizeableArea.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Render/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <cassert>
#include <utility>

namespace Fsl::UI
{
  using TClass = ResizeableArea;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyEnabled = TFactory::Create<bool, TClass, &TClass::GetEnabled, &TClass::SetEnabled>("Enabled");
  TDef TClass::PropertyDragHandleLocation =
    TFactory::Create<ResizeableAreaDragHandleLocation, TClass, &TClass::GetDragHandleLocation, &TClass::SetDragHandleLocation>("DragHandleLocation");
  TDef TClass::PropertyDragHandleSizeDpf =
    TFactory::Create<DpSize1DF, TClass, &TClass::GetDragHandleSizeDpf, &TClass::SetDragHandleSizeDpf>("DragHandleSizeDpf");
  TDef TClass::PropertyDragHandleColor =
    TFactory::Create<UIColor, TClass, &TClass::GetDragHandleColor, &TClass::SetDragHandleColor>("DragHandleColor");
  TDef TClass::PropertyRectangleColor = TFactory::Create<UIColor, TClass, &TClass::GetRectangleColor, &TClass::SetRectangleColor>("RectangleColor");
}

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr DpSize1DF DefaultDragHandleSizeDpf = DpSize1DF::Create(16);
    }

    constexpr PxSize1D EnsureOddSize(const PxSize1D value) noexcept
    {
      return (value.RawValue() & 1) != 0 ? value : value + PxSize1D::Create(1);
    }


    void DrawCustom(UIRawBasicMeshBuilder2D& rBuilder, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const DrawClipContext& clipContext,
                    const RenderBasicImageInfo& renderInfo, const ICustomDrawData* const pCustomDrawData)
    {
      const auto* const pRenderData = dynamic_cast<const ResizeableAreaRenderData*>(pCustomDrawData);
      if (pRenderData != nullptr)
      {
        const PxValue rectLPx = pRenderData->ClipRectanglePx.Left();
        const PxValue rectRPx = pRenderData->ClipRectanglePx.Right();
        const PxValue rectTPx = pRenderData->ClipRectanglePx.Top();
        const PxValue rectBPx = pRenderData->ClipRectanglePx.Bottom();

        // const UIRenderColor color(rBuilder.GetColor());

        constexpr auto OnePx = PxValue::Create(1);
        const auto finalDstPxf = pRenderData->RenderOffsetPxf;

        rBuilder.AddRect(finalDstPxf, rectLPx, rectTPx, rectRPx, rectTPx + OnePx, renderInfo.TextureArea);    // Top line
        rBuilder.AddRect(finalDstPxf, rectLPx, rectBPx - OnePx, rectRPx, rectBPx, renderInfo.TextureArea);    // Bottom line
        rBuilder.AddRect(finalDstPxf, rectLPx, rectTPx, rectLPx + OnePx, rectBPx, renderInfo.TextureArea);    // Left line
        rBuilder.AddRect(finalDstPxf, rectRPx - OnePx, rectTPx, rectRPx, rectBPx, renderInfo.TextureArea);    // Right line

        if (pRenderData->ShowRenderHandles)
        {
          auto oldColor = rBuilder.GetColor();
          rBuilder.SetColor(pRenderData->DragHandleColor);

          for (const auto& drag : pRenderData->DragHandles)
          {
            rBuilder.AddRect(finalDstPxf, drag.Rectangle, renderInfo.TextureArea);
          }

          rBuilder.SetColor(oldColor);
        }
      }
    }
  }


  ResizeableArea::ResizeableArea(const std::shared_ptr<WindowContext>& context)
    : BaseWindow(context)
    , m_fill(context->TheUIContext.Get()->MeshManager, (3 * 2) * (4 + 4))
    , m_propertyDragHandleSizeDpf(LocalConfig::DefaultDragHandleSizeDpf)
    , m_propertyDragHandleColor(context->ColorConverter, UIColors::White())
    , m_propertyRectangleColor(context->ColorConverter, UIColors::White())
    , m_renderData(std::make_shared<ResizeableAreaRenderData>())

  {
    SetAlignmentX(ItemAlignment::Stretch);
    SetAlignmentY(ItemAlignment::Stretch);
    UpdateCachedDragHandleSizePx();

    Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput | WindowFlags::MouseOver));
  }

  ResizeableArea::~ResizeableArea() = default;


  void ResizeableArea::SetFillSprite(const std::shared_ptr<BasicImageSprite>& value)
  {
    if (m_fill.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  void ResizeableArea::SetFillSprite(std::shared_ptr<BasicImageSprite>&& value)
  {
    if (m_fill.SetSprite(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool ResizeableArea::SetEnabled(const bool value)
  {
    const bool changed = m_propertyEnabled.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_logic.SetEnabled(value);
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  bool ResizeableArea::SetDragHandleLocation(const ResizeableAreaDragHandleLocation value)
  {
    const bool changed = m_propertyDragHandleLocation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  bool ResizeableArea::SetDragHandleSizeDpf(const DpSize1DF value)
  {
    const bool changed = m_propertyDragHandleSizeDpf.Set(ThisDependencyObject(), value);
    if (changed)
    {
      UpdateCachedDragHandleSizePx();
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  bool ResizeableArea::SetDragHandleColor(const UIColor value)
  {
    const bool changed = m_propertyDragHandleColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  bool ResizeableArea::SetRectangleColor(const UIColor value)
  {
    const bool changed = m_propertyRectangleColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ContentDraw);
    }
    return changed;
  }


  void ResizeableArea::WinResolutionChanged(const ResolutionChangedInfo& info)
  {
    BaseWindow::WinResolutionChanged(info);
    UpdateCachedDragHandleSizePx();
  }


  void ResizeableArea::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    const UIRenderColor finalBaseColor(GetFinalBaseColor());
    if (m_fill.IsValid())
    {
      const auto finalContentColor = finalBaseColor * GetRectangleRenderColor();

      m_renderData->SetRenderData(context.TargetRect.Location(), m_logic.GetAreaRectangle(), m_cachedDragHandleSizePx,
                                  m_propertyDragHandleLocation.Get(), m_logic.IsEnabled(), m_propertyDragHandleColor.InternalColor);
      const PxRectangle renderRectanglePx = m_renderData->RenderRectanglePx;

      const PxVector2 dstPositionPxf(m_renderData->RenderOffsetPxf + TypeConverter::To<PxVector2>(renderRectanglePx.Location()));
      context.CommandBuffer.DrawCustom(m_fill.Get(), dstPositionPxf, renderRectanglePx.GetSize(), finalContentColor, context.ClipContext, DrawCustom,
                                       m_renderData);
    }
  }


  void ResizeableArea::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    BaseWindow::OnClickInput(theEvent);
    if (theEvent->IsHandled())
    {
      return;
    }

    if (!m_logic.IsDragging())
    {
      if (theEvent->GetState() == EventTransactionState::Begin && !theEvent->IsRepeat())
      {
        const PxPoint2 posPx = PointFromScreen(theEvent->GetScreenPosition());
        const int32_t hitIndex = m_renderData->TryGetFindHitIndex(posPx);
        if (hitIndex >= 0 && m_logic.TryBeginDrag(posPx, m_renderData->DragHandles[hitIndex].Corner))
        {
          theEvent->Handled();
        }
      }
    }
    else
    {
      const PxPoint2 posPx = PointFromScreen(theEvent->GetScreenPosition());
      switch (theEvent->GetState())
      {
      case EventTransactionState::Begin:
        m_logic.TryDrag(posPx);
        break;
      case EventTransactionState::End:
        m_logic.EndDrag(posPx);
        break;
      case EventTransactionState::Canceled:
        m_logic.CancelDrag();
        break;
      }
      theEvent->Handled();
    }
  }

  void ResizeableArea::OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    BaseWindow::OnMouseOver(theEvent);
    if (theEvent->IsHandled())
    {
      return;
    }

    PxPoint2 posPx = PointFromScreen(theEvent->GetScreenPosition());
    const int32_t hitIndex = m_renderData->TryGetFindHitIndex(posPx);
    if (hitIndex < 0)
    {
      return;
    }

    theEvent->Handled();
    const bool isBegin = theEvent->GetState() == EventTransactionState::Begin;
    if (m_renderData->DragHandles[hitIndex].Hover != isBegin)
    {
      m_renderData->DragHandles[hitIndex].Hover = isBegin;
    }
  }

  PxSize2D ResizeableArea::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    m_logic.SetClampAreaRectangle(PxRectangle(PxPoint2(), finalSizePx));
    return base_type::ArrangeOverride(finalSizePx);
  }


  PxSize2D ResizeableArea::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    return base_type::MeasureOverride(availableSizePx);
  }


  void ResizeableArea::UpdateCachedDragHandleSizePx()
  {
    PxSize1D handleSizePx = GetContext()->UnitConverter.ToPxSize1D(m_propertyDragHandleSizeDpf.Get());
    switch (m_propertyDragHandleLocation.Get())
    {
    case ResizeableAreaDragHandleLocation::Centered:
      handleSizePx = EnsureOddSize(handleSizePx);
      break;
    case ResizeableAreaDragHandleLocation::Inside:
      break;
    }
    m_cachedDragHandleSizePx = handleSizePx;
  }


  DataBinding::DataBindingInstanceHandle ResizeableArea::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef, PropLinkRefs(PropertyEnabled, m_propertyEnabled),
                                                            PropLinkRefs(PropertyDragHandleLocation, m_propertyDragHandleLocation),
                                                            PropLinkRefs(PropertyDragHandleSizeDpf, m_propertyDragHandleSizeDpf),
                                                            PropLinkRefs(PropertyDragHandleColor, m_propertyDragHandleColor.ExternalColor),
                                                            PropLinkRefs(PropertyRectangleColor, m_propertyRectangleColor.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ResizeableArea::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                         const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(PropertyEnabled, m_propertyEnabled),
      PropLinkRefs(PropertyDragHandleLocation, m_propertyDragHandleLocation), PropLinkRefs(PropertyDragHandleSizeDpf, m_propertyDragHandleSizeDpf),
      PropLinkRefs(PropertyDragHandleColor, m_propertyDragHandleColor.ExternalColor),
      PropLinkRefs(PropertyRectangleColor, m_propertyRectangleColor.ExternalColor));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ResizeableArea::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyEnabled);
    rProperties.push_back(PropertyDragHandleLocation);
    rProperties.push_back(PropertyDragHandleSizeDpf);
    rProperties.push_back(PropertyDragHandleColor);
    rProperties.push_back(PropertyRectangleColor);
  }
}
