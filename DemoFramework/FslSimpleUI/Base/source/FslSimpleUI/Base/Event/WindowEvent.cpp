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

#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <FslSimpleUI/Base/IWindowId.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    WindowEvent::~WindowEvent() = default;


    bool WindowEvent::IsOriginalSource(const IWindowId* const pWindowId) const
    {
      return GetOriginalSource().get() == pWindowId;
    }


    bool WindowEvent::IsSource(const IWindowId* const pWindowId) const
    {
      return GetSource().get() == pWindowId;
    }


    const std::shared_ptr<IWindowId>& WindowEvent::GetOriginalSource() const
    {
      assert(!IsDisposed());
      return m_originalSource;
    }


    const std::shared_ptr<IWindowId>& WindowEvent::GetSource() const
    {
      assert(!IsDisposed());
      return m_source;
    }


    bool WindowEvent::IsHandled() const
    {
      assert(!IsDisposed());
      return m_isHandled;
    }


    void WindowEvent::Handled()
    {
      assert(!IsDisposed());
      m_isHandled = true;
    }


    void WindowEvent::SYS_SetSource(const std::shared_ptr<IWindowId>& value)
    {
      m_source = value;
    }


    void WindowEvent::SYS_SetOriginalSource(const std::shared_ptr<IWindowId>& value)
    {
      m_originalSource = value;
      m_source = value;
    }


    WindowEvent::WindowEvent(const EventTypeId typeId, const EventDescription& eventDescription)
      : m_eventTypeId(typeId)
      , m_eventDescription(eventDescription)
      , m_isHandled(false)
      , m_isInitialized(false)
    {
    }


    void WindowEvent::SYS_DoConstruct()
    {
      assert(!m_isInitialized);
      m_isInitialized = true;
    }


    void WindowEvent::SYS_Destruct()
    {
      assert(m_isInitialized);
      m_originalSource.reset();
      m_source.reset();
      m_isHandled = false;
      m_isInitialized = false;
    }
  }
}
