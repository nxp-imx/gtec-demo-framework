/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <Shared/UI/Benchmark/Activity/ActivityStack.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslSimpleUI/Base/DefaultAnim.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <Shared/UI/Benchmark/Activity/FakeActivity.hpp>

namespace Fsl
{
  namespace UI
  {
    ActivityStack::ActivityStack(const std::shared_ptr<WindowContext>& context)
      : FillLayout(context)
      , m_context(context)
      , m_parentBaseColor(m_context->UITransitionCache, DefaultAnim::ColorChangeTime, TransitionType::Smooth)
    {
      m_parentBaseColor.SetActualValue(Color::White());
      UpdateAnimationState(true);
    }


    ActivityStack::~ActivityStack()
    {
      for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
      {
        itr->Promise.set_exception(std::make_exception_ptr(std::runtime_error("activity canceled")));
      }
    }


    uint32_t ActivityStack::Count() const
    {
      return UncheckedNumericCast<uint32_t>(m_stack.size());
    }


    std::future<bool> ActivityStack::Push(std::shared_ptr<FakeActivity> activity)
    {
      try
      {
        AddChild(activity);
        m_stack.emplace_front(activity, Color::TransparentWhite(), m_context->UITransitionCache, DefaultAnim::ColorChangeTime);
        CheckAnimationState();
        return m_stack.front().Promise.get_future();
      }
      catch (const std::exception& ex)
      {
        if (!m_stack.empty() && m_stack.front().Activity == activity)
        {
          m_stack.pop_front();
        }
        RemoveChild(activity);
        FSLLOG3_ERROR("Push threw exception: {}", ex.what());
        throw;
      }
    }

    void ActivityStack::SchedulePop(const bool completed)
    {
      if (!m_stack.empty() && m_stack.front().State == ActivityState::Ready)
      {
        m_stack.front().State = ActivityState::Closing;
        m_stack.front().PopResult = completed ? ActivityPopResult::Completed : ActivityPopResult::Cancelled;
        CheckAnimationState();
      }
    }


    void ActivityStack::OnKeyEvent(const KeyEvent& event)
    {
      if (!m_stack.empty())
      {
        const auto& frontActivity = m_stack.front().Activity;
        if (frontActivity)
        {
          frontActivity->OnKeyEvent(event);
        }
      }
    }


    void ActivityStack::UpdateAnimation(const TransitionTimeSpan& timeSpan)
    {
      FillLayout::UpdateAnimation(timeSpan);

      for (auto& rEntry : m_stack)
      {
        rEntry.Update(timeSpan);
      }
      m_parentBaseColor.Update(timeSpan);
    }


    bool ActivityStack::UpdateAnimationState(const bool forceCompleteAnimation)
    {
      {
        uint32_t col = 255;
        for (auto& rEntry : m_stack)
        {
          switch (rEntry.State)
          {
          case ActivityState::Ready:
            rEntry.BaseColor.SetValue(col == 255 ? Color::White() : Color(col, col, col, 255u));
            col = (col >= 0x40 ? col - 0x40 : 0);
            break;
          case ActivityState::Closing:
            rEntry.BaseColor.SetValue(Color::TransparentWhite());
            break;
          default:
            FSLLOG3_ERROR("Not handled");
            break;
          }
        }
        m_parentBaseColor.SetValue(col == 255 ? Color::White() : Color(col, col, col, 255u));
      }


      bool stackIsAnimating = false;
      if (forceCompleteAnimation)
      {
        for (auto& rEntry : m_stack)
        {
          rEntry.ForceComplete();
          rEntry.Activity->SetBaseColor(rEntry.BaseColor.GetValue());
        }
        m_parentBaseColor.ForceComplete();
      }
      else
      {
        for (auto& rEntry : m_stack)
        {
          rEntry.Activity->SetBaseColor(rEntry.BaseColor.GetValue());
          if (rEntry.IsAnimating())
          {
            stackIsAnimating = true;
          }
        }
      }

      {    // Pop all closing elements in the right order
        auto uiContext = m_context->TheUIContext.Get();
        while (!m_stack.empty() && m_stack.front().State == ActivityState::Closing && !m_stack.front().IsAnimating())
        {
          uiContext->WindowManager->ScheduleClose(m_stack.front().Activity);
          {
            auto& rEntry = m_stack.front();
            switch (rEntry.PopResult)
            {
            case ActivityPopResult::Completed:
              rEntry.Promise.set_value(true);
              break;
            case ActivityPopResult::Cancelled:
              rEntry.Promise.set_value(false);
              break;
            case ActivityPopResult::NotSet:
            default:
              rEntry.Promise.set_exception(std::make_exception_ptr(InternalErrorException("no result was set")));
              break;
            }
          }
          m_stack.pop_front();
        }
      }

      const bool parentIsAnimating = FillLayout::UpdateAnimationState(forceCompleteAnimation);
      return parentIsAnimating || stackIsAnimating || !m_parentBaseColor.IsCompleted();
    }
  }
}
