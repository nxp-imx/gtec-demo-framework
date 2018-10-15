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

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__) || defined(__QNXNTO__)
#include <ctime>
#else
#error Unsupported platform
#endif

#include <FslBase/System/HighResolutionTimer.hpp>

namespace Fsl
{
  HighResolutionTimer::HighResolutionTimer()    // NOLINT(modernize-use-equals-default)
  {
#ifdef _WIN32
    LARGE_INTEGER value;
    QueryPerformanceFrequency(&value);
    m_frequency = value.QuadPart / 1000000.0;
#endif
  }


  uint64_t HighResolutionTimer::GetTime() const
  {
#ifdef _WIN32
    {
      LARGE_INTEGER value;
      QueryPerformanceCounter(&value);
      return static_cast<uint64_t>(value.QuadPart / m_frequency);
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
#endif
  }
}
