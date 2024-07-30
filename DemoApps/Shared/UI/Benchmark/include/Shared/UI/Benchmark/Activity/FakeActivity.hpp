#ifndef SHARED_UI_BENCHMARK_ACTIVITY_FAKEACTIVITY_HPP
#define SHARED_UI_BENCHMARK_ACTIVITY_FAKEACTIVITY_HPP
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

#include <FslSimpleUI/Base/Control/ContentControlBase.hpp>
#include <memory>

namespace Fsl
{
  class KeyEvent;

  namespace UI
  {
    class IActivityStack;
    class Layout;
    class WindowContext;

    //! The lightweight basic controls UI does not currently support 'activities' so here we do some 'faking' using full screen overlays to
    //! simulate the way a activity would work
    class FakeActivity : public ContentControlBase
    {
      std::weak_ptr<IActivityStack> m_activityStack;

    public:
      explicit FakeActivity(const std::shared_ptr<WindowContext>& context, std::weak_ptr<IActivityStack> activityStack,
                            const std::shared_ptr<BaseWindow>& window);
      ~FakeActivity() override;

      void OnClickInputPreview(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      void OnMouseOverPreview(const std::shared_ptr<WindowMouseOverEvent>& theEvent) override;
      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) override;
      void OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent) override;
      void OnContentChanged(const std::shared_ptr<WindowContentChangedEvent>& theEvent) override;
      virtual void OnKeyEvent(const KeyEvent& theEvent);

      void PushActivity(std::shared_ptr<FakeActivity> activity);
      void ScheduleCloseActivity(const bool completed);

    protected:
      const std::weak_ptr<IActivityStack>& GetWeakActivityStack() const
      {
        return m_activityStack;
      }

      uint32_t ActivityCount() const;
    };
  }
}

#endif
