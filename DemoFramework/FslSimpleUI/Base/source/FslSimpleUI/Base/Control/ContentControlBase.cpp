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
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Control/ContentControlBase.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <cassert>

namespace Fsl::UI
{
  ContentControlBase::ContentControlBase(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
    , m_isInitialized(false)
  {
    Enable(WindowFlags(WindowFlags::WinInit));
  }


  void ContentControlBase::WinInit()
  {
    assert(!m_isInitialized);
    BaseWindow::WinInit();

    m_isInitialized = true;
    if (m_content)
    {
      auto uiContext = GetContext()->TheUIContext.Get();
      uiContext->WindowManager->AddChild(this, m_content);
    }
  }

  void ContentControlBase::OnPropertiesUpdated(const PropertyTypeFlags& flags)
  {
    BaseWindow::OnPropertiesUpdated(flags);
    if (flags.IsFlagged(PropertyType::BaseColor) && m_content)
    {
      m_content->SYS_SetParentBaseColor(GetFinalBaseColor());
    }
  }

  PxSize2D ContentControlBase::GetContentDesiredSizePx() const
  {
    auto paddingPx = GetContext()->UnitConverter.ToPxThickness(m_paddingDp);
    auto paddingSizePx = paddingPx.Sum();
    return m_content ? PxSize2D::Add(m_content->DesiredSizePx(), paddingSizePx) : paddingSizePx;
  }


  void ContentControlBase::DoSetPadding(const DpThickness& valueDp)
  {
    if (valueDp != m_paddingDp)
    {
      m_paddingDp = valueDp;
      PropertyUpdated(PropertyType::Layout);
    }
  }


  void ContentControlBase::DoSetContent(const std::shared_ptr<BaseWindow>& value)
  {
    if (value == m_content)
    {
      return;
    }

    auto uiContext = GetContext()->TheUIContext.Get();
    const auto& winMgr = uiContext->WindowManager;

    // Remove the old content if any
    if (m_content)
    {
      winMgr->ScheduleClose(m_content);
      m_content.reset();
    }

    m_content = value;

    if (m_content && m_isInitialized)
    {
      m_content->SYS_SetParentBaseColor(GetFinalBaseColor());
      winMgr->AddChild(this, m_content);
    }

    PropertyUpdated(PropertyType::Content);
  }


  PxSize2D ContentControlBase::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    return CustomArrange(finalSizePx, PxPoint2());
  }


  PxSize2D ContentControlBase::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
    const auto paddingPx = unitConverter.ToPxThickness(m_paddingDp);

    PxSize2D contentDesiredSizePx;
    if (m_content)
    {
      const PxAvailableSize localAvailableSizePx(PxAvailableSize::Subtract(availableSizePx, paddingPx.Sum()));

      // Measure our content
      m_content->Measure(localAvailableSizePx);
      contentDesiredSizePx = m_content->DesiredSizePx();
    }
    return {paddingPx.SumX() + contentDesiredSizePx.Width(), paddingPx.SumY() + contentDesiredSizePx.Height()};
  }


  PxSize2D ContentControlBase::CustomArrange(const PxSize2D& finalSizePx, const PxPoint2& positionOffsetPx)
  {
    BaseWindow* const pContent = m_content.get();
    if (pContent == nullptr)
    {
      return finalSizePx;
    }
    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
    const auto paddingPx = unitConverter.ToPxThickness(m_paddingDp);
    const PxSize2D localFinalSizePx(PxSize2D::Subtract(finalSizePx, paddingPx.Sum()));

    // Arrange the control inside this one
    pContent->Arrange(PxRectangle(positionOffsetPx + paddingPx.TopLeft(), localFinalSizePx));
    return finalSizePx;
  }
}
