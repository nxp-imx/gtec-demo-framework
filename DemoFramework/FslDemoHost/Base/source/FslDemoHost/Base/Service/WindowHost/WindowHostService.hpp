#ifndef FSLDEMOHOST_BASE_SERVICE_WINDOWHOST_WINDOWHOSTSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_WINDOWHOST_WINDOWHOSTSERVICE_HPP
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

#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfo.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfoControl.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>

namespace Fsl
{

  class WindowHostService
    : public ThreadLocalService
    , public IWindowHostInfo
    , public IWindowHostInfoControl
  {
    std::weak_ptr<INativeWindowSystem> m_windowSystem;
    mutable std::deque<std::weak_ptr<INativeWindow> > m_windows;
  public:
    WindowHostService(const ServiceProvider& serviceProvider);
    ~WindowHostService();

    // From IWindowHostInfoControl
    virtual void ClearWindowSystem() override;
    virtual void SetWindowSystem(const std::weak_ptr<INativeWindowSystem>& windowSystem) override;
    virtual void AddWindow(const std::weak_ptr<INativeWindow>& window) override;
    virtual void RemoveWindow(const std::weak_ptr<INativeWindow>& window) override;

    // From IWindowHostInfo
    virtual std::weak_ptr<INativeWindowSystem> GetWindowSystem() const override;
    virtual std::deque<std::weak_ptr<INativeWindow> > GetWindows() const override;
  private:
    static void RemoveExpiredWindows(std::deque<std::weak_ptr<INativeWindow> >& rWindows);
  };
}

#endif
