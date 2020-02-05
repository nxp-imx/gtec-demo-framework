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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/IO/Directory.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslDemoPlatform/DemoRunner.hpp>
#include <windows.h>
#include "../../DemoSignalHandlerInternal.hpp"

namespace
{
  HANDLE g_currentMainThread = INVALID_HANDLE_VALUE;

  // WARNING: this is called from a random thread!!!
  BOOL WINAPI ConsoleHandler(DWORD dwType)
  {
    switch (dwType)
    {
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      // SetExitRequest is safe to call from other threads and signals, so this is safe
      Fsl::DemoFramework::SetExitRequest();

      // Wait for the main thread to exit (allow it 9seconds to respond).
      WaitForSingleObject(g_currentMainThread, 1000 * 9);
      return TRUE;
    default:
      break;
    }
    return FALSE;
  }
}

int main(int argc, char* argv[])
{
  //_CrtSetBreakAlloc(280);

  // Get the current thread handle
  if (DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_currentMainThread, 0, 0, DUPLICATE_SAME_ACCESS) == 0)
  {
    FSLLOG3_ERROR("Failed to get the current thread");
    return EXIT_FAILURE;
  }

  int result;
  try
  {
    // Set up a special handler for the console control flow, ensuring that we close down nicely if the user
    // clicks the 'x' on the console window and other related events.
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    const std::shared_ptr<Fsl::ITag> nativeWindowTag;

    Fsl::IO::Path strContentPath = Fsl::IO::Directory::GetCurrentWorkingDirectory();
    Fsl::IO::Path strPersistentPath(strContentPath);
    strContentPath = Fsl::IO::Path::Combine(strContentPath, "Content");

    Fsl::DemoRunnerConfig config(true, strContentPath, strPersistentPath, nativeWindowTag);
    result = Fsl::RunDemo(argc, argv, config);

    CloseHandle(g_currentMainThread);
  }
  catch (...)
  {
    // Ensure that we close then handle
    CloseHandle(g_currentMainThread);
    throw;
  }
  //#if !defined(NDEBUG)
  //  _CrtDumpMemoryLeaks();
  //#endif
  return result;
}
#endif
