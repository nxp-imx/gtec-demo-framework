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
#include <fstream>
#include <iostream>

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>

#include "JNIUtil.hpp"

#define CLASS_NAME "android/app/NativeActivity"

namespace Fsl
{
  JNIUtil* JNIUtil::GetInstance()
  {
    static JNIUtil util;
    return &util;
  }


  JNIUtil::JNIUtil()
  {
    pthread_mutex_init(&mutex_, nullptr);
  }


  JNIUtil::~JNIUtil()
  {
    pthread_mutex_lock(&mutex_);

    JNIEnv *env;
    activity_->vm->AttachCurrentThread(&env, nullptr);

    env->DeleteGlobalRef(jni_util_java_ref_);
    env->DeleteGlobalRef(jni_util_java_class_);

    activity_->vm->DetachCurrentThread();

    pthread_mutex_destroy(&mutex_);
  }

  void JNIUtil::Init(ANativeActivity* activity, const char* util_class_name)
  {
    JNIUtil& util = *GetInstance();
    pthread_mutex_lock(&util.mutex_);

    util.activity_ = activity;

    JNIEnv *env;
    util.activity_->vm->AttachCurrentThread(&env, nullptr);

    //Retrieve app name
    jclass android_content_Context = env->GetObjectClass(util.activity_->clazz);
    jmethodID midGetPackageName = env->GetMethodID(android_content_Context, "getPackageName",
      "()Ljava/lang/String;");

    jstring packageName = (jstring)env->CallObjectMethod(util.activity_->clazz,
      midGetPackageName);
    const char* appname = env->GetStringUTFChars(packageName, nullptr);
    util.app_name_ = std::string(appname);

    jclass cls = util.RetrieveClass(env, util_class_name);
    util.jni_util_java_class_ = (jclass)env->NewGlobalRef(cls);

    jmethodID constructor = env->GetMethodID(util.jni_util_java_class_, "<init>", "()V");
    util.jni_util_java_ref_ = env->NewObject(util.jni_util_java_class_, constructor);
    util.jni_util_java_ref_ = env->NewGlobalRef(util.jni_util_java_ref_);

    env->ReleaseStringUTFChars(packageName, appname);
    util.activity_->vm->DetachCurrentThread();

    pthread_mutex_unlock(&util.mutex_);
  }


  const std::string JNIUtil::SyncNow()
  {
    if (activity_ == nullptr)
    {
      LOGI("JNIutil has not been initialized. Call init() to initialize the util");
      return std::string();
    }

    JNIEnv *env;
    jmethodID mid;

    pthread_mutex_lock(&mutex_);
    activity_->vm->AttachCurrentThread(&env, nullptr);



    mid = env->GetMethodID(jni_util_java_class_, "SyncNow", "(Landroid/content/Context;)Ljava/lang/String;");
    jstring str_path = (jstring) env->CallObjectMethod(jni_util_java_ref_, mid, activity_->clazz);

    std::string s;
    if( str_path != nullptr )
    {
      const char* path = env->GetStringUTFChars( str_path, nullptr );
      s = path;
      env->ReleaseStringUTFChars( str_path, path );
      env->DeleteLocalRef( str_path );
    }


    activity_->vm->DetachCurrentThread();
    pthread_mutex_unlock(&mutex_);

    return s;
  }


  bool JNIUtil::TryLoadImage(Bitmap& rBitmap, const std::string& path)
  {
    if (activity_ == nullptr)
    {
      LOGI("JNIutil has not been initialized. Call init() to initialize the util");
      return false;
    }

    JNIEnv *env;
    jmethodID mid;

    bool loadCompleted = false;

    pthread_mutex_lock(&mutex_);
    activity_->vm->AttachCurrentThread(&env, nullptr);
    {
      // First we try to open the file
      mid = env->GetMethodID(jni_util_java_class_, "TryOpen", "(Landroid/content/Context;Ljava/lang/String;)Landroid/graphics/Bitmap;");
      jstring javaPath = env->NewStringUTF(path.c_str());
      jobject javaBitmap = env->CallObjectMethod(jni_util_java_ref_, mid, activity_->clazz, javaPath);
      env->DeleteLocalRef(javaPath);
      if( javaBitmap != nullptr )
      {
        // We managed to get a bitmap, so lets query the bitmap for some basic information
        mid = env->GetMethodID(jni_util_java_class_, "GetWidth", "(Landroid/graphics/Bitmap;)I");
        const int bitmapWidth = env->CallIntMethod(jni_util_java_ref_, mid, javaBitmap);
        mid = env->GetMethodID(jni_util_java_class_, "GetHeight", "(Landroid/graphics/Bitmap;)I");
        const int bitmapHeight = env->CallIntMethod(jni_util_java_ref_, mid, javaBitmap);

        // Then extract the bitmap data
        jintArray javaPixelArray = env->NewIntArray(bitmapWidth * bitmapHeight);
        if( javaPixelArray != nullptr )
        {
          mid = env->GetMethodID(jni_util_java_class_, "GetPixels", "(Landroid/graphics/Bitmap;[I)V");
          env->CallVoidMethod(jni_util_java_ref_, mid, javaBitmap, javaPixelArray);

          // Now we extract the pixels
          {
            jint* pPixels = env->GetIntArrayElements(javaPixelArray, 0);
            try
            {
              rBitmap.Reset(pPixels, Extent2D(bitmapWidth, bitmapHeight), PixelFormat::B8G8R8A8_UINT);
              loadCompleted = true;
            }
            catch(const std::exception& ex)
            {
              FSLLOG_ERROR("Failed to load: " << ex.what());
            }

            env->ReleaseIntArrayElements(javaPixelArray, pPixels, 0);
          }
          env->DeleteLocalRef(javaPixelArray);
        }

        // And then we close the bitmap again
        mid = env->GetMethodID(jni_util_java_class_, "Close", "(Landroid/graphics/Bitmap;)V");
        env->CallVoidMethod(jni_util_java_ref_, mid, javaBitmap);
        env->DeleteLocalRef(javaBitmap);
      }
    }
    activity_->vm->DetachCurrentThread();
    pthread_mutex_unlock(&mutex_);

    return loadCompleted;
  }



  jclass JNIUtil::RetrieveClass(JNIEnv *jni, const char* class_name)
  {
    jclass activity_class = jni->FindClass(CLASS_NAME);
    jmethodID get_class_loader = jni->GetMethodID(activity_class, "getClassLoader",
      "()Ljava/lang/ClassLoader;");
    jobject cls = jni->CallObjectMethod(activity_->clazz, get_class_loader);
    jclass class_loader = jni->FindClass("java/lang/ClassLoader");
    jmethodID find_class = jni->GetMethodID(class_loader, "loadClass",
      "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = jni->NewStringUTF(class_name);
    jclass class_retrieved = (jclass)jni->CallObjectMethod(cls, find_class, str_class_name);
    jni->DeleteLocalRef(str_class_name);
    return class_retrieved;
  }


} //namespace ndkutil
#endif
