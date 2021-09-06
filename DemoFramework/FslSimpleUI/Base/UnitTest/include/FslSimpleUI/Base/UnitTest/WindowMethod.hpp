#ifndef FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_WINDOWMETHOD_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_FSLSIMPLEUI_BASE_UNITTEST_WINDOWMETHOD_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <cstdint>

enum class WindowMethod : uint32_t
{
  WinInit = 0x0001,
  WinMarkLayoutAsDirty = 0x0002,
  // WinGetContentPxRectangle = 0x0004,
  WinHandleEvent = 0x0008,
  WinUpdate = 0x0010,
  WinResolve = 0x0020,
  WinDraw = 0x0040,
  OnClickInputPreview = 0x0080,
  OnClickInput = 0x0100,
  OnSelect = 0x0200,
  OnContentChanged = 0x0400,
  ArrangeOverride = 0x0800,
  MeasureOverride = 0x1000,
  OnPropertiesUpdated = 0x2000,

  All = 0xFFFF
};

inline constexpr WindowMethod operator|(const WindowMethod lhs, const WindowMethod rhs)
{
  return static_cast<WindowMethod>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline WindowMethod& operator|=(WindowMethod& rLhs, const WindowMethod rhs)
{
  rLhs = static_cast<WindowMethod>(static_cast<uint32_t>(rLhs) | static_cast<uint32_t>(rhs));
  return rLhs;
}

#endif
