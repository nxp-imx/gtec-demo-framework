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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>

namespace Fsl
{
  Color::Color(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    : m_r(static_cast<uint8_t>(r & 0xFF))
    , m_g(static_cast<uint8_t>(g & 0xFF))
    , m_b(static_cast<uint8_t>(b & 0xFF))
    , m_a(static_cast<uint8_t>(a & 0xFF))
  {
    FSLLOG3_DEBUG_WARNING_IF(r < 0 || r > 255, "Red is expected to be between 0-255 so {} is not valid.", r);
    FSLLOG3_DEBUG_WARNING_IF(g < 0 || g > 255, "Green is expected to be between 0-255 so {} is not valid.", g);
    FSLLOG3_DEBUG_WARNING_IF(b < 0 || b > 255, "Blue is expected to be between 0-255 so {} is not valid.", b);
    FSLLOG3_DEBUG_WARNING_IF(a < 0 || a > 255, "Alpha is expected to be between 0-255 so {} is not valid.", a);
  }


  Color::Color(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    : m_r(static_cast<uint8_t>(r & 0xFF))
    , m_g(static_cast<uint8_t>(g & 0xFF))
    , m_b(static_cast<uint8_t>(b & 0xFF))
    , m_a(static_cast<uint8_t>(a & 0xFF))
  {
    FSLLOG3_DEBUG_WARNING_IF(r > 255, "Red is expected to be between 0-255 so {} is not valid.", r);
    FSLLOG3_DEBUG_WARNING_IF(g > 255, "Green is expected to be between 0-255 so {} is not valid.", g);
    FSLLOG3_DEBUG_WARNING_IF(b > 255, "Blue is expected to be between 0-255 so {} is not valid.", b);
    FSLLOG3_DEBUG_WARNING_IF(a > 255, "Alpha is expected to be between 0-255 so {} is not valid.", a);
  }
}
