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

#include "RootWindow.hpp"
#include <FslBase/Math/EqualHelper.hpp>
#include <FslSimpleUI/PropertyTypeFlags.hpp>
#include <FslSimpleUI/WindowContext.hpp>
#include <FslSimpleUI/System/IEventListener.hpp>
#include <FslSimpleUI/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Event/WindowSelectEvent.hpp>
#include <algorithm>
#include <iterator>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      struct ReceiverComp
      {
        const std::shared_ptr<IEventListener> Key;
        explicit ReceiverComp(const std::shared_ptr<IEventListener>& key)
          : Key(key)
        {
        }
        inline bool operator()(const std::weak_ptr<IEventListener>& record) const { return record.lock() == Key; }
      };


      void CopyContent(std::deque<std::weak_ptr<IEventListener> >& rDst, const std::deque<std::weak_ptr<IEventListener> >& src)
      {
        rDst.clear();
        std::copy(src.begin(), src.end(), std::back_inserter(rDst));
      }

    }


    RootWindow::RootWindow(const Vector2& resolution)
      : BaseWindow(std::shared_ptr<WindowContext>())
      , m_resolution(resolution)
    {
      Enable(WindowFlags::ClickInput);

      Measure(resolution);
      Arrange(Rect(0, 0, resolution.X, resolution.Y));
    }


    void RootWindow::SetScreenResolution(const Vector2& value)
    {
      if (!EqualHelper::IsAlmostEqual(value, m_resolution))
      {
        m_resolution = value;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    RootWindow::~RootWindow()
    {
    }


    void RootWindow::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_eventListeners.push_back(eventListener);
    }


    void RootWindow::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      auto listener = eventListener.lock();
      // If the pointer has become invalid we just ignore the request as the next broadcast will remove the entry
      if (!listener)
        return;

      auto itr = std::find_if(m_eventListeners.begin(), m_eventListeners.end(), ReceiverComp(listener));
      if (itr != m_eventListeners.end())
        m_eventListeners.erase(itr);
    }


    void RootWindow::OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      auto& rEventListeners = m_eventListenersScratchpad;
      CopyContent(rEventListeners, m_eventListeners);

      auto itr = rEventListeners.begin();
      while (itr != rEventListeners.end())
      {
        auto listener = itr->lock();
        if (listener)
        {
          listener->OnClickInputPreview(args, theEvent);
          ++itr;
        }
        else
          itr = rEventListeners.erase(itr);
      }
    }


    void RootWindow::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      auto& rEventListeners = m_eventListenersScratchpad;
      CopyContent(rEventListeners, m_eventListeners);

      auto itr = rEventListeners.begin();
      while (itr != rEventListeners.end())
      {
        auto listener = itr->lock();
        if (listener)
        {
          listener->OnClickInput(args, theEvent);
          ++itr;
        }
        else
          itr = rEventListeners.erase(itr);
      }
    }


    void RootWindow::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
    {
      auto& rEventListeners = m_eventListenersScratchpad;
      CopyContent(rEventListeners, m_eventListeners);

      auto itr = rEventListeners.begin();
      while (itr != rEventListeners.end())
      {
        auto listener = itr->lock();
        if (listener)
        {
          listener->OnSelect(args, theEvent);
          ++itr;
        }
        else
          itr = rEventListeners.erase(itr);
      }
    }


    void RootWindow::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
    {
      auto& rEventListeners = m_eventListenersScratchpad;
      CopyContent(rEventListeners, m_eventListeners);

      auto itr = rEventListeners.begin();
      while (itr != rEventListeners.end())
      {
        auto listener = itr->lock();
        if (listener)
        {
          listener->OnContentChanged(args, theEvent);
          ++itr;
        }
        else
          itr = rEventListeners.erase(itr);
      }
    }
  }
}
