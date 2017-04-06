#ifndef FSLSIMPLEUI_CONTROL_IMPL_IMAGEIMPL_HPP
#define FSLSIMPLEUI_CONTROL_IMPL_IMAGEIMPL_HPP
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

#include <FslSimpleUI/UIDrawContext.hpp>
#include <FslSimpleUI/UIScaleUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {

    //! @brief Since we have multiple controls that need this code its been isolated here as inline methods
    namespace ImageImpl
    {
      inline static void WinDraw(const UIDrawContext& context, const AtlasTexture2D& content, const ItemScalePolicy scalepolicy, const Color& color, const std::shared_ptr<INativeBatch2D>& batch2D)
      {
        if (!content.IsValid())
          return;

        batch2D->ChangeTo(BlendState::AlphaBlend);
        batch2D->Draw(content, context.TargetRect, color);
      }

      inline static Vector2 ArrangeOverride(const Vector2& finalSize, const AtlasTexture2D& content, const ItemScalePolicy scalepolicy)
      {
        if (!content.IsValid())
          return finalSize;

        Vector2 contentSize(content.GetSize().X, content.GetSize().Y);
        Vector2 calcedSize;
        if (!UIScaleUtil::TryCalcSize(calcedSize, finalSize, contentSize, scalepolicy))
          return finalSize;
        return calcedSize;
      }

      inline static Vector2 MeasureOverride(const Vector2& availableSize, const AtlasTexture2D& content)
      {
        if (!content.IsValid())
          return Vector2();

        return Vector2(content.GetSize().X, content.GetSize().Y);
      }
    }
  }
}

#endif
