#ifndef PLATFORM_ANDROID_JNIUTIL_JNIUTIL_HPP
#define PLATFORM_ANDROID_JNIUTIL_JNIUTIL_HPP
#if defined(__ANDROID__)
/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// The code here is based on JNIHelper from the NDK

#include <jni.h>
#include <mutex>
#include <string>
#include <vector>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include "NDKHelper.h"

namespace Fsl
{
  class JNIUtil
  {
  private:
    std::string app_name_;

    ANativeActivity* activity_{nullptr};
    jobject jni_util_java_ref_{};
    jclass jni_util_java_class_{};

    // mutex for synchronization
    // This class uses singleton pattern and can be invoked from multiple threads,
    // each methods locks the mutex for a thread safety
    mutable std::mutex mutex_;

    jstring GetExternalFilesDirJString(JNIEnv* env);
    jclass RetrieveClass(JNIEnv* jni, const char* class_name);

    JNIUtil();
    ~JNIUtil();
    JNIUtil(const JNIUtil& rhs);
    JNIUtil& operator=(const JNIUtil& rhs);

  public:
    /*
     * To load your own Java classes, JNIUtil requires to be initialized with a ANativeActivity handle.
     * This methods need to be called before any call to the util class.
     * Static member of the class
     *
     * arguments:
     * in: activity, pointer to ANativeActivity. Used internally to set up JNI environment
     * in: util_class_name, pointer to Java side util class name. (e.g. "com/sample/util/NDKutil" in samples )
     */
    static void Init(ANativeActivity* activity, const char* util_class_name);

    /*
    * Retrieve the singleton object of the util.
    * Static member of the class

    * Methods in the class are designed as thread safe.
    */
    static JNIUtil* GetInstance();

    //! @brief Returns the base path that content can be written to
    const std::string SyncNow();

    //! @brief Try to load a image.
    bool TryLoadImage(Bitmap& rBitmap, const std::string& path);

    //! @brief Check if the display is considered HDR compatible
    bool IsDisplayHDRCompatible() const;
  };
}
#endif
#endif
