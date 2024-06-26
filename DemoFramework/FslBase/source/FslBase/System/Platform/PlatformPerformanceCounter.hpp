#ifndef FSLBASE_SYSTEM_PLATFORM_PLATFORMPERFORMANCECOUNTER_HPP
#define FSLBASE_SYSTEM_PLATFORM_PLATFORMPERFORMANCECOUNTER_HPP
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

#include <FslBase/BasicTypes.hpp>
#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__) || defined(__QNXNTO__)
#include <ctime>
#elif defined(FSL_PLATFORM_FREERTOS)
#include <FslBase/Log/Log3Core.hpp>
#elif defined(FSL_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#else
#error Unsupported platform
#endif

namespace Fsl::PlatformPerformanceCounter
{
  inline uint64_t GetPerformanceFrequency() noexcept
  {
#ifdef _WIN32
    LARGE_INTEGER value;
    QueryPerformanceFrequency(&value);
    return value.QuadPart;
#elif defined(__linux__) || defined(__QNXNTO__)
    return 1000000u;
#elif defined(FSL_PLATFORM_EMSCRIPTEN)
    return 1000000u;
#else
    return 1000000u;
#endif
  }

  inline uint64_t GetPerformanceCounter() noexcept
  {
#ifdef _WIN32
    {
      LARGE_INTEGER value;
      QueryPerformanceCounter(&value);
      return value.QuadPart;
    }
#elif defined(__linux__) || defined(__QNXNTO__)
    {
      using SafeTimespec = struct timespec;
      SafeTimespec currentTime{};
      clock_gettime(CLOCK_MONOTONIC, &currentTime);
      uint64_t time = currentTime.tv_sec;
      time *= 1000000;
      time += (currentTime.tv_nsec / 1000);
      return time;
    }
#elif defined(FSL_PLATFORM_EMSCRIPTEN)
    {
      double timestampInMilliseconds = emscripten_get_now();
      double timestampInMicroseconds = timestampInMilliseconds * 1000.0;
      return static_cast<uint64_t>(timestampInMicroseconds);
    }
#else
    {
      FSLLOG3_WARNING("PlatformPerformanceCounter::GetPerformanceCounter() not implemented");
      return 0;
    }
#endif
  }
}
#endif
