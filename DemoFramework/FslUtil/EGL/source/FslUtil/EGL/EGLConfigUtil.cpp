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

#include <FslUtil/EGL/EGLConfigUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslUtil/EGL/EGLUtil.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace EGLConfigUtil
  {
    namespace
    {
      struct RatedConfigRecord
      {
        EGLConfig Config;
        uint64_t Rating{0};

        RatedConfigRecord()
          : Config{}
        {
        }

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


      uint64_t ApplyScoreAdjustments(const uint64_t score, const EGLDisplay hDisplay, const EGLConfig& config)
      {
        uint64_t adjustedScore = std::max(score, static_cast<uint64_t>(1)) << 32;

        EGLint actualValue;

        if (eglGetConfigAttrib(hDisplay, config, EGL_CONFIG_CAVEAT, &actualValue) != 0u)
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

          EGLint configValue;
          if (eglGetConfigAttrib(hDisplay, config, key, &configValue) == 0u)
          {
            FSLLOG_DEBUG_WARNING("Failed to retrieve attribute value for key: " << key);
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
    }


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
        const auto activeConfig = allConfigs[i];
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
}
