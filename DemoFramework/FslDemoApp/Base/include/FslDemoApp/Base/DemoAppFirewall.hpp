#ifndef FSLDEMOAPP_BASE_DEMOAPPFIREWALL_HPP
#define FSLDEMOAPP_BASE_DEMOAPPFIREWALL_HPP
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

#include <FslDemoApp/Base/ADemoApp.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class IBasic2D;
  class IDemoAppFactory;
  class KeyEvent;

  class DemoAppFirewall : public ADemoApp
  {
    std::shared_ptr<IDemoApp> m_app;
    std::string m_errorString;
    std::shared_ptr<IBasic2D> m_basic2D;
    bool m_appNotAllocatedInfoShown = false;

  public:
    DemoAppFirewall(const DemoAppConfig& demoAppConfig, const std::shared_ptr<IDemoAppFactory>& appFactory, const bool isConsoleBasedApp);
    ~DemoAppFirewall() override;

    // Check if the app is allocated
    bool IsAllocated() const
    {
      return m_app != nullptr;
    }

    void _PostConstruct() override;
    void _OnEvent(IEvent* const pEvent) override;
    void _Resized(const Point2& size) override;
    void _PreUpdate(const DemoTime& demoTime) override;
    void _FixedUpdate(const DemoTime& demoTime) override;
    void _Update(const DemoTime& demoTime) override;
    void _PostUpdate(const DemoTime& demoTime) override;
    void _Draw(const DemoTime& demoTime) override;

  private:
    void SafeDispose();
    void BuildErrorString(const std::string& desc, const std::exception& ex);
    void BuildErrorString(const std::string& desc, const std::string& message);
  };
}

#endif
