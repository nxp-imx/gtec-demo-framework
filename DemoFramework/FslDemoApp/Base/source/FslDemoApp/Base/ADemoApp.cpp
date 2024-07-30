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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDemoApp/Base/ADemoApp.hpp>
#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/IEvent.hpp>
#include <FslDemoApp/Base/Service/Exceptions.hpp>
#include <FslDemoApp/Shared/Log/Host/FmtDemoWindowMetrics.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

#if 0
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG(X) FSLLOG3_VERBOSE(X)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG(X) \
  {                    \
  }
#endif

namespace Fsl
{
  namespace
  {
    void ToggleTimestep(const std::shared_ptr<IDemoAppControl>& demoAppControl, const TimeStepMode mode)
    {
      const TimeStepMode newTimeStepMode = demoAppControl->GetTimeStepMode() != mode ? mode : TimeStepMode::Normal;
      demoAppControl->SetTimeStepMode(newTimeStepMode);
    }

    struct ExtensionCompare
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::shared_ptr<IDemoAppExtension> m_ext;

      explicit ExtensionCompare(std::shared_ptr<IDemoAppExtension> ext)
        : m_ext(std::move(ext))
      {
      }

      inline bool operator()(const std::weak_ptr<IDemoAppExtension>& value) const
      {
        auto val = value.lock();
        return (val == m_ext);
      }
    };


    //! @brief Loop over all the registered extensions calling the predicate for all valid extensions and removing all invalid ones.
    template <typename TPred>
    inline void CallExtensions(std::deque<std::weak_ptr<IDemoAppExtension>>& rExtensions, const TPred& predicate)
    {
      auto itr = rExtensions.begin();
      while (itr != rExtensions.end())
      {
        auto extension = itr->lock();
        if (extension)
        {
          predicate(*extension);
          ++itr;
        }
        else
        {
          itr = rExtensions.erase(itr);
        }
      }
    }

    //! @brief Loop over all the registered extensions calling the predicate for all valid extensions and removing all invalid ones.
    template <typename TPred>
    inline void CallExtensionsPre(std::deque<std::weak_ptr<IDemoAppExtension>>& rExtensions, const TPred& predicate)
    {
      auto itr = rExtensions.begin();
      while (itr != rExtensions.end())
      {
        auto extension = itr->lock();
        if (extension)
        {
          predicate(*extension, DemoAppExtensionCallOrder::PreApp);
          ++itr;
        }
        else
        {
          itr = rExtensions.erase(itr);
        }
      }
    }

    //! @brief Reverse loop over all the registered extensions calling the predicate for all valid extensions
    template <typename TPred>
    inline void CallExtensionsPost(std::deque<std::weak_ptr<IDemoAppExtension>>& rExtensions, const TPred& predicate)
    {
      // Post extensions are called in reverse order
      auto itr = rExtensions.rbegin();
      while (itr != rExtensions.rend())
      {
        auto extension = itr->lock();
        if (extension)
        {
          predicate(*extension, DemoAppExtensionCallOrder::PostApp);
          ++itr;
        }
      }
    }


  }


  ADemoApp::ADemoApp(const DemoAppConfig& demoAppConfig)
    : m_demoAppConfig(demoAppConfig)

  {
    FSLLOG3_VERBOSE("ADemopApp::ADemopApp()");

    if (demoAppConfig.CustomConfig.MaxFramesInFlight <= 0u)
    {
      throw std::invalid_argument("MaxFramesInFlight can not be <= 0");
    }

    m_renderConfig = RenderConfig(demoAppConfig.CustomConfig.MaxFramesInFlight);

    m_contentManger = demoAppConfig.DemoServiceProvider.Get<IContentManager>();
    m_persistentDataManager = demoAppConfig.DemoServiceProvider.Get<IPersistentDataManager>();
    m_demoAppControl = demoAppConfig.DemoServiceProvider.Get<IDemoAppControl>();
  }


  ADemoApp::~ADemoApp()
  {
    FSLLOG3_VERBOSE("ADemopApp::~ADemopApp()");
  }


  void ADemoApp::RegisterExtension(const std::shared_ptr<IDemoAppExtension>& extension)
  {
    auto itr = std::find_if(m_extensions.begin(), m_extensions.end(), ExtensionCompare(extension));
    if (itr != m_extensions.end())
    {
      throw UsageErrorException("A extension can only be registered once");
    }

    m_extensions.push_back(extension);
  }


  void ADemoApp::UnregisterExtension(const std::shared_ptr<IDemoAppExtension>& extension)
  {
    auto itr = std::find_if(m_extensions.begin(), m_extensions.end(), ExtensionCompare(extension));
    if (itr == m_extensions.end())
    {
      return;
    }

    // Remove the extension
    m_extensions.erase(itr);
  }

  void ADemoApp::_PostConstruct()
  {
    FSLLOG3_VERBOSE("ADemoApp::_PostConstruct()");

    // When this method is called, the object should be successfully constructed (unless someone calls it from a constructor, but thats a usage error
    // and its undetectable). So don't do that!
    m_currentLifeCycleState = ObjectLifeCycle::Constructed;
    OnConstructed();
  }

  void ADemoApp::_PreDestruct()
  {
    FSLLOG3_VERBOSE("ADemoApp::_PreDestruct()");

    // Give the app a chance to cleanup
    try
    {
      OnDestroy();
      m_currentLifeCycleState = ObjectLifeCycle::Destroyed;
    }
    catch (const std::exception&)
    {
      m_currentLifeCycleState = ObjectLifeCycle::Destroyed;
      throw;
    }
  }

  void ADemoApp::_OnEvent(IEvent* const pEvent)
  {
    VERBOSE_LOG("ADemoApp::_OnEvent()");

    // Done this way to prevent common mistakes where people forget to call the base class
    const EventType eventType = pEvent->GetEventType();

    if ((static_cast<uint32_t>(eventType) & static_cast<uint32_t>(EventType::ComplexEvent)) == 0)
    {
      auto* pBasicEvent = dynamic_cast<BasicEvent*>(pEvent);
      if (pBasicEvent == nullptr)
      {
        throw std::runtime_error("the event was not a BasicEvent as expected");
      }

      switch (pEvent->GetEventType())
      {
      case EventType::KeyPressed:
        {
          KeyEvent keyEvent(*pBasicEvent);

          // Call all registered extensions
          CallExtensions(m_extensions, [keyEvent](IDemoAppExtension& rExt) { rExt.OnKeyEvent(keyEvent); });

          OnKeyEvent(keyEvent);
          if (!keyEvent.IsHandled())
          {
            UnhandledKeyFallback(keyEvent);
          }
          break;
        }
      case EventType::MouseButton:
        {
          MouseButtonEvent mouseEvent(*pBasicEvent);
          // Call all registered extensions
          CallExtensions(m_extensions, [mouseEvent](IDemoAppExtension& rExt) { rExt.OnMouseButtonEvent(mouseEvent); });

          OnMouseButtonEvent(mouseEvent);
          break;
        }
      case EventType::MouseMove:
        {
          MouseMoveEvent mouseEvent(*pBasicEvent);
          // Call all registered extensions
          CallExtensions(m_extensions, [mouseEvent](IDemoAppExtension& rExt) { rExt.OnMouseMoveEvent(mouseEvent); });
          OnMouseMoveEvent(mouseEvent);
          break;
        }
      case EventType::MouseWheel:
        {
          MouseWheelEvent mouseEvent(*pBasicEvent);
          // Call all registered extensions
          CallExtensions(m_extensions, [mouseEvent](IDemoAppExtension& rExt) { rExt.OnMouseWheelEvent(mouseEvent); });
          OnMouseWheelEvent(mouseEvent);
          break;
        }
      case EventType::RawMouseMove:
        {
          RawMouseMoveEvent mouseEvent(*pBasicEvent);
          // Call all registered extensions
          CallExtensions(m_extensions, [mouseEvent](IDemoAppExtension& rExt) { rExt.OnRawMouseMoveEvent(mouseEvent); });
          OnRawMouseMoveEvent(mouseEvent);
          break;
        }
      case EventType::TimeState:
        {
          TimeStateEvent timeStateEvent(*pBasicEvent);
          // Call all registered extensions
          CallExtensions(m_extensions, [timeStateEvent](IDemoAppExtension& rExt) { rExt.OnTimeStateEvent(timeStateEvent); });
          OnTimeStateEvent(timeStateEvent);
          break;
        }
      default:
        FSLLOG3_DEBUG_WARNING("Unhandled event: {}", static_cast<int32_t>(pEvent->GetEventType()));
        break;
      }
    }
  }


  void ADemoApp::_ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSLLOG3_VERBOSE("ADemoApp::_ConfigurationChanged({})", windowMetrics);

    m_demoAppConfig.UpdateWindowMetrics(windowMetrics);

    // Call all registered extensions
    CallExtensions(m_extensions, [windowMetrics](IDemoAppExtension& rExt) { rExt.ConfigurationChanged(windowMetrics); });

    ConfigurationChanged(windowMetrics);
  }

  void ADemoApp::_Begin()
  {
    VERBOSE_LOG("ADemoApp::_Begin()");

    auto fn = [](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.Begin(callOrder); };
    CallExtensionsPre(m_extensions, fn);
    OnFrameSequenceBegin();
    CallExtensionsPost(m_extensions, fn);
  }

  void ADemoApp::_PreUpdate(const DemoTime& demoTime)
  {
    VERBOSE_LOG("ADemoApp::_PreUpdate()");

    // Call all registered extensions
    auto fn = [demoTime](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.PreUpdate(callOrder, demoTime); };
    CallExtensionsPre(m_extensions, fn);

    // Done this way to prevent common mistakes where people forget to call the base class
    PreUpdate(demoTime);

    // Call all registered extensions
    CallExtensionsPost(m_extensions, fn);
  }

  void ADemoApp::_FixedUpdate(const DemoTime& demoTime)
  {
    VERBOSE_LOG("ADemoApp::_FixedUpdate()");
    // Call all registered extensions
    auto fn = [demoTime](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.FixedUpdate(callOrder, demoTime); };
    CallExtensionsPre(m_extensions, fn);

    // Done this way to prevent common mistakes where people forget to call the base class
    FixedUpdate(demoTime);

    CallExtensionsPost(m_extensions, fn);
  }


  void ADemoApp::_Update(const DemoTime& demoTime)
  {
    VERBOSE_LOG("ADemoApp::_Update()");
    // Call all registered extensions
    auto fn = [demoTime](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.Update(callOrder, demoTime); };
    CallExtensionsPre(m_extensions, fn);

    // Done this way to prevent common mistakes where people forget to call the base class
    Update(demoTime);

    CallExtensionsPost(m_extensions, fn);
  }


  void ADemoApp::_PostUpdate(const DemoTime& demoTime)
  {
    VERBOSE_LOG("ADemoApp::_PostUpdate()");

    auto fn = [demoTime](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.PostUpdate(callOrder, demoTime); };
    CallExtensionsPre(m_extensions, fn);

    // Done this way to prevent common mistakes where people forget to call the base class
    PostUpdate(demoTime);

    // Here we call the extensions after the app, which allows for example a UI extension to do things after the app has finished

    // Call all registered extensions
    CallExtensionsPost(m_extensions, fn);
  }

  void ADemoApp::_Resolve(const DemoTime& demoTime)
  {
    VERBOSE_LOG("ADemoApp::_Resolve()");

    auto fn = [demoTime](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.Resolve(callOrder, demoTime); };
    CallExtensionsPre(m_extensions, fn);

    // Done this way to prevent common mistakes where people forget to call the base class
    Resolve(demoTime);

    // Here we call the extensions after the app, which allows for example a UI extension to do things after the app has finished

    // Call all registered extensions
    CallExtensionsPost(m_extensions, fn);
  }

  AppDrawResult ADemoApp::_TryPrepareDraw(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_TryPrepareDraw()");
    return TryPrepareDraw(frameInfo);
  }


  void ADemoApp::_BeginDraw(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_BeginDraw()");
    BeginDraw(frameInfo);
  }

  void ADemoApp::_Draw(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_Draw()");
    // Call all registered extensions
    // CallExtensions(m_extensions, PredMethodDraw());

    // Done this way to prevent common mistakes where people forget to call the base class
    Draw(frameInfo);
  }

  void ADemoApp::_EndDraw(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_EndDraw()");
    EndDraw(frameInfo);
  }

  void ADemoApp::_OnDrawSkipped(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_OnDrawSkipped()");
    // Call all registered extensions
    // CallExtensions(m_extensions, PredMethodDraw());

    // Done this way to prevent common mistakes where people forget to call the base class
    auto fn = [frameInfo](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.OnDrawSkipped(callOrder, frameInfo); };
    CallExtensionsPre(m_extensions, fn);
    OnDrawSkipped(frameInfo);
    CallExtensionsPost(m_extensions, fn);
  }

  AppDrawResult ADemoApp::_TrySwapBuffers(const FrameInfo& frameInfo)
  {
    VERBOSE_LOG("ADemoApp::_TrySwapBuffers()");
    // Done this way to prevent common mistakes where people forget to call the base class
    return TrySwapBuffers(frameInfo);
  }

  void ADemoApp::_End()
  {
    VERBOSE_LOG("ADemoApp::_End()");

    auto fn = [](IDemoAppExtension& rExt, const DemoAppExtensionCallOrder callOrder) { rExt.End(callOrder); };
    CallExtensionsPre(m_extensions, fn);

    OnFrameSequenceEnd();

    CallExtensionsPost(m_extensions, fn);
  }

  PxSize2D ADemoApp::GetWindowSizePx() const
  {
    return TypeConverter::UncheckedTo<PxSize2D>(m_demoAppConfig.WindowMetrics.ExtentPx);
  }

  float ADemoApp::GetWindowAspectRatio() const
  {
    return m_demoAppConfig.WindowMetrics.AspectRatio();
  }


  std::shared_ptr<IDemoAppControl> ADemoApp::GetDemoAppControl() const
  {
    std::shared_ptr<IDemoAppControl> demoAppControl = m_demoAppControl.lock();
    if (!demoAppControl)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return demoAppControl;
  }


  std::shared_ptr<IContentManager> ADemoApp::GetContentManager() const
  {
    std::shared_ptr<IContentManager> contentManager = m_contentManger.lock();
    if (!contentManager)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return contentManager;
  }


  std::shared_ptr<IPersistentDataManager> ADemoApp::GetPersistentDataManager() const
  {
    std::shared_ptr<IPersistentDataManager> manager = m_persistentDataManager.lock();
    if (!manager)
    {
      throw ServiceUnavailableException("The service is no longer available");
    }
    return manager;
  }


  const ExceptionMessageFormatter& ADemoApp::GetExceptionFormatter() const
  {
    return m_demoAppConfig.ExceptionFormatter;
  }


  void ADemoApp::UnhandledKeyFallback(const KeyEvent& keyEvent)
  {
    if (!keyEvent.IsPressed())
    {
      return;
    }


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
}    // namespace Fsl
