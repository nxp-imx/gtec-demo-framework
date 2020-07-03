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

#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <FslDemoService/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <memory>

namespace Fsl
{
  class DemoAppConfig;
  class IProfilerService;
  class TransitionCache;

  namespace UI
  {
    class IEventListener;
  }

  class UIDemoAppExtensionBase : public DemoAppExtension
  {
    UI::UIManager m_uiManager;
    std::shared_ptr<TransitionCache> m_transitionCache;

    std::shared_ptr<IProfilerService> m_profilerService;
    ScopedProfilerCustomCounterHandle m_hProfileCounterUpdate;
    ScopedProfilerCustomCounterHandle m_hProfileCounterResolve;
    ScopedProfilerCustomCounterHandle m_hProfileCounterDraw;
    ScopedProfilerCustomCounterHandle m_hProfileCounterWin;

  public:
    UIDemoAppExtensionBase(const UIDemoAppExtensionBase&) = delete;
    UIDemoAppExtensionBase& operator=(const UIDemoAppExtensionBase&) = delete;

    UIDemoAppExtensionBase(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener);
    ~UIDemoAppExtensionBase() override;

    void RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);
    void UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener);

    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void PreUpdate(const DemoTime& demoTime) override;
    void Update(const DemoTime& demoTime) override;
    void PostUpdate(const DemoTime& demoTime) override;

    //! @brief Get the window manager
    std::shared_ptr<UI::IWindowManager> GetWindowManager() const
    {
      return m_uiManager.GetWindowManager();
    }

    const std::shared_ptr<UI::UIContext>& GetUIContext() const
    {
      return m_uiManager.GetUIContext();
    }

    //! @brief Check if the UI system is considered idle
    bool IsIdle() const;

  protected:
    void DoDraw();
  };
}

#endif
