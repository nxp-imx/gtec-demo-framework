#ifndef FSLSIMPLEUI_BASE_UICOLORS_HPP
#define FSLSIMPLEUI_BASE_UICOLORS_HPP
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

#include <FslGraphics/ColorsU16.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>

namespace Fsl::UI::UIColors
{
  // Some predefined colors (values taken from HTML)
  inline constexpr UIColor Transparent() noexcept
  {
    return UIColor(ColorsU16::Transparent());
  }

  inline constexpr UIColor TransparentWhite() noexcept
  {
    return UIColor(ColorsU16::TransparentWhite());
  }

  inline constexpr UIColor Black() noexcept
  {
    return UIColor(ColorsU16::Black());
  }

  inline constexpr UIColor Red() noexcept
  {
    return UIColor(ColorsU16::Red());
  }

  inline constexpr UIColor Green() noexcept
  {
    return UIColor(ColorsU16::Green());
  }

  inline constexpr UIColor Blue() noexcept
  {
    return UIColor(ColorsU16::Blue());
  }

  inline constexpr UIColor Cyan() noexcept
  {
    return UIColor(ColorsU16::Cyan());
  }

  inline constexpr UIColor Yellow() noexcept
  {
    return UIColor(ColorsU16::Yellow());
  }

  inline constexpr UIColor White() noexcept
  {
    return UIColor(ColorsU16::White());
  }

  inline constexpr UIColor Orange() noexcept
  {
    return UIColor(ColorsU16::Orange());
  }

  inline constexpr UIColor Pink() noexcept
  {
    return UIColor(ColorsU16::Pink());
  }

  inline constexpr UIColor Purple() noexcept
  {
    return UIColor(ColorsU16::Purple());
  }

  inline constexpr UIColor Marrom() noexcept
  {
    return UIColor(ColorsU16::Marrom());
  }

  inline constexpr UIColor Brown() noexcept
  {
    return UIColor(ColorsU16::Brown());
  }

  inline constexpr UIColor Olive() noexcept
  {
    return UIColor(ColorsU16::Olive());
  }

  inline constexpr UIColor Silver() noexcept
  {
    return UIColor(ColorsU16::Silver());
  }

  inline constexpr UIColor DarkBlue() noexcept
  {
    return UIColor(ColorsU16::DarkBlue());
  }

  inline constexpr UIColor LightBlue() noexcept
  {
    return UIColor(ColorsU16::LightBlue());
  }

  inline constexpr UIColor Lime() noexcept
  {
    return UIColor(ColorsU16::Lime());
  }
}

#endif
