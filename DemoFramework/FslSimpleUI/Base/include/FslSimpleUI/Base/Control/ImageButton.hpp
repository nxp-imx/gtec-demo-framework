#ifndef FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
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
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>

namespace Fsl
{
  namespace UI
  {
    //! @brief A simple image button. While you can create a image button with the more
    //         generic button class this one performs better.
    class ImageButton : public ButtonBase
    {
      AtlasTexture2D m_content;
      ItemScalePolicy m_scalePolicy;
      Color m_colorUp;
      Color m_colorDown;
    public:
      ImageButton(const std::shared_ptr<WindowContext>& context);

      const AtlasTexture2D& GetContent() const
      {
        return m_content;
      }
      void SetContent(const AtlasTexture2D& value);

      ItemScalePolicy GetScalePolicy() const
      {
        return m_scalePolicy;
      }
      void SetScalePolicy(const ItemScalePolicy value);

      Color GetColorUp() const

      { return m_colorUp;
      }
      void SetColorUp(const Color& value);

      Color GetColorDown() const
      {
        return m_colorDown;
      }
      void SetColorDown(const Color& value);

      virtual void WinDraw(const UIDrawContext& context) override;
    protected:
      virtual Vector2 ArrangeOverride(const Vector2& finalSize) override;
      virtual Vector2 MeasureOverride(const Vector2& availableSize) override;
    };
  }
}

#endif
