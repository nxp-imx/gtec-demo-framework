#if defined(__linux__)
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
#include <stdexcept>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

namespace Fsl
{
  std::string Platform::GetCurrentWorkingDirectory()
  {
    char buffer[FILENAME_MAX];
    if (getcwd(buffer, sizeof(buffer)) == nullptr)
    {
      throw std::runtime_error("Failed to retrieve the current working directory");
    }
    return std::string(buffer);
  }


  std::string Platform::GetFullPath(const std::string& path)
  {
    char* pPath = realpath(path.c_str(), nullptr);
    if (pPath == nullptr)
    {
      throw std::runtime_error("failed to create the full path");
    }

    try
    {
      std::string fullPath;
      fullPath = pPath;
      free(pPath);    // NOLINT(cppcoreguidelines-no-malloc)
      pPath = nullptr;
      return fullPath;
    }
    catch (const std::exception&)
    {
      free(pPath);    // NOLINT(cppcoreguidelines-no-malloc)
      pPath = nullptr;
      throw;
    }
  }

  //  int64_t Platform::GetPerformanceFrequency()
  //  {
  //#error not implemented
  //  }
  //
  //
  //  int64_t Platform::GetPerformanceCounter()
  //  {
  //#error not implemented
  //    //struct timespec currentTime;
  //    //clock_gettime(CLOCK_MONOTONIC, &currentTime);
  //    //uint64_t time = currentTime.tv_sec;
  //    //time *= 1000000;
  //    //time += (currentTime.tv_nsec / 1000);
  //    //return time;
  //  }
}

#endif
