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

#include <FslSimpleUI/App/UIDemoAppExtensionBase.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Profiler/DefaultProfilerColors.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <utility>

namespace Fsl
{
  UIDemoAppExtensionBase::UIDemoAppExtensionBase(const DemoAppConfig& demoAppConfig, const std::shared_ptr<UI::IEventListener>& eventListener)
    : m_uiManager(demoAppConfig.WindowMetrics)
    , m_transitionCache(std::make_shared<TransitionCache>())
    , m_profilerService(demoAppConfig.DemoServiceProvider.Get<IProfilerService>())
    , m_hProfileCounterUpdate(m_profilerService, m_profilerService->CreateCustomCounter("update", 0, 200, DefaultProfilerColors::UIUpdate))
    , m_hProfileCounterResolve(m_profilerService, m_profilerService->CreateCustomCounter("resolve", 0, 200, DefaultProfilerColors::UIResolve))
    , m_hProfileCounterDraw(m_profilerService, m_profilerService->CreateCustomCounter("draw", 0, 200, DefaultProfilerColors::UIDraw))
    , m_hProfileCounterWin(m_profilerService, m_profilerService->CreateCustomCounter("win", 0, 200, DefaultProfilerColors::UIWinCount))
  {
    m_uiManager.RegisterEventListener(eventListener);
  }


  UIDemoAppExtensionBase::~UIDemoAppExtensionBase() = default;

  void UIDemoAppExtensionBase::RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager.RegisterEventListener(eventListener);
  }

  void UIDemoAppExtensionBase::UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager.UnregisterEventListener(eventListener);
  }


  void UIDemoAppExtensionBase::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_uiManager.SendMouseButtonEvent(event);
  }


  void UIDemoAppExtensionBase::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    m_uiManager.SendMouseMoveEvent(event);
  }

  void UIDemoAppExtensionBase::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_uiManager.Resized(windowMetrics);
  }

  void UIDemoAppExtensionBase::PreUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_uiManager.ProcessEvents();
  }

  void UIDemoAppExtensionBase::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    m_uiManager.ProcessEvents();
  }


  void UIDemoAppExtensionBase::PostUpdate(const DemoTime& demoTime)
  {
    // We call the UIManager in post update to allow the app to modify the UI in its update method
    m_uiManager.Update(demoTime);
  }

  bool UIDemoAppExtensionBase::IsIdle() const
  {
    return m_uiManager.IsIdle();
  }

  void UIDemoAppExtensionBase::DoDraw()
  {
    m_uiManager.Draw();

    const auto stats = m_uiManager.GetStats();
    m_profilerService->Set(m_hProfileCounterUpdate, UncheckedNumericCast<int32_t>(stats.UpdateCalls));
    m_profilerService->Set(m_hProfileCounterResolve, UncheckedNumericCast<int32_t>(stats.ResolveCalls));
    m_profilerService->Set(m_hProfileCounterDraw, UncheckedNumericCast<int32_t>(stats.DrawCalls));
    m_profilerService->Set(m_hProfileCounterWin, UncheckedNumericCast<int32_t>(stats.WindowCount));
  }

}
