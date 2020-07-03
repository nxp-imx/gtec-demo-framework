#ifndef FSLGRAPHICS_UNITTEST_HELPER_PIXELTESTUTIL_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_PIXELTESTUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  namespace PixelTestUtil
  {
    // Test utils for R8G8B8 pixels
    struct R8G8B8
    {
      struct PixelUInt8
      {
        uint8_t R{0};
        uint8_t G{0};
        uint8_t B{0};

        constexpr PixelUInt8() = default;

        constexpr PixelUInt8(const uint8_t r, const uint8_t g, const uint8_t b)
          : R(r)
          , G(g)
          , B(b)
        {
        }

        constexpr bool operator==(const PixelUInt8& rhs) const
        {
          return R == rhs.R && G == rhs.G && B == rhs.B;
        }

        constexpr bool operator!=(const PixelUInt8& rhs) const
        {
          return !(*this == rhs);
        }
      };


      struct DefaultColorTemplate
      {
        static constexpr PixelUInt8 R()
        {
          return {0xFF, 0x00, 0x00};
        }
        static constexpr PixelUInt8 G()
        {
          return {0x00, 0xFF, 0x00};
        }
        static constexpr PixelUInt8 B()
        {
          return {0x00, 0x00, 0xFF};
        }
        static constexpr PixelUInt8 Black()
        {
          return {0x00, 0x00, 0x00};
        }
        static constexpr PixelUInt8 White()
        {
          return {0xFF, 0xFF, 0xFF};
        }
      };
    };


    // Test utils for B8G8R8 pixels
    using B8G8R8 = R8G8B8;


    // Test utils for R8G8B8A8 pixels
    struct R8G8B8A8
    {
      struct PixelUInt8
      {
        uint8_t R{0};
        uint8_t G{0};
        uint8_t B{0};
        uint8_t A{0};

        constexpr PixelUInt8() = default;

        constexpr PixelUInt8(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
          : R(r)
          , G(g)
          , B(b)
          , A(a)
        {
        }

        constexpr bool operator==(const PixelUInt8& rhs) const
        {
          return R == rhs.R && G == rhs.G && B == rhs.B && A == rhs.A;
        }

        constexpr bool operator!=(const PixelUInt8& rhs) const
        {
          return !(*this == rhs);
        }
      };

      struct DefaultColorTemplate
      {
        static constexpr PixelUInt8 R()
        {
          return {0xFF, 0x00, 0x00, 0x40};
        }
        static constexpr PixelUInt8 G()
        {
          return {0x00, 0xFF, 0x00, 0x40};
        }

        static constexpr PixelUInt8 B()
        {
          return {0x00, 0x00, 0xFF, 0x40};
        }

        static constexpr PixelUInt8 Black()
        {
          return {0x00, 0x00, 0x00, 0x40};
        }

        static constexpr PixelUInt8 White()
        {
          return {0xFF, 0xFF, 0xFF, 0x40};
        }
      };
    };

    // Test utils for B8G8R8A8 pixels
    using B8G8R8A8 = R8G8B8A8;
  }
}

#endif
