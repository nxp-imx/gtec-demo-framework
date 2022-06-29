#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_TESTUITREE_WINDOW_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_TESTUITREE_WINDOW_HPP
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

#include <FslBase/Time/TimeSpan.hpp>
#include <FslSimpleUI/Base/System/UITree.hpp>
#include <FslSimpleUI/Base/UnitTest/BaseWindowTest.hpp>
#include <FslSimpleUI/Base/UnitTest/TestFixtureFslSimpleUIUITree.hpp>
#include <FslSimpleUI/Base/UnitTest/WindowMethodFlags.hpp>

class TestUITree_Window : public TestFixtureFslSimpleUIUITree
{
public:
  void CheckZeroExcept(const Fsl::UI::WindowCallCount& callCount, const WindowMethodFlags& ignoreFlags)
  {
    CheckZero(callCount, WindowMethodFlags(static_cast<uint32_t>(WindowMethod::All) & (~ignoreFlags.Value)));
  }

  void CheckZero(const Fsl::UI::WindowCallCount& callCount, const WindowMethodFlags& checkFlags)
  {
    if (checkFlags.IsEnabled(WindowMethod::WinInit))
    {
      ASSERT_EQ(0u, callCount.WinInit);
    }
    if (checkFlags.IsEnabled(WindowMethod::WinMarkLayoutAsDirty))
    {
      ASSERT_EQ(0u, callCount.WinMarkLayoutAsDirty);
    }
    // if (checkFlags.IsEnabled(WindowMethod::WinGetContentPxRectangle))
    //{
    //  ASSERT_EQ(0u, callCount.WinGetContentRect);
    //}
    if (checkFlags.IsEnabled(WindowMethod::WinHandleEvent))
    {
      ASSERT_EQ(0u, callCount.WinHandleEvent);
    }
    if (checkFlags.IsEnabled(WindowMethod::WinUpdate))
    {
      ASSERT_EQ(0u, callCount.WinUpdate);
    }
    if (checkFlags.IsEnabled(WindowMethod::WinResolve))
    {
      ASSERT_EQ(0u, callCount.WinResolve);
    }
    if (checkFlags.IsEnabled(WindowMethod::WinDraw))
    {
      ASSERT_EQ(0u, callCount.WinDraw);
    }
    if (checkFlags.IsEnabled(WindowMethod::OnClickInputPreview))
    {
      ASSERT_EQ(0u, callCount.OnClickInputPreview);
    }
    if (checkFlags.IsEnabled(WindowMethod::OnClickInput))
    {
      ASSERT_EQ(0u, callCount.OnClickInput);
    }
    if (checkFlags.IsEnabled(WindowMethod::OnSelect))
    {
      ASSERT_EQ(0u, callCount.OnSelect);
    }
    if (checkFlags.IsEnabled(WindowMethod::OnContentChanged))
    {
      ASSERT_EQ(0u, callCount.OnContentChanged);
    }
    if (checkFlags.IsEnabled(WindowMethod::ArrangeOverride))
    {
      ASSERT_EQ(0u, callCount.ArrangeOverride);
    }
    if (checkFlags.IsEnabled(WindowMethod::MeasureOverride))
    {
      ASSERT_EQ(0u, callCount.MeasureOverride);
    }
    if (checkFlags.IsEnabled(WindowMethod::OnPropertiesUpdated))
    {
      ASSERT_EQ(0u, callCount.OnPropertiesUpdated);
    }
  }
};

#endif
