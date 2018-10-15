#ifndef FSLDEMOAPP_BASE_ADEMOAPP_HPP
#define FSLDEMOAPP_BASE_ADEMOAPP_HPP
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

#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/IDemoApp.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
#include <FslDemoApp/Base/Service/DemoAppControl/IDemoAppControl.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseWheelEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/RawMouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/TimeStateEvent.hpp>
#include <memory>

namespace Fsl
{
  class DemoAppExtension;

  class ADemoApp : public IDemoApp
  {
    DemoAppConfig m_demoAppConfig;
    std::weak_ptr<IContentManager> m_contentManger;
    std::weak_ptr<IPersistentDataManager> m_persistentDataManager;
    std::weak_ptr<IDemoAppControl> m_demoAppControl;
    std::deque<std::weak_ptr<DemoAppExtension>> m_extensions;

  public:
    ADemoApp(const DemoAppConfig& demoAppConfig);
    void _PostConstruct() override;
    void _OnEvent(IEvent* const pEvent) override;
    void _Resized(const Point2& size) override;
    void _PreUpdate(const DemoTime& demoTime) override;
    void _FixedUpdate(const DemoTime& demoTime) override;
    void _Update(const DemoTime& demoTime) override;
    void _PostUpdate(const DemoTime& demoTime) override;
    void _Draw(const DemoTime& demoTime) override;

  protected:
    virtual void OnKeyEvent(const KeyEvent& event)
    {
    }

    virtual void OnMouseButtonEvent(const MouseButtonEvent& event)
    {
    }

    virtual void OnMouseMoveEvent(const MouseMoveEvent& event)
    {
    }

    virtual void OnMouseWheelEvent(const MouseWheelEvent& event)
    {
    }

    virtual void OnTimeStateEvent(const TimeStateEvent& event)
    {
    }

    virtual void OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
    {
    }

    // Overload these methods instead of the original IDemoApp ones!
    virtual void Resized(const Point2& size)
    {
    }

    virtual void PreUpdate(const DemoTime& demoTime)
    {
    }

    virtual void FixedUpdate(const DemoTime& demoTime)
    {
    }

    virtual void Update(const DemoTime& demoTime)
    {
    }

    virtual void PostUpdate(const DemoTime& demoTime)
    {
    }

    virtual void Draw(const DemoTime& demoTime)
    {
    }

    //! @brief Register a demo app extension
    void RegisterExtension(const std::shared_ptr<DemoAppExtension>& extension);
    void UnregisterExtension(const std::shared_ptr<DemoAppExtension>& extension);

    //! @brief Get the current screen resolution
    Point2 GetScreenResolution() const
    {
      return m_demoAppConfig.ScreenResolution;
    }

    Extent2D GetScreenExtent() const
    {
      return Extent2D(m_demoAppConfig.ScreenResolution);
    }

    //! @brief Get access to the demo app control interface
    std::shared_ptr<IDemoAppControl> GetDemoAppControl() const;
    //! @brief Access the content manager;
    std::shared_ptr<IContentManager> GetContentManager() const;
    //! @brief Access the Persistent Data Manager;
    std::shared_ptr<IPersistentDataManager> GetPersistentDataManager() const;

    const ExceptionMessageFormatter& GetExceptionFormatter() const;

  private:
    void UnhandledKeyFallback(const KeyEvent& keyEvent);
  };
}

#endif
