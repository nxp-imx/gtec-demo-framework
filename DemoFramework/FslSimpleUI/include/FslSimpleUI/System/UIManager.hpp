#ifndef FSLSIMPLEUI_SYSTEM_UIMANAGER_HPP
#define FSLSIMPLEUI_SYSTEM_UIMANAGER_HPP
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

#include <memory>

namespace Fsl
{
  struct DemoTime;
  class KeyEvent;
  class MouseButtonEvent;
  class MouseMoveEvent;
  class MouseWheelEvent;
  struct Point2;

  namespace UI
  {
    class IEventListener;
    class IModuleHost;
    class InputModule;
    class IWindowManager;
    class ModuleCallbackRegistry;
    class UITree;
    class WindowEventPool;
    class WindowEventQueue;
    class WindowEventQueueEx;
    class WindowEventSender;

    class UIManager
    {
      std::shared_ptr<ModuleCallbackRegistry> m_moduleCallbackRegistry;
      std::shared_ptr<WindowEventPool> m_eventPool;
      std::shared_ptr<WindowEventQueueEx> m_eventQueue;
      std::shared_ptr<UITree> m_tree;
      std::shared_ptr<WindowEventSender> m_eventSender;
      std::shared_ptr<IModuleHost> m_moduleHost;
      std::shared_ptr<InputModule> m_inputModule;
      bool m_leftButtonDown;
    public:
      UIManager(const Point2& size);
      ~UIManager();

      std::shared_ptr<IWindowManager> GetWindowManager() const;
      std::shared_ptr<WindowEventPool> GetEventPool() const;
      std::shared_ptr<WindowEventSender> GetEventSender() const;

      //bool SendKeyEvent(const KeyEvent& event);
      //! @brief Send a mouse button event
      //! @note Returns true if the event was handled by a UIElement
      bool SendMouseButtonEvent(const MouseButtonEvent& event);
      bool SendMouseMoveEvent(const MouseMoveEvent& event);
      //bool SendMouseWheelEvent(const MouseWheelEvent& event);
      void Resized(const Point2& size);
      void FixedUpdate(const DemoTime& demoTime);
      void Update(const DemoTime& demoTime);
      void Draw();

      //! @brief Register a event listener
      void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

      //! @brief Unregister a event listener
      void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
    private:
    };
  }
}

#endif
