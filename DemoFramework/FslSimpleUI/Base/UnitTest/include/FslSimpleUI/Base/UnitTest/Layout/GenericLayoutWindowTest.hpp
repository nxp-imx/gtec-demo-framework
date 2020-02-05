#ifndef FSLSIMPLEUI_BASE_UNITTEST_LAYOUT_FSLSIMPLEUI_BASE_UNITTEST_GENERICLAYOUTWINDOWTEST_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_LAYOUT_FSLSIMPLEUI_BASE_UNITTEST_GENERICLAYOUTWINDOWTEST_HPP
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

#include <FslSimpleUI/Base/UnitTest/WindowCallIdManager.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowCallCount.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowCallbacks.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowCallId.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowMethod.hpp>
#include <functional>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    template <typename TParent>
    class GenericLayoutWindowTest : public TParent
    {
      std::shared_ptr<WindowCallIdManager> m_callIdManager;
      mutable WindowCallCount m_callCount{};
      mutable WindowCallId m_callId;

    public:
      using parent_type = TParent;

      WindowCallbacks Callbacks;

      void SetCallbackIdManager(const std::shared_ptr<WindowCallIdManager>& callIdManager)
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

      GenericLayoutWindowTest(const std::shared_ptr<BaseWindowContext>& context, const WindowFlags& windowFlags = WindowFlags())
        : parent_type(context)
      {
        if (windowFlags.GetValue() > 0)
        {
          parent_type::Enable(windowFlags);
        }
      }

      void WinInit() override
      {
        ++m_callCount.WinInit;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinInit))
        {
          m_callId.WinInit = m_callIdManager->Claim();
        }

        parent_type::WinInit();

        Callbacks.WinInit();
      }

      bool WinMarkLayoutAsDirty() override
      {
        ++m_callCount.WinMarkLayoutAsDirty;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinMarkLayoutAsDirty))
        {
          m_callId.WinMarkLayoutAsDirty = m_callIdManager->Claim();
        }

        auto res = parent_type::WinMarkLayoutAsDirty();

        Callbacks.WinMarkLayoutAsDirty();
        return res;
      }

      Rect WinGetContentRect() const override
      {
        ++m_callCount.WinGetContentRect;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinGetContentRect))
        {
          m_callId.WinGetContentRect = m_callIdManager->Claim();
        }

        return parent_type::WinGetContentRect();
      }

      void WinHandleEvent(const RoutedEvent& routedEvent) override
      {
        ++m_callCount.WinHandleEvent;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinHandleEvent))
        {
          m_callId.WinHandleEvent = m_callIdManager->Claim();
        }

        parent_type::WinHandleEvent(routedEvent);

        Callbacks.WinHandleEvent(routedEvent);
      }

      void WinUpdate(const DemoTime& demoTime) override
      {
        ++m_callCount.WinUpdate;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinUpdate))
        {
          m_callId.WinUpdate = m_callIdManager->Claim();
        }

        parent_type::WinUpdate(demoTime);

        Callbacks.WinUpdate(demoTime);
      }

      void WinResolve(const DemoTime& demoTime) override
      {
        ++m_callCount.WinResolve;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinResolve))
        {
          m_callId.WinResolve = m_callIdManager->Claim();
        }

        parent_type::WinResolve(demoTime);

        Callbacks.WinResolve(demoTime);
      }

      void WinDraw(const UIDrawContext& context) override
      {
        ++m_callCount.WinDraw;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::WinDraw))
        {
          m_callId.WinDraw = m_callIdManager->Claim();
        }

        parent_type::WinDraw(context);

        Callbacks.WinDraw(context);
      }

    protected:
      void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        ++m_callCount.OnClickInputPreview;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnClickInputPreview))
        {
          m_callId.OnClickInputPreview = m_callIdManager->Claim();
        }

        parent_type::OnClickInputPreview(args, theEvent);

        Callbacks.OnClickInputPreview(args, theEvent);
      }

      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        ++m_callCount.OnClickInput;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnClickInput))
        {
          m_callId.OnClickInput = m_callIdManager->Claim();
        }

        parent_type::OnClickInput(args, theEvent);

        Callbacks.OnClickInput(args, theEvent);
      }

      void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) override
      {
        ++m_callCount.OnSelect;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnSelect))
        {
          m_callId.OnSelect = m_callIdManager->Claim();
        }

        parent_type::OnSelect(args, theEvent);

        Callbacks.OnSelect(args, theEvent);
      }

      void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) override
      {
        ++m_callCount.OnContentChanged;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnContentChanged))
        {
          m_callId.OnContentChanged = m_callIdManager->Claim();
        }

        parent_type::OnContentChanged(args, theEvent);

        Callbacks.OnContentChanged(args, theEvent);
      }

      Vector2 ArrangeOverride(const Vector2& finalSize) override
      {
        ++m_callCount.ArrangeOverride;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::ArrangeOverride))
        {
          m_callId.ArrangeOverride = m_callIdManager->Claim();
        }

        auto result = parent_type::ArrangeOverride(finalSize);

        Callbacks.ArrangeOverride(finalSize);
        return result;
      }

      Vector2 MeasureOverride(const Vector2& availableSize) override
      {
        ++m_callCount.MeasureOverride;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::MeasureOverride))
        {
          m_callId.MeasureOverride = m_callIdManager->Claim();
        }

        auto result = parent_type::MeasureOverride(availableSize);

        Callbacks.MeasureOverride(availableSize);
        return result;
      }

      void OnPropertiesUpdated(const PropertyTypeFlags& flags) override
      {
        ++m_callCount.OnPropertiesUpdated;
        if (m_callIdManager && m_callIdManager->IsEnabled(WindowMethod::OnPropertiesUpdated))
        {
          m_callId.OnPropertiesUpdated = m_callIdManager->Claim();
        }

        parent_type::OnPropertiesUpdated(flags);

        Callbacks.OnPropertiesUpdated(flags);
      }
    };
  }
}

#endif
