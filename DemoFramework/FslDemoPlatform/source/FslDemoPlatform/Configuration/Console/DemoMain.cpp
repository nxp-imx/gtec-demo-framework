#if !defined(_WIN32) && !defined(__ANDROID__)
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

#include <FslBase/Log/BasicLog.hpp>
#include <FslBase/IO/Directory.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include <csignal>

int main(int argc, char* argv[])
{
  const std::shared_ptr<Fsl::ITag> nativeWindowTag;

  Fsl::IO::Path strContentPath;
  Fsl::IO::Path strPersistentPath;
  if (argc >= 1 && argv[0] != nullptr)
  {
    strContentPath = Fsl::IO::Path::GetDirectoryName(argv[0]);
  }
  else
  {
    strContentPath = Fsl::IO::Directory::GetCurrentWorkingDirectory();
    FSLBASICLOG_WARNING("Could not find the exe path, trying to use current working directory instead.");
  }

  strContentPath = Fsl::IO::Path::GetFullPath(strContentPath);
  strPersistentPath = strContentPath;
  strContentPath = Fsl::IO::Path::Combine(strContentPath, "Content");

  Fsl::DemoRunnerConfig config(true, strContentPath, strPersistentPath, nativeWindowTag);
  return Fsl::RunDemo(argc, argv, config);
}
#endif
