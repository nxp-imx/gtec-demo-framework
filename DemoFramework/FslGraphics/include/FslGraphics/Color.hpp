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
#include <FslBase/Math/MathHelper_Bilinear.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <algorithm>
#include <cmath>

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
    constexpr Color() noexcept = default;

    constexpr explicit Color(const uint32_t value) noexcept
      : m_value(value)
    {
    }

    constexpr Color(const float r, const float g, const float b, const float a) noexcept
      : m_value(ToUInt32(b) | (ToUInt32(g) << 8) | (ToUInt32(r) << 16) | (ToUInt32(a) << 24))
    {
    }

    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
      : m_value(static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(a) << 24))
    {
    }

    Color(const int32_t r, const int32_t g, const int32_t b, const int32_t a);
    Color(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a);

    constexpr Vector4 ToVector4() const
    {
      return {static_cast<float>(R()) / 255.0f, static_cast<float>(G()) / 255.0f, static_cast<float>(B()) / 255.0f, static_cast<float>(A()) / 255.0f};
    }

    //! get the packed value
    constexpr uint32_t PackedValue() const noexcept
    {
      return m_value;
    }

    //! @brief get the alpha component
    constexpr uint8_t A() const noexcept
    {
      return static_cast<uint8_t>((m_value >> 24) & 0xFF);
    }

    //! @brief get the red component
    constexpr uint8_t R() const noexcept
    {
      return static_cast<uint8_t>((m_value >> 16) & 0xFF);
    }

    //! @brief get the green component
    constexpr uint8_t G() const noexcept
    {
      return static_cast<uint8_t>((m_value >> 8) & 0xFF);
    }

    //! @brief get the blue component
    constexpr uint8_t B() const noexcept
    {
      return static_cast<uint8_t>(m_value & 0xFF);
    }

    // Some predefined colors (values taken from HTML)
    static constexpr Color Transparent() noexcept
    {
      return Color(0x00000000);
    }

    static constexpr Color Black() noexcept
    {
      return Color(0xFF000000);
    }

    static constexpr Color Red() noexcept
    {
      return Color(0xFFFF0000);
    }

    static constexpr Color Green() noexcept
    {
      return Color(0xFF00FF00);
    }

    static constexpr Color Blue() noexcept
    {
      return Color(0xFF0000FF);
    }

    static constexpr Color Cyan() noexcept
    {
      return Color(0xFF00FFFF);
    }

    static constexpr Color Yellow() noexcept
    {
      return Color(0xFFFFFF00);
    }

    static constexpr Color White() noexcept
    {
      return Color(0xFFFFFFFF);
    }

    static constexpr Color Orange() noexcept
    {
      return Color(0xFFFFA500);
    }

    static constexpr Color Pink() noexcept
    {
      return Color(0xFFFAAFBE);
    }

    static constexpr Color Purple() noexcept
    {
      return Color(0xFF800080);
    }

    static constexpr Color Marrom() noexcept
    {
      return Color(0xFF800000);
    }

    static constexpr Color Brown() noexcept
    {
      return Color(0xFFA52A2A);
    }

    static constexpr Color Olive() noexcept
    {
      return Color(0xFF808000);
    }

    static constexpr Color Silver() noexcept
    {
      return Color(0xFFC0C0C0);
    }

    static constexpr Color DarkBlue() noexcept
    {
      return Color(0xFF0000A0);
    }

    static constexpr Color LightBlue() noexcept
    {
      return Color(0xFFADD8E6);
    }

    static constexpr Color Lime() noexcept
    {
      return Color(0xFF00FF00);
    }


    constexpr bool operator==(const Color& rhs) const noexcept
    {
      return m_value == rhs.m_value;
    }

    constexpr bool operator!=(const Color& rhs) const noexcept
    {
      return m_value != rhs.m_value;
    }

    static constexpr inline Color Premultiply(const Color color)
    {
      const uint32_t fixed24A = (uint32_t(color.A()) << 24u) / 255;
      // basically ((r * a) + 0.5f) / 2.0f
      const uint32_t r = ((color.R() * fixed24A) + (1u << 23u)) >> 24u;
      const uint32_t g = ((color.G() * fixed24A) + (1u << 23u)) >> 24u;
      const uint32_t b = ((color.B() * fixed24A) + (1u << 23u)) >> 24u;
      return {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), color.A()};
    }


    static constexpr inline Color Premultiply(const Color color, const float alphaMul)
    {
      // while the +0.5f trick has issues, its precise enough for what we are doing here and
      // it allows this to be constexpr (which std::round is not)

      // See http:// blog.frama-c.com/index.php?post/2013/05/02/nearbyintf1
      // For issues with +0.5f

      // pretty close for fixed point
      // (((((uint32_t(color.A()) * uint32_t(alphaMod)) << 17u) + (1 << 16u))) / 255) >> 1
      const float alpha = (static_cast<float>(color.A()) / 255.0f) * std::max(alphaMul, 0.0f);
      constexpr auto roundMagic = 0.49999997f;
      auto r = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.R())) + roundMagic), 0u, 255u);
      auto g = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.G())) + roundMagic), 0u, 255u);
      auto b = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.B())) + roundMagic), 0u, 255u);
      auto a = MathHelper::Clamp(static_cast<uint32_t>((alpha * 255.0f) + roundMagic), 0u, 255u);
      return {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), uint8_t(a)};
    }

    static constexpr inline Color PremultiplyRGB(const Color color, const float alphaMul, uint8_t newA)
    {
      // while the +0.5f trick has issues, its precise enough for what we are doing here and
      // it allows this to be constexpr (which std::round is not)

      // See http:// blog.frama-c.com/index.php?post/2013/05/02/nearbyintf1
      // For issues with +0.5f

      // pretty close for fixed point
      // (((((uint32_t(color.A()) * uint32_t(alphaMod)) << 17u) + (1 << 16u))) / 255) >> 1
      const float alpha = (static_cast<float>(color.A()) / 255.0f) * std::max(alphaMul, 0.0f);
      constexpr auto roundMagic = 0.49999997f;
      auto r = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.R())) + roundMagic), 0u, 255u);
      auto g = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.G())) + roundMagic), 0u, 255u);
      auto b = MathHelper::Clamp(static_cast<uint32_t>((alpha * float(color.B())) + roundMagic), 0u, 255u);
      return {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), newA};
    }

    static constexpr inline Color SetA(const Color color, uint8_t value)
    {
      return Color((color.m_value & 0x00FFFFFF) | (uint32_t(value) << 24));
    }

    static constexpr inline Color SetR(const Color color, uint8_t value)
    {
      return Color((color.m_value & 0xFF00FFFF) | (uint32_t(value) << 16));
    }

    static constexpr inline Color SetG(const Color color, uint8_t value)
    {
      return Color((color.m_value & 0xFFFF00FF) | (uint32_t(value) << 8));
    }

    static constexpr inline Color SetB(const Color color, uint8_t value)
    {
      return Color((color.m_value & 0xFFFFFF00) | value);
    }

    static constexpr inline Color SetA(const Color color, float value)
    {
      return SetA(color, ToUInt8(value));
    }

    static constexpr inline Color SetR(const Color color, float value)
    {
      return SetR(color, ToUInt8(value));
    }

    static constexpr inline Color SetG(const Color color, float value)
    {
      return SetG(color, ToUInt8(value));
    }

    static constexpr inline Color SetB(const Color color, float value)
    {
      return SetB(color, ToUInt8(value));
    }

    static constexpr inline Color Bilinear(const Color& val00, const Color& val10, const Color& val01, const Color& val11, const float weightX,
                                           const float weightY)
    {
      return {
        MathHelper::Bilinear(static_cast<float>(val00.R()) / 255.0f, static_cast<float>(val10.R()) / 255.0f, static_cast<float>(val01.R()) / 255.0f,
                             static_cast<float>(val11.R()) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.G()) / 255.0f, static_cast<float>(val10.G()) / 255.0f, static_cast<float>(val01.G()) / 255.0f,
                             static_cast<float>(val11.G()) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.B()) / 255.0f, static_cast<float>(val10.B()) / 255.0f, static_cast<float>(val01.B()) / 255.0f,
                             static_cast<float>(val11.B()) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.A()) / 255.0f, static_cast<float>(val10.A()) / 255.0f, static_cast<float>(val01.A()) / 255.0f,
                             static_cast<float>(val11.A()) / 255.0f, weightX, weightY),
      };
    }

  private:
    static constexpr inline uint8_t ToUInt8(const float value)
    {
      const auto asInt = static_cast<int32_t>(value * 255.0f);
      return static_cast<uint8_t>(MathHelper::Clamp(asInt, 0, 255));
    }

    static constexpr inline uint32_t ToUInt32(const float value)
    {
      const auto asInt = static_cast<int32_t>(value * 255.0f);
      return static_cast<uint32_t>(MathHelper::Clamp(asInt, 0, 255));
    }

    // static constexpr inline float ToFloat(const uint8_t value)
    //{
    //  return value / 255.0f;
    //}
  };
}

#endif
