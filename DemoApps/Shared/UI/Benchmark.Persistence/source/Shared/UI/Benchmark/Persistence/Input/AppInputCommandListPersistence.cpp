#ifndef FSLCUSTOM_USE_JSON_FOR_COMMANDLIST
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

#include <Shared/UI/Benchmark/Persistence/Input/AppInputCommandListPersistence.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include "Custom/AppInputCommandListIO.hpp"

namespace Fsl
{
  namespace AppInputCommandListPersistence
  {
    Optional<AppInputCommandList> TryLoad(const IO::Path& path)
    {
      try
      {
        std::vector<uint8_t> content;
        return IO::File::TryReadAllBytes(content, path) ? AppInputCommandListIO::Decode(ReadOnlySpanUtil::AsSpan(content))
                                                        : Optional<AppInputCommandList>();
      }
      catch (std::exception& ex)
      {
        FSLLOG3_DEBUG_WARNING("Exception: {}", ex.what());
        FSL_PARAM_NOT_USED(ex);
        return {};
      }
    }

    void Save(const IO::Path& path, const AppInputCommandList& commandList)
    {
      auto newContent = AppInputCommandListIO::Encode(commandList);
      std::vector<uint8_t> existing;
      if (!IO::File::TryReadAllBytes(existing, path) || ReadOnlySpanUtil::AsSpan(existing) != ReadOnlySpanUtil::AsSpan(newContent))
      {
        IO::File::WriteAllBytes(path, newContent);
      }
    }
  }
}

#endif
