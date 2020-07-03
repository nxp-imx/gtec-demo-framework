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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <algorithm>
#include "../Impl/ImageImpl_AtlasTexture2D.hpp"

namespace Fsl
{
  namespace UI
  {
    LabelTexture2DImage::LabelTexture2DImage(const std::shared_ptr<WindowContext>& context)
      : LabelBase(context)
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
      {
        return;
      }

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
      const PxSize2D finalSizePx = RenderSizePx();
      const PxSize2D contentSizePx(m_texture.GetSize());
      PxSize2D textureSizePx;
      if (UIScaleUtil::TryCalcSize(textureSizePx, finalSizePx, contentSizePx, m_scalePolicy))
      {
        auto targetRectPxf = context.TargetRect;
        const PxSize2D availablePx = TypeConverter::UncheckedChangeTo<PxSize2D>(targetRectPxf.GetSize());
        if (availablePx.Width() != textureSizePx.Width())
        {
          const int32_t newXPx = (availablePx.Width() - textureSizePx.Width()) / 2;
          targetRectPxf.MoveLeft(targetRectPxf.Left() + static_cast<float>(newXPx));
          targetRectPxf.SetWidth(static_cast<float>(textureSizePx.Width()));
        }
        if (availablePx.Height() != textureSizePx.Height())
        {
          const int32_t newYPx = (availablePx.Height() - textureSizePx.Height()) / 2;
          targetRectPxf.MoveTop(targetRectPxf.Top() + static_cast<float>(newYPx));
          targetRectPxf.SetHeight(static_cast<float>(textureSizePx.Height()));
        }

        m_windowContext->Batch2D->Draw(m_texture, targetRectPxf, Color::White());
        LabelBase::WinDraw(context);
      }
    }


    PxSize2D LabelTexture2DImage::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      const PxSize2D labelSizePx = LabelBase::ArrangeOverride(finalSizePx);

      PxSize2D textureSizePx;
      if (m_texture.IsValid())
      {
        const PxSize2D contentSizePx(m_texture.GetSize());
        if (!UIScaleUtil::TryCalcSize(textureSizePx, finalSizePx, contentSizePx, m_scalePolicy))
        {
          textureSizePx = finalSizePx;
        }
      }

      // Pick the largest of each dimension
      return {std::max(labelSizePx.Width(), textureSizePx.Width()), std::max(labelSizePx.Height(), textureSizePx.Height())};
    }


    PxSize2D LabelTexture2DImage::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      const PxSize2D labelSizePx = LabelBase::MeasureOverride(availableSizePx);

      PxSize2D textureSizePx = m_texture.GetSize();

      // Pick the largest of each dimension
      return {std::max(labelSizePx.Width(), textureSizePx.Width()), std::max(labelSizePx.Height(), textureSizePx.Height())};
    }
  }
}
