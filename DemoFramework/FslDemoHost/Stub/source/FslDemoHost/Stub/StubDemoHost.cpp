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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Stub/StubDemoHost.hpp>
#include <FslDemoHost/Stub/StubDemoHostOptionParser.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <cassert>
#include <iostream>
#include <vector>

namespace Fsl
{
  StubDemoHost::StubDemoHost(const DemoHostConfig& demoHostConfig)
    : ADemoHost(demoHostConfig)
    , m_demoHostConfig(demoHostConfig)
    , m_isActivated(true)
    , m_activeApi(DemoHostFeatureName::Stub, 0)
  {
    const DemoHostAppSetup& hostAppSetup = demoHostConfig.GetDemoHostAppSetup();

    if (hostAppSetup.DemoHostFeatures->empty())
    {
      throw NotSupportedException("Must use at least one feature");
    }

    m_activeApi = hostAppSetup.DemoHostFeatures->front();

    const std::shared_ptr<INativeWindowEventQueue> eventQueue = demoHostConfig.GetEventQueue().lock();
    eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
  }


  StubDemoHost::~StubDemoHost() = default;


  void StubDemoHost::OnActivate()
  {
    m_isActivated = true;
  }


  void StubDemoHost::OnDeactivate()
  {
    m_isActivated = false;
  }


  void StubDemoHost::OnSuspend()
  {
  }


  void StubDemoHost::OnResume()
  {
  }


  DemoHostFeature StubDemoHost::GetActiveAPI() const
  {
    return m_activeApi;
  }


  Point2 StubDemoHost::GetScreenResolution() const
  {
    // FIX: this is the only real invalid data that we return
    return Point2(0, 0);
  }


  bool StubDemoHost::SwapBuffers()
  {
    if (!m_isActivated)
    {
      return true;
    }

    // Validate that we are not suspended
    return true;
  }


  bool StubDemoHost::ProcessNativeMessages(const bool allowBlock)
  {
    return true;
  }
}
