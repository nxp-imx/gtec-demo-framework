#ifndef FSLBASE_SYSTEM_PLATFORM_PLATFORMPATHTRANSFORM_HPP
#define FSLBASE_SYSTEM_PLATFORM_PLATFORMPATHTRANSFORM_HPP
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

// FOR CPP FILES ONLY, NEVER INCLUDE THIS INTO A HEADER FILE!!

#include <FslBase/IO/Path.hpp>
#include <string>

// Nasty hack for dealing with UTF8 file names on windows,
// since its the only supported platform that doesn't allow UTF8 strings
// but instead provides its own 'hack' for opening wstring's
#if defined(FSL_PLATFORM_WINDOWS) && defined(_WIN32) && (defined(_UNICODE) || defined(__UNICODE))

#include <FslBase/System/Platform/PlatformWin32.hpp>

namespace Fsl::PlatformPathTransform
{
  inline std::wstring ToSystemPath(const IO::Path& path)
  {
    return PlatformWin32::Widen(path.ToUTF8String());
  }
}

#else

// For systems that support UTF8 natively
namespace Fsl::PlatformPathTransform
{
  inline const std::string& ToSystemPath(const IO::Path& path)
  {
    return path.ToUTF8String();
  }
}

#endif
#endif
