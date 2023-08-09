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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslUtil/EGL/EGLCheck.hpp>
#include <FslUtil/EGL/EGLConfigUtil.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl::EGLConfigUtil
{
  namespace
  {
    struct RatedConfigRecord
    {
      EGLConfig Config{};
      uint64_t Rating{0};

      RatedConfigRecord() = default;

      RatedConfigRecord(const EGLConfig& config, const uint64_t& rating)
        : Config(config)
        , Rating(rating)
      {
      }
    };


    uint64_t RateKeyValue(const EGLint key, const EGLint desiredValue, const EGLint actualValue, const EGLint maxColorBits)
    {
      switch (key)
      {
      case EGL_CONFIG_ID:
        return 0;    // We dont support config id
      case EGL_COLOR_BUFFER_TYPE:
        return desiredValue == actualValue ? 1 : 0;
      case EGL_BLUE_SIZE:
      case EGL_GREEN_SIZE:
      case EGL_RED_SIZE:
        if (desiredValue < 0 || actualValue > maxColorBits)
        {
          return 0;
        }
        else if (desiredValue > 0)
        {
          return (actualValue >= desiredValue ? 1 : 0);
        }
        return 1;
      case EGL_DEPTH_SIZE:
        if (desiredValue < 0)
        {
          return 0;
        }
        else if (desiredValue == 0)
        {
          return actualValue == 0 ? 10 : 1;
        }
        else if (actualValue >= 128)
        {
          return 1;
        }
        return 128 - actualValue;
      case EGL_LEVEL:
        return desiredValue == actualValue ? 1 : 0;
      case EGL_CONFORMANT:
      case EGL_RENDERABLE_TYPE:
      case EGL_SURFACE_TYPE:
        return (actualValue & desiredValue) == desiredValue ? 1 : 0;
      default:
        // We don't know to rate this
        return 0;
      }
    }


    // NOLINTNEXTLINE(misc-misplaced-const)
    uint64_t ApplyScoreAdjustments(const uint64_t score, const EGLDisplay hDisplay, const EGLConfig& config)
    {
      uint64_t adjustedScore = std::max(score, static_cast<uint64_t>(1)) << 32;

      EGLint actualValue = 0;

      if (eglGetConfigAttrib(hDisplay, config, EGL_CONFIG_CAVEAT, &actualValue) == EGL_TRUE)
      {
        if (actualValue != EGL_NONE)
        {
          adjustedScore += 10;
        }
        else
        {
          adjustedScore -= 10;
        }
      }
      return adjustedScore;
    }


    // NOLINTNEXTLINE(misc-misplaced-const)
    uint64_t RateConfig(const EGLDisplay hDisplay, const EGLConfig& config, const std::deque<EGLint>& configAttribs, const EGLint maxColorBits)
    {
      assert((configAttribs.size() % 2) == 0);

      uint64_t score = 0;
      auto itr = configAttribs.begin();
      const auto itrEnd = configAttribs.end();
      while (itr != itrEnd)
      {
        const auto key = *itr;
        ++itr;
        const auto desiredValue = *itr;
        ++itr;

        EGLint configValue = 0;
        if (eglGetConfigAttrib(hDisplay, config, key, &configValue) == EGL_FALSE)
        {
          FSLLOG3_DEBUG_WARNING("Failed to retrieve attribute value for key: {}", key);
          return 0;
        }

        const uint64_t keyScore = RateKeyValue(key, desiredValue, configValue, maxColorBits);
        if (keyScore <= 0)
        {
          return 0;
        }

        score += keyScore;
      }

      return ApplyScoreAdjustments(score, hDisplay, config);
    }


    void AddIfMissing(std::deque<EGLint>& rConfigAttribs, const EGLint key, const EGLint defaultValue)
    {
      auto itrFind = std::find(rConfigAttribs.begin(), rConfigAttribs.end(), key);
      if (itrFind != rConfigAttribs.end())
      {
        return;
      }

      rConfigAttribs.push_back(key);
      rConfigAttribs.push_back(defaultValue);
    }


    std::deque<EGLint> BuildSearchConfigAttribs(const std::deque<EGLint>& configAttribs)
    {
      std::deque<EGLint> result(configAttribs);
      if ((result.size() % 2) == 1)
      {
        if (result.back() != EGL_NONE)
        {
          throw UsageErrorException("The configAttribs format was not valid");
        }
        result.pop_back();
      }

      // AddIfMissing(result, EGL_CONFIG_ID, EGL_DONT_CARE);
      // AddIfMissing(result, EGL_CONFORMANT, 0);

      AddIfMissing(result, EGL_RED_SIZE, 0);
      AddIfMissing(result, EGL_BLUE_SIZE, 0);
      AddIfMissing(result, EGL_GREEN_SIZE, 0);

      // AddIfMissing(result, EGL_CONFIG_CAVEAT, EGL_DONT_CARE);
      return result;
    }

    std::optional<EGLConfig> TryDoChooseFullHDRConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes,
                                                      const ReadOnlySpan<EGLConfig> prioritizedConfigList, const bool allowAlphaChannel)
    {
      FSLLOG3_VERBOSE4("EGLDemoHost: - TryDoChooseFullHDRConfig(allowAlphaChannel:{}) from {} provided entries", allowAlphaChannel,
                       prioritizedConfigList.size());
      assert((attributes.HasAlphaChannelRequest() && allowAlphaChannel) || !attributes.HasAlphaChannelRequest());

      for (const auto& config : prioritizedConfigList)
      {
        {    // Skip formats that has less than 8bit per channel
          EGLint configRedValue = 0;
          EGLint configGreenValue = 0;
          EGLint configBlueValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_RED_SIZE, &configRedValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_GREEN_SIZE, &configBlueValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_BLUE_SIZE, &configGreenValue) != EGL_TRUE ||
              (configRedValue <= 8 || configGreenValue <= 8 || configBlueValue <= 8))
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config as it does not have full HDR RGB channels: R{}G{}B{}", configRedValue, configBlueValue,
                             configGreenValue);
            continue;
          }
        }
        if (!allowAlphaChannel)
        {
          // Skip if the config has a Alpha Channel
          EGLint configValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_ALPHA_SIZE, &configValue) != EGL_TRUE || configValue != 0)
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config contains a unrequested Alpha channel: A{}", configValue);
            continue;
          }
        }
        FSLLOG3_VERBOSE4("EGLDemoHost: - Found HDR configuration");
        return config;
      }
      return {};
    }

    std::optional<EGLConfig> TryDoChoosePartialHDRConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes,
                                                         const ReadOnlySpan<EGLConfig> prioritizedConfigList, const bool allowAlphaChannel)
    {
      FSLLOG3_VERBOSE4("EGLDemoHost: - TryDoChoosePartialHDRConfig(allowAlphaChannel:{}) from {} provided entries", allowAlphaChannel,
                       prioritizedConfigList.size());
      assert((attributes.HasAlphaChannelRequest() && allowAlphaChannel) || !attributes.HasAlphaChannelRequest());
      for (const auto& config : prioritizedConfigList)
      {
        {    // Skip formats that has less than 8bit per channel
          EGLint configRedValue = 0;
          EGLint configGreenValue = 0;
          EGLint configBlueValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_RED_SIZE, &configRedValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_GREEN_SIZE, &configBlueValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_BLUE_SIZE, &configGreenValue) != EGL_TRUE ||
              (configRedValue <= 8 && configGreenValue <= 8 && configBlueValue <= 8))
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config as it does not have at least partial HDR RGB channels: R{}G{}B{}", configRedValue,
                             configBlueValue, configGreenValue);
            continue;
          }
        }
        if (!allowAlphaChannel)
        {
          // Skip if the config has a Alpha Channel
          EGLint configValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_ALPHA_SIZE, &configValue) != EGL_TRUE || configValue != 0)
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config contains a unrequested Alpha channel: A{}", configValue);
            continue;
          }
        }
        FSLLOG3_VERBOSE4("EGLDemoHost: - Found partial HDR configuration");
        return config;
      }
      return {};
    }

    std::optional<EGLConfig> TryDoChooseSDRConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes,
                                                  const ReadOnlySpan<EGLConfig> prioritizedConfigList, const bool allowAlphaChannel)
    {
      FSLLOG3_VERBOSE4("EGLDemoHost: - TryDoChooseSDRConfig(allowAlphaChannel:{}) from {} provided entries", allowAlphaChannel,
                       prioritizedConfigList.size());

      assert((attributes.HasAlphaChannelRequest() && allowAlphaChannel) || !attributes.HasAlphaChannelRequest());

      for (const auto& config : prioritizedConfigList)
      {
        {    // Skip formats that has more than 8bit per channel
          EGLint configRedValue = 0;
          EGLint configGreenValue = 0;
          EGLint configBlueValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_RED_SIZE, &configRedValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_GREEN_SIZE, &configBlueValue) != EGL_TRUE ||
              eglGetConfigAttrib(hDisplay, config, EGL_BLUE_SIZE, &configGreenValue) != EGL_TRUE ||
              (configRedValue > 8 || configGreenValue > 8 || configBlueValue > 8))
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config as it does have at least partial HDR RGB channels: R{}G{}B{}", configRedValue,
                             configBlueValue, configGreenValue);
            continue;
          }
        }
        if (!allowAlphaChannel)
        {
          // Skip if the config has a Alpha Channel
          EGLint configValue = 0;
          if (eglGetConfigAttrib(hDisplay, config, EGL_ALPHA_SIZE, &configValue) != EGL_TRUE || configValue != 0)
          {
            FSLLOG3_VERBOSE4("EGLDemoHost: - Skipped config contains a unrequested Alpha channel: A{}", configValue);
            continue;
          }
        }
        FSLLOG3_VERBOSE4("EGLDemoHost: - Found SDR configuration");
        return config;
      }
      return {};
    }

    std::optional<EGLConfig> TryDoChooseHDRConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes,
                                                  const ReadOnlySpan<EGLConfig> prioritizedConfigList)
    {
      FSLLOG3_VERBOSE4("EGLDemoHost: - TryDoChooseHDRConfig");
      if (!attributes.HasAlphaChannelRequest())
      {
        auto res = TryDoChooseFullHDRConfig(hDisplay, attributes, prioritizedConfigList, false);
        if (res.has_value())
        {
          return res;
        }
      }
      {    // No HDR format without a alpha channel found or alpha channel was requested by caller
        auto res = TryDoChooseFullHDRConfig(hDisplay, attributes, prioritizedConfigList, true);
        if (res.has_value())
        {
          return res;
        }
      }

      if (!attributes.HasAlphaChannelRequest())
      {
        auto res = TryDoChoosePartialHDRConfig(hDisplay, attributes, prioritizedConfigList, false);
        if (res.has_value())
        {
          return res;
        }
      }
      // No HDR format without a alpha channel found or alpha channel was requested by caller
      return TryDoChoosePartialHDRConfig(hDisplay, attributes, prioritizedConfigList, true);
    }

    std::optional<EGLConfig> TryDoChooseSDRConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes,
                                                  const ReadOnlySpan<EGLConfig> prioritizedConfigList)
    {
      if (!attributes.HasAlphaChannelRequest())
      {
        // The user did not request a alpha channel so prefer a format without alpha channel (so we don't waste resources blending)
        auto optionalConfig = TryDoChooseSDRConfig(hDisplay, attributes, prioritizedConfigList, false);
        if (optionalConfig.has_value())
        {
          return optionalConfig;
        }
      }
      {    // No SDR format without a alpha channel found or alpha channel was requested by caller
        auto optionalConfig = TryDoChooseSDRConfig(hDisplay, attributes, prioritizedConfigList, true);
        if (optionalConfig.has_value())
        {
          return optionalConfig;
        }
      }
      return {};
    }

  }

  // NOLINTNEXTLINE(misc-misplaced-const)
  std::optional<EGLConfig> TryEGLGuidedChooseConfig(EGLDisplay hDisplay, const Fsl::EGL::ReadOnlyEGLAttributeSpan attributes, const bool allowHDR)
  {
    FSLLOG3_VERBOSE4("EGLDemoHost: TryEGLGuidedChooseConfig allowHDR: {}", allowHDR);
    {    // Try some guided choices based on the EGL recommendations
      std::vector<EGLConfig> configs = EGLUtil::GetChooseConfigs(hDisplay, attributes);
      if (!configs.empty())
      {
        if (allowHDR && attributes.IsHDRRequest())
        {
          auto optionalConfig = TryDoChooseHDRConfig(hDisplay, attributes, ReadOnlySpanUtil::AsSpan(configs));
          if (optionalConfig.has_value())
          {
            return optionalConfig;
          }
        }
        {    // Not a HDR request, so try to chose a SDR format
          auto optionalConfig = TryDoChooseSDRConfig(hDisplay, attributes, ReadOnlySpanUtil::AsSpan(configs));
          if (optionalConfig.has_value())
          {
            return optionalConfig;
          }
        }
      }
      else
      {
        FSLLOG3_VERBOSE4("EGLDemoHost: - eglChooseConfig did not provide any suggestions");
      }
    }

    // fallback to the old way of letting EGL handle it
    {
      FSLLOG3_VERBOSE4("EGLDemoHost: - Trying legacy solution of asking EGL to chose a config, selecting the first");
      const EGLint* const pAttributes = attributes.data();
      EGLint numConfigs = 0;
      EGLConfig chosenConfig{};
      EGL_CHECK(eglChooseConfig(hDisplay, pAttributes, &chosenConfig, 1, &numConfigs));
      return (numConfigs == 1) ? std::optional<EGLConfig>(chosenConfig) : std::optional<EGLConfig>();
    }
  }

  // NOLINTNEXTLINE(misc-misplaced-const)
  bool TryChooseConfig(const EGLDisplay hDisplay, const std::deque<EGLint>& configAttribs, const bool allowHDR, EGLConfig& rEGLConfig)
  {
    auto searchConfigAttribs = BuildSearchConfigAttribs(configAttribs);

    auto allConfigs = EGLUtil::GetConfigs(hDisplay);
    std::vector<RatedConfigRecord> records(allConfigs.size());

    const EGLint maxColorBits = allowHDR ? 8192 : 8;

    std::size_t bestIndex = 0;
    uint64_t bestScore = 0;
    for (std::size_t i = 0; i < records.size(); ++i)
    {
      EGLConfig activeConfig = allConfigs[i];
      const auto rating = RateConfig(hDisplay, activeConfig, searchConfigAttribs, maxColorBits);
      RatedConfigRecord current(activeConfig, rating);
      records[i] = current;
      if (current.Rating > bestScore)
      {
        bestScore = current.Rating;
        bestIndex = i;
      }
    }

    if (bestScore > 0)
    {
      rEGLConfig = allConfigs[bestIndex];
      return true;
    }
    return false;
  }
}
