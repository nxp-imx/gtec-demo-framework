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

#include <FslSimpleUI/Base/Control/ImageButton.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include "Impl/ImageImpl.hpp"

namespace Fsl
{
  namespace UI
  {

    ImageButton::ImageButton(const std::shared_ptr<WindowContext>& context)
      : ButtonBase(context)
      , m_content()
      , m_scalePolicy(ItemScalePolicy::FitKeepAR)
      , m_colorUp(Color::White())
      , m_colorDown(0xB0B0B0B0)
    {
      Enable(WindowFlags::DrawEnabled);
    }


    void ImageButton::SetContent(const AtlasTexture2D& value)
    {
      if (value != m_content)
      {
        m_content = value;
        PropertyUpdated(PropertyType::Content);
      }
    }


    void ImageButton::SetScalePolicy(const ItemScalePolicy value)
    {
      if (value != m_scalePolicy)
      {
        m_scalePolicy = value;
        PropertyUpdated(PropertyType::ScalePolicy);
      }
    }


    void ImageButton::SetColorUp(const Color& value)
    {
      if (value != m_colorUp)
      {
        m_colorUp = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void ImageButton::SetColorDown(const Color& value)
    {
      if (value != m_colorDown)
      {
        m_colorDown = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void ImageButton::WinDraw(const UIDrawContext& context)
    {
      ButtonBase::WinDraw(context);

      const auto color = !IsDown() ? m_colorUp : m_colorDown;
      ImageImpl::WinDraw(context, m_content, m_scalePolicy, color, GetContext()->Batch2D);
    }


    Vector2 ImageButton::ArrangeOverride(const Vector2& finalSize)
    {
      return ImageImpl::ArrangeOverride(finalSize, m_content, m_scalePolicy);
    }


    Vector2 ImageButton::MeasureOverride(const Vector2& availableSize)
    {
      return ImageImpl::MeasureOverride(availableSize, m_content);
    }
  }
}
