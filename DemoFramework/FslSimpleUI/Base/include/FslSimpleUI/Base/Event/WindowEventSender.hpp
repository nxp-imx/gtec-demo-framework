#ifndef FSLSIMPLEUI_BASE_EVENT_WINDOWEVENTSENDER_HPP
#define FSLSIMPLEUI_BASE_EVENT_WINDOWEVENTSENDER_HPP
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

#include <memory>

namespace Fsl::UI
{
  class ITreeNodeLocator;
  class IWindowId;
  class WindowEvent;
  class WindowEventPool;
  class WindowEventQueue;

  class WindowEventSender final
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    const std::shared_ptr<WindowEventQueue> m_windowEventQueue;
    // NOLINTNEXTLINE(readability-identifier-naming)
    const std::shared_ptr<ITreeNodeLocator> m_treeNodeLocator;

  public:
    WindowEventSender(const WindowEventSender&) = delete;
    WindowEventSender& operator=(const WindowEventSender&) = delete;

    const std::shared_ptr<WindowEventPool> EventPool;

    WindowEventSender(std::shared_ptr<WindowEventQueue> windowEventQueue, std::shared_ptr<WindowEventPool> eventPool,
                      std::shared_ptr<ITreeNodeLocator> treeNodeLocator);
    ~WindowEventSender();

    //! @brief Send a event from the supplied source
    void SendEvent(const std::shared_ptr<WindowEvent>& theEvent, const IWindowId* const pSource);
    void SendEvent(const std::shared_ptr<WindowEvent>& theEvent, const std::shared_ptr<IWindowId>& source);
    // bool TrySendEvent(const std::shared_ptr<WindowEvent>& theEvent, const IWindowId*const pSource);
    // bool TrySendEvent(const std::shared_ptr<WindowEvent>& theEvent, const std::shared_ptr<IWindowId>& source);
  };
}

#endif
