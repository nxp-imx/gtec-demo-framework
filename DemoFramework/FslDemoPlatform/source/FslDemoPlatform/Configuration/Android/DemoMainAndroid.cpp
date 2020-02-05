#if defined(__ANDROID__)
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

#include <jni.h>
#include <errno.h>
#include <memory>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <FslBase/IO/Path.hpp>
#include <FslBase/ITag.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslNativeWindow/Platform/Android/PlatformNativeWindowSystemAndroidTag.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include "NDKHelper.h"
#include <Platform/Android/JNIUtil/JNIUtil.hpp>


#define SYNC_CLASS_NAME "com/freescale/demoplatform/DemoHelper"

namespace Fsl
{
  int AndroidMain(android_app* state, const char* const pszHelperClassName)
  {
    // Init helper functions
    ndk_helper::JNIHelper::Init(state->activity, pszHelperClassName);
    JNIUtil::Init(state->activity, SYNC_CLASS_NAME);

    const std::string externalFilesDir = ndk_helper::JNIHelper::GetInstance()->GetExternalFilesDir();

    const IO::Path baseDir = JNIUtil::GetInstance()->SyncNow();
    const IO::Path contentDir = IO::Path::Combine(baseDir, "fsl-content");
    const IO::Path saveDir = IO::Path::Combine(baseDir, "fsl-save");


#ifdef USE_NDK_PROFILER
    monstartup("libDemoNativeActivity.so");
#endif

    char pszEmpty[] = {0};
    char* argv[] = {pszEmpty, pszEmpty};
    std::shared_ptr<Fsl::ITag> tag(new Fsl::PlatformNativeWindowSystemAndroidTag(state));
    Fsl::DemoRunnerConfig config(false, contentDir, saveDir, tag);
    return Fsl::RunDemo(1, argv, config);
  }
}

#endif
