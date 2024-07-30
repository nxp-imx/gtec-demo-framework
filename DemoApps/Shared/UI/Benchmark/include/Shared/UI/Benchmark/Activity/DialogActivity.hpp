#ifndef SHARED_UI_BENCHMARK_ACTIVITY_DIALOGACTIVITY_HPP
#define SHARED_UI_BENCHMARK_ACTIVITY_DIALOGACTIVITY_HPP
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

#include <FslSimpleUI/Theme/Base/WindowType.hpp>
#include <Shared/UI/Benchmark/Activity/FakeActivity.hpp>
#include <memory>

namespace Fsl::UI
{
  namespace Theme
  {
    class IThemeControlFactory;
  }

  class DialogActivity : public FakeActivity
  {
    std::weak_ptr<Theme::IThemeControlFactory> m_controlFactory;
    std::shared_ptr<BaseWindow> m_window;

  public:
    DialogActivity(std::weak_ptr<IActivityStack> activityStack, const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                   std::shared_ptr<BaseWindow> window, const Theme::WindowType windowType = Theme::WindowType::Normal,
                   const ItemAlignment windowAlignmentX = ItemAlignment::Center, const ItemAlignment windowAlignmentY = ItemAlignment::Center);

    BaseWindow& GetWindow() const;

  protected:
    void SetBackgroundColor(const UI::UIColor color);

    std::shared_ptr<Theme::IThemeControlFactory> GetThemeControlFactory() const;

    void SetWindowAlignmentX(const ItemAlignment alignment);
    void SetWindowAlignmentY(const ItemAlignment alignment);
  };
}


#endif
