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

#include <FslBase/IO/Path.hpp>
#include <FslBase/ITag.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include <FslNativeWindow/Platform/Adapter/Android/PlatformNativeWindowSystemAndroidTag.hpp>
#include <Platform/Android/JNIUtil/JNIUtil.hpp>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/sensor.h>
#include <errno.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <jni.h>
#include <memory>

#define SYNC_CLASS_NAME "com/freescale/demoplatform/DemoHelper"

namespace Fsl
{
  std::vector<std::string> GetCommandLineArguments(android_app* pAppState)
  {
    JNIEnv* jni;
    pAppState->activity->vm->AttachCurrentThread(&jni, nullptr);

    // Default class retrieval
    jclass clazz = jni->GetObjectClass(pAppState->activity->javaGameActivity);
    jmethodID methodID0 = jni->GetMethodID(clazz, "GetCommandLineArgumentCount", "()I");
    jmethodID methodID1 = jni->GetMethodID(clazz, "TryGetCommandLineArgument", "(I)Ljava/lang/String;");
    jint argCount = jni->CallIntMethod(pAppState->activity->javaGameActivity, methodID0);

    std::vector<std::string> args(argCount >= 0 ? argCount : 0);

    for (jint i = 0; i < argCount; ++i)
    {
      jstring argument = (jstring)jni->CallObjectMethod(pAppState->activity->javaGameActivity, methodID1, i);
      const char* pszArgument = jni->GetStringUTFChars(argument, nullptr);
      args[i].assign(pszArgument);
      jni->ReleaseStringUTFChars(argument, pszArgument);
      jni->DeleteLocalRef(argument);
    }
    jni->DeleteLocalRef(clazz);

    pAppState->activity->vm->DetachCurrentThread();
    return args;
  }

  int AndroidMain(android_app* state)
  {
    // LogConfig::SetLogLevel(LogType::Verbose4);

    // Init helper functions
    JNIUtil::Init(state->activity, SYNC_CLASS_NAME);

    const std::string externalFilesDir = JNIUtil::GetInstance()->GetExternalFilesDir();

    const IO::Path baseDir(JNIUtil::GetInstance()->SyncNow());
    const IO::Path contentDir = IO::Path::Combine(baseDir, "fsl-content");
    const IO::Path saveDir = IO::Path::Combine(baseDir, "fsl-save");

    FSLLOG3_INFO("PersistentDataPath: {}", saveDir);

#ifdef USE_NDK_PROFILER
    monstartup("libDemoNativeActivity.so");
#endif
    const auto arguments = GetCommandLineArguments(state);
    const char* const pszApp = "AndroidExe";
    std::vector<StringViewLite> argv(arguments.size() + 1);
    argv[0] = StringViewLite(pszApp);
    for (std::size_t i = 0; i < arguments.size(); ++i)
    {
      argv[i + 1] = StringViewLite(arguments[i]);
    }

    std::shared_ptr<Fsl::ITag> tag(new Fsl::PlatformNativeWindowSystemAndroidTag(state));
    Fsl::DemoRunnerConfig config(false, contentDir, saveDir, tag);
    return Fsl::RunDemo(SpanUtil::AsReadOnlySpan(argv), config);
  }
}

#endif
