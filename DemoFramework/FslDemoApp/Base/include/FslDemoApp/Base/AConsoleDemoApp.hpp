#ifndef FSLDEMOAPP_BASE_ACONSOLEDEMOAPP_HPP
#define FSLDEMOAPP_BASE_ACONSOLEDEMOAPP_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/IDemoApp.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/DemoAppControl/IDemoAppControl.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseWheelEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/TimeStateEvent.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
#include <memory>

namespace Fsl
{
  class DemoAppExtension;

  class AConsoleDemoApp : public IDemoApp
  {
    DemoAppConfig m_demoAppConfig;
    std::weak_ptr<IContentManager> m_contentManger;
    std::weak_ptr<IPersistentDataManager> m_persistentDataManager;
    std::weak_ptr<IDemoAppControl> m_demoAppControl;
    std::deque<std::weak_ptr<DemoAppExtension>> m_extensions;
    bool m_destroyed = false;

  public:
    explicit AConsoleDemoApp(const DemoAppConfig& demoAppConfig);
    ~AConsoleDemoApp() override;
    void _PostConstruct() override;
    void _PreDestruct() override;
    void _Begin() override;
    void _OnEvent(IEvent* const pEvent) override;
    void _ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
    void _PreUpdate(const DemoTime& demoTime) override;
    void _FixedUpdate(const DemoTime& demoTime) override;
    void _Update(const DemoTime& demoTime) override;
    void _PostUpdate(const DemoTime& demoTime) override;
    void _Resolve(const DemoTime& demoTime) override;
    AppDrawResult _TryPrepareDraw(const FrameInfo& frameInfo) override;
    void _BeginDraw(const FrameInfo& frameInfo) override;
    void _Draw(const FrameInfo& frameInfo) override;
    void _EndDraw(const FrameInfo& frameInfo) override;
    void _OnDrawSkipped(const FrameInfo& frameInfo) override;
    AppDrawResult _TrySwapBuffers(const FrameInfo& frameInfo) override
    {
      // No default implementation, so fail
      FSL_PARAM_NOT_USED(frameInfo);
      return AppDrawResult::Failed;
    }
    void _End() override;

  protected:
    virtual void Run(){};

    //! @brief Get access to the demo app control interface
    std::shared_ptr<IDemoAppControl> GetDemoAppControl() const;
    //! @brief Access the content manager;
    std::shared_ptr<IContentManager> GetContentManager() const;
    //! @brief Access the persistent data manager;
    std::shared_ptr<IPersistentDataManager> GetPersistentDataManager() const;
  };
}

#endif
