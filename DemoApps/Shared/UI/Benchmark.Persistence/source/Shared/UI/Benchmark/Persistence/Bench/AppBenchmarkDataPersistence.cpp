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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringDateUtil.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkData.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkDataPersistence.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include "../JsonConfig.hpp"
#include "Json/JsonAppBenchmarkCpuData.hpp"
#include "Json/JsonAppBenchmarkGpuData.hpp"
#include "Json/JsonAppBenchmarkInfo.hpp"
#include "Json/JsonAppBenchmarkRenderInfo.hpp"

namespace Fsl::AppBenchmarkDataPersistence
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t CurrentVersion = 1;
    }
    namespace LocalSettings
    {
      constexpr auto Version = "Version";
      constexpr auto Info = "Info";
      constexpr auto CpuData = "CpuData";
      constexpr auto GpuData = "GpuData";
      constexpr auto RenderInfo = "RenderInfo";
      constexpr auto Time = "Time";
    }

    AppBenchmarkData Parse(const std::string& jsonContent)
    {
      auto json = nlohmann::json::parse(jsonContent);

      if (json[LocalSettings::Version].get<uint32_t>() != LocalConfig::CurrentVersion)
      {
        throw NotSupportedException("Unsupported version");
      }
      auto strTime = json[LocalSettings::Time].get<std::string>();

      std::optional<AppBenchmarkGpuData> gpuData = json.contains(LocalSettings::GpuData)
                                                     ? std::optional<AppBenchmarkGpuData>(json[LocalSettings::GpuData].get<AppBenchmarkGpuData>())
                                                     : std::optional<AppBenchmarkGpuData>();

      std::optional<AppBenchmarkRenderInfo> renderInfo =
        json.contains(LocalSettings::RenderInfo)
          ? std::optional<AppBenchmarkRenderInfo>(json[LocalSettings::RenderInfo].get<AppBenchmarkRenderInfo>())
          : std::optional<AppBenchmarkRenderInfo>();

      return AppBenchmarkData(json[LocalSettings::Info].get<AppBenchmarkInfo>(), json[LocalSettings::CpuData].get<AppBenchmarkCpuData>(),
                              std::move(gpuData), renderInfo, StringDateUtil::Parse(StringViewLite(strTime)));
    }

    std::string Encode(const AppBenchmarkData& data)
    {
      nlohmann::json json;
      json[LocalSettings::Version] = LocalConfig::CurrentVersion;
      json[LocalSettings::Info] = data.Info;
      json[LocalSettings::CpuData] = data.CpuData;
      if (data.GpuData.has_value())
      {
        json[LocalSettings::GpuData] = data.GpuData.value();
      }
      if (data.RenderInfo.has_value())
      {
        json[LocalSettings::RenderInfo] = data.RenderInfo.value();
      }
      json[LocalSettings::Time] = StringDateUtil::ToString(data.Time);

      // Pretty print JSON document to string
      return json.dump(JsonConfig::Indent);
    }
  }

  std::optional<AppBenchmarkData> TryLoad(const IO::Path& path)
  {
    try
    {
      std::string content;
      return IO::File::TryReadAllText(content, path) ? Parse(content) : std::optional<AppBenchmarkData>();
    }
    catch (std::exception& ex)
    {
      FSLLOG3_DEBUG_WARNING("Exception: {}", ex.what());
      FSL_PARAM_NOT_USED(ex);
      return {};
    }
  }

  void Save(const IO::Path& path, const AppBenchmarkData& data)
  {
    // Since this data contains a timestamp it is likely to have been changed on write.
    std::string strNewJson = Encode(data);
    IO::File::WriteAllText(path, strNewJson);
  }
}
