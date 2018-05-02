/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/Base/DemoAppFirewall.hpp>
#include <FslDemoApp/Base/Host/IDemoAppFactory.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/Base/Service/Exceptions.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <cassert>

namespace Fsl
{

  DemoAppFirewall::DemoAppFirewall(const DemoAppConfig& demoAppConfig, const std::shared_ptr<IDemoAppFactory>& appFactory)
    : ADemoApp(demoAppConfig)
    , m_app()
    , m_errorString()
    , m_basic2D()
  {
    { // Allocate basic 2D if available
      auto graphicsService = demoAppConfig.DemoServiceProvider.TryGet<IGraphicsService>();
      if (graphicsService)
        m_basic2D = graphicsService->GetBasic2D();
    }

    try
    {
      m_app = appFactory->Allocate(demoAppConfig);
    }
    catch (const std::exception& ex)
    {
      FSLLOG_ERROR("App allocation threw exception: " << ex.what());
      SafeDispose();
      BuildErrorString("App threw exception on construction:", ex);
    }
  }


  DemoAppFirewall::~DemoAppFirewall()
  {
    SafeDispose();
  }


  void DemoAppFirewall::_PostConstruct()
  {
    if (!m_app)
    {
      ADemoApp::_PostConstruct();
      return;
    }

    try
    {
      m_app->_PostConstruct();
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._PostConstruct threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._PostConstruct threw exception:", message);
    }
  }


  void DemoAppFirewall::_OnEvent(IEvent*const pEvent)
  {
    if ( ! m_app )
    {
      ADemoApp::_OnEvent(pEvent);
      return;
    }

    try
    {
      m_app->_OnEvent(pEvent);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._OnEvent threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._OnEvent threw exception:", message);
    }
  }


  void DemoAppFirewall::_Resized(const Point2& size)
  {
    if (!m_app)
    {
      ADemoApp::_Resized(size);
      return;
    }

    try
    {
      m_app->_Resized(size);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._Resized threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._Resized threw exception:", message);
    }
  }


  void DemoAppFirewall::_PreUpdate(const DemoTime& demoTime)
  {
    if (!m_app)
    {
      ADemoApp::_PreUpdate(demoTime);
      return;
    }

    try
    {
      m_app->_PreUpdate(demoTime);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._PreUpdate threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._PreUpdate threw exception:", message);
    }
  }


  void DemoAppFirewall::_FixedUpdate(const DemoTime& demoTime)
  {
    if (!m_app)
    {
      ADemoApp::_FixedUpdate(demoTime);
      return;
    }

    try
    {
      m_app->_FixedUpdate(demoTime);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._FixedUpdate threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._FixedUpdate threw exception:", message);
    }
  }


  void DemoAppFirewall::_Update(const DemoTime& demoTime)
  {
    if (!m_app)
    {
      ADemoApp::_Update(demoTime);
      return;
    }

    try
    {
      m_app->_Update(demoTime);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._Update threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._Update threw exception:", message);
    }
  }


  void DemoAppFirewall::_PostUpdate(const DemoTime& demoTime)
  {
    if (!m_app)
    {
      ADemoApp::_PostUpdate(demoTime);
      return;
    }

    try
    {
      m_app->_PostUpdate(demoTime);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._PostUpdate threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._PostUpdate threw exception:", message);
    }
  }

  void DemoAppFirewall::_Draw(const DemoTime& demoTime)
  {
    if (!m_app)
    {
      ADemoApp::_Draw(demoTime);
      if (m_basic2D)
      {
        m_basic2D->Begin();
        m_basic2D->DrawString(m_errorString, Vector2::Zero());
        m_basic2D->End();
      }
      else
      {
        FSLLOG(m_errorString);
      }
      return;
    }

    try
    {
      m_app->_Draw(demoTime);
    }
    catch (const std::exception& ex)
    {
      std::string message;
      message = GetExceptionFormatter().TryFormatException(ex, message) ? message : ex.what();
      FSLLOG_ERROR("App._Draw threw exception: " << message);
      SafeDispose();
      BuildErrorString("App._Draw threw exception:", message);
    }
  }


  void DemoAppFirewall::SafeDispose()
  {
    if (!m_app)
      return;

    try
    {
      m_app.reset();
    }
    catch (const std::exception& ex)
    {
      FSLLOG_ERROR("App.Destructors are not allowed to throw, but this one did. Exception: " << ex.what());
    }
  }


  void DemoAppFirewall::BuildErrorString(const std::string& desc, const std::exception& ex)
  {
    m_errorString = desc + " '" + ex.what() + "'";
  }


  void DemoAppFirewall::BuildErrorString(const std::string& desc, const std::string& message)
  {
    m_errorString = desc + " '" + message + "'";
  }


}
