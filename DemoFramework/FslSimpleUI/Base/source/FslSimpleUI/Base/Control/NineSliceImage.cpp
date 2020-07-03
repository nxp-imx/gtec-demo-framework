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

#include <FslSimpleUI/Base/Control/NineSliceImage.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <utility>
#include "Impl/ImageImpl_NineSliceSprite.hpp"

namespace Fsl
{
  namespace UI
  {
    NineSliceImage::NineSliceImage(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
    {
      Enable(WindowFlags::DrawEnabled);
    }


    void NineSliceImage::SetContent(const std::shared_ptr<NineSliceSprite>& value)
    {
      if (value != m_content)
      {
        m_content = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void NineSliceImage::SetContent(std::shared_ptr<NineSliceSprite>&& value)
    {
      if (value != m_content)
      {
        m_content = std::move(value);
        PropertyUpdated(PropertyType::Content);
      }
    }


    void NineSliceImage::SetContentColor(const Color& value)
    {
      if (value != m_contentColor)
      {
        m_contentColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void NineSliceImage::SetRotateImageCW(const bool enabled)
    {
      if (enabled != m_rotate90)
      {
        m_rotate90 = enabled;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void NineSliceImage::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);

      if (!m_rotate90)
      {
        ImageImpl::Draw(*m_windowContext->Batch2D, m_content.get(), context.TargetRect.TopLeft(), RenderSizePx(), m_contentColor);
      }
      else
      {
        ImageImpl::DrawRotated90CW(*m_windowContext->Batch2D, m_content.get(), context.TargetRect.TopLeft(), RenderSizePx(), m_contentColor);
      }
    }


    PxSize2D NineSliceImage::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return ImageImpl::ArrangeOverride(finalSizePx, m_content.get(), m_rotate90);
    }


    PxSize2D NineSliceImage::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      return ImageImpl::MeasureOverride(availableSizePx, m_content.get(), m_rotate90);
    }
  }
}
