/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>

namespace Fsl
{
  namespace UI
  {
    BaseWindowTest::BaseWindowTest(const std::shared_ptr<BaseWindowContext>& context, const WindowFlags windowFlags)
      : BaseWindow(context)
    {
      if (windowFlags.GetValue() > 0)
      {
        Enable(windowFlags);
      }
    }


    void BaseWindowTest::WinInit()
    {
      ++m_callCount.WinInit;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinInit))
      {
        m_callId.WinInit = m_callIdManager->Claim();
      }

      BaseWindow::WinInit();

      Callbacks.WinInit();
    }

    bool BaseWindowTest::WinMarkLayoutAsDirty()
    {
      ++m_callCount.WinMarkLayoutAsDirty;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinMarkLayoutAsDirty))
      {
        m_callId.WinMarkLayoutAsDirty = m_callIdManager->Claim();
      }

      auto res = BaseWindow::WinMarkLayoutAsDirty();

      Callbacks.WinMarkLayoutAsDirty();
      return res;
    }

    // const PxRectangle& BaseWindowTest::WinGetContentPxRectangle() const
    //{
    //  ++m_callCount.WinGetContentRect;
    //  if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinGetContentPxRectangle))
    //  {
    //    m_callId.WinGetContentRect = m_callIdManager->Claim();
    //  }

    //  return BaseWindow::WinGetContentRectanglePx();
    //}

    void BaseWindowTest::WinHandleEvent(const RoutedEvent& routedEvent)
    {
      ++m_callCount.WinHandleEvent;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinHandleEvent))
      {
        m_callId.WinHandleEvent = m_callIdManager->Claim();
      }

      BaseWindow::WinHandleEvent(routedEvent);

      Callbacks.WinHandleEvent(routedEvent);
    }

    void BaseWindowTest::WinUpdate(const TransitionTimeSpan& timeSpan)
    {
      ++m_callCount.WinUpdate;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinUpdate))
      {
        m_callId.WinUpdate = m_callIdManager->Claim();
      }

      BaseWindow::WinUpdate(timeSpan);

      Callbacks.WinUpdate(timeSpan);
    }

    void BaseWindowTest::WinResolve(const TransitionTimeSpan& timeSpan)
    {
      ++m_callCount.WinResolve;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinResolve))
      {
        m_callId.WinResolve = m_callIdManager->Claim();
      }

      BaseWindow::WinResolve(timeSpan);

      Callbacks.WinResolve(timeSpan);
    }

    void BaseWindowTest::WinDraw(const UIDrawContext& context)
    {
      ++m_callCount.WinDraw;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinDraw))
      {
        m_callId.WinDraw = m_callIdManager->Claim();
      }

      BaseWindow::WinDraw(context);

      Callbacks.WinDraw(context);
    }


    void BaseWindowTest::OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      ++m_callCount.OnClickInputPreview;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnClickInputPreview))
      {
        m_callId.OnClickInputPreview = m_callIdManager->Claim();
      }

      BaseWindow::OnClickInputPreview(args, theEvent);

      Callbacks.OnClickInputPreview(args, theEvent);
    }

    void BaseWindowTest::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      ++m_callCount.OnClickInput;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnClickInput))
      {
        m_callId.OnClickInput = m_callIdManager->Claim();
      }

      BaseWindow::OnClickInput(args, theEvent);

      Callbacks.OnClickInput(args, theEvent);
    }

    void BaseWindowTest::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
    {
      ++m_callCount.OnSelect;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnSelect))
      {
        m_callId.OnSelect = m_callIdManager->Claim();
      }

      BaseWindow::OnSelect(args, theEvent);

      Callbacks.OnSelect(args, theEvent);
    }

    void BaseWindowTest::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
    {
      ++m_callCount.OnContentChanged;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnContentChanged))
      {
        m_callId.OnContentChanged = m_callIdManager->Claim();
      }

      BaseWindow::OnContentChanged(args, theEvent);

      Callbacks.OnContentChanged(args, theEvent);
    }

    PxSize2D BaseWindowTest::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      ++m_callCount.ArrangeOverride;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::ArrangeOverride))
      {
        m_callId.ArrangeOverride = m_callIdManager->Claim();
      }

      auto result = BaseWindow::ArrangeOverride(finalSizePx);

      Callbacks.ArrangeOverride(finalSizePx);
      return result;
    }

    PxSize2D BaseWindowTest::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      ++m_callCount.MeasureOverride;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::MeasureOverride))
      {
        m_callId.MeasureOverride = m_callIdManager->Claim();
      }

      auto result = BaseWindow::MeasureOverride(availableSizePx);

      Callbacks.MeasureOverride(availableSizePx);
      return result;
    }

    void BaseWindowTest::OnPropertiesUpdated(const PropertyTypeFlags& flags)
    {
      ++m_callCount.OnPropertiesUpdated;
      if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnPropertiesUpdated))
      {
        m_callId.OnPropertiesUpdated = m_callIdManager->Claim();
      }

      BaseWindow::OnPropertiesUpdated(flags);

      Callbacks.OnPropertiesUpdated(flags);
    }
  }
}
