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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <deque>
#include <memory>
#include "Event/EventListenerManager.hpp"

namespace Fsl::UI
{
  class IEventListener;
  struct RoutedEvent;

  class RootWindow final : public BaseWindow
  {
    PxSize2D m_resolutionPx;
    uint32_t m_densityDpi;
    EventListenerManager m_eventListenerManager;

  public:
    RootWindow(const std::shared_ptr<BaseWindowContext>& context, const PxExtent2D& extentPx, const uint32_t densityDpi);
    ~RootWindow() override;

    void WinInit() final;

    PxSize2D GetScreenResolutionPx() const
    {
      return m_resolutionPx;
    }
    uint32_t GetDensityDpi() const
    {
      return m_densityDpi;
    }

    //! return true if the resolution was modified
    bool SetScreenResolution(const PxExtent2D& valuePx, const uint32_t densityDpi);

    //! @brief Register a event listener
    void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
    //! @brief Unregister a event listener
    void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

  protected:
    // Event forwarding
    void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) final;
    void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) final;
    void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) final;
    void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) final;

    //! Layout
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final
    {
      FSL_PARAM_NOT_USED(finalSizePx);
      return m_resolutionPx;
    }

    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final
    {
      FSL_PARAM_NOT_USED(availableSizePx);
      return m_resolutionPx;
    }
  };
}

#endif
