#ifdef _WIN32
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include "DPIHelperWin32.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <VersionHelpers.h>
#include <cassert>

namespace Fsl
{
  DPIHelperWin32::DPIHelperWin32()
    : m_mode(Mode::Basic)
    , m_shcore(nullptr)
    , m_funcGetDpiForMonitor(nullptr)
  {
    // Check if this is Windows 8.1 or greater
    if (IsWindows8Point1OrGreater())
    {
      m_mode = Mode::PerMonitor;
    }

    if (m_mode == Mode::PerMonitor)
    {
      m_shcore = LoadLibraryEx(TEXT("SHCore.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
      if (m_shcore != nullptr)
      {
        m_funcGetDpiForMonitor = reinterpret_cast<FuncGetDpiForMonitor>(GetProcAddress(m_shcore, "GetDpiForMonitor"));
        if (m_funcGetDpiForMonitor == nullptr)
        {
          FSLLOG3_WARNING("Failed to locate GetDpiForMonitor in SHCore.dll");
          FreeLibrary(m_shcore);
          m_shcore = nullptr;
          m_mode = Mode::Basic;
        }
      }
      else
      {
        FSLLOG3_WARNING("Failed to load SHCore.dll dynamically");
        m_mode = Mode::Basic;
      }
    }
  }


  DPIHelperWin32::~DPIHelperWin32()
  {
    if (m_shcore != nullptr)
    {
      FreeLibrary(m_shcore);
      m_shcore = nullptr;
    }
  }


  bool DPIHelperWin32::TryGetDpi(HWND hWnd, Point2& rDPI) const
  {
    if (m_mode != Mode::PerMonitor)
    {
      // Fallback mode if we don't have access to per monitor DPI
      HDC screen = GetDC(nullptr);
      int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
      int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
      ReleaseDC(nullptr, screen);
      rDPI = Point2(dpiX, dpiY);
      return true;
    }
    assert(m_funcGetDpiForMonitor != nullptr);

    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 0;
    UINT dpiY = 0;
    if (m_funcGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) != S_OK)
    {
      rDPI = Point2();
      return false;
    }
    rDPI = Point2(dpiX, dpiY);
    return true;
  }
}    // namespace Fsl
#endif
