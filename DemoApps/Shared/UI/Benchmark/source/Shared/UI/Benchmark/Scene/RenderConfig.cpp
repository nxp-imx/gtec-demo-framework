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

#include "RenderConfig.hpp"
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <Shared/UI/Benchmark/App/TestAppFactory.hpp>
#include <array>

namespace Fsl::RenderConfig
{
  namespace
  {
    RenderMethodInfo CreateRenderMethodInfo(const AppRenderMethod method, const StringViewLite name)
    {
      return {method, name, TestAppFactory::GetRenderSystemInfo(method)};
    }

    std::array<RenderMethodInfo, 2> g_renderRecords = {CreateRenderMethodInfo(AppRenderMethod::FlexImBatch, "FlexImBatch"),
                                                       CreateRenderMethodInfo(AppRenderMethod::BasicImBatch, "BasicImBatch")};

  }

  uint32_t GetSize()
  {
    return UncheckedNumericCast<uint32_t>(g_renderRecords.size());
  }

  ReadOnlySpan<RenderMethodInfo> Get()
  {
    return SpanUtil::AsReadOnlySpan(g_renderRecords);
  }
}
