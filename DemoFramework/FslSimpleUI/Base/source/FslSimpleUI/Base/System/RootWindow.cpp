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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslSimpleUI/Base/Event/RoutedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/LayoutHelperPxfConverter.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/System/IEventListener.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>
#include <iterator>


namespace Fsl
{
  namespace UI
  {
    RootWindow::RootWindow(const std::shared_ptr<BaseWindowContext>& context, const PxExtent2D& extentPx, const uint32_t densityDpi)
      : BaseWindow(context, WindowFlags::WinInit)
      , m_resolutionPx(TypeConverter::UncheckedTo<PxSize2D>(extentPx))
      , m_densityDpi(densityDpi)
    {
      // The root window gets created at time where the context isn't ready
      // So it should do its stuff during WinInit instead as it will always be called on the root window.
    }


    RootWindow::~RootWindow() = default;


    void RootWindow::WinInit()
    {
      Enable(WindowFlags::ClickInput);

      Measure(LayoutHelperPxfConverter::ToPxAvailableSize(m_resolutionPx));
      Arrange(PxRectangle(0, 0, m_resolutionPx.Width(), m_resolutionPx.Height()));
    }


    bool RootWindow::SetScreenResolution(const PxExtent2D& valuePx, const uint32_t densityDpi)
    {
      auto sizePx = TypeConverter::UncheckedTo<PxSize2D>(valuePx);
      if (sizePx == m_resolutionPx && densityDpi == m_densityDpi)
      {
        return false;
      }
      m_resolutionPx = sizePx;
      m_densityDpi = densityDpi;
      PropertyUpdated(PropertyType::Layout);
      return true;
    }


    void RootWindow::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_eventListenerManager.RegisterEventListener(eventListener);
    }


    void RootWindow::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_eventListenerManager.UnregisterEventListener(eventListener);
    }


    void RootWindow::OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      auto lambda = [&args, &theEvent](std::shared_ptr<IEventListener>& listener) { listener->OnClickInputPreview(args, theEvent); };
      m_eventListenerManager.Call(lambda);
    }


    void RootWindow::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      auto lambda = [&args, &theEvent](std::shared_ptr<IEventListener>& listener) { listener->OnClickInput(args, theEvent); };
      m_eventListenerManager.Call(lambda);
    }


    void RootWindow::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
    {
      auto lambda = [&args, &theEvent](std::shared_ptr<IEventListener>& listener) { listener->OnSelect(args, theEvent); };
      m_eventListenerManager.Call(lambda);
    }


    void RootWindow::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
    {
      auto lambda = [&args, &theEvent](std::shared_ptr<IEventListener>& listener) { listener->OnContentChanged(args, theEvent); };
      m_eventListenerManager.Call(lambda);
    }
  }
}
