/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <Shared/UI/Benchmark/Persistence/AppSettingsPersistence.hpp>
#include <nlohmann/json.hpp>
#include "Json/JsonAppRenderBasicOptions.hpp"
#include "Json/JsonAppRenderMethod.hpp"
#include "Json/JsonAppRenderOptions.hpp"
#include "JsonConfig.hpp"
#include "JsonHelper.hpp"

namespace Fsl::AppSettingsPersistence
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
      constexpr auto EnableClipping = "EnableClipping";
      constexpr auto ShowClipRectangle = "ShowClipRectangle";
      constexpr auto ActiveRenderIndex = "ActiveRenderIndex";
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


    std::optional<std::array<AppRenderOptions, 2>> TryParseAppRenderOptions(const nlohmann::json& jsonValue)
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

    std::optional<AppBenchSettings> TryParseAppBenchSettings(const nlohmann::json& jsonValue)
    {
      if (!jsonValue.contains(LocalBenchSettings::BasicOptions) || !jsonValue.contains(LocalBenchSettings::RenderOptions))
      {
        FSLLOG3_DEBUG_WARNING("AppBenchSettings is missing a section");
        return {};
      }

      try
      {
        std::optional<AppRenderBasicOptions> basicOptions = jsonValue.contains(LocalBenchSettings::BasicOptions)
                                                              ? std::optional<AppRenderBasicOptions>(jsonValue[LocalBenchSettings::BasicOptions])
                                                              : std::optional<AppRenderBasicOptions>();
        std::optional<AppRenderMethod> renderMethod = jsonValue.contains(LocalBenchSettings::RenderMethod)
                                                        ? std::optional<AppRenderMethod>(jsonValue[LocalBenchSettings::RenderMethod])
                                                        : std::optional<AppRenderMethod>();
        std::optional<std::array<AppRenderOptions, 2>> renderOptions = TryParseAppRenderOptions(jsonValue[LocalBenchSettings::RenderOptions]);
        if (!basicOptions.has_value() || !renderMethod.has_value() || !renderOptions.has_value())
        {
          FSLLOG3_DEBUG_WARNING_IF(basicOptions.has_value(), "Failed to parse: '{}'", LocalBenchSettings::BasicOptions);
          FSLLOG3_DEBUG_WARNING_IF(renderOptions.has_value(), "Failed to parse: '{}'", LocalBenchSettings::RenderMethod);
          FSLLOG3_DEBUG_WARNING_IF(renderOptions.has_value(), "Failed to parse: '{}'", LocalBenchSettings::RenderOptions);
          return {};
        }
        return AppBenchSettings(basicOptions.value(), renderMethod.value(), renderOptions.value());
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_WARNING("Failed to parse AppBenchSettings: {}", ex.what());
        return {};
      }
    }


    std::optional<AppTestSettings> TryParseAppTestSettings(const nlohmann::json& jsonValue)
    {
      if (!jsonValue.contains(LocalSystemSettings::ShowStats) || !jsonValue.contains(LocalSystemSettings::ShowIdle) ||
          !jsonValue.contains(LocalSystemSettings::NoOpaqueMaterials))
      {
        FSLLOG3_DEBUG_WARNING("AppTestSettings is missing a section");
        return {};
      }

      std::optional<uint32_t> activeRenderIndex;
      if (jsonValue.contains(LocalSystemSettings::ActiveRenderIndex))
      {
        activeRenderIndex = JsonHelper::TryParseUInt32(jsonValue[LocalSystemSettings::ActiveRenderIndex]);
      }
      else
      {
        // Key not present using default value
        activeRenderIndex = 0;
      }


      std::optional<bool> showStats = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::ShowStats]);
      std::optional<bool> showIdle = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::ShowIdle]);
      std::optional<bool> noOpaqueMaterials = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::NoOpaqueMaterials]);
      std::optional<bool> enableClipping = false;
      std::optional<bool> showClipRectangle = false;

      if (jsonValue.contains(LocalSystemSettings::EnableClipping))
      {
        enableClipping = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::EnableClipping]);
      }
      if (jsonValue.contains(LocalSystemSettings::ShowClipRectangle))
      {
        showClipRectangle = JsonHelper::TryParseBool(jsonValue[LocalSystemSettings::ShowClipRectangle]);
      }

      if (!showStats.has_value() || !showIdle.has_value() || !noOpaqueMaterials.has_value() || !enableClipping.has_value() ||
          !showClipRectangle.has_value() || !activeRenderIndex.has_value())
      {
        FSLLOG3_DEBUG_WARNING_IF(showStats.has_value(), "Failed to parse: '{}'", LocalSystemSettings::ShowStats);
        FSLLOG3_DEBUG_WARNING_IF(showIdle.has_value(), "Failed to parse: '{}'", LocalSystemSettings::ShowIdle);
        FSLLOG3_DEBUG_WARNING_IF(noOpaqueMaterials.has_value(), "Failed to parse: '{}'", LocalSystemSettings::NoOpaqueMaterials);
        FSLLOG3_DEBUG_WARNING_IF(enableClipping.has_value(), "Failed to parse: '{}'", LocalSystemSettings::EnableClipping);
        FSLLOG3_DEBUG_WARNING_IF(showClipRectangle.has_value(), "Failed to parse: '{}'", LocalSystemSettings::ShowClipRectangle);
        FSLLOG3_DEBUG_WARNING_IF(activeRenderIndex.has_value(), "Failed to parse: '{}'", LocalSystemSettings::ActiveRenderIndex);
        return {};
      }
      return AppTestSettings(showStats.value(), showIdle.value(), noOpaqueMaterials.value(), enableClipping.value(), showClipRectangle.value(),
                             activeRenderIndex.value());
    }


    std::optional<AppUISettings> TryParseAppUISettings(const nlohmann::json& jsonValue)
    {
      if (!jsonValue.contains(LocalUISettings::ShowStats) || !jsonValue.contains(LocalUISettings::ShowChart))
      {
        FSLLOG3_DEBUG_WARNING("AppUISettings is missing a section");
        return {};
      }

      std::optional<bool> showStats = JsonHelper::TryParseBool(jsonValue[LocalUISettings::ShowStats]);
      std::optional<bool> showChart = JsonHelper::TryParseBool(jsonValue[LocalUISettings::ShowChart]);


      if (!showStats.has_value() || !showChart.has_value())
      {
        FSLLOG3_DEBUG_WARNING_IF(showStats.has_value(), "Failed to parse: '{}'", LocalUISettings::ShowStats);
        FSLLOG3_DEBUG_WARNING_IF(showChart.has_value(), "Failed to parse: '{}'", LocalUISettings::ShowChart);
        return {};
      }
      return AppUISettings(showStats.value(), showChart.value());
    }


    std::optional<AppSettings> TryParse(const std::string& jsonContent)
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

      std::optional<AppBenchSettings> bench = json.contains(LocalSettings::BenchSettings)
                                                ? TryParseAppBenchSettings(json[LocalSettings::BenchSettings])
                                                : std::optional<AppBenchSettings>();

      if (!bench.has_value())
      {
        // Falling back to the default settings.
        bench = std::optional<AppBenchSettings>(AppBenchSettings());
      }

      std::optional<AppTestSettings> test = TryParseAppTestSettings(json[LocalSettings::TestSettings]);
      if (!test.has_value())
      {
        FSLLOG3_DEBUG_WARNING("Failed to parse '{}'", LocalSettings::TestSettings);
        return {};
      }

      std::optional<AppUISettings> ui = TryParseAppUISettings(json[LocalSettings::UISettings]);
      if (!ui.has_value())
      {
        FSLLOG3_DEBUG_WARNING("Failed to parse '{}'", LocalSettings::UISettings);
        return {};
      }

      return AppSettings(bench.value(), test.value(), ui.value());
    }

    std::string Encode(const AppSettings& settings)
    {
      nlohmann::json json;

      json[LocalSettings::BenchSettings] = {{LocalBenchSettings::BasicOptions, settings.Bench.BasicOptions},
                                            {LocalBenchSettings::RenderMethod, settings.Bench.RenderMethod},
                                            {LocalBenchSettings::RenderOptions, settings.Bench.RenderOptions}};

      json[LocalSettings::TestSettings] = {{LocalSystemSettings::ShowStats, settings.Test.ShowStats},
                                           {LocalSystemSettings::ShowIdle, settings.Test.ShowIdle},
                                           {LocalSystemSettings::NoOpaqueMaterials, settings.Test.NoOpaqueMaterials},
                                           {LocalSystemSettings::EnableClipping, settings.Test.EnableClipping},
                                           {LocalSystemSettings::ShowClipRectangle, settings.Test.ShowClipRectangle},
                                           {LocalSystemSettings::ActiveRenderIndex, settings.Test.ActiveRenderIndex}};

      json[LocalSettings::UISettings] = {{LocalUISettings::ShowStats, settings.UI.ShowStats}, {LocalUISettings::ShowChart, settings.UI.ShowChart}};

      // Pretty print JSON document to string
      return json.dump(JsonConfig::Indent);
    }
  }


  std::optional<AppSettings> TryLoad(const IO::Path& path)
  {
    try
    {
      std::string content;
      return IO::File::TryReadAllText(content, path) ? TryParse(content) : std::optional<AppSettings>();
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
