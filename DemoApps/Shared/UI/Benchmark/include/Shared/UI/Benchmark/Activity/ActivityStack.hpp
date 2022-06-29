#ifndef SHARED_UI_BENCHMARK_ACTIVITY_ACTIVITYSTACK_HPP
#define SHARED_UI_BENCHMARK_ACTIVITY_ACTIVITYSTACK_HPP
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

#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <Shared/UI/Benchmark/Activity/IActivityStack.hpp>
#include <deque>
#include <utility>

namespace Fsl
{
  class KeyEvent;

  namespace UI
  {
    class FakeActivity;
    class WindowContext;

    class ActivityStack final
      : public FillLayout
      , public IActivityStack
    {
      enum class ActivityState
      {
        Ready,
        Closing,
      };
      enum class ActivityPopResult
      {
        NotSet,
        Completed,
        Cancelled,
      };

      struct ActivityRecord
      {
        ActivityState State{ActivityState::Ready};
        std::shared_ptr<FakeActivity> Activity;
        TransitionColor BaseColor;
        ActivityPopResult PopResult{ActivityPopResult::NotSet};
        std::promise<bool> Promise;

        explicit ActivityRecord(std::shared_ptr<FakeActivity> activity, const Color color, TransitionCache& rTransitionCache,
                                const TimeSpan transitionTimeSpan)
          : Activity(std::move(activity))
          , BaseColor(rTransitionCache, transitionTimeSpan, TransitionType::Smooth)
        {
          BaseColor.SetActualValue(Color::TransparentWhite());
          BaseColor.SetValue(color);
        }

        void Update(const TimeSpan& deltaTime)
        {
          BaseColor.Update(deltaTime);
        }

        void ForceComplete()
        {
          BaseColor.ForceComplete();
        }

        bool IsAnimating() const
        {
          return !BaseColor.IsCompleted();
        }
      };

      std::shared_ptr<WindowContext> m_context;
      std::deque<ActivityRecord> m_stack;
      TransitionColor m_parentBaseColor;

    public:
      explicit ActivityStack(const std::shared_ptr<WindowContext>& context);
      ~ActivityStack() final;

      bool IsEmpty() const
      {
        return m_stack.empty();
      }

      Color GetDesiredParentColor() const
      {
        return m_parentBaseColor.GetValue();
      }

      uint32_t Count() const final;
      std::future<bool> Push(std::shared_ptr<FakeActivity> activity) final;
      void SchedulePop(const bool completed) final;

      void OnKeyEvent(const KeyEvent& event);

    protected:
      void UpdateAnimation(const TimeSpan& timeSpan) override;
      bool UpdateAnimationState(const bool forceCompleteAnimation) override;
    };
  }
}


#endif
