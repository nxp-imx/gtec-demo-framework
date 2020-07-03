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

#include <FslDemoApp/Base/AConsoleDemoApp.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/IEvent.hpp>
#include <FslDemoApp/Base/Service/Exceptions.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  AConsoleDemoApp::AConsoleDemoApp(const DemoAppConfig& demoAppConfig)
    : m_demoAppConfig(demoAppConfig)
    , m_contentManger(demoAppConfig.DemoServiceProvider.Get<IContentManager>())
    , m_persistentDataManager(demoAppConfig.DemoServiceProvider.Get<IPersistentDataManager>())
    , m_demoAppControl(demoAppConfig.DemoServiceProvider.Get<IDemoAppControl>())

  {
  }


  AConsoleDemoApp::~AConsoleDemoApp() = default;


  void AConsoleDemoApp::_PostConstruct()
  {
  }


  void AConsoleDemoApp::_PreDestruct()
  {
    m_destroyed = true;
  }


  void AConsoleDemoApp::_OnEvent(IEvent* const pEvent)
  {
    FSL_PARAM_NOT_USED(pEvent);
    //// Done this way to prevent common mistakes where people forget to call the base class
    // const EventType eventType = pEvent->GetEventType();

    // if ((eventType & EventType::ComplexEvent) == 0)
    //{
    //  FSLLOG3_DEBUG_WARNING("Unhandled event: " << static_cast<int32_t>(pEvent->GetEventType()));
    //}
  }


  void AConsoleDemoApp::_ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }


  void AConsoleDemoApp::_PreUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  void AConsoleDemoApp::_FixedUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  void AConsoleDemoApp::_Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  void AConsoleDemoApp::_PostUpdate(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }


  AppDrawResult AConsoleDemoApp::_TryPrepareDraw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    return AppDrawResult::Completed;
  }


  void AConsoleDemoApp::_Draw(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    Run();

    // Ensure that app is shutdown
    auto appControl = GetDemoAppControl();
    if (!appControl->HasExitRequest())
    {
      appControl->RequestExit();
    }
  }


  std::shared_ptr<IDemoAppControl> AConsoleDemoApp::GetDemoAppControl() const
  {
    std::shared_ptr<IDemoAppControl> demoAppControl = m_demoAppControl.lock();
    if (!demoAppControl)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return demoAppControl;
  }


  std::shared_ptr<IContentManager> AConsoleDemoApp::GetContentManager() const
  {
    std::shared_ptr<IContentManager> contentManager = m_contentManger.lock();
    if (!contentManager)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return contentManager;
  }


  std::shared_ptr<IPersistentDataManager> AConsoleDemoApp::GetPersistentDataManager() const
  {
    std::shared_ptr<IPersistentDataManager> manager = m_persistentDataManager.lock();
    if (!manager)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return manager;
  }
}
