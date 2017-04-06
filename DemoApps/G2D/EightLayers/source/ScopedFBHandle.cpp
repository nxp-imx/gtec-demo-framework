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

// The interaction with FB was copied from the existing G2D overlay example

#include "ScopedFBHandle.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cstring>
#include <fcntl.h>
#include <linux/fb.h>

#include <sys/ioctl.h>
#include <unistd.h>

namespace Fsl
{
  namespace
  {
    void LogIt(const std::string& str, fb_bitfield bf)
    {
      FSLLOG(str << ".offset: " << bf.offset);
      FSLLOG(str << ".length: " << bf.length);
      FSLLOG(str << ".msb_right: " << bf.msb_right);
    }
  }


  ScopedFBHandle::ScopedFBHandle(const bool force32BPP, const bool alignFrameBuffer)
    : PhysicalMemStart(0)
    , m_alignedYOffset(0)
    //, Size(0)

  {
    if ((Handle = open("/dev/fb0", O_RDWR, 0)) == -1)
    {
      if ((Handle = open("/dev/graphics/fb0", O_RDWR, 0)) == -1)
        throw GraphicsException("Unable to open fb0");
    }

    try
    {
      // Get fix screen info.
      if (ioctl(Handle, FBIOGET_FSCREENINFO, &OrgFixInfo) == -1)
        throw GraphicsException("Failed to get FB screen info");

      // Get variable screen info.
      if (ioctl(Handle, FBIOGET_VSCREENINFO, &OrgScreenInfo) == -1)
        throw GraphicsException("Failed to get FB variable screen info");

      FSLLOG("fbInfo.smem_start: " << OrgFixInfo.smem_start);
      Log(OrgScreenInfo, "org");
    }
    catch (const std::exception&)
    {
      close(Handle);
      throw;
    }

    try
    {
      ScreenInfo = OrgScreenInfo;

      // Make room for double buffering
      m_alignedYOffset = ScreenInfo.yres;
      if (alignFrameBuffer)
      {
        m_alignedYOffset = ((m_alignedYOffset / 64) * 64) + (m_alignedYOffset % 64 == 0 ? 0 : 64);
        FSLLOG("Aligning second FB from " << ScreenInfo.yres << " to " << m_alignedYOffset);
      }

      ScreenInfo.xres_virtual = ScreenInfo.xres;
      ScreenInfo.yres_virtual = m_alignedYOffset * 2;
      ScreenInfo.width = ScreenInfo.xres;
      ScreenInfo.height = ScreenInfo.yres;
      ScreenInfo.xoffset = 0;
      ScreenInfo.yoffset = 0;

      Log(ScreenInfo, "BeforeSet");
      if (force32BPP && ScreenInfo.bits_per_pixel != 32)
      {
        ScreenInfo.bits_per_pixel = 32;
        ScreenInfo.red.length = 8;
        ScreenInfo.red.offset = 0;
        ScreenInfo.green.length = 8;
        ScreenInfo.green.offset = 8;
        ScreenInfo.blue.length = 8;
        ScreenInfo.blue.offset = 16;
        Log(ScreenInfo, "BeforeSet 32bpp");
      }

      // create a copy of the screen info since the ioctl call
      fb_var_screeninfo copiedScreenInfo = ScreenInfo;
      if (ioctl(Handle, FBIOPUT_VSCREENINFO, &copiedScreenInfo) == -1)
        throw GraphicsException("Failed to create double buffering");

      Log(copiedScreenInfo, "AfterSet-ioctl modified (discarded)");

      // Get fix screen info.
      if (ioctl(Handle, FBIOGET_FSCREENINFO, &FixInfo) == -1)
        throw GraphicsException("Failed to get FB screen info");
      FSLLOG("fbInfo.smem_start: " << FixInfo.smem_start);
    }
    catch (const std::exception&)
    {
      if (ioctl(Handle, FBIOPUT_VSCREENINFO, &OrgScreenInfo) == -1)
      {
        FSLLOG_ERROR("Could not restore variable screen info!");
      }
      close(Handle);
      throw;
    }

    UpdateInfo();
  }

  ScopedFBHandle::~ScopedFBHandle()
  {
    FSLLOG("Resetting FB");
    Log(OrgScreenInfo, "Restoring fb to");
    if (ioctl(Handle, FBIOPUT_VSCREENINFO, &OrgScreenInfo) == -1)
    {
      FSLLOG_ERROR("Could not restore variable screen info!");
    }
    FSLLOG("Closing FB");
    close(Handle);
    FSLLOG("Closed FB");
  }

  void ScopedFBHandle::Log(const fb_var_screeninfo& info, const char*const psz)
  {
    FSLLOG(psz);
    FSLLOG("fbScreenInfo.xres: " << info.xres);
    FSLLOG("fbScreenInfo.yres: " << info.yres);
    FSLLOG("fbScreenInfo.xres_virtual: " << info.xres_virtual);
    FSLLOG("fbScreenInfo.yres_virtual: " << info.yres_virtual);
    FSLLOG("fbScreenInfo.xoffset: " << info.xoffset);
    FSLLOG("fbScreenInfo.yoffset: " << info.yoffset);
    FSLLOG("fbScreenInfo.bits_per_pixel: " << info.bits_per_pixel);
    FSLLOG("fbScreenInfo.grayscale: " << info.grayscale);
    LogIt("fbScreenInfo.red", info.red);
    LogIt("fbScreenInfo.green", info.green);
    LogIt("fbScreenInfo.blue", info.blue);
    LogIt("fbScreenInfo.transp", info.transp);
    FSLLOG("fbScreenInfo.nonstd: " << info.nonstd);
    FSLLOG("fbScreenInfo.activate: " << info.activate);
    FSLLOG("fbScreenInfo.height: " << info.height);
    FSLLOG("fbScreenInfo.width: " << info.width);
    FSLLOG("fbScreenInfo.accel_flags: " << info.accel_flags);
    FSLLOG("fbScreenInfo.pixclock: " << info.pixclock);
    FSLLOG("fbScreenInfo.left_margin: " << info.left_margin);
    FSLLOG("fbScreenInfo.right_margin: " << info.right_margin);
    FSLLOG("fbScreenInfo.upper_margin: " << info.upper_margin);
    FSLLOG("fbScreenInfo.lower_margin: " << info.lower_margin);
    FSLLOG("fbScreenInfo.hsync_len: " << info.hsync_len);
    FSLLOG("fbScreenInfo.vsync_len: " << info.vsync_len);
    FSLLOG("fbScreenInfo.sync: " << info.sync);
    FSLLOG("fbScreenInfo.vmode: " << info.vmode);
    FSLLOG("fbScreenInfo.rotate: " << info.rotate);
    FSLLOG("fbScreenInfo.colorspace: " << info.colorspace);
    FSLLOG("fbScreenInfo.reserved[0]: " << info.reserved[0]);
    FSLLOG("fbScreenInfo.reserved[1]: " << info.reserved[1]);
    FSLLOG("fbScreenInfo.reserved[2]: " << info.reserved[2]);
    FSLLOG("fbScreenInfo.reserved[3]: " << info.reserved[3]);
  }


  void ScopedFBHandle::UpdateInfo()
  {
    const int yOffset = (ScreenInfo.yoffset == 0 ? m_alignedYOffset : 0);
    PhysicalMemStart = FixInfo.smem_start + (ScreenInfo.xres_virtual * yOffset * ScreenInfo.bits_per_pixel / 8);
    //FSLLOG("yOffset: " << yOffset);
  }

  void ScopedFBHandle::WaitForVSync()
  {
    ioctl(Handle, FBIO_WAITFORVSYNC, nullptr);
  }


  void ScopedFBHandle::SwapBuffers(bool forceVSync, const int testValue)
  {
    if (testValue < 1)
    {
      if (ScreenInfo.yoffset == 0)
        ScreenInfo.yoffset = m_alignedYOffset;
      else
        ScreenInfo.yoffset = 0;

      if (ioctl(Handle, FBIOPAN_DISPLAY, &ScreenInfo) == -1)
      {
        FSLLOG_WARNING("pan failed");
      }
    }

    if (forceVSync)
      WaitForVSync();

    UpdateInfo();
  }


}
