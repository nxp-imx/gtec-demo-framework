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
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionTimeSpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Profiler/DefaultProfilerColors.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslSimpleUI/App/DemoPerformanceCapture.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionCreateInfo.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystem.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemCreateInfo.hpp>
#include <FslSimpleUI/Render/IMBatch/RenderSystemFactory.hpp>
#include <utility>

namespace Fsl
{
  namespace
  {
    BasicWindowMetrics Convert(const DemoWindowMetrics& windowMetrics)
    {
      return {windowMetrics.ExtentPx, windowMetrics.ExactDpi, windowMetrics.DensityDpi};
    }

    TransitionTimeSpan Convert(const DemoTime& time)
    {
      return {time.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds};
    }

    std::unique_ptr<UI::IRenderSystem> CreateUIRenderSystem(const UI::IRenderSystemFactory& factory, IGraphicsService& graphicsService,
                                                            const UIDemoAppExtensionCreateInfo& createInfo,
                                                            const SpriteMaterialInfo& defaultMaterialInfo)
    {
      UI::RenderSystemCreateInfo renderSystemCreateInfo(Convert(createInfo.WindowMetrics), graphicsService.GetBasicRenderSystem(),
                                                        defaultMaterialInfo, createInfo.RenderCreateInfo.MaterialConfig.AllowDepthBuffer,
                                                        createInfo.RenderCreateInfo.Capacity.InitalVertexCapacity,
                                                        createInfo.RenderCreateInfo.Capacity.InitalIndexCapacity);
      return factory.Create(renderSystemCreateInfo);
    }


    std::unique_ptr<UI::UIManager> CreateUIManager(const UIDemoAppExtensionCreateInfo& createInfo)
    {
      UI::RenderIMBatch::RenderSystemFactory defaultFactory;

      const UI::IRenderSystemFactory& factory = createInfo.pRenderSystemFactory != nullptr ? *createInfo.pRenderSystemFactory : defaultFactory;
      auto graphicsService = createInfo.DemoServiceProvider.Get<IGraphicsService>();


      return std::make_unique<UI::UIManager>(
        CreateUIRenderSystem(factory, *graphicsService, createInfo,
                             UIAppDefaultMaterial::CreateDefaultMaterial(createInfo.DemoServiceProvider, factory.GetVertexDeclarationSpan(),
                                                                         createInfo.RenderCreateInfo.MaterialCreateInfo.AllowDynamicCustomViewport,
                                                                         createInfo.RenderCreateInfo.MaterialConfig.AllowDepthBuffer)),
        graphicsService->GetNativeBatch2D()->SYS_IsTextureCoordinateYFlipped(), Convert(createInfo.WindowMetrics),
        createInfo.ExternalModuleFactories);
    }
  }
  UIDemoAppExtensionBase::UIDemoAppExtensionBase(const UIDemoAppExtensionCreateInfo& createInfo,
                                                 const std::shared_ptr<UI::IEventListener>& eventListener)
    : m_uiManager(CreateUIManager(createInfo))
    , m_transitionCache(std::make_shared<TransitionCache>())
    , m_demoPerformanceCapture(createInfo.Profiler)
    , m_profilerService(createInfo.DemoServiceProvider.Get<IProfilerService>())
    , m_hProfileCounterUpdate(m_profilerService, m_profilerService->CreateCustomCounter("update", 0, 200, DefaultProfilerColors::UIUpdate))
    , m_hProfileCounterResolve(m_profilerService, m_profilerService->CreateCustomCounter("resolve", 0, 200, DefaultProfilerColors::UIResolve))
    , m_hProfileCounterDraw(m_profilerService, m_profilerService->CreateCustomCounter("draw", 0, 200, DefaultProfilerColors::UIDraw))
    , m_hProfileCounterWin(m_profilerService, m_profilerService->CreateCustomCounter("win", 0, 200, DefaultProfilerColors::UIWinCount))
  {
    m_uiManager->RegisterEventListener(eventListener);
  }


  UIDemoAppExtensionBase::~UIDemoAppExtensionBase()
  {
    if (m_mainWindow)
    {
      auto windowManager = GetWindowManager();
      windowManager->ScheduleClose(m_mainWindow);
      m_mainWindow.reset();
    }
  }


  void UIDemoAppExtensionBase::SYS_SetRenderSystemViewport(const PxViewport& viewportPx)
  {
    FSL_PARAM_NOT_USED(viewportPx);
  }

  void UIDemoAppExtensionBase::RegisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager->RegisterEventListener(eventListener);
  }

  void UIDemoAppExtensionBase::UnregisterEventListener(const std::shared_ptr<UI::IEventListener>& eventListener)
  {
    m_uiManager->UnregisterEventListener(eventListener);
  }


  void UIDemoAppExtensionBase::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.GetButton() != VirtualMouseButton::Left)
    {
      return;
    }

    const bool isHandled = m_uiManager->SendMouseButtonEvent(event.GetPosition(), event.IsPressed());
    if (isHandled && !event.IsHandled())
    {
      event.Handled();
    }
  }


  void UIDemoAppExtensionBase::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    const bool isHandled = m_uiManager->SendMouseMoveEvent(event.GetPosition());
    if (isHandled && !event.IsHandled())
    {
      event.Handled();
    }
  }

  void UIDemoAppExtensionBase::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_uiManager->Resized(Convert(windowMetrics));
  }

  void UIDemoAppExtensionBase::PreUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    DemoPerformanceCapture* pDemoPerformanceCapture = TryGetDemoPerformanceCapture();
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->BeginProfile(DemoPerformanceCaptureId::UIProcessEvents);
    }
    m_uiManager->ProcessEvents();
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->EndProfile(DemoPerformanceCaptureId::UIProcessEvents);
    }
  }

  void UIDemoAppExtensionBase::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    DemoPerformanceCapture* pDemoPerformanceCapture = TryGetDemoPerformanceCapture();
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->BeginProfile(DemoPerformanceCaptureId::UIProcessEvents);
    }
    m_uiManager->ProcessEvents();
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->EndProfile(DemoPerformanceCaptureId::UIProcessEvents);
    }
  }


  void UIDemoAppExtensionBase::PostUpdate(const DemoTime& demoTime)
  {
    DemoPerformanceCapture* pDemoPerformanceCapture = TryGetDemoPerformanceCapture();
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->BeginProfile(DemoPerformanceCaptureId::UIUpdate);
    }
    // We call the UIManager in post update to allow the app to modify the UI in its update method
    m_uiManager->Update(Convert(demoTime));
    if (pDemoPerformanceCapture != nullptr)
    {
      pDemoPerformanceCapture->EndProfile(DemoPerformanceCaptureId::UIUpdate);
    }
  }


  void UIDemoAppExtensionBase::SetMainWindow(const std::shared_ptr<UI::BaseWindow>& mainWindow)
  {
    if (mainWindow != m_mainWindow)
    {
      auto windowManager = GetWindowManager();
      // Schedule a close of the old window
      if (m_mainWindow)
      {
        windowManager->ScheduleClose(m_mainWindow);
        m_mainWindow.reset();
      }
      if (mainWindow)
      {    // add the new one
        m_mainWindow = mainWindow;
        windowManager->Add(m_mainWindow);
      }
    }
  }

  std::shared_ptr<UI::IWindowManager> UIDemoAppExtensionBase::GetWindowManager() const
  {
    return m_uiManager->GetWindowManager();
  }

  const std::shared_ptr<UI::UIContext>& UIDemoAppExtensionBase::GetUIContext() const
  {
    return m_uiManager->GetUIContext();
  }


  bool UIDemoAppExtensionBase::IsIdle() const
  {
    return m_uiManager->IsIdle();
  }


  const UI::IRenderSystemBase& UIDemoAppExtensionBase::GetRenderSystem() const
  {
    return m_uiManager->GetRenderSystem();
  }

  UI::IRenderSystemBase* UIDemoAppExtensionBase::TryGetRenderSystem()
  {
    return m_uiManager->TryGetRenderSystem();
  }

  void UIDemoAppExtensionBase::ForceInvalidateLayout()
  {
    m_uiManager->ForceInvalidateLayout();
  }


  DemoPerformanceCapture* UIDemoAppExtensionBase::TryGetDemoPerformanceCapture() const
  {
    return m_demoPerformanceCapture.get();
  }


  void UIDemoAppExtensionBase::DoDraw()
  {
    m_uiManager->PreDraw();
    DemoPerformanceCapture* pDemoPerformanceCapture = TryGetDemoPerformanceCapture();

    if (pDemoPerformanceCapture == nullptr)
    {
      m_uiManager->Draw(nullptr);
    }
    else
    {
      UI::RenderPerformanceCapture performanceCapture;
      pDemoPerformanceCapture->BeginProfile(DemoPerformanceCaptureId::UIDraw);
      m_uiManager->Draw(&performanceCapture);
      pDemoPerformanceCapture->EndProfile(DemoPerformanceCaptureId::UIDraw);
      pDemoPerformanceCapture->SetRenderPerformanceCapture(performanceCapture);
    }

    m_uiManager->PostDraw();

    const auto stats = m_uiManager->GetStats();
    m_profilerService->Set(m_hProfileCounterUpdate, UncheckedNumericCast<int32_t>(stats.UpdateCalls));
    m_profilerService->Set(m_hProfileCounterResolve, UncheckedNumericCast<int32_t>(stats.ResolveCalls));
    m_profilerService->Set(m_hProfileCounterDraw, UncheckedNumericCast<int32_t>(stats.DrawCalls));
    m_profilerService->Set(m_hProfileCounterWin, UncheckedNumericCast<int32_t>(stats.WindowCount));
  }

  bool UIDemoAppExtensionBase::SYS_GetUseYFlipTextureCoordinates() const
  {
    return m_uiManager->SYS_GetUseYFlipTextureCoordinates();
  }

  std::shared_ptr<UI::AExternalModule> UIDemoAppExtensionBase::DoGetExternalModule(const UI::ExternalModuleId& moduleId) const
  {
    return m_uiManager->GetExternalModule(moduleId);
  }
}
