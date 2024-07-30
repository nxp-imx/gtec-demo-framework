#ifndef FSLGRAPHICS_COLORSU16_HPP
#define FSLGRAPHICS_COLORSU16_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslGraphics/ColorU16.hpp>

namespace Fsl::ColorsU16
{
  // Some predefined colors (values taken from HTML)
  inline constexpr ColorU16 Transparent() noexcept
  {
    return ColorU16(PackedColor64(0x00000000));
  }

  inline constexpr ColorU16 TransparentWhite() noexcept
  {
    return ColorU16(PackedColor64(0x0000FFFFFFFFFFFF));
  }

  inline constexpr ColorU16 Black() noexcept
  {
    return ColorU16(PackedColor64(0xFFFF000000000000));
  }

  inline constexpr ColorU16 Red() noexcept
  {
    return ColorU16(PackedColor64(0xFFFFFFFF00000000));
  }

  inline constexpr ColorU16 Green() noexcept
  {
    return ColorU16(PackedColor64(0xFFFF0000FFFF0000));
  }

  inline constexpr ColorU16 Blue() noexcept
  {
    return ColorU16(PackedColor64(0xFFFF00000000FFFF));
  }

  inline constexpr ColorU16 Cyan() noexcept
  {
    return ColorU16(PackedColor64(0xFFFF0000FFFFFFFF));
  }

  inline constexpr ColorU16 Yellow() noexcept
  {
    return ColorU16(PackedColor64(0xFFFFFFFFFFFF0000));
  }

  inline constexpr ColorU16 White() noexcept
  {
    return ColorU16(PackedColor64(0xFFFFFFFFFFFFFFFF));
  }

  inline constexpr ColorU16 Orange() noexcept
  {
    return ColorU16(PackedColor32(0xFFFFA500));
  }

  inline constexpr ColorU16 Pink() noexcept
  {
    return ColorU16(PackedColor32(0xFFFAAFBE));
  }

  inline constexpr ColorU16 Purple() noexcept
  {
    return ColorU16(PackedColor32(0xFF800080));
  }

  inline constexpr ColorU16 Marrom() noexcept
  {
    return ColorU16(PackedColor32(0xFF800000));
  }

  inline constexpr ColorU16 Brown() noexcept
  {
    return ColorU16(PackedColor32(0xFFA52A2A));
  }

  inline constexpr ColorU16 Olive() noexcept
  {
    return ColorU16(PackedColor32(0xFF808000));
  }

  inline constexpr ColorU16 Silver() noexcept
  {
    return ColorU16(PackedColor32(0xFFC0C0C0));
  }

  inline constexpr ColorU16 DarkBlue() noexcept
  {
    return ColorU16(PackedColor32(0xFF0000A0));
  }

  inline constexpr ColorU16 LightBlue() noexcept
  {
    return ColorU16(PackedColor32(0xFFADD8E6));
  }

  inline constexpr ColorU16 Lime() noexcept
  {
    return ColorU16(PackedColor64(0xFFFF0000FFFF0000));
  }
}

#endif
