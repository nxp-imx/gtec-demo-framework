/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/IO/Directory.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/UnitTest/Helper/ContentPathUtil.hpp>
#include <FslUnitTest/CurrentExePath.hpp>

namespace Fsl::ContentPathUtil
{
#ifdef _WIN32
  IO::Path GetBasePath()
  {
    return Fsl::IO::Directory::GetCurrentWorkingDirectory();
  }
#else
  IO::Path GetBasePath()
  {
    const auto* pszExePath = CurrentExePath::TryGetCurrentExePath();
    if (pszExePath != nullptr)
    {
      return Fsl::IO::Path::GetDirectoryName(pszExePath);
    }
    FSLLOG3_WARNING("Could not find the exe path, trying to use current working directory instead.");
    return Fsl::IO::Directory::GetCurrentWorkingDirectory();
  }
#endif

  IO::Path GetContentPath()
  {
    IO::Path strExePath = GetBasePath();
    strExePath = IO::Path::GetFullPath(strExePath);
    return IO::Path::Combine(strExePath, IO::PathView("Content"));
  }
}
