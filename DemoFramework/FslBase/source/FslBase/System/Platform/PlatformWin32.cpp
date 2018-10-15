#ifdef _WIN32
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "Platform.hpp"
#include <FslBase/System/Platform/PlatformWin32.hpp>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include <codecvt>
#include <locale>

namespace Fsl
{
  namespace
  {
    const int32_t DEFAULT_CONV_BUFFER_SIZE = 512;

    //! @brief Convert the character to from UTF16 to UTF8
    //! @param psz the UTF16 string to convert
    //! @param srcLen the length of psz (-1 a unknown null terminated string else the length of the string)
    inline const std::string DoNarrow(const wchar_t* psz, const int srcLen)
    {
      assert(psz != nullptr);
      if (srcLen == 0)
      {
        return std::string();
      }

      assert(srcLen > 0 || srcLen == -1);

      if (srcLen <= DEFAULT_CONV_BUFFER_SIZE)
      {
        // Try a conversion with minimal heap allocations
        char tmpBuffer[DEFAULT_CONV_BUFFER_SIZE + 1];
        const int dstLen = WideCharToMultiByte(CP_UTF8, 0, psz, srcLen, tmpBuffer, DEFAULT_CONV_BUFFER_SIZE, nullptr, nullptr);
        if (dstLen > 0)
        {
          assert(dstLen <= DEFAULT_CONV_BUFFER_SIZE);
          tmpBuffer[dstLen] = 0;
          return std::string(tmpBuffer);
        }
      }

      // Query the required buffer size
      const int dstLen = WideCharToMultiByte(CP_UTF8, 0, psz, srcLen, nullptr, 0, nullptr, nullptr);
      if (dstLen <= 0)
      {
        throw std::runtime_error("UTF16 to UTF8 conversion failed");
      }

      // Try again with a buffer of the correct size
      std::vector<char> buffer(dstLen + 1);
      if (WideCharToMultiByte(CP_UTF8, 0, psz, srcLen, buffer.data(), dstLen, nullptr, nullptr) <= 0)
      {
        throw std::runtime_error("UTF16 to UTF8 conversion failed");
      }
      return std::string(buffer.data());
    }
  }


  std::string PlatformWin32::Narrow(const wchar_t* psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }
    return DoNarrow(psz, -1);
  }


  std::string PlatformWin32::Narrow(const std::wstring& str)
  {
    if (str.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
      throw std::runtime_error("string is too large");
    }
    return DoNarrow(str.c_str(), static_cast<int>(str.size()));
  }


  std::wstring PlatformWin32::Widen(const char* psz)
  {
    if (psz == nullptr)
    {
      throw std::invalid_argument("psz can not be null");
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> convl;
    return convl.from_bytes(psz);
  }


  std::wstring PlatformWin32::Widen(const std::string& str)
  {
    if (str.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
      throw std::runtime_error("string is too large");
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> convl;
    return convl.from_bytes(str);
  }


  std::string Platform::GetCurrentWorkingDirectory()
  {
    uint32_t expectedBufferLength;
    {
      const uint32_t maxPathBufferLength = MAX_PATH + 1;
      TCHAR tmpBuffer[maxPathBufferLength];

      expectedBufferLength = GetCurrentDirectory(maxPathBufferLength, tmpBuffer);
      // < because the terminating zero is not reported when its a success
      if (expectedBufferLength < maxPathBufferLength)
      {
        return PlatformWin32::Narrow(tmpBuffer);
      }
    }

    // our stack buffer was too small, so lets try the recommended size
    std::vector<TCHAR> buffer(expectedBufferLength);
    if (GetCurrentDirectory(static_cast<DWORD>(buffer.size()), buffer.data()) >= expectedBufferLength)
    {
      throw std::runtime_error("Failed to retrieve the current working directory");
    }

    return PlatformWin32::Narrow(buffer.data());
  }


  std::string Platform::GetFullPath(const std::string& path)
  {
    std::wstring widePath(PlatformWin32::Widen(path));

    wchar_t* pPath = _wfullpath(nullptr, widePath.c_str(), 0);
    if (pPath == nullptr)
    {
      throw std::runtime_error("failed to create the full path");
    }
    try
    {
      widePath = pPath;
      free(pPath);    // NOLINT(cppcoreguidelines-no-malloc)
      pPath = nullptr;
    }
    catch (const std::exception&)
    {
      free(pPath);    // NOLINT(cppcoreguidelines-no-malloc)
      pPath = nullptr;
      throw;
    }
    return PlatformWin32::Narrow(widePath);
  }


  // int64_t Platform::GetPerformanceFrequency()
  //{
  //  LARGE_INTEGER frequency;
  //  if (!QueryPerformanceFrequency(&frequency))
  //    throw std::runtime_error("Failed to QueryPerformanceFrequency.");

  //  return frequency.QuadPart;
  //}


  // int64_t Platform::GetPerformanceCounter()
  //{
  //  LARGE_INTEGER value;
  //  if (!QueryPerformanceCounter(&value))
  //    throw std::runtime_error("Failed to QueryPerformanceCounter");
  //  return value.QuadPart;
  //}
}
#endif
