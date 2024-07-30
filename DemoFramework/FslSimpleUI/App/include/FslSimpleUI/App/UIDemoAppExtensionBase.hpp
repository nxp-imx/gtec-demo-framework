#ifndef FSLSIMPLEUI_APP_UIDEMOAPPEXTENSIONBASE_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPEXTENSIONBASE_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslDataBinding/App/DataBindingDemoAppExtension.hpp>
#include <FslDemoService/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslSimpleUI/App/UIAppDefaultMaterial.hpp>
#include <memory>

namespace Fsl
{
  class DemoPerformanceCapture;
  class IProfilerService;
  struct PxViewport;
  struct UIDemoAppExtensionCreateInfo;

  namespace UI
  {
    class AExternalModule;
    class BaseWindow;
    struct ExternalModuleId;
    class IEventListener;
    class IRenderSystemBase;
    class IWindowManager;
    class UIContext;
    class ActivitySystem;
  }


  class UIDemoAppExtensionBase : public DataBindingDemoAppExtension
  {
    std::unique_ptr<UI::ActivitySystem> m_activitySystem;
    std::shared_ptr<DemoPerformanceCapture> m_demoPerformanceCapture;

    std::shared_ptr<IProfilerService> m_profilerService;
    ScopedProfilerCustomCounterHandle m_hProfileCounterUpdate;
    ScopedProfilerCustomCounterHandle m_hProfileCounterResolve;
    ScopedProfilerCustomCounterHandle m_hProfileCounterDraw;
    ScopedProfilerCustomCounterHandle m_hProfileCounterWin;
    std::shared_ptr<UI::BaseWindow> m_mainWindow;

  public:
    UIDemoAppExtensionBase(const UIDemoAppExtensionBase&) = delete;
    UIDemoAppExtensionBase& operator=(const UIDemoAppExtensionBase&) = delete;

    UIDemoAppExtensionBase(const UIDemoAppExtensionCreateInfo& createInfo, const std::shared_ptr<UI::IEventListener>& eventListener);
    ~UIDemoAppExtensionBase() override;

    template <typename T>
    std::shared_ptr<T> GetExternalModule(const UI::ExternalModuleId& moduleId) const
    {
      auto module = DoGetExternalModule(moduleId);
      auto typedModule = std::dynamic_pointer_cast<T>(module);
      return typedModule ? typedModule : throw UsageErrorException("external module was not of the expected type");
    }

    void SetUseDrawCache(const bool useDrawCache);

    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void SYS_SetRenderSystemViewport(const PxViewport& viewportPx);

    void RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);
    void UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);

    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;

    void SetMainWindow(const std::shared_ptr<UI::BaseWindow>& mainWindow);

    //! @brief Get the window manager
    std::shared_ptr<UI::IWindowManager> GetWindowManager() const;

    const std::shared_ptr<UI::UIContext>& GetUIContext() const;

    //! @brief Check if the UI system is considered idle
    bool IsIdle() const noexcept;

    //! @brief Check if the UI requires a redraw due to changes
    bool IsRedrawRequired() const noexcept;

    const UI::IRenderSystemBase& GetRenderSystem() const;
    UI::IRenderSystemBase* TryGetRenderSystem();

  protected:
    void ForceInvalidateLayout();

    DemoPerformanceCapture* TryGetDemoPerformanceCapture() const;
    void DoDraw();

    // NOLINTNEXTLINE(readability-identifier-naming)
    bool SYS_GetUseYFlipTextureCoordinates() const noexcept;
    std::shared_ptr<UI::AExternalModule> DoGetExternalModule(const UI::ExternalModuleId& moduleId) const;
  };
}

#endif
