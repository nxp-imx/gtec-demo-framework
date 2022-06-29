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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/Image/IImageServiceControl.hpp>
#include <FslDemoHost/Base/Service/WindowHost/IWindowHostInfoControl.hpp>
#include <FslDemoHost/EGL/Config/DemoAppHostConfigEGL.hpp>
#include <FslDemoHost/EGL/EGLDemoHost.hpp>
#include <FslDemoHost/EGL/EGLDemoHostOptionParser.hpp>
#include <FslDemoService/Graphics/Control/GraphicsDeviceCreateInfo.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceHost.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindowSystemFactory.hpp>
#include <FslNativeWindow/EGL/IEGLNativeWindowSystem.hpp>
#include <FslNativeWindow/EGL/IEGLNativeWindow.hpp>
#include <FslNativeWindow/Base/NativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowProcessMessagesArgs.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslUtil/EGL/EGLCheck.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <FslUtil/EGL/EGLConfigUtil.hpp>
#include <FslUtil/EGL/EGLStringUtil.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <fmt/core.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
//#include <EGL/eglext.h>
#include "Service/EGLHost/EGLHostService.hpp"

#if 1
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG(X) FSLLOG3_VERBOSE4("EGLDemoHost: {}", (X))
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG(X) \
  {                  \
  }
#endif

#if 1
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG_WARNING(X) FSLLOG3_WARNING("EGLDemoHost: {}", (X))
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_LOG_WARNING(X) \
  {                          \
  }
#endif

#ifndef EGL_PLATFORM_ANGLE_ANGLE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_ANGLE 0x3202
#endif
#ifndef EGL_PLATFORM_ANGLE_TYPE_ANGLE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
#endif
#ifndef EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE 0x3450
#endif
#ifndef EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE 0x3209
#endif
#ifndef EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE 0x320A
#endif
#ifndef EGL_PLATFORM_ANGLE_DEBUG_LAYERS_ENABLED
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_PLATFORM_ANGLE_DEBUG_LAYERS_ENABLED 0x3451
#endif


namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const EGLConfig EmptyValueEGLConfig = nullptr;
    }

    struct RGBConfig
    {
      int32_t R{8};
      int32_t G{8};
      int32_t B{8};

      RGBConfig() = default;

      RGBConfig(const int32_t r, const int32_t g, const int32_t b)
        : R(r)
        , G(g)
        , B(b)
      {
      }
    };

    const int32_t DEFAULT_DEPTH_BUFFER_SIZE = 24;
    // const int32_t FALLBACK_DEPTH_BUFFER_SIZE = 16;
    const RGBConfig DEFAULT_RGB_CONFIG(8, 8, 8);


    //! @brief copy the supplied EGL config into a deque
    void CopyConfig(std::deque<EGLint>& rEglAttribs, const EGLint* pEglAttribs)
    {
      rEglAttribs.clear();
      if (pEglAttribs != nullptr)
      {
        const int MAX = 1024;
        int count = 0;
        while (*pEglAttribs != EGL_NONE && count < MAX)
        {
          rEglAttribs.push_back(*pEglAttribs);
          ++pEglAttribs;
          ++count;
        }
        if (*pEglAttribs != EGL_NONE)
        {
          throw std::invalid_argument("pEglAttribs seems to be invalid");
        }

        rEglAttribs.push_back(*pEglAttribs);
      }
    }


    void BuildDefaultConfigAttributes(std::deque<EGLint>& rConfigAttributes, const EGLDemoHostFeatureConfig& featureConfig,
                                      const bool bForceConformance, const RGBConfig& preferredRGBConfig, const int32_t preferredDepthBufferSize)
    {
      FSL_PARAM_NOT_USED(bForceConformance);
      rConfigAttributes.push_back(EGL_RED_SIZE);
      rConfigAttributes.push_back(preferredRGBConfig.R);

      rConfigAttributes.push_back(EGL_GREEN_SIZE);
      rConfigAttributes.push_back(preferredRGBConfig.G);

      rConfigAttributes.push_back(EGL_BLUE_SIZE);
      rConfigAttributes.push_back(preferredRGBConfig.B);

      if (featureConfig.EnableGLES)
      {
        switch (featureConfig.ESVersionMajor)
        {
        case 2:
          rConfigAttributes.push_back(EGL_RENDERABLE_TYPE);
          rConfigAttributes.push_back(EGL_OPENGL_ES2_BIT);
          break;
        case 3:
#ifdef EGL_OPENGL_ES3_BIT
          rConfigAttributes.push_back(EGL_RENDERABLE_TYPE);
          rConfigAttributes.push_back(EGL_OPENGL_ES3_BIT);
#elif defined(EGL_OPENGL_ES3_BIT_KHR)
          rConfigAttributes.push_back(EGL_RENDERABLE_TYPE);
          rConfigAttributes.push_back(EGL_OPENGL_ES3_BIT_KHR);
#endif
          break;
        default:
          throw NotSupportedException("Unknown OpenGLES version request");
        }
        rConfigAttributes.push_back(EGL_DEPTH_SIZE);
        rConfigAttributes.push_back(preferredDepthBufferSize);
      }

      if (featureConfig.EnableVG)
      {
        rConfigAttributes.push_back(EGL_RENDERABLE_TYPE);
        rConfigAttributes.push_back(EGL_OPENVG_BIT);

        //  rConfigAttributes.push_back(EGL_ALPHA_SIZE);
        //  rConfigAttributes.push_back(8);
      }

      // if (bUseOpenGLES && bForceConformance)
      //{
      //  rConfigAttributes.push_back(EGL_CONFORMANT);
      //  rConfigAttributes.push_back(EGL_OPENGL_ES2_BIT);
      //}

      // While the default ought to be  EGL_SURFACE_TYPE = EGL_WINDOW_BIT
      // The arm mali 1.3.2 and 1.4 emulator fails to render correctly without this on newer nvidia cards.
      rConfigAttributes.push_back(EGL_SURFACE_TYPE);
      rConfigAttributes.push_back(EGL_WINDOW_BIT);

      rConfigAttributes.push_back(EGL_SAMPLES);
      rConfigAttributes.push_back(0);

      rConfigAttributes.push_back(EGL_NONE);
    }


    Point2 GetEGLSurfaceResolution(EGLDisplay hEglDisplay, EGLSurface hEglSurface)
    {
      EGLint windowWidth = 0;
      EGLint windowHeight = 0;
      eglQuerySurface(hEglDisplay, hEglSurface, EGL_WIDTH, &windowWidth);
      eglQuerySurface(hEglDisplay, hEglSurface, EGL_HEIGHT, &windowHeight);
      return {windowWidth, windowHeight};
    }


    //! @brief replace the attribute if its found
    //! @return true if replaced, false if not found
    bool ReplaceAttribute(std::deque<EGLint>& rConfigAttributes, const EGLint key, const EGLint value)
    {
      assert((rConfigAttributes.size() % 2) == 1 && rConfigAttributes.back() == EGL_NONE);

      auto itr = rConfigAttributes.begin();
      int index = 0;
      while (itr != rConfigAttributes.end() && *itr != EGL_NONE)
      {
        if ((index & 1) == 0 && *itr == key)
        {
          ++itr;
          *itr = value;
          return true;
        }
        ++itr;
        ++index;
      }
      return false;
    }

    void MergeWithOverwriteConfigAttributes(std::deque<EGLint>& rConfigAttributes, const std::deque<EGLint>& srcConfigAttribs)
    {
      if (srcConfigAttribs.empty())
      {
        return;
      }

      assert((rConfigAttributes.size() % 2) == 1);
      assert((srcConfigAttribs.size() % 2) == 1);

      auto itr = srcConfigAttribs.begin();
      while (itr != srcConfigAttribs.end() && *itr != EGL_NONE)
      {
        EGLint key = *itr;
        ++itr;
        EGLint value = *itr;
        ++itr;

        if (!ReplaceAttribute(rConfigAttributes, key, value))
        {
          rConfigAttributes.pop_back();
          rConfigAttributes.push_back(key);
          rConfigAttributes.push_back(value);
          rConfigAttributes.push_back(EGL_NONE);
        }
      }
    }

    void ApplyHostOverrides(std::deque<EGLint>& rConfigAttributes, const std::shared_ptr<EGLDemoHostOptionParser>& options)
    {
      std::deque<EGLint> forceConfigAttributes;
      options->ExtractConfigAttributes(forceConfigAttributes);
      if (!forceConfigAttributes.empty())
      {
        MergeWithOverwriteConfigAttributes(rConfigAttributes, forceConfigAttributes);
      }
    }

    EGLDemoHostFeatureConfig ExamineFeatureRequest(const std::shared_ptr<DemoHostAppSetup::DemoHostFeatureDeque>& features)
    {
      EGLDemoHostFeatureConfig featureConfig;
      auto itr = features->begin();
      while (itr != features->end())
      {
        if (itr->Name == DemoHostFeatureName::OpenGLES)
        {
          featureConfig.EnableGLES = true;

          int major = 0;
          int minor = 0;
          DemoHostFeatureUtil::DecodeOpenGLESVersion(itr->Version, major, minor);
          featureConfig.ESVersionMajor = major;
          featureConfig.ESVersionMinor = minor;
          featureConfig.Feature = *itr;
        }
        else if (itr->Name == DemoHostFeatureName::OpenVG)
        {
          featureConfig.EnableVG = true;

          int major = 0;
          int minor = 0;
          DemoHostFeatureUtil::DecodeOpenVGVersion(itr->Version, major, minor);
          featureConfig.VGVersion = major;
          featureConfig.Feature = *itr;
        }
        ++itr;
      }
      return featureConfig;
    }

    void ExamineFinalConfig(const std::vector<EGLint>& finalConfigAttribs)
    {
      // start at 1 so that the size check is simpler
      for (std::size_t i = 1; i < finalConfigAttribs.size(); i += 2)
      {
        switch (finalConfigAttribs[i - 1])
        {
        case EGL_RED_SIZE:
        case EGL_GREEN_SIZE:
        case EGL_BLUE_SIZE:
        case EGL_ALPHA_SIZE:
          if (finalConfigAttribs[i] < 0)
          {
            FSLLOG3_WARNING("{} can not be negative.", EGLStringUtil::GetConfigEnumToString(finalConfigAttribs[i - 1]));
          }
          break;
        }
      }
    }

    void DoCheckExtensions(const EGLDisplay display, const std::deque<ExtensionRequestRecord>& extensionRequests)
    {
      const auto extensions = EGLUtil::GetExtensions(display);
      for (const auto& request : extensionRequests)
      {
        if (request.Type == ExtensionType::EGL && std::find(extensions.begin(), extensions.end(), request.Name) == extensions.end())
        {
          // Request not found, lets check how severe it is
          switch (request.Precense)
          {
          case ExtensionPrecense::Mandatory:
            throw std::runtime_error(std::string("Required EGL extension '") + request.Name + "' not found");
          case ExtensionPrecense::Optional:
            FSLLOG3_DEBUG_INFO("Optional EGL extension '{}' not available.", request.Name);
            break;
          default:
            throw NotSupportedException("Unsupported ExtensionPrecense");
          }
        }
      }
    }


    void DoLogExtensions(const EGLDisplay display, const bool isClientExtensions = false)
    {
      auto extensions = EGLUtil::GetExtensions(display);
      std::sort(extensions.begin(), extensions.end());
      FSLLOG3_INFO_IF(!isClientExtensions, "EGL Extensions");
      FSLLOG3_INFO_IF(isClientExtensions, "EGL Client Extensions");
      for (const auto& entry : extensions)
      {
        FSLLOG3_INFO("- {}", entry);
      }
    }

    inline bool IsHDRChannel(const EGLDisplay display, const EGLConfig config, const EGLint attribute)
    {
      EGLint value = 0;
      if (eglGetConfigAttrib(display, config, attribute, &value) == EGL_TRUE)
      {
        return value > 8;
      }
      FSLLOG3_DEBUG_WARNING("Failed to retrieve attribute: {}", EGLStringUtil::GetConfigEnumToString(attribute));
      return false;
    }

    //! For now any config that has a channel greater than 8 is considered hdr
    inline bool IsHDRConfig(const EGLDisplay display, const EGLConfig config)
    {
      return IsHDRChannel(display, config, EGL_RED_SIZE) || IsHDRChannel(display, config, EGL_GREEN_SIZE) ||
             IsHDRChannel(display, config, EGL_BLUE_SIZE);
    }


    void DoLogConfigs(EGLDemoHostOptionParser::ConfigLogMode logMode, const EGLDisplay display)
    {
      if (logMode == EGLDemoHostOptionParser::ConfigLogMode::Off)
      {
        return;
      }

      const bool hdrFilter = (logMode == EGLDemoHostOptionParser::ConfigLogMode::HDR);

      const auto configs = EGLUtil::GetConfigs(display);
      FSLLOG3_INFO("EGL display configs: {}", configs.size());
      std::size_t index = 0;
      std::size_t foundCount = 0;
      const auto attribs = EGLUtil::GetConfigAttribs();
      for (auto* const config : configs)
      {
        if (!hdrFilter || IsHDRConfig(display, config))
        {
          ++foundCount;
          FSLLOG3_INFO("*** Config index {}", index);
          for (const auto attribute : attribs)
          {
            EGLint value = 0;
            if (eglGetConfigAttrib(display, config, attribute, &value) == EGL_TRUE)
            {
              FSLLOG3_INFO("- {}: {}", EGLStringUtil::GetConfigEnumToString(attribute), EGLStringUtil::GetConfigAttribToString(attribute, value));
            }
            else
            {
              FSLLOG3_INFO("- {}: query failed", EGLStringUtil::GetConfigEnumToString(attribute));
            }
          }
        }
        ++index;
      }
      FSLLOG3_INFO_IF(hdrFilter && foundCount <= 0, "*** No HDR configs found");
    }


    void DoLogConfig(const std::vector<EGLint>& finalConfigAttribs)
    {
      // start at 1 so that the size check is simpler
      FSLLOG3_INFO("Requested EGL config");
      for (std::size_t i = 1; i < finalConfigAttribs.size(); i += 2)
      {
        FSLLOG3_INFO("{}={}", EGLStringUtil::GetConfigEnumToString(finalConfigAttribs[i - 1]),
                     EGLStringUtil::GetConfigAttribToString(finalConfigAttribs[i - 1], finalConfigAttribs[i]));
      }
    }

    void DoLogConfigComparison(EGLDisplay hDisplay, EGLConfig config, const std::vector<EGLint>& finalConfigAttribs)
    {
      FSLLOG3_INFO("Final vs requested EGL config");

      for (std::size_t i = 1; i < finalConfigAttribs.size(); i += 2)
      {
        const EGLint attrib = finalConfigAttribs[i - 1];
        const EGLint requestedValue = finalConfigAttribs[i];
        EGLint actualValue = 0;
        EGL_CHECK(eglGetConfigAttrib(hDisplay, config, attrib, &actualValue));

        if (actualValue != requestedValue)
        {
          FSLLOG3_INFO("{} is {} requested {}", EGLStringUtil::GetConfigEnumToString(finalConfigAttribs[i - 1]),
                       EGLStringUtil::GetConfigAttribToString(finalConfigAttribs[i - 1], actualValue),
                       EGLStringUtil::GetConfigAttribToString(finalConfigAttribs[i - 1], requestedValue));
        }
      }
    }


    void BuildAttribConfig(std::deque<EGLint>& rAppAttribs, const EGLint* appAttribs)
    {
      // Do a basic validation on the app's custom EGL config attribs
      CopyConfig(rAppAttribs, appAttribs);

      if (!(rAppAttribs.empty() || ((rAppAttribs.size() % 2) == 1 && rAppAttribs.back() == EGL_NONE)))
      {
        throw std::invalid_argument("eglConfigAttribs is in a unexpected format");
      }
    }


    void BuildEGLConfig(std::vector<EGLint>& rFinalConfigAttribs, const std::deque<EGLint>& appAglConfigAttribs, const ConfigControl configControl,
                        const EGLDemoHostFeatureConfig& featureConfig, const std::shared_ptr<EGLDemoHostOptionParser>& options,
                        const RGBConfig& preferredRGBConfig, const int32_t preferredDepthBufferSize)
    {
      // Then build our default egl config
      std::deque<EGLint> configAttribs;
      if (appAglConfigAttribs.empty() || configControl == ConfigControl::Overwrite)
      {
        BuildDefaultConfigAttributes(configAttribs, featureConfig, false, preferredRGBConfig, preferredDepthBufferSize);
      }
      else
      {
        configAttribs.push_back(EGL_NONE);
      }

      if (!appAglConfigAttribs.empty())
      {
        MergeWithOverwriteConfigAttributes(configAttribs, appAglConfigAttribs);
      }

      // Allow the command line options to override things in the config
      ApplyHostOverrides(configAttribs, options);

      // Store the final configuration for later use
      rFinalConfigAttribs.resize(configAttribs.size());
      std::copy(configAttribs.begin(), configAttribs.end(), rFinalConfigAttribs.begin());

      ExamineFinalConfig(rFinalConfigAttribs);
    }


    EGLContext DoEGLCreateContext(const EGLDisplay hDisplay, const EGLConfig hConfig, const bool enableGLES, const int eglContextClientVersionMajor,
                                  const int eglContextClientVersionMinor, const int eglContextClientVersionMinimumMinor)
    {
      std::array<EGLint, 3> contextAttribListESMajorOnly = {EGL_CONTEXT_CLIENT_VERSION, eglContextClientVersionMajor, EGL_NONE};
      bool supportsMinorVersion = true;
#if defined(EGL_CONTEXT_MAJOR_VERSION) && defined(EGL_CONTEXT_MINOR_VERSION)
      std::array<EGLint, 5> contextAttribListESMajorMinor = {EGL_CONTEXT_MAJOR_VERSION, eglContextClientVersionMajor, EGL_CONTEXT_MINOR_VERSION,
                                                             eglContextClientVersionMinor, EGL_NONE};
      EGLint* contextAttribListES = (eglContextClientVersionMajor < 3 || eglContextClientVersionMinor == 0) ? contextAttribListESMajorOnly.data()
                                                                                                            : contextAttribListESMajorMinor.data();
#elif defined(EGL_CONTEXT_MAJOR_VERSION_KHR) && defined(EGL_CONTEXT_MINOR_VERSION_KHR)
      std::array<EGLint, 5> contextAttribListESMajorMinor = {EGL_CONTEXT_MAJOR_VERSION_KHR, eglContextClientVersionMajor,
                                                             EGL_CONTEXT_MINOR_VERSION_KHR, eglContextClientVersionMinor, EGL_NONE};
      EGLint* contextAttribListES = (eglContextClientVersionMajor < 3 || eglContextClientVersionMinor == 0) ? contextAttribListESMajorOnly.data()
                                                                                                            : contextAttribListESMajorMinor.data();
#else
      EGLint* contextAttribListES = contextAttribListESMajorOnly.data();
      FSLLOG3_WARNING_IF(eglContextClientVersionMinor != 0, "EGL does not supported requesting a minor version. Hoping for the best");
      supportsMinorVersion = false;
#endif
      std::array<EGLint, 1> contextAttribListVG = {EGL_NONE};
      EGLint* contextAttribList = (enableGLES ? contextAttribListES : contextAttribListVG.data());

      EGLContext hContext = eglCreateContext(hDisplay, hConfig, EGL_NO_CONTEXT, contextAttribList);
      if (hContext == EGL_NO_CONTEXT)
      {
        // Get the error and thereby clearing the error flag
        const EGLint eglError = eglGetError();

        if (supportsMinorVersion && eglContextClientVersionMinor > eglContextClientVersionMinimumMinor)
        {
          // Recursive try a lesser minor version until we succeed or hit minor version zero
          hContext = DoEGLCreateContext(hDisplay, hConfig, enableGLES, eglContextClientVersionMajor, eglContextClientVersionMinor - 1,
                                        eglContextClientVersionMinimumMinor);
        }
        else if (eglError != EGL_SUCCESS)
        {
          throw EGLGraphicsException(fmt::format("Failed to create the requested context with error code {} at {}({})", eglError, __FILE__, __LINE__),
                                     eglError, __FILE__, __LINE__);
        }
        else
        {
          throw EGLGraphicsException("Failed to create the requested context");
        }
      }
      EGL_CHECK_FOR_ERROR();
      return hContext;
    }

    template <typename TContainer>
    bool IsHDRRequest(const TContainer& appEglConfigAttribs)
    {
      auto itr = appEglConfigAttribs.begin();
      const auto itrEnd = appEglConfigAttribs.end();
      while (itr != itrEnd)
      {
        const auto attrib = *itr;
        ++itr;
        if (itr != itrEnd)
        {
          switch (attrib)
          {
          case EGL_RED_SIZE:
          case EGL_GREEN_SIZE:
          case EGL_BLUE_SIZE:
            if (*itr > 8)
            {
              return true;
            }
            break;
          default:
            break;
          }

          ++itr;
        }
      }
      return false;
    }


    std::deque<EGLint> RemoveAttribs(const std::deque<EGLint>& appEglConfigAttribs, const std::vector<EGLint>& entriesToRemove)
    {
      std::deque<EGLint> filteredDeque;
      auto itr = appEglConfigAttribs.begin();
      const auto itrEnd = appEglConfigAttribs.end();
      while (itr != itrEnd)
      {
        if (std::find(entriesToRemove.begin(), entriesToRemove.end(), *itr) != entriesToRemove.end())
        {
          // Skipping the entries
          ++itr;
          if (itr != itrEnd)
          {
            ++itr;
          }
        }
        else
        {
          filteredDeque.push_back(*itr);
          ++itr;
          if (itr != itrEnd)
          {
            filteredDeque.push_back(*itr);
            ++itr;
          }
        }
      }
      return filteredDeque;
    }


    bool TryConfigFallbackDepth(const EGLDisplay hDisplay, std::vector<EGLint>& rFinalConfigAttribs, const std::deque<EGLint>& appAglConfigAttribs,
                                const ConfigControl configControl, const EGLDemoHostFeatureConfig& featureConfig,
                                const std::shared_ptr<EGLDemoHostOptionParser>& options, const RGBConfig& preferredRGBConfig,
                                const int32_t preferredDepthBufferSize, EGLConfig& rEGLConfig)
    {
      auto customAppAglConfigAttribs = RemoveAttribs(appAglConfigAttribs, {EGL_DEPTH_SIZE});
      {
        FSLLOG3_INFO("- Trying with a different color depth and depth buffer size. (D={})", preferredDepthBufferSize);
        BuildEGLConfig(rFinalConfigAttribs, customAppAglConfigAttribs, configControl, featureConfig, options, preferredRGBConfig,
                       preferredDepthBufferSize);

        // Lets try again
        EGLint numConfigs = 0;
        EGL_CHECK(eglChooseConfig(hDisplay, rFinalConfigAttribs.data(), &rEGLConfig, 1, &numConfigs));
        if (numConfigs == 1)
        {
          return true;
        }
      }
      return false;
    }


    bool TryConfigFallbackColorsAndDepth(const EGLDisplay hDisplay, std::vector<EGLint>& rFinalConfigAttribs,
                                         const std::deque<EGLint>& appAglConfigAttribs, const ConfigControl configControl,
                                         const EGLDemoHostFeatureConfig& featureConfig, const std::shared_ptr<EGLDemoHostOptionParser>& options,
                                         const RGBConfig& preferredRGBConfig, const int32_t preferredDepthBufferSize, EGLConfig& rEGLConfig)
    {
      auto customAppAglConfigAttribs = RemoveAttribs(appAglConfigAttribs, {EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE});
      {
        FSLLOG3_INFO("- Trying with a different color depth. (R={} G={} B={})", preferredRGBConfig.R, preferredRGBConfig.G, preferredRGBConfig.B);
        BuildEGLConfig(rFinalConfigAttribs, customAppAglConfigAttribs, configControl, featureConfig, options, preferredRGBConfig,
                       preferredDepthBufferSize);

        // Lets try again
        EGLint numConfigs = 0;
        EGL_CHECK(eglChooseConfig(hDisplay, rFinalConfigAttribs.data(), &rEGLConfig, 1, &numConfigs));
        if (numConfigs == 1)
        {
          return true;
        }
      }

      return TryConfigFallbackDepth(hDisplay, rFinalConfigAttribs, customAppAglConfigAttribs, configControl, featureConfig, options,
                                    preferredRGBConfig, preferredDepthBufferSize, rEGLConfig);
    }

    bool DetectEGLClientExtensionsSupport()
    {
      // EGL_EXT_client_extensions
      const auto* pszExtensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
      if (pszExtensions == nullptr)
      {
        auto error = eglGetError();
        if (error != EGL_BAD_DISPLAY)
        {
          FSLLOG3_VERBOSE2("Unexpected error from query {}");
        }
        return false;
      }
      // EGL_EXT_client_extensions might be supported, lets verify it
      auto extensions = StringUtil::Split(pszExtensions, ' ', true);
      return std::find(extensions.begin(), extensions.end(), "EGL_EXT_client_extensions") != extensions.end();
    }

    bool HasAngleSupport(const bool logExtensions)
    {
      auto supported = DetectEGLClientExtensionsSupport();
      if (!supported)
      {
        return false;
      }
      FSLLOG3_VERBOSE3("EGL_EXT_client_extensions: supported");

      if (logExtensions)
      {
        DoLogExtensions(EGL_NO_DISPLAY, true);
      }

      if (!EGLUtil::HasExtension(EGL_NO_DISPLAY, "EGL_EXT_platform_base"))
      {
        return false;
      }
      FSLLOG3_VERBOSE3("EGL_EXT_platform_base: supported");

      if (!EGLUtil::HasExtension(EGL_NO_DISPLAY, "EGL_ANGLE_platform_angle"))
      {
        return false;
      }

      FSLLOG3_VERBOSE3("EGL_ANGLE_platform_angle: supported");
      return true;
    }

    bool TryGetAngleDisplay(EGLDisplay& rDisplay, EGLNativeDisplayType nativeDisplayType)
    {
#ifdef FSL_PLATFORM_EMSCRIPTEN
      return false;
#else
      if (!EGLUtil::HasExtension(EGL_NO_DISPLAY, "EGL_ANGLE_platform_angle_vulkan"))
      {
        return false;
      }
      FSLLOG3_VERBOSE3("EGL_ANGLE_platform_angle_vulkan: supported");

      rDisplay = EGL_NO_DISPLAY;
      auto eglGetPlatformDisplayEXT =
        reinterpret_cast<EGLDisplay(EGLAPIENTRY*)(EGLenum, void*, const EGLint*)>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
      if (eglGetPlatformDisplayEXT == nullptr)
      {
        FSLLOG3_WARNING("Failed to get eglGetPlatformDisplayEXT");
        return false;
      }

      // eglGetPlatformDisplayEXT(EGLenum platform, void *native_display, const EGLint *attrib_list)

      std::array<EGLint, 7> attribList = {EGL_PLATFORM_ANGLE_TYPE_ANGLE,
                                          EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE,
                                          EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE,
                                          EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE,
                                          EGL_PLATFORM_ANGLE_DEBUG_LAYERS_ENABLED,
                                          EGL_FALSE,
                                          EGL_NONE};
      rDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, nativeDisplayType, attribList.data());
      EGL_CHECK_FOR_ERROR();
      return true;
#endif
    }
  }


  EGLDemoHost::EGLDemoHost(const DemoHostConfig& demoHostConfig)
    : ADemoHost(demoHostConfig, false)
    , m_demoHostConfig(demoHostConfig)
    , m_eglContextClientVersionMajor(0)
    , m_eglContextClientVersionMinor(0)
    , m_eglContextClientVersionMinimumMinor(0)
    , m_nativeWindowSetup()
    , m_windowHostInfoControl(demoHostConfig.GetServiceProvider().Get<IWindowHostInfoControl>())
    , m_hostService(demoHostConfig.GetServiceProvider().Get<EGLHostService>())
    , m_graphicsService(demoHostConfig.GetServiceProvider().Get<IGraphicsServiceHost>())
    , m_hDisplay(EGL_NO_DISPLAY)
    , m_hSurface(EGL_NO_SURFACE)
    , m_hContext(EGL_NO_CONTEXT)
    , m_hConfig(LocalConfig::EmptyValueEGLConfig)
    , m_isActivated(true)
    , m_options(demoHostConfig.GetOptions<EGLDemoHostOptionParser>())
    , m_configControl(ConfigControl::Default)
    , m_enableGLES(false)
    , m_enableVG(false)
    , m_logSelectedConfig(false)
    , m_logExtensions(false)
    , m_apiInit(false)
    , m_activeApi(DemoHostFeatureName::OpenGLES, 0)
    , m_maxFramesInFlight(demoHostConfig.GetDemoHostAppSetup().AppSetup.CustomAppConfig.MaxFramesInFlight)
  {
    const NativeWindowSystemSetup nativeWindowSystemSetup(demoHostConfig.GetEventQueue(), m_demoHostConfig.GetVerbosityLevel(),
                                                          m_options->GetNativeWindowConfig(), m_options->GetNativeWindowTag());

    m_windowSystem = EGLNativeWindowSystemFactory::Allocate(nativeWindowSystemSetup);
    // Set the window system in the host service so that any services or app that is interested will be able to access it
    m_windowHostInfoControl->SetWindowSystem(m_windowSystem);
    try
    {
      const DemoHostAppSetup& hostAppSetup = demoHostConfig.GetDemoHostAppSetup();
      // Retrieve the custom config
      m_appHostConfig = hostAppSetup.GetDemoAppHostConfig<DemoAppHostConfigEGL>();

      m_configControl = m_appHostConfig->GetConfigControl();

      m_extensionRequests = m_appHostConfig->GetExtensionRequests();

      // Check that its a OpenGLES demo app
      m_featureConfig = ExamineFeatureRequest(hostAppSetup.DemoHostFeatures);
      if (!m_featureConfig.EnableGLES && !m_featureConfig.EnableVG)
      {
        throw UsageErrorException("The EGLDemo host currently expects that OpenGLES or OpenVG has to be enabled");
      }

      m_activeApi = m_featureConfig.Feature;
      m_enableGLES = m_featureConfig.EnableGLES;
      m_enableVG = m_featureConfig.EnableVG;

      m_eglContextClientVersionMajor = m_featureConfig.ESVersionMajor;
      m_eglContextClientVersionMinor = m_featureConfig.ESVersionMinor;
      if (!m_appHostConfig->IsMinimumMiniorVersionSet())
      {
        m_eglContextClientVersionMinimumMinor = m_featureConfig.ESVersionMinor;
      }
      else
      {
        m_eglContextClientVersionMinimumMinor = std::min(m_featureConfig.ESVersionMinor, m_appHostConfig->GetMinimumMiniorVersion());
      }

      if (m_enableGLES)
      {
        std::shared_ptr<IImageServiceControl> imageControl = demoHostConfig.GetServiceProvider().Get<IImageServiceControl>();
        imageControl->SetPreferredBitmapOrigin(BitmapOrigin::LowerLeft);
      }

      BuildAttribConfig(m_appEglConfigAttribs, m_appHostConfig->GetEglConfigAttribs());
      BuildEGLConfig(m_finalConfigAttribs, m_appEglConfigAttribs, m_configControl, m_featureConfig, m_options, DEFAULT_RGB_CONFIG,
                     DEFAULT_DEPTH_BUFFER_SIZE);
      assert(m_finalConfigAttribs.size() > 0);

      if (m_options->IsLogConfigEnabled())
      {
        DoLogConfig(m_finalConfigAttribs);
        m_logSelectedConfig = true;
      }
      m_logExtensions = m_options->IsLogExtensionsEnabled();

      // Prepare the native window setup
      m_nativeWindowSetup =
        std::make_unique<NativeWindowSetup>(demoHostConfig.GetDemoHostAppSetup().AppSetup.ApplicationName, demoHostConfig.GetEventQueue(),
                                            m_options->GetNativeWindowConfig(), m_demoHostConfig.GetVerbosityLevel());
    }
    catch (const std::exception&)
    {
      m_windowHostInfoControl->ClearWindowSystem();
      throw;
    }
  }


  EGLDemoHost::~EGLDemoHost()
  {
    try
    {
      m_windowHostInfoControl->ClearWindowSystem();
      LOCAL_LOG("Destroying");
      Shutdown();
      LOCAL_LOG("Destroyed");
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("EGLDemoHost destructor can not throw so aborting. {}", ex.what())
      std::abort();
    }
  }


  void EGLDemoHost::OnConstructed()
  {
    // Prepare everything
    Init();
  }


  void EGLDemoHost::OnActivate()
  {
    m_isActivated = true;
  }


  void EGLDemoHost::OnDeactivate()
  {
    m_isActivated = false;
  }


  void EGLDemoHost::OnSuspend()
  {
    Shutdown();
  }


  void EGLDemoHost::OnResume()
  {
    Init();
  }


  DemoHostFeature EGLDemoHost::GetActiveAPI() const
  {
    return m_activeApi;
  }


  DemoWindowMetrics EGLDemoHost::GetWindowMetrics() const
  {
    assert(!m_enableGLES || m_eglContextClientVersionMajor > 0);
    // The EGL state should be 100% ready
    assert(m_hDisplay != EGL_NO_DISPLAY);
    assert(m_hContext != EGL_NO_CONTEXT);
    assert(m_hSurface != EGL_NO_SURFACE);
    assert(m_hConfig != LocalConfig::EmptyValueEGLConfig);
    assert(m_window);

    const auto size = GetEGLSurfaceResolution(m_hDisplay, m_hSurface);

    auto nativeWindowMetrics = (m_window ? m_window->GetWindowMetrics() : NativeWindowMetrics());

    PxExtent2D extent(std::max(size.X, 0), std::max(size.Y, 0));
    return {extent, nativeWindowMetrics.ExactDpi, nativeWindowMetrics.DensityDpi};
  }


  SwapBuffersResult EGLDemoHost::TrySwapBuffers()
  {
    if (!m_isActivated)
    {
      return SwapBuffersResult::Completed;
    }

    // Validate that we are not suspended
    assert(m_window);
    assert(m_hDisplay != EGL_NO_DISPLAY);
    assert(m_hContext != EGL_NO_CONTEXT);
    assert(m_hSurface != EGL_NO_SURFACE);
    assert(m_hConfig != LocalConfig::EmptyValueEGLConfig);
    return (eglSwapBuffers(m_hDisplay, m_hSurface) == EGL_TRUE ? SwapBuffersResult::Completed : SwapBuffersResult::Failed);
  }


  bool EGLDemoHost::ProcessNativeMessages(const bool allowBlock)
  {
    // This will be called while suspended
    // Allow the native window to process messages
    NativeWindowProcessMessagesArgs args(allowBlock);
    return m_windowSystem->ProcessMessages(args);
  }


  void EGLDemoHost::Init()
  {
    LOCAL_LOG("Init");
    try
    {
      // Get the basic GL configuration ready
      InitEGL();

      // Create the main surface and context
      InitSurfaceAndContext();

      //! Give extending classes a chance to react
      m_apiInit = true;
      OnAPIInitialized();

      m_graphicsService->SetActiveApi(m_activeApi);

      //! Let the graphics service know that the device is ready
      GraphicsDeviceCreateInfo createInfo(m_maxFramesInFlight, m_demoHostConfig.GetPreallocateBasic2D(), nullptr);
      m_graphicsService->CreateDevice(createInfo);
    }
    catch (const std::exception&)
    {
      // LOCAL_LOG("Init failed with exception: " << ex.what());
      Shutdown();
      throw;
    }
  }


  void EGLDemoHost::Shutdown()
  {
    LOCAL_LOG("Shutdown");

    // Give extending classes a chance to react
    if (m_apiInit)
    {
      //! Let the graphics service know that the device is ready
      m_graphicsService->DestroyDevice();

      m_graphicsService->ClearActiveApi();

      m_apiInit = false;
      OnAPIShutdown();
    }

    ShutdownSurfaceAndContext();
    ShutdownEGL();
  }


  void EGLDemoHost::InitEGL()
  {
    assert(m_nativeWindowSetup);
    assert(!m_window);
    assert(m_hDisplay == EGL_NO_DISPLAY);
    assert(m_hContext == EGL_NO_CONTEXT);
    assert(m_hSurface == EGL_NO_SURFACE);
    assert(m_hConfig == LocalConfig::EmptyValueEGLConfig);

    FSLLOG3_VERBOSE("EGLDemoHost::InitEGL");

    // The OpenVG reference implementation EGL layer fails if this is executed before init.
    // So for now if we are using OpenVG we do not check for Angle support.
    const bool hasAngleSupport = !m_enableVG ? HasAngleSupport(m_logExtensions) : false;

    // Acquire the various native display handles
    EGLNativeDisplayType hDisplay = m_windowSystem->GetDisplayType();
    try
    {
      bool useDefault = true;
      if (hasAngleSupport)
      {
        useDefault = !TryGetAngleDisplay(m_hDisplay, hDisplay);
        FSLLOG3_VERBOSE2("Customized angle display enabled: {}", !useDefault);
      }
      if (useDefault)
      {
        m_hDisplay = EGL_CHECK(eglGetDisplay(hDisplay));
      }

      LOCAL_LOG("Initialize");
      // Configure the display
      EGLint majorVersion = 0;
      EGLint minorVersion = 0;
      EGL_CHECK(eglInitialize(m_hDisplay, &majorVersion, &minorVersion));
      FSLLOG3_VERBOSE("EGL V{}.{}", majorVersion, minorVersion);


      // Update the host service
      m_hostService->SetDisplay(m_hDisplay);

      if (m_enableVG)
      {
        LOCAL_LOG("Binding VG");
        EGL_CHECK(eglBindAPI(EGL_OPENVG_API));
      }
      if (m_enableGLES)
      {
        LOCAL_LOG("Binding GLES");
        EGL_CHECK(eglBindAPI(EGL_OPENGL_ES_API));
      }

      LOCAL_LOG("Choosing config");

      // Logs configs if enabled
      DoLogConfigs(m_options->GetConfigLogMode(), m_hDisplay);

      // Take a copy of the final user desired config so we can use it for logging purposes later.
      std::vector<EGLint> finalConfigAttribsCopy = m_finalConfigAttribs;

      EGLint numConfigs = 0;
      EGL_CHECK(eglChooseConfig(m_hDisplay, m_finalConfigAttribs.data(), &m_hConfig, 1, &numConfigs));

      if (numConfigs != 1)
      {
        // eglChooseConfig might fail to find HDR requests, so we fallback to our own search
        bool configSelected = false;
        bool isHDRRequest = IsHDRRequest(m_appEglConfigAttribs);
        if (isHDRRequest)
        {
          configSelected = TryInitEGLHDRConfig(m_appEglConfigAttribs);
        }

        if (!configSelected)
        {
          if (!TryInitEGLTryConfigFallback(m_configControl, m_appEglConfigAttribs))
          {
            throw EGLGraphicsException("Failed to find a compatible EGL config");
          }
        }
      }

      if (m_logSelectedConfig)
      {
        DoLogConfigComparison(m_hDisplay, m_hConfig, finalConfigAttribsCopy);
      }
      if (m_logExtensions)
      {
        DoLogExtensions(m_hDisplay);
      }

      // Do EGL extension check
      DoCheckExtensions(m_hDisplay, m_extensionRequests);

      FSLLOG3_VERBOSE("EGLDemoHost::InitEGL: Creating native window");

      // Prepare the native window
      const NativeEGLSetup nativeEglSetup(hDisplay, m_hDisplay, m_hConfig);
      m_window = m_windowSystem->CreateNativeWindow(*m_nativeWindowSetup, nativeEglSetup);
      m_windowHostInfoControl->AddWindow(m_window);
    }
    catch (const std::exception&)
    {
      ShutdownEGL();
      throw;
    }
  }

  bool EGLDemoHost::TryInitEGLHDRConfig(const std::deque<EGLint>& appAglConfigAttribs)
  {
    return EGLConfigUtil::TryChooseConfig(m_hDisplay, appAglConfigAttribs, true, m_hConfig);
  }


  bool EGLDemoHost::TryInitEGLTryConfigFallback(const ConfigControl configControl, const std::deque<EGLint>& appEglConfigAttribs,
                                                const bool isLastResort)
  {
    if (!m_enableGLES)
    {
      return false;
    }

    FSLLOG3_WARNING("Preferred configuration not available, trying to locate a alternative.");
    if (configControl != ConfigControl::Exact)
    {
      if (TryConfigFallbackDepth(m_hDisplay, m_finalConfigAttribs, appEglConfigAttribs, m_configControl, m_featureConfig, m_options,
                                 DEFAULT_RGB_CONFIG, DEFAULT_DEPTH_BUFFER_SIZE, m_hConfig))
      {
        return true;
      }

      const RGBConfig configRGB888(8, 8, 8);
      if (TryConfigFallbackColorsAndDepth(m_hDisplay, m_finalConfigAttribs, appEglConfigAttribs, m_configControl, m_featureConfig, m_options,
                                          configRGB888, DEFAULT_DEPTH_BUFFER_SIZE, m_hConfig))
      {
        return true;
      }

      const RGBConfig configRGB565(5, 6, 5);
      if (TryConfigFallbackColorsAndDepth(m_hDisplay, m_finalConfigAttribs, appEglConfigAttribs, m_configControl, m_featureConfig, m_options,
                                          configRGB565, DEFAULT_DEPTH_BUFFER_SIZE, m_hConfig))
      {
        return true;
      }
    }

    // If the app requested a exact config, lets try to do a merge instead
    if (configControl == ConfigControl::Exact)
    {
      FSLLOG3_INFO("- Switching to merge app and default config.");
      if (TryInitEGLTryConfigFallback(ConfigControl::Overwrite, appEglConfigAttribs))
      {
        return true;
      }
    }

    // As a last resort lets try to ignore the app
    if (!isLastResort && !appEglConfigAttribs.empty())
    {
      FSLLOG3_INFO("- Ignoring the app requested config.");
      std::deque<EGLint> emptyAppAglConfigAttribs = {EGL_NONE};
      if (TryInitEGLTryConfigFallback(configControl, emptyAppAglConfigAttribs, true))
      {
        return true;
      }
    }
    return false;
  }


  void EGLDemoHost::InitSurfaceAndContext()
  {
    //! The app manager should not exist when this is called
    FSLLOG3_VERBOSE("EGLDemoHost::InitSurfaceAndContext");

    assert(!m_enableGLES || m_eglContextClientVersionMajor > 0);
    assert(m_hDisplay != EGL_NO_DISPLAY);
    assert(m_hConfig != LocalConfig::EmptyValueEGLConfig);
    assert(m_window);
    if (m_hDisplay == EGL_NO_DISPLAY || m_hConfig == LocalConfig::EmptyValueEGLConfig || !m_window)
    {
      FSLLOG3_VERBOSE("EGLDemoHost::InitSurfaceAndContext, no display or invalid config");
      return;
    }

    try
    {
      const EGLNativeWindowType hWindow = m_window->GetWindowType();

      // Setup the rest
      LOCAL_LOG("Creating window surface");

      std::deque<EGLint> tempWindowAttribsDeque;

      // Better than nothing for now
      // FIX: query the m_window's current display id instead of the one we started it on (especially since the backend might change that value)
      const auto displayId = m_nativeWindowSetup->GetConfig().GetDisplayId();
      const bool isConfigAttribsHDR = IsHDRRequest(m_finalConfigAttribs);
      // We only check for display HDR compatibility when the configs request a HDR buffer
      const bool isDisplayHDRCompatible = isConfigAttribsHDR ? m_windowSystem->IsDisplayHDRCompatible(displayId) : false;

      DemoAppHostCreateWindowSurfaceInfoEGL createInfo(m_demoHostConfig.GetServiceProvider(), m_window, isConfigAttribsHDR, isDisplayHDRCompatible);
      BuildAttribConfig(tempWindowAttribsDeque, m_appHostConfig->GetEglCreateWindowAttribs(m_hDisplay, createInfo));

      std::vector<EGLint> finalCreateWindowAttribs(tempWindowAttribsDeque.begin(), tempWindowAttribsDeque.end());

      const EGLint* pCreateWindowAttribs = finalCreateWindowAttribs.size() <= 1 ? nullptr : finalCreateWindowAttribs.data();
      m_hSurface = EGL_CHECK(eglCreateWindowSurface(m_hDisplay, m_hConfig, hWindow, pCreateWindowAttribs));

      m_hostService->SetSurface(m_hSurface);

      LOCAL_LOG("Creating context");
      m_hContext = DoEGLCreateContext(m_hDisplay, m_hConfig, m_enableGLES, m_eglContextClientVersionMajor, m_eglContextClientVersionMinor,
                                      m_eglContextClientVersionMinimumMinor);

      m_hostService->SetContext(m_hContext);

      LOCAL_LOG("Making current");
      EGL_CHECK(eglMakeCurrent(m_hDisplay, m_hSurface, m_hSurface, m_hContext));
    }
    catch (const std::exception&)
    {
      ShutdownSurfaceAndContext();
      throw;
    }
  }


  void EGLDemoHost::ShutdownSurfaceAndContext()
  {
    if (m_hDisplay == EGL_NO_DISPLAY)
    {
      return;
    }
    FSLLOG3_VERBOSE("EGLDemoHost::ShutdownSurfaceAndContext");

    assert(m_hConfig != LocalConfig::EmptyValueEGLConfig);
    assert(m_window);

    EGL_LOG_ERROR(eglMakeCurrent(m_hDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

    assert(eglGetError() == EGL_SUCCESS);
    if (m_hContext != EGL_NO_CONTEXT)
    {
      LOCAL_LOG("Destroying context");
      m_hostService->SetContext(EGL_NO_CONTEXT);
      // NOTE: ARM-EMU - For some reason this gives a error on shutdown
      EGL_LOG_ERROR(eglDestroyContext(m_hDisplay, m_hContext));
      m_hContext = EGL_NO_CONTEXT;
    }

    if (m_hSurface != EGL_NO_SURFACE)
    {
      LOCAL_LOG("Destroying surface");
      m_hostService->SetSurface(EGL_NO_SURFACE);
      EGL_LOG_ERROR(eglDestroySurface(m_hDisplay, m_hSurface));
      m_hSurface = EGL_NO_SURFACE;
    }
  }


  void EGLDemoHost::ShutdownEGL()
  {
    //! The app manager should have been released before calling this
    assert(m_hContext == EGL_NO_CONTEXT);
    assert(m_hSurface == EGL_NO_SURFACE);

    FSLLOG3_VERBOSE("EGLDemoHost::ShutdownEGL");

    if (m_hDisplay != EGL_NO_DISPLAY)
    {
      LOCAL_LOG("Destroying window");
      m_hostService->SetDisplay(EGL_NO_DISPLAY);
      // Release the native window
      m_windowHostInfoControl->RemoveWindow(m_window);
      m_window.reset();

      m_hConfig = LocalConfig::EmptyValueEGLConfig;
      LOCAL_LOG("Terminating egl");
      eglTerminate(m_hDisplay);
      m_hDisplay = EGL_NO_DISPLAY;
      eglReleaseThread();
    }
  }
}
