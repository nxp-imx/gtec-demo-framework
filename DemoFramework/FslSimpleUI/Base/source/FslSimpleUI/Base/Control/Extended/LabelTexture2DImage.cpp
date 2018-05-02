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

#include <FslSimpleUI/Base/Control/Extended/LabelTexture2DImage.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <algorithm>
#include "../Impl/ImageImpl.hpp"

namespace Fsl
{
  namespace UI
  {

    LabelTexture2DImage::LabelTexture2DImage(const std::shared_ptr<WindowContext>& context)
      : LabelBase(context)
      , m_texture()
      , m_scalePolicy(ItemScalePolicy::FitKeepAR)
    {
      Enable(WindowFlags::DrawEnabled);
    }


    void LabelTexture2DImage::SetTexture(const Texture2D& value)
    {
      if (value != m_texture)
      {
        m_texture = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void LabelTexture2DImage::SetContent(const std::string& value)
    {
      if (value == m_content)
        return;

      m_content = value;
      PropertyUpdated(PropertyType::Content);
    }


    void LabelTexture2DImage::SetScalePolicy(const ItemScalePolicy value)
    {
      if (value != m_scalePolicy)
      {
        m_scalePolicy = value;
        PropertyUpdated(PropertyType::ScalePolicy);
      }
    }


    void LabelTexture2DImage::WinDraw(const UIDrawContext& context)
    {
      const Vector2 finalSize = RenderSize();
      const Vector2 contentSize(m_texture.GetSize().X, m_texture.GetSize().Y);
      Vector2 textureSize;
      if (UIScaleUtil::TryCalcSize(textureSize, finalSize, contentSize, m_scalePolicy))
      {
        auto targetRect = context.TargetRect;
        const auto available = targetRect.GetSize();
        if (available.X != textureSize.X)
        {
          targetRect.SetX(targetRect.X() + ((available.X - textureSize.X) / 2.0f));
          targetRect.SetWidth(textureSize.X);
        }
        if (available.Y != textureSize.Y)
        {
          targetRect.SetY(targetRect.Y() + ((available.Y - textureSize.Y) / 2.0f));
          targetRect.SetHeight(textureSize.Y);
        }

        GetContext()->Batch2D->Draw(m_texture, targetRect, Color::White());
        LabelBase::WinDraw(context);
      }
    }


    Vector2 LabelTexture2DImage::ArrangeOverride(const Vector2& finalSize)
    {
      const auto labelSize = LabelBase::ArrangeOverride(finalSize);

      Vector2 textureSize;
      if (m_texture.IsValid())
      {
        Vector2 contentSize(m_texture.GetSize().X, m_texture.GetSize().Y);
        if (!UIScaleUtil::TryCalcSize(textureSize, finalSize, contentSize, m_scalePolicy))
          textureSize = finalSize;
      }

      // Pick the largest of each dimension
      return Vector2(std::max(labelSize.X, textureSize.X), std::max(labelSize.Y, textureSize.Y));
    }


    Vector2 LabelTexture2DImage::MeasureOverride(const Vector2& availableSize)
    {
      const auto labelSize = LabelBase::MeasureOverride(availableSize);

      Vector2 textureSize;
      if (m_texture.IsValid())
        textureSize = Vector2(m_texture.GetSize().X, m_texture.GetSize().Y);

      // Pick the largest of each dimension
      return Vector2(std::max(labelSize.X, textureSize.X), std::max(labelSize.Y, textureSize.Y));
    }
  }
}
