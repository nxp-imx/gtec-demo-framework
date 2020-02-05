#ifndef FSLGRAPHICS_COLOR_HPP
#define FSLGRAPHICS_COLOR_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <algorithm>

namespace Fsl
{
  struct Color
  {
    // Colors are encoded like this
    // Bits:  0- 7 = Blue
    // Bits:  8-15 = Green
    // Bits: 16-23 = Red
    // Bits: 24-31 = Alpha
    uint32_t m_value{0};

  public:
    constexpr Color() = default;

    constexpr explicit Color(const uint32_t value)
      : m_value(value)
    {
    }

    constexpr Color(const float r, const float g, const float b, const float a)
      : m_value(static_cast<uint32_t>(Convert(b)) | (static_cast<uint32_t>(Convert(g)) << 8) | (static_cast<uint32_t>(Convert(r)) << 16) |
                (static_cast<uint32_t>(Convert(a)) << 24))
    {
    }

    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
      : m_value(static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(a) << 24))
    {
    }

    Color(const int32_t r, const int32_t g, const int32_t b, const int32_t a);
    Color(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a);

    constexpr Vector4 ToVector4() const
    {
      return Vector4(R() / 255.0f, G() / 255.0f, B() / 255.0f, A() / 255.0f);
    }

    //! get the packed value
    constexpr uint32_t PackedValue() const
    {
      return m_value;
    }

    //! @brief get the alpha component
    constexpr uint8_t A() const
    {
      return static_cast<uint8_t>((m_value >> 24) & 0xFF);
    }

    //! @brief get the red component
    constexpr uint8_t R() const
    {
      return static_cast<uint8_t>((m_value >> 16) & 0xFF);
    }

    //! @brief get the green component
    constexpr uint8_t G() const
    {
      return static_cast<uint8_t>((m_value >> 8) & 0xFF);
    }

    //! @brief get the blue component
    constexpr uint8_t B() const
    {
      return static_cast<uint8_t>(m_value & 0xFF);
    }

    // Some predefined colors (values taken from HTML)
    static constexpr Color Transparent()
    {
      return Color(0x00000000);
    }

    static constexpr Color Black()
    {
      return Color(0xFF000000);
    }

    static constexpr Color Red()
    {
      return Color(0xFFFF0000);
    }

    static constexpr Color Green()
    {
      return Color(0xFF00FF00);
    }

    static constexpr Color Blue()
    {
      return Color(0xFF0000FF);
    }

    static constexpr Color Cyan()
    {
      return Color(0xFF00FFFF);
    }

    static constexpr Color Yellow()
    {
      return Color(0xFFFFFF00);
    }

    static constexpr Color White()
    {
      return Color(0xFFFFFFFF);
    }

    static constexpr Color Orange()
    {
      return Color(0xFFFFA500);
    }

    static constexpr Color Pink()
    {
      return Color(0xFFFAAFBE);
    }

    static constexpr Color Purple()
    {
      return Color(0xFF800080);
    }

    static constexpr Color Marrom()
    {
      return Color(0xFF800000);
    }

    static constexpr Color Brown()
    {
      return Color(0xFFA52A2A);
    }

    static constexpr Color Olive()
    {
      return Color(0xFF808000);
    }

    static constexpr Color Silver()
    {
      return Color(0xFFC0C0C0);
    }

    static constexpr Color DarkBlue()
    {
      return Color(0xFF0000A0);
    }

    static constexpr Color LightBlue()
    {
      return Color(0xFFADD8E6);
    }

    static constexpr Color Lime()
    {
      return Color(0xFF00FF00);
    }


    constexpr bool operator==(const Color& rhs) const
    {
      return m_value == rhs.m_value;
    }

    constexpr bool operator!=(const Color& rhs) const
    {
      return m_value != rhs.m_value;
    }

  private:
    static constexpr inline uint8_t Convert(const float value)
    {
      const auto asInt = static_cast<int32_t>(value * 255.0f);
      // Since std::min and max are not constexpr in C++11
      return static_cast<uint8_t>(std::min(std::max(asInt, 0), 255));
    }
  };
}

#endif
