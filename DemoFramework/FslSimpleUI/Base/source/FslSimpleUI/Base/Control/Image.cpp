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

#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Sprite/ISizedSprite.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace UI
  {
    Image::Image(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
      , m_content(context->TheUIContext.Get()->MeshManager)
      , m_scalePolicy(ItemScalePolicy::FitKeepAR)
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


    void Image::SetContentColor(const Color& value)
    {
      if (value != m_contentColor)
      {
        m_contentColor = value;
        PropertyUpdated(PropertyType::Other);
      }
    }

    void Image::SetScalePolicy(const ItemScalePolicy value)
    {
      if (value != m_scalePolicy)
      {
        m_scalePolicy = value;
        PropertyUpdated(PropertyType::ScalePolicy);
      }
    }


    void Image::SetRotateImageCW(const bool enabled)
    {
      if (enabled != m_rotate90)
      {
        m_rotate90 = enabled;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void Image::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);

      if (!m_rotate90)
      {
        context.CommandBuffer.Draw(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), GetFinalBaseColor() * m_contentColor);
      }
      else
      {
        context.CommandBuffer.DrawRotated90CW(m_content.Get(), context.TargetRect.Location(), RenderSizePx(), GetFinalBaseColor() * m_contentColor);
      }
    }


    PxSize2D Image::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      return m_content.Measure(finalSizePx, m_scalePolicy, m_rotate90);
    }


    PxSize2D Image::MeasureOverride(const PxAvailableSize& /*availableSizePx*/)
    {
      PxSize2D desiredSizePx = m_content.Measure();
      return !m_rotate90 ? desiredSizePx : PxSize2D::Flip(desiredSizePx);
    }
  }
}
