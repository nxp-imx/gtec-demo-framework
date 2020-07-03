#ifndef FSLDEMOHOST_EGL_EGLDEMOHOST_HPP
#define FSLDEMOHOST_EGL_EGLDEMOHOST_HPP
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
#include <FslDemoApp/Shared/Host/ExtensionRequestRecord.hpp>
#include <FslDemoHost/Base/ADemoHost.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <EGL/egl.h>

namespace Fsl
{
  class DemoAppHostConfigEGL;
  class IEGLNativeWindow;
  class IEGLNativeWindowSystem;
  class IWindowHostInfoControl;
  class EGLHostService;
  class EGLDemoHostOptionParser;

  struct EGLDemoHostFeatureConfig
  {
    bool EnableGLES{false};
    int ESVersionMajor{0};
    int ESVersionMinor{0};

    bool EnableVG{false};
    int VGVersion{0};

    DemoHostFeature Feature;

    EGLDemoHostFeatureConfig()
      : Feature(DemoHostFeatureName::OpenGLES, 0)
    {
    }
  };

  class EGLDemoHost : public ADemoHost
  {
    DemoHostConfig m_demoHostConfig;
    std::deque<EGLint> m_appEglConfigAttribs;
    std::vector<EGLint> m_finalConfigAttribs;
    int m_eglContextClientVersionMajor;
    int m_eglContextClientVersionMinor;
    int m_eglContextClientVersionMinimumMinor;
    std::unique_ptr<NativeWindowSetup> m_nativeWindowSetup;
    std::shared_ptr<IEGLNativeWindowSystem> m_windowSystem;
    std::shared_ptr<IEGLNativeWindow> m_window;
    std::shared_ptr<IWindowHostInfoControl> m_windowHostInfoControl;
    std::shared_ptr<EGLHostService> m_hostService;
    EGLDisplay m_hDisplay;
    EGLSurface m_hSurface;
    EGLContext m_hContext;
    EGLConfig m_hConfig;
    bool m_isActivated;
    std::shared_ptr<EGLDemoHostOptionParser> m_options;
    std::shared_ptr<DemoAppHostConfigEGL> m_appHostConfig;
    ConfigControl m_configControl;
    std::deque<ExtensionRequestRecord> m_extensionRequests;
    EGLDemoHostFeatureConfig m_featureConfig;
    bool m_enableGLES;
    bool m_enableVG;
    bool m_logSelectedConfig;
    bool m_logExtensions;
    bool m_apiInit;
    DemoHostFeature m_activeApi;

  public:
    explicit EGLDemoHost(const DemoHostConfig& demoHostConfig);
    ~EGLDemoHost() override;

    void OnConstructed() override;
    void OnActivate() override;
    void OnDeactivate() override;
    void OnSuspend() override;
    void OnResume() override;
    DemoHostFeature GetActiveAPI() const override;
    DemoWindowMetrics GetWindowMetrics() const override;
    SwapBuffersResult TrySwapBuffers() override;
    bool ProcessNativeMessages(const bool allowBlock) override;

  protected:
    bool IsLogExtensionsEnabled() const
    {
      return m_logExtensions;
    }

    const std::deque<ExtensionRequestRecord>& GetExtensionRequests() const
    {
      return m_extensionRequests;
    }

    //! @brief Called when the DemoHost API initialization has been completed
    virtual void OnAPIInitialized()
    {
    }
    //! @brief Called just before the API is being shutdown
    virtual void OnAPIShutdown()
    {
    }

  private:
    void Init();
    void Shutdown();
    void InitEGL();
    bool TryInitEGLHDRConfig(const std::deque<EGLint>& appAglConfigAttribs);
    bool TryInitEGLTryConfigFallback(const ConfigControl configControl, const std::deque<EGLint>& appEglConfigAttribs,
                                     const bool isLastResort = false);
    void ShutdownEGL();
    void InitSurfaceAndContext();
    void ShutdownSurfaceAndContext();
  };
}

#endif
