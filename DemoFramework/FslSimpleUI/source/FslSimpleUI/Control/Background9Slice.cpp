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

#include <FslSimpleUI/Control/Background9Slice.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/IWindowManager.hpp>
#include <FslSimpleUI/PropertyTypeFlags.hpp>
#include <FslSimpleUI/WindowContext.hpp>
#include <FslSimpleUI/UIDrawContext.hpp>
#include <cassert>
#include "Impl/Draw9SliceImpl.hpp"

namespace Fsl
{
  namespace UI
  {

    Background9Slice::Background9Slice(const std::shared_ptr<WindowContext>& context)
      : ContentControl(context)
      , m_background()
      , m_nineSlice()
      , m_minimumBackgroundMargin()
    {
      Enable(WindowFlags::DrawEnabled);
    }


    void Background9Slice::SetBackground(const AtlasTexture2D& value)
    {
      if (value != m_background)
      {
        m_background = value;
        PropertyUpdated(PropertyType::Content);
      }
    }

    void Background9Slice::SetNineSlice(const NineSlice& value)
    {
      if (value != m_nineSlice)
      {
        m_nineSlice = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Background9Slice::SetMinimumBackgroundMargin(const ThicknessF& value)
    {
      if (value != m_minimumBackgroundMargin)
      {
        m_minimumBackgroundMargin = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Background9Slice::WinDraw(const UIDrawContext& context)
    {
      ContentControl::WinDraw(context);

      Draw9SliceImpl::WinDraw(GetContext()->Batch2D, context.TargetRect, m_background, m_nineSlice, m_minimumBackgroundMargin, Color::White());
    }
  }
}
