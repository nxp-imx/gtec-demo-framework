#ifndef FSLDEMOHOST_BASE_SERVICE_TEST_TESTSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_TEST_TESTSERVICE_HPP
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

#include <FslDemoHost/Base/Service/Test/ITestService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <memory>

namespace Fsl
{
  class IDemoAppControlEx;
  class IGraphicsService;
  class IPersistentDataManager;

  class TestService final
    : public ThreadLocalService
    , public ITestService
  {
    std::shared_ptr<IDemoAppControlEx> m_demoAppControlService;
    std::shared_ptr<IPersistentDataManager> m_persistentDataManager;
    std::shared_ptr<IGraphicsService> m_graphicsService;
    TestScreenshotConfig m_config;
    uint32_t m_frameCounter;
    uint32_t m_screenshotNameCounter;
    int32_t m_userScreenshotCount;
    Bitmap m_screenshot;

  public:
    explicit TestService(const ServiceProvider& serviceProvider);
    ~TestService() final;

    // From ITestService
    uint32_t GetScreenshotFrequency() const final;
    void SetScreenshotFrequency(const uint32_t frequency) final;
    TestScreenshotNameScheme GetScreenshotNameScheme() const final;
    void SetScreenshotNameScheme(const TestScreenshotNameScheme scheme) final;
    void SetScreenshotConfig(const TestScreenshotConfig& config) final;
    void OnFrameSwapCompleted() final;

  private:
    void SaveBitmap();
  };
}

#endif
