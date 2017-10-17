#ifndef FSLNATIVEWINDOW_PLATFORM_ANDROID_PLATFORMNATIVEWINDOWANDROIDCALLBACKCOMBINER_HPP
#define FSLNATIVEWINDOW_PLATFORM_ANDROID_PLATFORMNATIVEWINDOWANDROIDCALLBACKCOMBINER_HPP
#ifdef __ANDROID__
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

#include <FslNativeWindow/Platform/Android/PlatformNativeWindowAndroidCallbacks.hpp>
#include <functional>
#include <memory>

namespace Fsl
{
  class PlatformNativeWindowAndroidCallbackCombiner
  {
    // Make sure the two callbacks are called
    static void CombineCallbackNativeWindowAndroidOnWindow(ANativeWindow* pNativeWindow, android_app* pAndroidApp, const PlatformCallbackNativeWindowAndroidOnWindowCreate& func1, const PlatformCallbackNativeWindowAndroidOnWindowCreate& func2)
    {
      func1(pNativeWindow, pAndroidApp);
      func2(pNativeWindow, pAndroidApp);
    }

  public:
    //! Create a new function pointer that calls both func1 and func2 in order
    static PlatformCallbackNativeWindowAndroidOnWindowCreate Combine(const PlatformCallbackNativeWindowAndroidOnWindowCreate& func1, const PlatformCallbackNativeWindowAndroidOnWindowCreate& func2)
    {
      using namespace std::placeholders;
      if( func1 )
      {
        if( func2 )
          return std::bind(CombineCallbackNativeWindowAndroidOnWindow, _1, _2, func1, func2);
        else
          return func1;
      }
      else if( func2 )
      {
        return func2;
      }
      return nullptr;
    }
  };
}

#endif
#endif
