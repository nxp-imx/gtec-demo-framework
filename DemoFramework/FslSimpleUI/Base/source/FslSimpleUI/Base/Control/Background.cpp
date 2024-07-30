/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <cassert>

namespace Fsl::UI
{
  using TClass = Background;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyBackgroundColor =
    TFactory::Create<UIColor, TClass, &TClass::GetBackgroundColor, &TClass::SetBackgroundColor>("BackgroundColor");
}

namespace Fsl::UI
{
  Background::Background(const std::shared_ptr<WindowContext>& context)
    : ContentControl(context)
    , m_windowContext(context)
    , m_background(context->TheUIContext.Get()->MeshManager)
    , m_propertyBackgroundColor(context->ColorConverter, DefaultColor::Palette::Primary)
  {
    Enable(WindowFlags::DrawEnabled);
  }

  void Background::SetBackground(const std::shared_ptr<IContentSprite>& value)
  {
    if (m_background.SetSprite(value))
    {
      PropertyUpdated(PropertyType::Content);
    }
  }

  bool Background::SetBackgroundColor(const UIColor value)
  {
    const bool changed = m_propertyBackgroundColor.Set(GetContext()->ColorConverter, ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Other);
    }
    return changed;
  }

  void Background::WinDraw(const UIDrawContext& context)
  {
    ContentControl::WinDraw(context);

    const auto finalColor = GetFinalBaseColor() * m_propertyBackgroundColor.InternalColor;
    context.CommandBuffer.Draw(m_background.Get(), context.TargetRect.Location(), RenderSizePx(), finalColor);
  }


  PxSize2D Background::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    PxSize2D finalContentSizePx(finalSizePx);
    PxSize2D resultPx;
    if (m_background.IsValid())
    {
      // There is a background content sprite so we need to apply it
      const PxThickness& backgroundContentMarginPx = m_background.GetSpriteObject().GetRenderContentMarginPx();

      // Remove the nine-slice content margin from the final available size
      const PxSize2D contentMarginSize = backgroundContentMarginPx.Sum();
      finalContentSizePx -= contentMarginSize;

      // arrange the content
      resultPx = ContentControl::CustomArrange(finalContentSizePx, backgroundContentMarginPx.TopLeft());

      // Apply the nine-slice content margin, then measure the minimum size of the nine slice
      resultPx += contentMarginSize;

      resultPx.SetMax(m_background.Measure(finalSizePx));
    }
    else
    {
      // The simple case where we have no background nine slice
      resultPx = ContentControl::ArrangeOverride(finalSizePx);
    }
    return resultPx;
  }


  PxSize2D Background::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    PxAvailableSize availableContentSizePx(availableSizePx);
    PxSize2D desiredSizePx;
    if (m_background.IsValid())
    {
      const PxThickness& backgroundContentMarginPx = m_background.GetSpriteObject().GetRenderContentMarginPx();

      // Remove the background content margin from the available size
      const PxSize2D contentMarginSize = backgroundContentMarginPx.Sum();
      availableContentSizePx = PxAvailableSize::Subtract(availableContentSizePx, contentMarginSize);

      // Measure the content using the available content size
      desiredSizePx = ContentControl::MeasureOverride(availableContentSizePx);

      // Apply the background content margin, then measure the minimum size of the background
      desiredSizePx += contentMarginSize;
      desiredSizePx.SetMax(m_background.Measure());
    }
    else
    {
      // The simple case where we have no background nine slice
      desiredSizePx = ContentControl::MeasureOverride(availableSizePx);
    }
    return desiredSizePx;
  }

  DataBinding::DataBindingInstanceHandle Background::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(
      this, ThisDependencyObject(), sourceDef, DataBinding::PropLinkRefs(PropertyBackgroundColor, m_propertyBackgroundColor.ExternalColor));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult Background::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                     const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(
      this, ThisDependencyObject(), targetDef, binding, DataBinding::PropLinkRefs(PropertyBackgroundColor, m_propertyBackgroundColor.ExternalColor));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void Background::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyBackgroundColor);
  }

}
