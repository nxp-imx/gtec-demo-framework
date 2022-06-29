/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "Window101.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfo.hpp>
#include <FslNativeWindow/Base/INativeWindow.hpp>
#include <FslNativeWindow/Base/INativeWindowSystem.hpp>

namespace Fsl
{
  Window101::Window101(const DemoAppConfig& config)
    : DemoAppWindow(config)
  {
    auto windowHostInfo = config.DemoServiceProvider.Get<IWindowHostInfo>();

    // Be very careful what you call on this
    auto windowSystem = windowHostInfo->GetWindowSystem();

    // Get all the active windows
    auto windows = windowHostInfo->GetWindows();

    // Query them for information directly
    FSLLOG3_INFO("Active windows: {}", windows.size());
    for (std::size_t i = 0; i < windows.size(); ++i)
    {
      auto window = windows[i].lock();
      FSLLOG3_INFO("Window #{}", i);
      if (window)
      {
        PxExtent2D extent;
        if (window->TryGetExtent(extent))
        {
          FSLLOG3_INFO("- Extent:     {}", extent);
        }
        else
        {
          FSLLOG3_INFO("- Extent:     Failed");
        }

        Vector2 dpi;
        if (window->TryGetDpi(dpi))
        {
          FSLLOG3_INFO("- DPI:        {}", dpi);
        }
        else
        {
          FSLLOG3_INFO("- DPI:        Failed");
        }

        uint32_t densityDpi = 0;
        if (window->TryGetDensityDpi(densityDpi))
        {
          FSLLOG3_INFO("- DensityDPI: {}", densityDpi);
        }
        else
        {
          FSLLOG3_INFO("- DensityDPI: Failed");
        }

        auto windowMetrics = window->GetWindowMetrics();
        FSLLOG3_INFO("- WindowMetrics.ExactDpi: {}", windowMetrics.ExactDpi);
        FSLLOG3_INFO("- WindowMetrics.DensityDpi: {}", windowMetrics.DensityDpi);
        FSLLOG3_INFO("- WindowMetrics.DensityScaleFactor: {}", windowMetrics.DensityScaleFactor);
      }
      else
      {
        FSLLOG3_INFO("- has been closed");
      }
    }
  }


  Window101::~Window101() = default;


  void Window101::Update(const DemoTime& /*demoTime*/)
  {
  }


  void Window101::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);
  }
}
