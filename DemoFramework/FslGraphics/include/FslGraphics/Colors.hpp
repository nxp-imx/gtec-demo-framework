#ifndef FSLGRAPHICS_COLORS_HPP
#define FSLGRAPHICS_COLORS_HPP
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

#include <FslGraphics/Color.hpp>

namespace Fsl::Colors
{
  // Some predefined colors (values taken from HTML)
  inline constexpr Color Transparent() noexcept
  {
    return Color(0x00000000);
  }

  inline constexpr Color TransparentWhite() noexcept
  {
    return Color(0x00FFFFFF);
  }

  inline constexpr Color Black() noexcept
  {
    return Color(0xFF000000);
  }

  inline constexpr Color Red() noexcept
  {
    return Color(0xFFFF0000);
  }

  inline constexpr Color Green() noexcept
  {
    return Color(0xFF00FF00);
  }

  inline constexpr Color Blue() noexcept
  {
    return Color(0xFF0000FF);
  }

  inline constexpr Color Cyan() noexcept
  {
    return Color(0xFF00FFFF);
  }

  inline constexpr Color Yellow() noexcept
  {
    return Color(0xFFFFFF00);
  }

  inline constexpr Color White() noexcept
  {
    return Color(0xFFFFFFFF);
  }

  inline constexpr Color Orange() noexcept
  {
    return Color(0xFFFFA500);
  }

  inline constexpr Color Pink() noexcept
  {
    return Color(0xFFFAAFBE);
  }

  inline constexpr Color Purple() noexcept
  {
    return Color(0xFF800080);
  }

  inline constexpr Color Marrom() noexcept
  {
    return Color(0xFF800000);
  }

  inline constexpr Color Brown() noexcept
  {
    return Color(0xFFA52A2A);
  }

  inline constexpr Color Olive() noexcept
  {
    return Color(0xFF808000);
  }

  inline constexpr Color Silver() noexcept
  {
    return Color(0xFFC0C0C0);
  }

  inline constexpr Color DarkBlue() noexcept
  {
    return Color(0xFF0000A0);
  }

  inline constexpr Color LightBlue() noexcept
  {
    return Color(0xFFADD8E6);
  }

  inline constexpr Color Lime() noexcept
  {
    return Color(0xFF00FF00);
  }
}

#endif
