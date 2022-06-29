/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include "AppBenchmarkDataToTextFile.hpp"
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Common/FmtVersionInfo.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxSize2D.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkData.hpp>
#include <fmt/format.h>
#include <vector>

namespace Fsl::AppBenchmarkDataToTextFile
{
  void Save(const IO::Path& dst, const AppBenchmarkData& benchmarkData)
  {
    std::string res;
    res +=
      fmt::format("// Name: {} Version: {} Debug: {}\n", benchmarkData.Info.AppName, benchmarkData.Info.AppVersion, benchmarkData.Info.AppDebugMode);
    res += fmt::format("// Scene: {} Version: {} ResolutionPx: {} DensityDpi: {}\n", static_cast<int32_t>(benchmarkData.Info.Scene),
                       benchmarkData.Info.SceneVersion, benchmarkData.Info.ResolutionPx, benchmarkData.Info.DensityDpi);
    res += "// Entries: UIProcessEvents, UIUpdate, UIDraw, UIDrawPreprocess, UIDrawGenMesh, UIDrawFillBuffers, UIDrawSchedule\n";
    res += fmt::format("std::array<BenchDataEntry, {}> =\n", benchmarkData.CpuData.Entries.size());
    res += "{\n";
    for (const auto& entry : benchmarkData.CpuData.Entries)
    {
      res += fmt::format("  BenchDataEntry({}, {}, {}, {}, {}, {}, {}),\n", entry.UIProcessEvents, entry.UIUpdate, entry.UIDraw,
                         entry.UIDrawPreprocess, entry.UIDrawGenMesh, entry.UIDrawFillBuffers, entry.UIDrawSchedule);
    }
    res += "};\n";

    IO::File::WriteAllText(dst, res);
  }
}
