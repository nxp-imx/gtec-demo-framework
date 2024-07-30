#ifndef FSLNATIVEWINDOW_BASE_VIRTUALMOUSEBUTTONTOSTRING_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALMOUSEBUTTONTOSTRING_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslNativeWindow/Base/VirtualMouseButton.hpp>
#include <fmt/format.h>
#include <string>

namespace Fsl::Debug
{
  inline const char* TryToString(const VirtualMouseButton button)
  {
    switch (button)
    {
    case VirtualMouseButton::Left:
      return "Left";
    case VirtualMouseButton::Middle:
      return "Middle";
    case VirtualMouseButton::Right:
      return "Right";
    case VirtualMouseButton::X1:
      return "X1";
    case VirtualMouseButton::X2:
      return "X2";
    case VirtualMouseButton::X3:
      return "X3";
    case VirtualMouseButton::X4:
      return "X4";
    case VirtualMouseButton::X5:
      return "X5";
    case VirtualMouseButton::X6:
      return "X6";
    case VirtualMouseButton::X7:
      return "X7";
    case VirtualMouseButton::Undefined:
      return "Undefined";
    }
    return nullptr;
  }

  inline std::string ToString(const VirtualMouseButton button)
  {
    const char* const psz = TryToString(button);
    if (psz != nullptr)
    {
      return fmt::format("{} (0x{:x})", psz, static_cast<int64_t>(button));
    }
    return fmt::format("0x{:x}", static_cast<int64_t>(button));
  }
}

#endif
