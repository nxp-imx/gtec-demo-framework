#ifndef SHARED_UI_BENCHMARK_APP_TESTAPPHOST_HPP
#define SHARED_UI_BENCHMARK_APP_TESTAPPHOST_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslDemoApp/Base/IDemoAppExtension.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <Shared/UI/Benchmark/App/ITestApp.hpp>
#include <memory>
#include <vector>

namespace Fsl
{
  class CustomWindowInfoModuleProxy;
  class DemoPerformanceCapture;
  struct FrameInfo;
  class ICustomWindowInfoModule;
  class IDemoAppControl;
  class ITestAppFactory;
  struct UIDemoAppExtensionCreateInfo;
  struct UIDemoAppMaterialCreateInfo;
  struct UIDemoAppMaterialConfig;
  class WindowIdGeneratorModule;

  namespace UI
  {
    class IExternalModuleFactory;
    class IRenderSystemBase;
  }

  class TestAppHost final : public IDemoAppExtension
  {
    struct ConfigRecord
    {
      bool EmulateDpiEnabled{false};
      uint16_t EmulatedDpi{0};
      PxViewport AppViewportPx;

      constexpr ConfigRecord(const bool emulateDpiEnabled, const uint16_t emulatedDpi, const PxViewport appViewportPx)
        : EmulateDpiEnabled(emulateDpiEnabled)
        , EmulatedDpi(emulatedDpi)
        , AppViewportPx(appViewportPx)
      {
      }
    };
    struct AppRecord
    {
      std::unique_ptr<ITestApp> TestApp;
      std::shared_ptr<CustomUIDemoAppExtension> DemoExtension;
      std::shared_ptr<WindowIdGeneratorModule> CustomWindowModule;

      AppRecord() = default;
    };

    ServiceProvider m_serviceProvider;
    ConfigRecord m_config;
    DemoWindowMetrics m_realWindowMetrics;
    DemoWindowMetrics m_appWindowMetrics;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    std::shared_ptr<DemoPerformanceCapture> m_demoPerformanceCapture;

    std::vector<std::shared_ptr<UI::IExternalModuleFactory>> m_externalModules;

    std::shared_ptr<CustomWindowInfoModuleProxy> m_windowInfoProxy;
    AppRecord m_appRecord;

  public:
    TestAppHost(const ServiceProvider& serviceProvider, const DemoWindowMetrics& windowMetrics, const bool useCustomModule = false);
    ~TestAppHost() override;

    Color GetRootColor() const;
    bool TrySetRootColor(const Color color);

    void ClearProfileData();

    const DemoPerformanceCapture& GetProfiler() const
    {
      return *m_demoPerformanceCapture;
    }

    const UI::IRenderSystemBase& GetRenderSystem() const;
    UI::IRenderSystemBase* TryGetRenderSystem();

    bool IsUIIdle() const;

    void StartTestApp(ITestAppFactory& testAppFactory, const UIDemoAppMaterialCreateInfo& materialCreateInfo,
                      const UIDemoAppMaterialConfig& materialConfig);
    void StopTestApp();

    void SetAppRectangle(const PxRectangle& appRectPx);

    void AppUpdate(const DemoTime& demoTime);
    void AppDraw(const DemoTime& demoTime);
    void AppOnDrawSkipped(const FrameInfo& frameInfo);

    const UIDemoAppMaterialConfig& GetDefaultMaterialConfig() const;
    void SetDefaultMaterialConfig(const UIDemoAppMaterialConfig& config);

    void SetEmulatedDpi(const bool emulateDpiEnabled, const uint16_t emulatedDpi);
    uint16_t GetTexDpi() const;

    void SetUseDrawCache(const bool useDrawCache);

    std::shared_ptr<ICustomWindowInfoModule> GetWindowInfoModule() const;


    // IDemoAppExtension
    void OnKeyEvent(const KeyEvent& event) final;
    void OnMouseButtonEvent(const MouseButtonEvent& event) final;
    void OnMouseMoveEvent(const MouseMoveEvent& event) final;
    void OnMouseWheelEvent(const MouseWheelEvent& event) final;
    void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) final;
    void OnTimeStateEvent(const TimeStateEvent& event) final;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void Begin(const DemoAppExtensionCallOrder callOrder) final;
    void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) final;
    void OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo) final;
    void End(const DemoAppExtensionCallOrder callOrder) final;
  };
}

#endif
