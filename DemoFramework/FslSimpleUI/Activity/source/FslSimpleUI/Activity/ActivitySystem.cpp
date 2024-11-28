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

#include <FslSimpleUI/Activity/ActivitySystem.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystem.hpp>
#include <utility>

namespace Fsl::UI
{
  ActivitySystem::ActivitySystem(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService,
                                 std::unique_ptr<IRenderSystem> renderSystem, const UIColorSpace colorSpace, const bool useYFlipTextureCoordinates,
                                 const BasicWindowMetrics& windowMetrics)
    : m_uiManager(dataBindingService, std::move(renderSystem), colorSpace, useYFlipTextureCoordinates, windowMetrics)
  {
  }


  ActivitySystem::ActivitySystem(const std::shared_ptr<DataBinding::DataBindingService>& dataBindingService,
                                 std::unique_ptr<IRenderSystem> renderSystem, const UIColorSpace colorSpace, const bool useYFlipTextureCoordinates,
                                 const BasicWindowMetrics& windowMetrics,
                                 ReadOnlySpan<std::shared_ptr<IExternalModuleFactory>> externalModuleFactories)
    : m_uiManager(dataBindingService, std::move(renderSystem), colorSpace, useYFlipTextureCoordinates, windowMetrics, externalModuleFactories)
  {
  }


  ActivitySystem::~ActivitySystem() = default;


  void ActivitySystem::SetUseDrawCache(const bool useDrawCache)
  {
    m_uiManager.SetUseDrawCache(useDrawCache);
  }


  void ActivitySystem::SetClipRectangle(const bool enabled, const PxRectangle& clipRectanglePx)
  {
    m_uiManager.SetClipRectangle(enabled, clipRectanglePx);
  }


  std::shared_ptr<AExternalModule> ActivitySystem::GetExternalModule(const ExternalModuleId& moduleId) const
  {
    return m_uiManager.GetExternalModule(moduleId);
  }


  const std::shared_ptr<UIContext>& ActivitySystem::GetUIContext() const
  {
    return m_uiManager.GetUIContext();
  }


  std::shared_ptr<IWindowManager> ActivitySystem::GetWindowManager() const
  {
    return m_uiManager.GetWindowManager();
  }


  const std::shared_ptr<WindowEventPool>& ActivitySystem::GetEventPool() const
  {
    return m_uiManager.GetEventPool();
  }


  const std::shared_ptr<WindowEventSender>& ActivitySystem::GetEventSender() const
  {
    return m_uiManager.GetEventSender();
  }


  bool ActivitySystem::SendMouseButtonEvent(const MillisecondTickCount32 timestamp, const PxPoint2 positionPx, const bool leftButtonDown,
                                            const bool isTouch)
  {
    // m_gestureManager.AddMovement(timestamp, positionPx, leftButtonDown);

    return m_uiManager.SendMouseButtonEvent(timestamp, positionPx, leftButtonDown, isTouch);
  }


  bool ActivitySystem::SendMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 positionPx, const bool isTouch)
  {
    // m_gestureManager.AddMovement(timestamp, positionPx);

    return m_uiManager.SendMouseMoveEvent(timestamp, positionPx, isTouch);
  }


  bool ActivitySystem::IsIdle() const noexcept
  {
    return m_uiManager.IsIdle();
  }


  bool ActivitySystem::IsRedrawRequired() const noexcept
  {
    return m_uiManager.IsRedrawRequired();
  }


  UIStats ActivitySystem::GetStats() const noexcept
  {
    return m_uiManager.GetStats();
  }


  void ActivitySystem::ProcessEvents()
  {
    m_uiManager.ProcessEvents();
  }


  void ActivitySystem::Resized(const BasicWindowMetrics& windowMetrics)
  {
    // m_gestureManager.OnConfigurationChanged(windowMetrics.DensityDpi);
    m_uiManager.Resized(windowMetrics);
  }


  void ActivitySystem::FixedUpdate(const TimeSpan& timespan)
  {
    m_uiManager.FixedUpdate(timespan);
  }


  void ActivitySystem::Update(const TimeSpan& timespan)
  {
    // m_gestureManager.Process(timeSpan)
    m_uiManager.Update(timespan);
  }


  void ActivitySystem::PreDraw()
  {
    m_uiManager.PreDraw();
  }


  void ActivitySystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
  {
    m_uiManager.Draw(pPerformanceCapture);
  }


  void ActivitySystem::PostDraw()
  {
    m_uiManager.PostDraw();
  }


  const UI::IRenderSystemBase& ActivitySystem::GetRenderSystem() const
  {
    return m_uiManager.GetRenderSystem();
  }


  UI::IRenderSystemBase* ActivitySystem::TryGetRenderSystem()
  {
    return m_uiManager.TryGetRenderSystem();
  }


  void ActivitySystem::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
  {
    m_uiManager.RegisterEventListener(eventListener);
  }


  void ActivitySystem::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
  {
    m_uiManager.UnregisterEventListener(eventListener);
  }


  bool ActivitySystem::SYS_GetUseYFlipTextureCoordinates() const noexcept
  {
    return m_uiManager.SYS_GetUseYFlipTextureCoordinates();
  }


  void ActivitySystem::ForceInvalidateLayout()
  {
    m_uiManager.ForceInvalidateLayout();
  }
}
