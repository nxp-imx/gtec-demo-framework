#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_WINDOWCALLBACKS_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_WINDOWCALLBACKS_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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
#include <FslSimpleUI/Base/UnitTest/WindowCallCount.hpp>
#include <functional>

namespace Fsl::UI
{
  struct WindowCallbacks
  {
    std::function<void()> HookWinInit;
    std::function<void()> HookWinMarkLayoutAsDirty;
    std::function<void(const RoutedEvent&)> HookWinHandleEvent;
    std::function<void(const TimeSpan&)> HookWinUpdate;
    std::function<void(const TimeSpan&)> HookWinResolve;
    std::function<void(const UIDrawContext&)> HookWinDraw;
    std::function<void(const RoutedEventArgs&, const std::shared_ptr<WindowInputClickEvent>&)> HookOnClickInputPreview;
    std::function<void(const RoutedEventArgs&, const std::shared_ptr<WindowInputClickEvent>&)> HookOnClickInput;
    std::function<void(const RoutedEventArgs&, const std::shared_ptr<WindowSelectEvent>&)> HookOnSelect;
    std::function<void(const RoutedEventArgs&, const std::shared_ptr<WindowContentChangedEvent>&)> HookOnContentChanged;
    std::function<void(const PxSize2D&)> HookArrangeOverride;
    std::function<void(const PxAvailableSize&)> HookMeasureOverride;
    std::function<void(const PropertyTypeFlags&)> HookOnPropertiesUpdated;

    void WinInit()
    {
      if (HookWinInit)
      {
        HookWinInit();
      }
    }

    void WinMarkLayoutAsDirty()
    {
      if (HookWinMarkLayoutAsDirty)
      {
        HookWinMarkLayoutAsDirty();
      }
    }

    void WinHandleEvent(const RoutedEvent& routedEvent)
    {
      if (HookWinHandleEvent)
      {
        HookWinHandleEvent(routedEvent);
      }
    }

    void WinUpdate(const TimeSpan& timeSpan)
    {
      if (HookWinUpdate)
      {
        HookWinUpdate(timeSpan);
      }
    }

    void WinResolve(const TimeSpan& timeSpan)
    {
      if (HookWinResolve)
      {
        HookWinResolve(timeSpan);
      }
    }

    void WinDraw(const UIDrawContext& context)
    {
      if (HookWinDraw)
      {
        HookWinDraw(context);
      }
    }

    void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      if (HookOnClickInputPreview)
      {
        HookOnClickInputPreview(args, theEvent);
      }
    }

    void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      if (HookOnClickInput)
      {
        HookOnClickInput(args, theEvent);
      }
    }

    void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
    {
      if (HookOnSelect)
      {
        HookOnSelect(args, theEvent);
      }
    }

    void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
    {
      if (HookOnContentChanged)
      {
        HookOnContentChanged(args, theEvent);
      }
    }

    void ArrangeOverride(const PxSize2D& finalSizePx)
    {
      if (HookArrangeOverride)
      {
        HookArrangeOverride(finalSizePx);
      }
    }

    void MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      if (HookMeasureOverride)
      {
        HookMeasureOverride(availableSizePx);
      }
    }

    void OnPropertiesUpdated(const PropertyTypeFlags& flags)
    {
      if (HookOnPropertiesUpdated)
      {
        HookOnPropertiesUpdated(flags);
      }
    }
  };
}

#endif
