/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <Shared/UI/Benchmark/Persistence/AppSettingsPersistence.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <nlohmann/json.hpp>
#include "JsonConfig.hpp"
#include "JsonHelper.hpp"
#include "Json/JsonAppRenderBasicOptions.hpp"
#include "Json/JsonAppRenderMethod.hpp"
#include "Json/JsonAppRenderOptions.hpp"

namespace Fsl
{
  namespace AppSettingsPersistence
  {
    namespace
    {
      namespace LocalBenchSettings
      {
        constexpr auto BasicOptions = "BasicOptions";
        constexpr auto RenderMethod = "RenderMethod";
        constexpr auto RenderOptions = "RenderOptions";
      }

      namespace LocalSystemSettings
      {
        constexpr auto ShowStats = "ShowStats";
        constexpr auto ShowIdle = "ShowIdle";
        constexpr auto NoOpaqueMaterials = "NoOpaqueMaterials";
      }

      namespace LocalUISettings
      {
        constexpr auto ShowStats = "ShowStats";
        constexpr auto ShowChart = "ShowChart";
      }

      namespace LocalSettings
      {
        constexpr auto BenchSettings = "BenchSettings";
        constexpr auto TestSettings = "TestSettings";
        constexpr auto UISettings = "UISettings";
      }


      Optional<std::array<AppRenderOptions, 2>> TryParseAppRenderOptions(const nlohmann::json& jsonValue)
      {
        try
        {
          return jsonValue.get<std::array<AppRenderOptions, 2>>();
        }
        catch (const std::exception& ex)
        {
          FSLLOG3_WARNING("Parse failed with: {}", ex.what());
          return {};
        }
      }

      Optional<AppBenchSettings> TryParseAppBenchSettings(const nlohmann::json& jsonValue)
      {
        if (!jsonValue.contains(LocalBenchSettings::BasicOptions) || !jsonValue.contains(LocalBenchSettings::RenderOptions))
        {
          FSLLOG3_DEBUG_WARNING("AppBenchSettings is missing a section");
          return {};
        }

        try
        {
          Optional<AppRenderBasicOptions> basicOptions = jsonValue.contains(LocalBenchSettings::BasicOptions)
                                                           ? Optional<AppRenderBasicOptions>(jsonValue[LocalBenchSettings::BasicOptions])
                                                           : Optional<AppRenderBasicOptions>();
          Optional<AppRenderMethod> renderMethod = jsonValue.contains(LocalBenchSettings::RenderMethod)
                                                     ? Optional<AppRenderMethod>(jsonValue[LocalBenchSettings::RenderMethod])
                                                     : Optional<AppRenderMethod>();
          Optional<std::array<AppRenderOptions, 2>> renderOptions = TryParseAppRenderOptions(jsonValue[LocalBenchSettings::RenderOptions]);
          if (!basicOptions.HasValue() || !renderMethod.HasValue() || !renderOptions.HasValue())
          {
            FSLLOG3_DEBUG_WARNING_IF(basicOptions.HasValue(), "Failed to parse: '{}'", LocalBenchSettings::BasicOptions);
            FSLLOG3_DEBUG_WARNING_IF(renderOptions.HasValue(), "Failed to parse: '{}'", LocalBenchSettings::RenderMethod);
            FSLLOG3_DEBUG_WARNING_IF(renderOptions.HasValue(), "Failed to parse: '{}'", LocalBenchSettings::RenderOptions);
            return {};
          }
          return AppBenchSettings(basicOptions.Value(), renderMethod.Value(), renderOptions.Value());
        }
        catch (const std::exception& ex)
        {
          FSLLOG3_WARNING("Failed to parse AppBenchSettings: {}", ex.what());
          return {};
        }
      }


      Optional<AppTestSettings> TryParseAppTestSettings(const nlohmann::json& jsonValue)
      {
        if (!jsonValue.contains(LocalSystemSettings::ShowStats) || !jsonValue.contains(LocalSystemSettings::ShowIdle) ||
            !jsonValue.contains(LocalSystemSettings::NoOpaqueMaterials))
        {
          FSLLOG3_DEBUG_WARNING("AppTestSettings is missing a section");
          return {};
        }

        Optional<bool> showStats = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::ShowStats]);
        Optional<bool> showIdle = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::ShowIdle]);
        Optional<bool> noOpaqueMaterials = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::NoOpaqueMaterials]);
        if (!showStats.HasValue() || !showIdle.HasValue() || !noOpaqueMaterials.HasValue())
        {
          FSLLOG3_DEBUG_WARNING_IF(showStats.HasValue(), "Failed to parse: '{}'", LocalSystemSettings::ShowStats);
          FSLLOG3_DEBUG_WARNING_IF(showIdle.HasValue(), "Failed to parse: '{}'", LocalSystemSettings::ShowIdle);
          FSLLOG3_DEBUG_WARNING_IF(noOpaqueMaterials.HasValue(), "Failed to parse: '{}'", LocalSystemSettings::NoOpaqueMaterials);
          return {};
        }
        return AppTestSettings(showStats.Value(), showIdle.Value(), noOpaqueMaterials.Value());
      }


      Optional<AppUISettings> TryParseAppUISettings(const nlohmann::json& jsonValue)
      {
        if (!jsonValue.contains(LocalUISettings::ShowStats) || !jsonValue.contains(LocalUISettings::ShowChart))
        {
          FSLLOG3_DEBUG_WARNING("AppUISettings is missing a section");
          return {};
        }

        Optional<bool> showStats = JsonHelper::TryParseBool(jsonValue[LocalUISettings::ShowStats]);
        Optional<bool> showChart = JsonHelper::TryParseBool(jsonValue[LocalUISettings::ShowChart]);
        if (!showStats.HasValue() || !showChart.HasValue())
        {
          FSLLOG3_DEBUG_WARNING_IF(showStats.HasValue(), "Failed to parse: '{}'", LocalUISettings::ShowStats);
          FSLLOG3_DEBUG_WARNING_IF(showChart.HasValue(), "Failed to parse: '{}'", LocalUISettings::ShowChart);
          return {};
        }
        return AppUISettings(showStats.Value(), showChart.Value());
      }


      Optional<AppSettings> TryParse(const std::string& jsonContent)
      {
        auto json = nlohmann::json::parse(jsonContent);

        if (!json.is_object())
        {
          FSLLOG3_DEBUG_WARNING("AppSettings file is not a json document");
          return {};
        }

        if (!json.contains(LocalSettings::TestSettings) || !json.contains(LocalSettings::UISettings))
        {
          FSLLOG3_DEBUG_WARNING("AppSettings file is missing a section");
          return {};
        }

        Optional<AppBenchSettings> bench =
          json.contains(LocalSettings::BenchSettings) ? TryParseAppBenchSettings(json[LocalSettings::BenchSettings]) : Optional<AppBenchSettings>();

        if (!bench.HasValue())
        {
          // Falling back to the default settings.
          bench = Optional<AppBenchSettings>(AppBenchSettings());
        }

        Optional<AppTestSettings> test = TryParseAppTestSettings(json[LocalSettings::TestSettings]);
        if (!test.HasValue())
        {
          FSLLOG3_DEBUG_WARNING("Failed to parse '{}'", LocalSettings::TestSettings);
          return {};
        }

        Optional<AppUISettings> ui = TryParseAppUISettings(json[LocalSettings::UISettings]);
        if (!ui.HasValue())
        {
          FSLLOG3_DEBUG_WARNING("Failed to parse '{}'", LocalSettings::UISettings);
          return {};
        }

        return AppSettings(bench.Value(), test.Value(), ui.Value());
      }

      std::string Encode(const AppSettings& settings)
      {
        nlohmann::json json;

        json[LocalSettings::BenchSettings] = {{LocalBenchSettings::BasicOptions, settings.Bench.BasicOptions},
                                              {LocalBenchSettings::RenderMethod, settings.Bench.RenderMethod},
                                              {LocalBenchSettings::RenderOptions, settings.Bench.RenderOptions}};

        json[LocalSettings::TestSettings] = {{LocalSystemSettings::ShowStats, settings.Test.ShowStats},
                                             {LocalSystemSettings::ShowIdle, settings.Test.ShowIdle},
                                             {LocalSystemSettings::NoOpaqueMaterials, settings.Test.NoOpaqueMaterials}};

        json[LocalSettings::UISettings] = {{LocalUISettings::ShowStats, settings.UI.ShowStats}, {LocalUISettings::ShowChart, settings.UI.ShowChart}};

        // Pretty print JSON document to string
        return json.dump(JsonConfig::Indent);
      }
    }


    Optional<AppSettings> TryLoad(const IO::Path& path)
    {
      try
      {
        std::string content;
        return IO::File::TryReadAllText(content, path) ? TryParse(content) : Optional<AppSettings>();
      }
      catch (std::exception& ex)
      {
        FSLLOG3_DEBUG_WARNING("Exception: {}", ex.what());
        FSL_PARAM_NOT_USED(ex);
        return {};
      }
    }

    void Save(const IO::Path& path, const AppSettings& settings)
    {
      std::string strNewJson = Encode(settings);
      // Only overwrite the file if it was modified
      std::string existingJsonFile;
      if (!IO::File::TryReadAllText(existingJsonFile, path) || existingJsonFile != strNewJson)
      {
        IO::File::WriteAllText(path, strNewJson);
      }
    }
  }
}
