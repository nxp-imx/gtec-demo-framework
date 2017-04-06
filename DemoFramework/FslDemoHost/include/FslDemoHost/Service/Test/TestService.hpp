#ifndef FSLDEMOHOST_SERVICE_TEST_TESTSERVICE_HPP
#define FSLDEMOHOST_SERVICE_TEST_TESTSERVICE_HPP
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

#include <FslDemoApp/Service/ServiceProvider.hpp>
#include <FslDemoHost/Service/Test/ITestService.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <memory>

namespace Fsl
{
  class IDemoAppControlEx;
  class IGraphicsService;
  class IPersistentDataManager;

  class TestService
    : public ThreadLocalService
    , public ITestService
  {
    std::shared_ptr<IDemoAppControlEx> m_demoAppControlService;
    std::shared_ptr<IPersistentDataManager> m_persistentDataManager;
    std::shared_ptr<IGraphicsService> m_graphicsService;
    TestScreenshotNameScheme m_screenshotNameScheme;
    uint32_t m_frameCounter;
    uint32_t m_screenshotNameCounter;
    int32_t m_screenshotFrequency;
    int32_t m_screenshotCount;
    Bitmap m_screenshot;
  public:
    TestService(const ServiceProvider& serviceProvider);
    ~TestService();

    // From ITestService
    virtual uint32_t GetScreenshotFrequency() const override;
    virtual void SetScreenshotFrequency(const uint32_t frequency) override;
    virtual TestScreenshotNameScheme GetScreenshotNameScheme() const override;
    virtual void SetScreenshotNameScheme(const TestScreenshotNameScheme scheme) override;
    virtual void OnFrameSwapCompleted() override;
  private:
  };
}

#endif
