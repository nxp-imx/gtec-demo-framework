/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>
#include <utility>

namespace Fsl::UI
{
  using TClass = Image;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyContentColor = TFactory::Create<UIColor, TClass, &TClass::GetContentColor, &TClass::SetContentColor>("Color");
  TDef TClass::PropertyScalePolicy = TFactory::Create<ItemScalePolicy, TClass, &TClass::GetScalePolicy, &TClass::SetScalePolicy>("ScalePolicy");
  TDef TClass::PropertyRotateImageCW = TFactory::Create<bool, TClass, &TClass::GetRotateImageCW, &TClass::SetRotateImageCW>("RotateImageCW");
}

namespace Fsl::UI
{
  Image::Image(const std::shared_ptr<WindowContext>& context)
    : BaseWindow(context)
    , m_windowContext(context)
    , m_content(context->TheUIContext.Get()->MeshManager)
    , m_propertyContentColor(context->ColorConverter, UIColors::White())
  {
    Enable(WindowFlags::DrawEnabled);
  }


  void Image::SetContent(const std::shared_ptr<ISizedSprite>& value)
  {
    if (m_content.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  void Image::SetContent(std::shared_ptr<ISizedSprite>&& value)
  {
    if (m_content.SetSprite(std::move(value)))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }


  bool Image::SetContentColor(const UIColor value)
  {
    const bool changed = m_propertyContentColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  bool Image::SetScalePolicy(const ItemScalePolicy value)
  {
    const bool changed = m_propertyScalePolicy.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::ScalePolicy);
    }
    return changed;
  }


  bool Image::SetRotateImageCW(const bool value)
  {
    const bool changed = m_propertyRotateImageCW.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Content);
    }
    return changed;
  }


  void Image::WinDraw(const UIDrawContext& context)
  {
    BaseWindow::WinDraw(context);

    if (!m_propertyRotateImageCW.Get())
    {
      context.CommandBuffer.Draw(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), GetFinalBaseColor() * GetContentInternalColor(),
                                 context.ClipContext);
    }
    else
    {
      context.CommandBuffer.DrawRotated90CW(m_content.Get(), context.TargetRect.Location(), RenderSizePx(),
                                            GetFinalBaseColor() * GetContentInternalColor(), context.ClipContext);
    }
  }


  PxSize2D Image::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return m_content.Measure(finalSizePx, m_propertyScalePolicy.Get(), m_propertyRotateImageCW.Get());
  }


  PxSize2D Image::MeasureOverride(const PxAvailableSize& /*availableSizePx*/)
  {
    PxSize2D desiredSizePx = m_content.Measure();
    return !m_propertyRotateImageCW.Get() ? desiredSizePx : PxSize2D::Flip(desiredSizePx);
  }


  DataBinding::DataBindingInstanceHandle Image::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor),
      PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy), PropLinkRefs(PropertyRotateImageCW, m_propertyRotateImageCW));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult Image::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(PropertyContentColor, m_propertyContentColor.ExternalColor),
      PropLinkRefs(PropertyScalePolicy, m_propertyScalePolicy), PropLinkRefs(PropertyRotateImageCW, m_propertyRotateImageCW));
    return res != PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void Image::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyContentColor);
    rProperties.push_back(PropertyScalePolicy);
    rProperties.push_back(PropertyRotateImageCW);
  }
}
