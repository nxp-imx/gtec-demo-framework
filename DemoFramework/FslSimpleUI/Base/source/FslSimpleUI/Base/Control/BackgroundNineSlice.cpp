/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/Base/Control/BackgroundNineSlice.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include "Impl/ImageImpl_NineSliceSprite.hpp"
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    BackgroundNineSlice::BackgroundNineSlice(const std::shared_ptr<WindowContext>& context)
      : ContentControl(context)
      , m_windowContext(context)
    {
      Enable(WindowFlags::DrawEnabled);
    }

    void BackgroundNineSlice::SetBackground(const std::shared_ptr<NineSliceSprite>& value)
    {
      if (value != m_background)
      {
        m_background = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void BackgroundNineSlice::SetBackgroundColor(const Color& value)
    {
      if (value != m_backgroundColor)
      {
        m_backgroundColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void BackgroundNineSlice::WinDraw(const UIDrawContext& context)
    {
      ContentControl::WinDraw(context);

      ImageImpl::Draw(*m_windowContext->Batch2D, m_background.get(), context.TargetRect.TopLeft(), RenderSizePx(), m_backgroundColor);
    }


    PxSize2D BackgroundNineSlice::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      PxSize2D finalContentSizePx(finalSizePx);
      const NineSliceSprite* const pSprite = m_background.get();
      PxSize2D resultPx;
      if (pSprite != nullptr)
      {
        // There is a background nine slice so we need to apply it

        // Remove the nine-slice content margin from the final available size
        const auto& info = pSprite->GetRenderInfo();
        const PxSize2D contentMarginSize = info.ScaledContentMarginPx.Sum();
        finalContentSizePx -= contentMarginSize;

        // arrange the content
        resultPx = ContentControl::CustomArrange(finalContentSizePx, info.ScaledContentMarginPx.TopLeft());

        // Apply the nine-slice content margin, then measure the minimum size of the nine slice
        resultPx += contentMarginSize;

        auto backgroundSizePx = ImageImpl::ArrangeOverride(finalSizePx, pSprite);
        resultPx.SetMax(backgroundSizePx);
      }
      else
      {
        // The simple case where we have no background nine slice
        resultPx = ContentControl::ArrangeOverride(finalSizePx);
      }
      return resultPx;
    }


    PxSize2D BackgroundNineSlice::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      PxAvailableSize availableContentSizePx(availableSizePx);
      const NineSliceSprite* const pSprite = m_background.get();
      PxSize2D desiredSizePx;
      if (pSprite != nullptr)
      {
        // Remove the nine-slice padding from the available size
        const PxSize2D contentMarginSize = pSprite->GetRenderInfo().ScaledContentMarginPx.Sum();
        availableContentSizePx = PxAvailableSize::Subtract(availableContentSizePx, contentMarginSize);

        // Measure the content using the available content size
        desiredSizePx = ContentControl::MeasureOverride(availableContentSizePx);

        // Apply the nine-slice padding, then measure the minimum size of the nine slice
        desiredSizePx += contentMarginSize;
        auto backgroundSize = ImageImpl::MeasureOverride(availableSizePx, pSprite);
        desiredSizePx.SetMax(backgroundSize);
      }
      else
      {
        // The simple case where we have no background nine slice
        desiredSizePx = ContentControl::MeasureOverride(availableSizePx);
      }
      return desiredSizePx;
    }
  }
}
