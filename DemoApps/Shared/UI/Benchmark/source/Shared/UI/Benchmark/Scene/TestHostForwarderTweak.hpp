#ifndef SHARED_UI_BENCHMARK_SCENE_TESTHOSTFORWARDERTWEAK_HPP
#define SHARED_UI_BENCHMARK_SCENE_TESTHOSTFORWARDERTWEAK_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <Shared/UI/Benchmark/App/TestAppHost.hpp>
#include <Shared/UI/Benchmark/IForwarderTweak.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  //! This tweak object ensures the set app rectangle 'workaround' is in sync (so we do the layout on the main UI, call the tweak, so we have the
  //! right sizes when we do the layout on the second)
  class TestHostForwarderTweak final : public IForwarderTweak
  {
    std::weak_ptr<UI::BaseWindow> m_contentWindow;
    std::weak_ptr<TestAppHost> m_appHost;

  public:
    TestHostForwarderTweak(std::weak_ptr<UI::BaseWindow> contentWindow, std::weak_ptr<TestAppHost> appHost)
      : m_contentWindow(std::move(contentWindow))
      , m_appHost(std::move(appHost))
    {
    }

    void PostUpdate() final
    {
      // m_uiProfile.ContentArea

      // This is a bit of a hack and there might be a one frame delay from the control having its layout applied to the host getting the
      // information. A proper solution would require us to wrap the host inside the 'content-control'. But that requires us to hookup the 'demo
      // host extensions' and 'demo app' in a different way so we have better control of when the different function are being called. This might be
      // required for run the 'second UI instance' in 'benchmark' mode anyway.
      std::shared_ptr<UI::BaseWindow> windowEx = m_contentWindow.lock();
      const UI::BaseWindow* const pContentWindow = windowEx.get();
      if (pContentWindow != nullptr)
      {
        std::shared_ptr<TestAppHost> appHost = m_appHost.lock();
        if (appHost)
        {
          const auto offsetPx = pContentWindow->PointTo(nullptr, PxPoint2());
          PxRectangle contentRectanglePx = pContentWindow->WinGetContentRectanglePx();
          contentRectanglePx.Add(offsetPx);
          if (contentRectanglePx.Width() > 0 && contentRectanglePx.Height() > 0)
          {
            appHost->SetAppRectangle(contentRectanglePx);
          }
        }
      }
    }
  };
}

#endif
