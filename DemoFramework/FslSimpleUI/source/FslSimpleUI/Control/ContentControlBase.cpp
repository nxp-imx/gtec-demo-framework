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

#include <FslSimpleUI/Control/ContentControlBase.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslSimpleUI/IWindowManager.hpp>
#include <FslSimpleUI/PropertyTypeFlags.hpp>
#include <FslSimpleUI/WindowContext.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {

    ContentControlBase::ContentControlBase(const std::shared_ptr<WindowContext>& context)
      : BaseWindow(context)
      , m_padding()
      , m_isInitialized(false)
    {
      Enable(WindowFlags(WindowFlags::WinInit));
    }


    void ContentControlBase::WinInit()
    {
      assert(!m_isInitialized);
      m_isInitialized = true;
      if (m_content)
        GetContext()->WindowManager->AddChild(this, m_content);
    }


    void ContentControlBase::DoSetPadding(const ThicknessF& value)
    {
      if (value != m_padding)
      {
        m_padding = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void ContentControlBase::DoSetContent(const std::shared_ptr<BaseWindow>& value)
    {
      if (value != m_content)
      {
        auto winMgr = GetContext()->WindowManager;

        // Remove the old content if any
        if (m_content)
        {
          winMgr->ScheduleClose(m_content);
          m_content.reset();
        }

        m_content = value;

        if (m_content && m_isInitialized)
          winMgr->AddChild(this, m_content);

        PropertyUpdated(PropertyType::Content);
      }
    }


    Vector2 ContentControlBase::ArrangeOverride(const Vector2& finalSize)
    {
      if (!m_content)
        return finalSize;

      const Vector2 localFinalSize(std::max(finalSize.X - m_padding.SumX(), 0.0f), std::max(finalSize.Y - m_padding.SumY(), 0.0f));

      // Arrange the control inside this one
      m_content->Arrange(Rect(m_padding.Left(), m_padding.Top(), localFinalSize.X, localFinalSize.Y));
      return finalSize;
    }


    Vector2 ContentControlBase::MeasureOverride(const Vector2& availableSize)
    {
      if (!m_content)
        return availableSize;

      const Vector2 localAvailableSize(std::max(availableSize.X - m_padding.SumX(), 0.0f), std::max(availableSize.Y - m_padding.SumY(), 0.0f));
      // Measure our content
      m_content->Measure(localAvailableSize);
      const Vector2 contentDesiredSize = m_content->DesiredSize();
      return Vector2(m_padding.SumX() + contentDesiredSize.X, m_padding.SumY() + contentDesiredSize.Y);
    }
  }
}
