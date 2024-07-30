#ifndef FSLSIMPLEUI_ACTIVITY_ACTIVITYSYSTEM_HPP
#define FSLSIMPLEUI_ACTIVITY_ACTIVITYSYSTEM_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <memory>

namespace Fsl::UI
{
  class ActivitySystem final
  {
    UIManager m_uiManager;

  public:
    ActivitySystem(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService, std::unique_ptr<IRenderSystem> renderSystem,
                   const UIColorSpace colorSpace, const bool useYFlipTextureCoordinates, const BasicWindowMetrics& windowMetrics);
    ActivitySystem(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService, std::unique_ptr<IRenderSystem> renderSystem,
                   const UIColorSpace colorSpace, const bool useYFlipTextureCoordinates, const BasicWindowMetrics& windowMetrics,
                   ReadOnlySpan<std::shared_ptr<IExternalModuleFactory>> externalModuleFactories);
    ~ActivitySystem();

    void SetUseDrawCache(const bool useDrawCache);

    std::shared_ptr<AExternalModule> GetExternalModule(const ExternalModuleId& moduleId) const;

    const std::shared_ptr<UIContext>& GetUIContext() const;
    std::shared_ptr<IWindowManager> GetWindowManager() const;
    const std::shared_ptr<WindowEventPool>& GetEventPool() const;
    const std::shared_ptr<WindowEventSender>& GetEventSender() const;

    // bool SendKeyEvent(const KeyEvent& event);
    //! @brief Send a mouse button event
    //! @note Returns true if the event was handled by a UIElement
    bool SendMouseButtonEvent(const MillisecondTickCount32 timestamp, const PxPoint2 positionPx, const bool leftButtonDown, const bool isTouch);
    bool SendMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 positionPx, const bool isTouch);
    // bool SendMouseWheelEvent(const MouseWheelEvent& event);

    //! Check if the UI system is considered idle
    bool IsIdle() const noexcept;
    //! Check if the UI content has been modified.
    bool IsRedrawRequired() const noexcept;

    UIStats GetStats() const noexcept;

    void ProcessEvents();

    void Resized(const BasicWindowMetrics& windowMetrics);
    void FixedUpdate(const TimeSpan& timespan);
    void Update(const TimeSpan& timespan);
    void PreDraw();
    void Draw(RenderPerformanceCapture* const pPerformanceCapture);
    void PostDraw();

    const UI::IRenderSystemBase& GetRenderSystem() const;
    UI::IRenderSystemBase* TryGetRenderSystem();

    //! @brief Register a event listener
    void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

    //! @brief Unregister a event listener
    void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

    // NOLINTNEXTLINE(readability-identifier-naming)
    bool SYS_GetUseYFlipTextureCoordinates() const noexcept;

    void ForceInvalidateLayout();
  };
}

#endif
