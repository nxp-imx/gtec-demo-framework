#ifndef SHARED_UI_BENCHMARK_SCENE_BENCHRESULTMANAGER_HPP
#define SHARED_UI_BENCHMARK_SCENE_BENCHRESULTMANAGER_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Optional.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkData.hpp>
#include <array>

namespace Fsl
{
  class ServiceProvider;
  class BenchResultManager
  {
    struct DataSet
    {
      IO::Path FilePath;
      AppBenchmarkData Data;
    };

    IO::Path m_persistentDataPath;
    std::array<DataSet, 2> m_entries;

    Optional<AppBenchmarkData> m_forceViewData;
    Optional<AppBenchmarkData> m_forceCompareData;

  public:
    explicit BenchResultManager(const ServiceProvider& serviceProvider, const Optional<IO::Path>& forceViewFile,
                                const Optional<IO::Path>& forceCompareFile, const bool debugMode);
    ~BenchResultManager();


    Optional<AppBenchmarkData> TryLoad();
    Optional<AppBenchmarkData> TryLoadCompare();
    bool TrySave(const AppBenchmarkData& benchmarkData);

  private:
    static DataSet CreateDataSet(const IO::PathView persistentDataPath, const IO::PathView filename);
    std::size_t DetermineSaveIndex() const;
    std::size_t DetermineNewestIndex() const;
  };
}

#endif
