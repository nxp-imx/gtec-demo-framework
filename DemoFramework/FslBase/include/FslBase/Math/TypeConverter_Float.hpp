#ifndef FSLBASE_MATH_TYPECONVERTER_FLOAT_HPP
#define FSLBASE_MATH_TYPECONVERTER_FLOAT_HPP
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

#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Fsl::TypeConverter
{
  // --- Int16

  template <>
  inline int16_t ChangeTo<int16_t, float>(const float& value)
  {
    if (std::isinf(value) || std::isnan(value))
    {
      throw ConversionException("unsupported float value");
    }
    auto rounded = std::round(value);
    if (rounded < static_cast<float>(std::numeric_limits<int16_t>::min()) || rounded > static_cast<float>(std::numeric_limits<int16_t>::max()))
    {
      throw ConversionException("overflow");
    }
    return static_cast<int16_t>(rounded);
  }

  // --- Int32

  template <>
  inline int32_t ChangeTo<int32_t, float>(const float& value)
  {
    if (std::isinf(value) || std::isnan(value))
    {
      throw ConversionException("unsupported float value");
    }
    auto rounded = std::round(value);
    if (rounded < static_cast<float>(std::numeric_limits<int32_t>::min()) || rounded > static_cast<float>(std::numeric_limits<int32_t>::max()))
    {
      throw ConversionException("overflow");
    }
    return static_cast<int32_t>(rounded);
  }

  // --- UInt16

  template <>
  inline uint16_t ChangeTo<uint16_t, float>(const float& value)
  {
    if (std::isinf(value) || std::isnan(value))
    {
      throw ConversionException("unsupported float value");
    }
    auto rounded = std::round(value);
    if (rounded < static_cast<float>(std::numeric_limits<uint16_t>::min()) || rounded > static_cast<float>(std::numeric_limits<uint16_t>::max()))
    {
      throw ConversionException("overflow");
    }
    return static_cast<uint16_t>(rounded);
  }

  // --- UInt32

  template <>
  inline uint32_t ChangeTo<uint32_t, float>(const float& value)
  {
    if (std::isinf(value) || std::isnan(value))
    {
      throw ConversionException("unsupported float value");
    }
    auto rounded = std::round(value);
    if (rounded < static_cast<float>(std::numeric_limits<uint32_t>::min()) || rounded > static_cast<float>(std::numeric_limits<uint32_t>::max()))
    {
      throw ConversionException("overflow");
    }
    return static_cast<uint32_t>(rounded);
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------


  // --- Int16

  template <>
  inline int16_t UncheckedChangeTo<int16_t, float>(const float& value)
  {
    assert(!std::isinf(value));
    assert(!std::isnan(value));
    auto rounded = std::round(value);
    assert(rounded <= static_cast<float>(std::numeric_limits<int16_t>::max()));
    assert(rounded >= static_cast<float>(std::numeric_limits<int16_t>::min()));
    return static_cast<int16_t>(rounded);
  }

  // --- Int32

  template <>
  inline int32_t UncheckedChangeTo<int32_t, float>(const float& value)
  {
    assert(!std::isinf(value));
    assert(!std::isnan(value));
    auto rounded = std::round(value);
    assert(rounded <= static_cast<float>(std::numeric_limits<int32_t>::max()));
    assert(rounded >= static_cast<float>(std::numeric_limits<int32_t>::min()));
    return static_cast<int32_t>(rounded);
  }

  // --- UInt16

  template <>
  inline uint16_t UncheckedChangeTo<uint16_t, float>(const float& value)
  {
    assert(!std::isinf(value));
    assert(!std::isnan(value));
    auto rounded = std::round(value);
    assert(rounded <= static_cast<float>(std::numeric_limits<uint16_t>::max()));
    assert(rounded >= static_cast<float>(std::numeric_limits<uint16_t>::min()));
    return static_cast<uint16_t>(rounded);
  }

  // --- UInt32

  template <>
  inline uint32_t UncheckedChangeTo<uint32_t, float>(const float& value)
  {
    assert(!std::isinf(value));
    assert(!std::isnan(value));
    auto rounded = std::round(value);
    assert(rounded <= static_cast<float>(std::numeric_limits<uint32_t>::max()));
    assert(rounded >= static_cast<float>(std::numeric_limits<uint32_t>::min()));
    return static_cast<uint32_t>(rounded);
  }
}

#endif
