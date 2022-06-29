/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <Shared/UI/Benchmark/Activity/FakeActivity.hpp>
#include <Shared/UI/Benchmark/Activity/IActivityStack.hpp>
#include <utility>

namespace Fsl::UI
{
  FakeActivity::FakeActivity(const std::shared_ptr<WindowContext>& context, std::weak_ptr<IActivityStack> activityStack,
                             const std::shared_ptr<BaseWindow>& window)
    : ContentControlBase(context)
    , m_activityStack(std::move(activityStack))
  {
    // Like a activity this activity will grab all input
    Enable(WindowFlags::InputAll);

    // Take up the entire available space (its up to the user to ensure this will fill the screen)
    SetAlignmentX(ItemAlignment::Stretch);
    SetAlignmentY(ItemAlignment::Stretch);

    DoSetContent(window);
  }

  FakeActivity::~FakeActivity() = default;

  void FakeActivity::OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    ContentControlBase::OnClickInputPreview(args, theEvent);
  }

  void FakeActivity::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    ContentControlBase::OnClickInput(args, theEvent);
    if (!theEvent->IsHandled())
    {
      theEvent->Handled();
    }
  }

  void FakeActivity::OnMouseOverPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    ContentControlBase::OnMouseOverPreview(args, theEvent);
  }

  void FakeActivity::OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent)
  {
    ContentControlBase::OnMouseOver(args, theEvent);
    if (!theEvent->IsHandled())
    {
      theEvent->Handled();
    }
  }

  void FakeActivity::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    ContentControlBase::OnSelect(args, theEvent);
    if (!theEvent->IsHandled())
    {
      theEvent->Handled();
    }
  }

  void FakeActivity::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
  {
    ContentControlBase::OnContentChanged(args, theEvent);
    if (!theEvent->IsHandled())
    {
      theEvent->Handled();
    }
  }


  void FakeActivity::OnKeyEvent(const KeyEvent& theEvent)
  {
    if (!theEvent.IsHandled())
    {
      theEvent.Handled();
    }
  }


  void FakeActivity::PushActivity(std::shared_ptr<FakeActivity> activity)
  {
    std::shared_ptr<IActivityStack> activityStack = m_activityStack.lock();
    if (!activityStack)
    {
      throw UsageErrorException("Activity stack is no longer valid");
    }
    activityStack->Push(std::move(activity));
  }


  void FakeActivity::ScheduleCloseActivity(const bool completed)
  {
    std::shared_ptr<IActivityStack> activityStack = m_activityStack.lock();
    if (!activityStack)
    {
      throw UsageErrorException("Activity stack is no longer valid");
    }
    activityStack->SchedulePop(completed);
  }


  uint32_t FakeActivity::ActivityCount() const
  {
    std::shared_ptr<IActivityStack> activityStack = m_activityStack.lock();
    return activityStack ? activityStack->Count() : 0;
  }

}
