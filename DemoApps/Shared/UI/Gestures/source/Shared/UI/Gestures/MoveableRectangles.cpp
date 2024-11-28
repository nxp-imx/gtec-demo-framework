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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragBeginBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureDragEndBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureFlickBasicEvent.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureTapBasicEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/ResolutionChangedInfo.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <Shared/UI/Gestures/MoveableRectangles.hpp>


namespace Fsl::UI
{
  using TClass = MoveableRectangles;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyDragFlickDeceleration =
    TFactory::Create<float, TClass, &TClass::GetDragFlickDeceleration, &TClass::SetDragFlickDeceleration>("DragFlickDeceleration");
  TDef TClass::PropertyDragEndAnimTimeMod =
    TFactory::Create<float, TClass, &TClass::GetDragEndAnimTimeMod, &TClass::SetDragEndAnimTimeMod>("DragEndAnimTimeMod");
}

namespace Fsl::UI
{
  namespace
  {
    std::array<SizedSpriteMesh, 3> CreateMeshes(const std::shared_ptr<IMeshManager>& meshManager)
    {
      return {SizedSpriteMesh(meshManager), SizedSpriteMesh(meshManager), SizedSpriteMesh(meshManager)};
    }
  }

  MoveableRectangles::MoveableRectangles(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_gestureManager(UI::GestureFlags::Drag | UI::GestureFlags::Tap, UI::GestureDetector(UI::GestureFlags::Everything),
                       context->UnitConverter.GetDensityDpi())
    , m_moveableRectangleManager(context->ColorConverter.GetColorSpace(), PxSize2D::Create(1, 1), context->UnitConverter.GetDensityDpi())
    , m_meshes(CreateMeshes(context->TheUIContext.Get()->MeshManager))
  {
    m_gestureManager.SetEnabled(true);

    SetAlignmentX(UI::ItemAlignment::Stretch);
    SetAlignmentY(UI::ItemAlignment::Stretch);

    Enable(WindowFlags(WindowFlags::ClickInput | WindowFlags::DrawEnabled));
  }

  void MoveableRectangles::SetFillSprite(const std::shared_ptr<ISizedSprite>& value)
  {
    if (value != m_meshes[0].GetSprite())
    {
      for (auto& rEntry : m_meshes)
      {
        rEntry.SetSprite(value);
      }
      PropertyUpdated(PropertyType::Content);
    }
  }


  void MoveableRectangles::WinResolutionChanged(const ResolutionChangedInfo& info)
  {
    BaseWindow::WinResolutionChanged(info);
    m_moveableRectangleManager.OnConfigurationChanged(GetContext()->ColorConverter.GetColorSpace(), info.DensityDpi);
  }


  void MoveableRectangles::OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    const auto localPositionPx = PointFromScreen(theEvent->GetScreenPosition());

    if (theEvent->IsBegin())
    {
      m_isDown = true;
    }
    else if (m_isDown)
    {
      m_isDown = false;
    }

    m_gestureManager.AddMovement(theEvent->GetTimestamp(), localPositionPx, theEvent->GetState(), theEvent->IsRepeat(), MovementOwnership::Unhandled);
    theEvent->Handled();
  }


  void MoveableRectangles::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    if (m_meshes.front().IsValid())
    {
      const auto span = m_moveableRectangleManager.GetDrawInfoSpan();
      for (std::size_t i = 0; i < span.size(); ++i)
      {
        const auto& entry = span[i];
        const PxVector2 dstPositionPxf = context.TargetRect.Location() + entry.RectanglePxf.Offset;
        context.CommandBuffer.Draw(m_meshes[i].Get(), dstPositionPxf, entry.RectanglePxf.Size, GetFinalBaseColor() * entry.RenderColor,
                                   context.ClipContext);
      }
    }
  }


  void MoveableRectangles::SetToDefaults()
  {
    m_moveableRectangleManager.SetToDefaults();
    PropertyUpdated(PropertyType::Content);
  }


  float MoveableRectangles::GetDragFlickDeceleration() const noexcept
  {
    return m_propertyDragFlickDeceleration.Get();
  }

  bool MoveableRectangles::SetDragFlickDeceleration(const float value) noexcept
  {
    const bool changed = m_propertyDragFlickDeceleration.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_moveableRectangleManager.SetDragFlickDeceleration(DpValueF::Create(value * 100.0f));
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  float MoveableRectangles::GetDragEndAnimTimeMod() const noexcept
  {
    return m_propertyDragEndAnimTimeMod.Get();
  }


  bool MoveableRectangles::SetDragEndAnimTimeMod(const float value) noexcept
  {
    const bool changed = m_propertyDragEndAnimTimeMod.Set(ThisDependencyObject(), value);
    if (changed)
    {
      m_moveableRectangleManager.SetDragEndAnimTimeMod(value);
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  PxSize2D MoveableRectangles::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    const auto densityDpi = GetContext()->UnitConverter.GetDensityDpi();
    m_gestureManager.SetDpi(densityDpi);
    m_moveableRectangleManager.Arrange(finalSizePx);

    return BaseWindow::ArrangeOverride(finalSizePx);
  }


  void MoveableRectangles::UpdateAnimation(const TimeSpan& timeSpan)
  {
    m_gestureManager.Process();
    UI::GestureBasicEvent currentEvent;
    while (m_gestureManager.TryGet(currentEvent))
    {
      switch (currentEvent.Type)
      {
      case UI::GestureEventType::Nop:
        FSLLOG3_INFO("Nop");
        break;
      case UI::GestureEventType::GestureDragBegin:
        {
          const auto typedEvent = UI::GestureDragBeginBasicEvent::Decode(currentEvent);
          const bool found = m_moveableRectangleManager.TryBeginDrag(typedEvent.PositionPx);
          FSLLOG3_INFO_IF(found, "GestureDragBegin PositionPx {}", typedEvent.PositionPx);
        }
        break;
      case UI::GestureEventType::GestureDrag:
        {
          const auto typedEvent = UI::GestureDragBasicEvent::Decode(currentEvent);
          FSLLOG3_VERBOSE_IF(m_moveableRectangleManager.IsDragging(), "GestureDrag PositionPx {}", typedEvent.PositionPx);
          m_moveableRectangleManager.TryDrag(typedEvent.PositionPx);
          break;
        }
      case UI::GestureEventType::GestureDragEnd:
        {
          const auto typedEvent = UI::GestureDragEndBasicEvent::Decode(currentEvent);

          FSLLOG3_INFO_IF(m_moveableRectangleManager.IsDragging(), "GestureDragEnd PositionPx {} FlickVelocityDp {}", typedEvent.PositionPx,
                          typedEvent.FlickVelocityDpf);
          m_moveableRectangleManager.TryDragEnd(typedEvent.PositionPx, typedEvent.FlickVelocityDpf);
          break;
        }
      case UI::GestureEventType::GestureFlick:
        {
          const auto typedEvent = UI::GestureFlickBasicEvent::Decode(currentEvent);
          FSLLOG3_INFO("GestureDragFlick PositionPx {} VelocityDp {}", typedEvent.PositionPx, typedEvent.VelocityDpf);
          break;
        }
      case UI::GestureEventType::GestureTap:
        {
          const auto typedEvent = UI::GestureTapBasicEvent::Decode(currentEvent);
          FSLLOG3_INFO("GestureTap PositionPx {}", typedEvent.PositionPx);
          break;
        }
      default:
        FSLLOG3_WARNING("Unknown event type: {}", fmt::underlying(currentEvent.Type));
        break;
      }
    }
    m_moveableRectangleManager.Process(timeSpan);
  }


  bool MoveableRectangles::UpdateAnimationState(const bool forceCompleteAnimation)
  {
    return m_moveableRectangleManager.IsAnimating() || !m_gestureManager.IsIdle();
  }

  DataBinding::DataBindingInstanceHandle MoveableRectangles::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyDragFlickDeceleration, m_propertyDragFlickDeceleration),
      DataBinding::PropLinkRefs(PropertyDragEndAnimTimeMod, m_propertyDragEndAnimTimeMod));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult MoveableRectangles::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                             const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyDragFlickDeceleration, m_propertyDragFlickDeceleration),
      DataBinding::PropLinkRefs(PropertyDragEndAnimTimeMod, m_propertyDragEndAnimTimeMod));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }

  void MoveableRectangles::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyDragFlickDeceleration);
    rProperties.push_back(PropertyDragEndAnimTimeMod);
  }
}
