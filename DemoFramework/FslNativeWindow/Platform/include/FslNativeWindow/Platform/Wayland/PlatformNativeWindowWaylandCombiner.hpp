#ifndef FSLNATIVEWINDOW_PLATFORM_WAYLAND_PLATFORMNATIVEWINDOWWAYLANDCOMBINER_HPP
#define FSLNATIVEWINDOW_PLATFORM_WAYLAND_PLATFORMNATIVEWINDOWWAYLANDCOMBINER_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowWaylandCallbacks.hpp>
#include <functional>
#include <memory>

namespace Fsl
{
  class PlatformNativeWindowWaylandDestroyCallbackCombiner
  {
    // Make sure the two callbacks are called
    static void CombineCallbackNativeWindowWaylandDestroy(void* nativeWindow, const PlatformCallbackNativeWindowWaylandDestroy& func1,
                                                          const PlatformCallbackNativeWindowWaylandDestroy& func2)
    {
      func1(nativeWindow);
      func2(nativeWindow);
    }

  public:
    //! Create a new function pointer that calls both func1 and func2 in order
    static PlatformCallbackNativeWindowWaylandDestroy Combine(const PlatformCallbackNativeWindowWaylandDestroy& func1,
                                                              const PlatformCallbackNativeWindowWaylandDestroy& func2)
    {
      using namespace std::placeholders;
      if (func1)
      {
        if (func2)
        {
          return std::bind(CombineCallbackNativeWindowWaylandDestroy, _1, func1, func2);
        }
        return func1;
      }
      if (func2)
      {
        return func2;
      }
      return nullptr;
    }
  };

  class PlatformNativeWindowWaylandResizeCallbackCombiner
  {
    // Make sure the two callbacks are called
    static void CombineCallbackNativeWindowWaylandResize(void* nativeWindow, int width, int height, int dx, int dy,
                                                         const PlatformCallbackNativeWindowWaylandResize& func1,
                                                         const PlatformCallbackNativeWindowWaylandResize& func2)
    {
      func1(nativeWindow, width, height, dx, dy);
      func2(nativeWindow, width, height, dx, dy);
    }

  public:
    //! Create a new function pointer that calls both func1 and func2 in order
    static PlatformCallbackNativeWindowWaylandResize Combine(const PlatformCallbackNativeWindowWaylandResize& func1,
                                                             const PlatformCallbackNativeWindowWaylandResize& func2)
    {
      using namespace std::placeholders;
      if (func1)
      {
        if (func2)
        {
          return std::bind(CombineCallbackNativeWindowWaylandResize, _1, _2, _3, _4, _5, func1, func2);
        }

        return func1;
      }
      if (func2)
      {
        return func2;
      }
      return nullptr;
    }
  };
}    // namespace Fsl

#endif
