#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_ATLASTEXTURE2D_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_ATLASTEXTURE2D_HPP
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

#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <memory>

namespace Fsl::UI::ImageImpl
{
  inline static void WinDraw(const UIDrawContext& context, const AtlasTexture2D& content, const ItemScalePolicy scalepolicy, const Color& color,
                             const std::shared_ptr<INativeBatch2D>& batch2D)
  {
    FSL_PARAM_NOT_USED(scalepolicy);

    if (!content.IsValid())
    {
      return;
    }

    // FIX: check if the image needs to be rendered using alpha blend or not
    batch2D->Draw(content, context.TargetRect, color);
  }

  inline static PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const AtlasTexture2D& content, const ItemScalePolicy scalepolicy)
  {
    if (!content.IsValid())
    {
      return finalSizePx;
    }

    PxSize2D contentSize(content.GetSize());
    PxSize2D calcedSizePx;
    if (!UIScaleUtil::TryCalcSize(calcedSizePx, finalSizePx, contentSize, scalepolicy))
    {
      return finalSizePx;
    }
    return calcedSizePx;
  }

  inline static PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const AtlasTexture2D& content)
  {
    FSL_PARAM_NOT_USED(availableSizePx);
    return content.GetSize();
  }
}

#endif
