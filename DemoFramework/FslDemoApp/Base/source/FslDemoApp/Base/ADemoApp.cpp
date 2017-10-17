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

#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/ADemoApp.hpp>
#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <FslDemoApp/Base/Service/Events/IEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Exceptions.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace
  {
    void ToggleTimestep(const std::shared_ptr<IDemoAppControl> demoAppControl, const TimeStepMode mode)
    {
      const TimeStepMode newTimeStepMode = demoAppControl->GetTimeStepMode() != mode ? mode : TimeStepMode::Normal;
      demoAppControl->SetTimeStepMode(newTimeStepMode);
    }

    struct ExtensionCompare
    {
      std::shared_ptr<DemoAppExtension> m_ext;

      ExtensionCompare(const std::shared_ptr<DemoAppExtension>& ext)
        : m_ext(ext)
      {
      }

      inline bool operator()(const std::weak_ptr<DemoAppExtension>& value) const
      {
        auto val = value.lock();
        return (val == m_ext);
      }
    };


    struct PredMethodOnKeyEvent
    {
      const KeyEvent& m_rParam0;
      PredMethodOnKeyEvent(const KeyEvent& rParam0) : m_rParam0(rParam0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->OnKeyEvent(m_rParam0);
      }
    };


    struct PredMethodOnMouseButtonEvent
    {
      const MouseButtonEvent& m_rParam0;
      PredMethodOnMouseButtonEvent(const MouseButtonEvent& rParam0) : m_rParam0(rParam0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->OnMouseButtonEvent(m_rParam0);
      }
    };

    struct PredMethodOnMouseMoveEvent
    {
      const MouseMoveEvent& m_rParam0;
      PredMethodOnMouseMoveEvent(const MouseMoveEvent& rParam0) : m_rParam0(rParam0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->OnMouseMoveEvent(m_rParam0);
      }
    };

    struct PredMethodOnMouseWheelEvent
    {
      const MouseWheelEvent& m_rParam0;
      PredMethodOnMouseWheelEvent(const MouseWheelEvent& rParam0) : m_rParam0(rParam0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->OnMouseWheelEvent(m_rParam0);
      }
    };

    struct PredMethodOnTimeStateEvent
    {
      const TimeStateEvent& m_rParam0;
      PredMethodOnTimeStateEvent(const TimeStateEvent& rParam0) : m_rParam0(rParam0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->OnTimeStateEvent(m_rParam0);
      }
    };


    struct PredMethodResized
    {
      Point2 m_param0;
      PredMethodResized(const Point2& param0) : m_param0(param0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->Resized(m_param0);
      }
    };

    struct PredMethodFixedUpdate
    {
      DemoTime m_param0;
      PredMethodFixedUpdate(const DemoTime& param0) : m_param0(param0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->FixedUpdate(m_param0);
      }
    };

    struct PredMethodUpdate
    {
      DemoTime m_param0;
      PredMethodUpdate(const DemoTime& param0) : m_param0(param0) {}

      inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
      {
        assert(value);
        value->Update(m_param0);
      }
    };

    //struct PredMethodDraw
    //{
    //  inline void operator()(const std::shared_ptr<DemoAppExtension>& value) const
    //  {
    //    assert(value);
    //    value->Draw();
    //  }
    //};

    //! @brief Loop over all the registered extensions calling the predicate for all valid extensions and removing all invalid ones.
    template<typename TPred>
    inline void CallExtensions(std::deque<std::weak_ptr<DemoAppExtension> >& rExtensions, const TPred& predicate)
    {
      auto itr = rExtensions.begin();
      while(itr != rExtensions.end())
      {
        auto extension = itr->lock();
        if (extension)
        {
          predicate(extension);
          ++itr;
        }
        else
          itr = rExtensions.erase(itr);
      }
    }

  }


  ADemoApp::ADemoApp(const DemoAppConfig& demoAppConfig)
    : m_demoAppConfig(demoAppConfig)
    , m_contentManger()
    , m_persistentDataManager()
    , m_demoAppControl()
    , m_extensions()
  {
    m_contentManger = demoAppConfig.DemoServiceProvider.Get<IContentManager>();
    m_persistentDataManager = demoAppConfig.DemoServiceProvider.Get<IPersistentDataManager>();
    m_demoAppControl = demoAppConfig.DemoServiceProvider.Get<IDemoAppControl>();
  }


  void ADemoApp::RegisterExtension(const std::shared_ptr<DemoAppExtension>& extension)
  {
    auto itr = std::find_if(m_extensions.begin(), m_extensions.end(), ExtensionCompare(extension));
    if (itr != m_extensions.end())
      throw UsageErrorException("A extension can only be registered once");

    m_extensions.push_back(extension);
  }


  void ADemoApp::UnregisterExtension(const std::shared_ptr<DemoAppExtension>& extension)
  {
    auto itr = std::find_if(m_extensions.begin(), m_extensions.end(), ExtensionCompare(extension));
    if (itr == m_extensions.end())
      return;

    // Remove the extension
    m_extensions.erase(itr);
  }


  void ADemoApp::_PostConstruct()
  {
  }


  void ADemoApp::_OnEvent(IEvent*const pEvent)
  {
    // Done this way to prevent common mistakes where people forget to call the base class
    const EventType eventType = pEvent->GetEventType();

    if ((static_cast<uint32_t>(eventType)& static_cast<uint32_t>(EventType::ComplexEvent)) == 0)
    {
      BasicEvent* pBasicEvent = dynamic_cast<BasicEvent*>(pEvent);
      assert(pBasicEvent != nullptr);

      switch (pEvent->GetEventType())
      {
      case EventType::KeyPressed:
      {
        KeyEvent keyEvent(*pBasicEvent);

        // Call all registered extensions
        CallExtensions(m_extensions, PredMethodOnKeyEvent(keyEvent));

        OnKeyEvent(keyEvent);
        if (!keyEvent.IsHandled())
          UnhandledKeyFallback(keyEvent);
        break;
      }
      case EventType::MouseButton:
      {
        MouseButtonEvent mouseEvent(*pBasicEvent);
        // Call all registered extensions
        CallExtensions(m_extensions, PredMethodOnMouseButtonEvent(mouseEvent));

        OnMouseButtonEvent(mouseEvent);
        break;
      }
      case EventType::MouseMove:
      {
        MouseMoveEvent mouseEvent(*pBasicEvent);
        // Call all registered extensions
        CallExtensions(m_extensions, PredMethodOnMouseMoveEvent(mouseEvent));
        OnMouseMoveEvent(mouseEvent);
        break;
      }
      case EventType::MouseWheel:
      {
        MouseWheelEvent mouseEvent(*pBasicEvent);
        // Call all registered extensions
        CallExtensions(m_extensions, PredMethodOnMouseWheelEvent(mouseEvent));
        OnMouseWheelEvent(mouseEvent);
        break;
      }
      case EventType::TimeState:
      {
        TimeStateEvent timeStateEvent(*pBasicEvent);
        // Call all registered extensions
        CallExtensions(m_extensions, PredMethodOnTimeStateEvent(timeStateEvent));
        OnTimeStateEvent(timeStateEvent);
        break;
      }
      default:
        FSLLOG_DEBUG_WARNING("Unhandled event: " << static_cast<int32_t>(pEvent->GetEventType()));
        break;
      }
    }

  }


  void ADemoApp::_Resized(const Point2& size)
  {
    m_demoAppConfig.ScreenResolution = size;

    // Call all registered extensions
    CallExtensions(m_extensions, PredMethodResized(size));

    // Done this way to prevent common mistakes where people forget to call the base class
    Resized(size);
  }


  void ADemoApp::_PreUpdate(const DemoTime& demoTime)
  {
    // Call all registered extensions
    //CallExtensions(m_extensions, PredMethodPreUpdate(demoTime));

    // Done this way to prevent common mistakes where people forget to call the base class
    PreUpdate(demoTime);
  }

  void ADemoApp::_FixedUpdate(const DemoTime& demoTime)
  {
    // Call all registered extensions
    CallExtensions(m_extensions, PredMethodFixedUpdate(demoTime));

    // Done this way to prevent common mistakes where people forget to call the base class
    FixedUpdate(demoTime);
  }


  void ADemoApp::_Update(const DemoTime& demoTime)
  {
    // Call all registered extensions
    CallExtensions(m_extensions, PredMethodUpdate(demoTime));

    // Done this way to prevent common mistakes where people forget to call the base class
    Update(demoTime);
  }


  void ADemoApp::_Draw(const DemoTime& demoTime)
  {
    // Call all registered extensions
    //CallExtensions(m_extensions, PredMethodDraw());

    // Done this way to prevent common mistakes where people forget to call the base class
    Draw(demoTime);
  }


  Point2 ADemoApp::GetScreenResolution() const
  {
    return m_demoAppConfig.ScreenResolution;
  }


  Extent2D ADemoApp::GetScreenExtent() const
  {
    return Extent2D(m_demoAppConfig.ScreenResolution);
  }


  std::shared_ptr<IDemoAppControl> ADemoApp::GetDemoAppControl() const
  {
    const std::shared_ptr<IDemoAppControl> demoAppControl = m_demoAppControl.lock();
    if (!demoAppControl)
      throw ServiceUnavailableException("The service is no longer available");
    return demoAppControl;
  }


  std::shared_ptr<IContentManager> ADemoApp::GetContentManager() const
  {
    const std::shared_ptr<IContentManager> contentManager = m_contentManger.lock();
    if (!contentManager)
      throw ServiceUnavailableException("The service is no longer available");
    return contentManager;
  }


  std::shared_ptr<IPersistentDataManager> ADemoApp::GetPersistentDataManager() const
  {
    const std::shared_ptr<IPersistentDataManager> manager = m_persistentDataManager.lock();
    if (!manager)
      throw ServiceUnavailableException("The service is no longer available");
    return manager;
  }


  const ExceptionMessageFormatter& ADemoApp::GetExceptionFormatter() const
  {
    return m_demoAppConfig.ExceptionFormatter;
  }


  void ADemoApp::UnhandledKeyFallback(const KeyEvent& keyEvent)
  {
    if (!keyEvent.IsPressed())
      return;


    const std::shared_ptr<IDemoAppControl> demoAppControl = GetDemoAppControl();
    switch (keyEvent.GetKey())
    {
    case VirtualKey::Escape:
      demoAppControl->RequestExit();
      break;
    case VirtualKey::F4:
      demoAppControl->RequestScreenshot();
      break;
    case VirtualKey::F5:
      demoAppControl->RequestAppRestart();
      break;
    case VirtualKey::Pause:
      ToggleTimestep(demoAppControl, TimeStepMode::Paused);
      break;
    case VirtualKey::PageDown:
      demoAppControl->SetTimeStepMode(TimeStepMode::Step);
      break;
    case VirtualKey::Delete:
      ToggleTimestep(demoAppControl, TimeStepMode::Slow2X);
      break;
    case VirtualKey::End:
      ToggleTimestep(demoAppControl, TimeStepMode::Slow4X);
      break;
    case VirtualKey::Insert:
      ToggleTimestep(demoAppControl, TimeStepMode::Fast2X);
      break;
    case VirtualKey::Home:
      ToggleTimestep(demoAppControl, TimeStepMode::Fast4X);
      break;
    default:
      break;
    }
  }

}
