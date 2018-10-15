#ifndef FSLDEMOHOST_STUB_STUBDEMOHOST_HPP
#define FSLDEMOHOST_STUB_STUBDEMOHOST_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <deque>
#include <vector>
#include <FslDemoApp/Shared/Host/ConfigControl.hpp>
#include <FslDemoHost/Base/ADemoHost.hpp>

namespace Fsl
{
  class StubDemoHost : public ADemoHost
  {
    DemoHostConfig m_demoHostConfig;
    bool m_isActivated;
    DemoHostFeature m_activeApi;

  public:
    StubDemoHost(const DemoHostConfig& demoHostConfig);
    ~StubDemoHost() override;

    void OnActivate() override;
    void OnDeactivate() override;
    void OnSuspend() override;
    void OnResume() override;
    DemoHostFeature GetActiveAPI() const override;
    Point2 GetScreenResolution() const override;
    bool SwapBuffers() override;
    bool ProcessNativeMessages(const bool allowBlock) override;

  private:
  };
}

#endif
