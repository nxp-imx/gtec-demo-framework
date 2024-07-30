#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_BASEWINDOWTEST_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_BASEWINDOWTEST_HPP
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
#include <FslSimpleUI/Base/UnitTest/WindowCallId.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowCallIdManager.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowCallbacks.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowMethod.hpp>
#include <functional>

namespace Fsl::UI
{
  class BaseWindowTest : public BaseWindow
  {
    std::shared_ptr<WindowCallIdManager> m_callIdManager;
    mutable WindowCallCount m_callCount{};
    mutable WindowCallId m_callId;

  public:
    WindowCallbacks Callbacks;

    void SetCallIdManager(const std::shared_ptr<WindowCallIdManager>& callIdManager)
    {
      m_callIdManager = callIdManager;
    }

    WindowCallCount GetCallCount() const
    {
      return m_callCount;
    }

    // Get the latest call id for all methods
    WindowCallId GetCallId() const
    {
      return m_callId;
    }

    explicit BaseWindowTest(const std::shared_ptr<BaseWindowContext>& context, const WindowFlags windowFlags = WindowFlags());

    void WinInit() override;
    bool WinMarkLayoutAsDirty() override;
    // const PxRectangle& WinGetContentRectanglePx() const override;
    void WinHandleEvent(const RoutedEvent& routedEvent) override;
    void WinUpdate(const TimeSpan& timeSpan) override;
    void WinResolve(const TimeSpan& timeSpan) override;
    void WinDraw(const UIDrawContext& context) override;

  protected:
    void OnClickInputPreview(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
    void OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent) override;
    void OnContentChanged(const std::shared_ptr<WindowContentChangedEvent>& theEvent) override;

    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;
    void OnPropertiesUpdated(const PropertyTypeFlags& flags) override;
  };
}

#endif
