/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslSimpleUI/Base/Layout/CanvasLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cmath>

// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    CanvasLayout::CanvasLayout(const std::shared_ptr<BaseWindowContext>& context)
      : Layout(context)
    {
    }


    void CanvasLayout::WinInit()
    {
      Layout::WinInit();

      auto uiContext = GetContext()->TheUIContext.Get();
      m_children.SYS_WinInit(this, uiContext->WindowManager);
    }


    void CanvasLayout::SetChildPosition(const std::shared_ptr<BaseWindow>& window, const Vector2& position)
    {
      auto itr =
        std::find_if(m_children.begin(), m_children.end(), [window](const collection_type::record_type& record) { return record.Window == window; });
      if (itr == m_children.end())
      {
        throw NotFoundException("The window is not part of this layout");
      }

      itr->Position = position;
    }


    Vector2 CanvasLayout::ArrangeOverride(const Vector2& finalSize)
    {
      for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
      {
        itr->Window->Arrange(Rect(itr->Position.X, itr->Position.Y, finalSize.X, finalSize.Y));
      }
      return finalSize;
    }


    Vector2 CanvasLayout::MeasureOverride(const Vector2& availableSize)
    {
      for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
      {
        itr->Window->Measure(availableSize);
      }

      // The canvas content does not affect its measurements.
      return Vector2::Zero();
    }
  }
}
