#ifndef FSLSIMPLEUI_BASE_CONTROL_SLIDER_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SLIDER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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

#include <FslSimpleUI/Base/Control/Impl/SliderRenderImpl.hpp>
#include <FslSimpleUI/Base/Control/SliderBase.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowFlags.hpp>

namespace Fsl::UI
{
  class WindowContext;

  template <typename T>
  class Slider final : public SliderBase<T>
  {
    using base_type = SliderBase<T>;

    SliderRenderImpl m_impl;

  public:
    explicit Slider(const std::shared_ptr<WindowContext>& context)
      : SliderBase<T>(context)
      , m_impl(context->ColorConverter, context->TheUIContext.Get()->MeshManager)
    {
      // We need to be draw enabled, accept click input and receive a notification on init
      this->Enable(WindowFlags(WindowFlags::DrawEnabled | WindowFlags::ClickInput | WindowFlags::MouseOver));
    }


    bool GetEnableVerticalGraphicsRotation() const
    {
      return m_impl.GetEnableVerticalGraphicsRotation();
    }

    void SetEnableVerticalGraphicsRotation(const bool enabled)
    {
      if (m_impl.SetEnableVerticalGraphicsRotation(enabled))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    const std::shared_ptr<ISizedSprite>& GetCursorSprite() const
    {
      return m_impl.GetCursorSprite();
    }

    void SetCursorSprite(const std::shared_ptr<ISizedSprite>& value)
    {
      if (m_impl.SetCursorSprite(value))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    DpPoint2 GetCursorOrigin() const
    {
      return m_impl.GetCursorOrigin();
    }

    void SetCursorOrigin(const DpPoint2 value)
    {
      if (m_impl.SetCursorOrigin(value))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    DpSize2D GetCursorSize() const
    {
      return m_impl.GetCursorSize();
    }

    void SetCursorSize(const DpSize2D value)
    {
      if (m_impl.SetCursorSize(value))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    UIColor GetCursorColor() const
    {
      return m_impl.GetCursorColor();
    }

    void SetCursorColor(const UIColor value)
    {
      if (m_impl.SetCursorColor(value))
      {
        this->PropertyUpdated(PropertyType::Other);
      }
    }

    UIColor GetCursorDisabledColor() const
    {
      return m_impl.GetCursorDisabledColor();
    }

    void SetCursorDisabledColor(const UIColor value)
    {
      if (m_impl.SetCursorDisabledColor(value))
      {
        this->PropertyUpdated(PropertyType::Other);
      }
    }

    // ------

    const std::shared_ptr<ISizedSprite>& GetCursorOverlaySprite() const
    {
      return m_impl.GetCursorOverlaySprite();
    }

    void SetCursorOverlaySprite(const std::shared_ptr<ISizedSprite>& value)
    {
      if (m_impl.SetCursorOverlaySprite(value))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    UIColor GetCursorOverlayColor() const
    {
      return m_impl.GetCursorOverlayColor();
    }

    void SetCursorOverlayColor(const UIColor value)
    {
      if (m_impl.SetCursorOverlayColor(value))
      {
        this->PropertyUpdated(PropertyType::Other);
      }
    }

    // ------

    const std::shared_ptr<IContentSprite>& GetBackgroundSprite() const
    {
      return m_impl.GetBackgroundSprite();
    }

    void SetBackgroundSprite(const std::shared_ptr<IContentSprite>& value)
    {
      if (m_impl.SetBackgroundSprite(value))
      {
        this->PropertyUpdated(PropertyType::Content);
      }
    }

    UIColor GetBackgroundColor() const
    {
      return m_impl.GetBackgroundColor();
    }

    void SetBackgroundColor(const UIColor value)
    {
      if (m_impl.SetBackgroundColor(value))
      {
        this->PropertyUpdated(PropertyType::Other);
      }
    }

    UIColor GetBackgroundDisabledColor() const
    {
      return m_impl.GetBackgroundDisabledColor();
    }

    void SetBackgroundDisabledColor(const UIColor value)
    {
      if (m_impl.SetBackgroundDisabledColor(value))
      {
        this->PropertyUpdated(PropertyType::Other);
      }
    }

    // ------

    void WinDraw(const UIDrawContext& context) final
    {
      SliderBase<T>::WinDraw(context);

      m_impl.Draw(context.CommandBuffer, context.TargetRect.Location(), this->GetFinalBaseColor(), this->GetCursorPositionPx(), this->IsDragging(),
                  context.ClipContext, this->m_windowContext->UnitConverter);
    }

  protected:
    void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) final
    {
      m_impl.OnMouseOver(theEvent, this->IsEnabled());
    }

    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final
    {
      return m_impl.Measure(availableSizePx);
    }

    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final
    {
      auto spanInfo = m_impl.Arrange(finalSizePx, this->GetOrientation(), this->GetDirection(), this->m_windowContext->UnitConverter);
      this->SetSpanInfo(spanInfo);
      return SliderBase<T>::ArrangeOverride(finalSizePx);
    }

    void UpdateAnimation(const TimeSpan& timeSpan) final
    {
      SliderBase<T>::UpdateAnimation(timeSpan);
      m_impl.UpdateAnimation(timeSpan);
    }

    bool UpdateAnimationState(const bool forceCompleteAnimation) final
    {
      return m_impl.UpdateAnimationState(forceCompleteAnimation, this->IsEnabled(), this->IsDragging());
    }
  };
}

#endif
