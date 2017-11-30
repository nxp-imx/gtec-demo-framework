/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include "WindowHostService.hpp"

namespace Fsl
{
  WindowHostService::WindowHostService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {

  }


  WindowHostService::~WindowHostService()
  {

  }


  void WindowHostService::ClearWindowSystem()
  {
    m_windowSystem.reset();
  }


  void WindowHostService::SetWindowSystem(const std::weak_ptr<INativeWindowSystem>& windowSystem)
  {
    m_windowSystem = windowSystem;
  }


  void WindowHostService::AddWindow(const std::weak_ptr<INativeWindow>& window)
  {
    RemoveExpiredWindows(m_windows);
    m_windows.push_back(window);
  }


  void WindowHostService::RemoveWindow(const std::weak_ptr<INativeWindow>& window)
  {
    const auto searchForWindow = window.lock();
    if (!searchForWindow)
      return;

    auto itr = m_windows.begin();
    while (itr != m_windows.end())
    {
      const auto currentWindow = itr->lock();
      if (currentWindow == searchForWindow)
      {
        itr->reset();
        itr = m_windows.erase(itr);
      }
      else
      {
        ++itr;
      }
    }
  }


  std::weak_ptr<INativeWindowSystem> WindowHostService::GetWindowSystem() const
  {
    return m_windowSystem;
  }


  std::deque<std::weak_ptr<INativeWindow> > WindowHostService::GetWindows() const
  {
    RemoveExpiredWindows(m_windows);
    return m_windows;
  }


  void WindowHostService::RemoveExpiredWindows(std::deque<std::weak_ptr<INativeWindow> >& rWindows)
  {
    auto itr = rWindows.begin();
    while(itr != rWindows.end())
    {
      if (itr->expired())
        itr = rWindows.erase(itr);
      else
        ++itr;
    }
  }

}
