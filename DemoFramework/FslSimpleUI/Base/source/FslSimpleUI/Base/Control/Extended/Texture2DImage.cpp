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

#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/Base/Control/Extended/Texture2DImage.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "../Impl/ImageImpl.hpp"

namespace Fsl
{
  namespace UI
  {
    Texture2DImage::Texture2DImage(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_windowContext(context)
      , m_scalePolicy(ItemScalePolicy::FitKeepAR)
    {
      Enable(WindowFlags::DrawEnabled);
    }


    void Texture2DImage::SetContent(const Texture2D& value)
    {
      if (value != m_content)
      {
        m_content = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void Texture2DImage::SetScalePolicy(const ItemScalePolicy value)
    {
      if (value != m_scalePolicy)
      {
        m_scalePolicy = value;
        PropertyUpdated(PropertyType::ScalePolicy);
      }
    }


    void Texture2DImage::WinDraw(const UIDrawContext& context)
    {
      BaseWindow::WinDraw(context);

      if (m_blendState != BlendState::AlphaBlend)
      {
        m_windowContext->Batch2D->ChangeTo(m_blendState);
      }

      m_windowContext->Batch2D->Draw(m_content, context.TargetRect, Color::White());

      if (m_blendState != BlendState::AlphaBlend)
      {
        m_windowContext->Batch2D->ChangeTo(BlendState::AlphaBlend);
      }
    }


    Vector2 Texture2DImage::ArrangeOverride(const Vector2& finalSize)
    {
      if (!m_content.IsValid())
      {
        return finalSize;
      }

      Vector2 contentSize(m_content.GetSize().X, m_content.GetSize().Y);
      Vector2 calcedSize;
      if (!UIScaleUtil::TryCalcSize(calcedSize, finalSize, contentSize, m_scalePolicy))
      {
        return finalSize;
      }
      return calcedSize;
    }


    Vector2 Texture2DImage::MeasureOverride(const Vector2& availableSize)
    {
      if (!m_content.IsValid())
      {
        return Vector2();
      }

      return Vector2(m_content.GetSize().X, m_content.GetSize().Y);
    }
  }
}
