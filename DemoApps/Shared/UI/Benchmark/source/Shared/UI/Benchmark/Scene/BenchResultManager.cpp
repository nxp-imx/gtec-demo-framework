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

#include "BenchResultManager.hpp"
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkDataPersistence.hpp>
#include <limits>
//#include "AppBenchmarkDataToTextFile.hpp"

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView Filename0("BenchmarkResult0.json");
      constexpr IO::PathView Filename1("BenchmarkResult1.json");
      constexpr IO::PathView FilenameDbg0("BenchmarkResult0_debug.json");
      constexpr IO::PathView FilenameDbg1("BenchmarkResult1_debug.json");
    }

    AppBenchmarkData TryLoadInputCommandList(const IO::Path& persistentDataFile)
    {
      // Try to load the custom recording first
      auto res = AppBenchmarkDataPersistence::TryLoad(persistentDataFile);
      if (res.has_value())
      {
        FSLLOG3_VERBOSE("Benchmark result loaded from '{}'", persistentDataFile);
        return res.value();
      }
      return {};
    }

    std::optional<AppBenchmarkData> TryLoadForceData(const std::optional<IO::Path>& forceCompareFile)
    {
      if (forceCompareFile.has_value())
      {
        std::optional<AppBenchmarkData> res = AppBenchmarkDataPersistence::TryLoad(forceCompareFile.value());
        FSLLOG3_WARNING_IF(!res.has_value(), "Failed to load the requested result file: '{}'", forceCompareFile.value());
        return res;
      }
      return {};
    }
  }

  BenchResultManager::BenchResultManager(const ServiceProvider& serviceProvider, const std::optional<IO::Path>& forceViewFile,
                                         const std::optional<IO::Path>& forceCompareFile, const bool debugMode)
    : m_persistentDataPath(serviceProvider.Get<IPersistentDataManager>()->GetPersistentDataPath())
    , m_entries{CreateDataSet(m_persistentDataPath.AsPathView(), !debugMode ? LocalConfig::Filename0 : LocalConfig::FilenameDbg0),
                CreateDataSet(m_persistentDataPath.AsPathView(), !debugMode ? LocalConfig::Filename1 : LocalConfig::FilenameDbg1)}
    , m_forceViewData(TryLoadForceData(forceViewFile))
    , m_forceCompareData(TryLoadForceData(forceCompareFile))
  {
  }


  BenchResultManager::~BenchResultManager() = default;


  std::optional<AppBenchmarkData> BenchResultManager::TryLoad()
  {
    if (m_forceViewData.has_value())
    {
      return m_forceViewData;
    }

    const auto& entry = m_entries[DetermineNewestIndex()];
    return !entry.Data.IsEmpty() ? std::optional<AppBenchmarkData>(entry.Data) : std::optional<AppBenchmarkData>();
  }


  std::optional<AppBenchmarkData> BenchResultManager::TryLoadCompare()
  {
    if (m_entries.size() != 2u)
    {
      throw InternalErrorException("Assumption not met");
    }
    if (m_forceCompareData.has_value())
    {
      return m_forceCompareData;
    }
    const auto index = DetermineNewestIndex();
    const auto compareIndex = index == 0 ? 1u : 0u;
    const auto& entry = m_entries[compareIndex];
    return !entry.Data.IsEmpty() ? std::optional<AppBenchmarkData>(entry.Data) : std::optional<AppBenchmarkData>();
  }


  bool BenchResultManager::TrySave(const AppBenchmarkData& benchmarkData)
  {
    // Dump data in a debug friendly format
    // AppBenchmarkDataToTextFile::Save("dump.txt", benchmarkData);

    try
    {
      auto& rEntry = m_entries[DetermineSaveIndex()];
      rEntry.Data = benchmarkData;
      AppBenchmarkDataPersistence::Save(rEntry.FilePath, benchmarkData);
      FSLLOG3_INFO("Benchmark result saved to '{}'", rEntry.FilePath);
      return true;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to save: {}", ex.what());
      return false;
    }
  }


  BenchResultManager::DataSet BenchResultManager::CreateDataSet(const IO::PathView persistentDataPath, const IO::PathView filename)
  {
    auto path = IO::Path::Combine(persistentDataPath, filename);
    return {path, TryLoadInputCommandList(path)};
  }

  std::size_t BenchResultManager::DetermineSaveIndex() const
  {
    std::size_t foundIndex = 0u;
    std::time_t firstTime = std::numeric_limits<std::time_t>::max();
    for (std::size_t i = 0; i < m_entries.size(); ++i)
    {
      const auto& entry = m_entries[i];
      if (entry.Data.IsEmpty())
      {
        // A empty entry was found
        return i;
      }
      if (entry.Data.Time < firstTime)
      {
        firstTime = entry.Data.Time;
        foundIndex = i;
      }
    }
    return foundIndex;
  }

  std::size_t BenchResultManager::DetermineNewestIndex() const
  {
    std::size_t foundIndex = 0u;
    std::time_t firstTime = std::numeric_limits<std::time_t>::min();
    for (std::size_t i = 0; i < m_entries.size(); ++i)
    {
      const auto& entry = m_entries[i];
      if (!entry.Data.IsEmpty() && entry.Data.Time > firstTime)
      {
        firstTime = entry.Data.Time;
        foundIndex = i;
      }
    }
    return foundIndex;
  }
}
