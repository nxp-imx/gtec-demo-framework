#ifndef FSLSIMPLEUI_BASE_SYSTEM_ROOTWINDOW_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_ROOTWINDOW_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <deque>
#include <memory>
#include "Event/EventListenerManager.hpp"

namespace Fsl
{
  namespace UI
  {
    class IEventListener;
    struct RoutedEvent;

    class RootWindow : public BaseWindow
    {
      Vector2 m_resolution;
      EventListenerManager m_eventListenerManager;

    public:
      RootWindow(const std::shared_ptr<BaseWindowContext>& context, const Vector2& resolution);
      ~RootWindow() override;

      void WinInit() override;
      WindowFlags WinGetFlags() const override;


      Vector2 GetScreenResolution() const
      {
        return m_resolution;
      }

      void SetScreenResolution(const Vector2& value);


      //! @brief Register a event listener
      void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
      //! @brief Unregister a event listener
      void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

    protected:
      // Event forwarding
      void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) override;
      void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) override;

      //! Layout
      Vector2 ArrangeOverride(const Vector2& finalSize) override
      {
        FSL_PARAM_NOT_USED(finalSize);
        return m_resolution;
      }

      Vector2 MeasureOverride(const Vector2& availableSize) override
      {
        FSL_PARAM_NOT_USED(availableSize);
        return availableSize;
      }
    };
  }
}

#endif
