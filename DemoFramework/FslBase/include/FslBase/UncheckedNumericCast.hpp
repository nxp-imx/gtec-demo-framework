#ifndef FSLBASE_UNCHECKEDNUMERICCAST_HPP
#define FSLBASE_UNCHECKEDNUMERICCAST_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Fsl
{
  //! Provides numerically checked type casts that checks if the cast value actually fit into the target type.
  //!
  //! The Unchecked function will validate that the conversion is valid in debug builds, but will do nothing in release builds.
  //! This is slightly better than just doing a static_cast as it will allow you to catch issues during debugging.

  //! The main goal of this file is to provide 'checked' conversion between the C++ fundamental data types
  //! https://en.cppreference.com/w/cpp/language/types
  //!
  //! The main methods are
  //! - To<type>                 Convert the input type to the given type with proper checks. This can be done as a constexpr.
  //!                            Failures are reported by exception.
  //! - UncheckedTo<type>        Convert the input type to the given type with proper debug only checks. This can be done as a constexpr.
  namespace IntegralConverter
  {
    template <typename TTo, typename TFrom>
    constexpr typename std::enable_if<
      std::is_integral<TTo>::value && std::is_integral<TFrom>::value && std::is_signed<TTo>::value && std::is_signed<TFrom>::value, TTo>::type
      UncheckedConvertIntegral(const TFrom& value) noexcept
    {
      assert(value >= std::numeric_limits<TTo>::min());
      assert(value <= std::numeric_limits<TTo>::max());
      return static_cast<TTo>(value);
    }

    template <typename TTo, typename TFrom>
    constexpr typename std::enable_if<
      std::is_integral<TTo>::value && std::is_integral<TFrom>::value && std::is_signed<TTo>::value && std::is_unsigned<TFrom>::value, TTo>::type
      UncheckedConvertIntegral(const TFrom& value) noexcept
    {
      assert(value <= static_cast<typename std::make_unsigned<TFrom>::type>(std::numeric_limits<TTo>::max()));
      return static_cast<TTo>(value);
    }

    template <typename TTo, typename TFrom>
    constexpr typename std::enable_if<
      std::is_integral<TTo>::value && std::is_integral<TFrom>::value && std::is_unsigned<TTo>::value && std::is_signed<TFrom>::value, TTo>::type
      UncheckedConvertIntegral(const TFrom& value) noexcept
    {
      assert(value >= 0);
      assert(static_cast<typename std::make_unsigned<TFrom>::type>(value) <= std::numeric_limits<TTo>::max());
      return static_cast<TTo>(value);
    }

    template <typename TTo, typename TFrom>
    constexpr typename std::enable_if<
      std::is_integral<TTo>::value && std::is_integral<TFrom>::value && std::is_unsigned<TTo>::value && std::is_unsigned<TFrom>::value, TTo>::type
      UncheckedConvertIntegral(const TFrom& value) noexcept
    {
      assert(value <= std::numeric_limits<TTo>::max());
      return static_cast<TTo>(value);
    }
  }

  template <typename TTo, typename TFrom, typename = std::enable_if<std::is_integral<TTo>::value && std::is_integral<TFrom>::value>>
  constexpr inline TTo UncheckedNumericCast(const TFrom& value) noexcept
  {
    return IntegralConverter::UncheckedConvertIntegral<TTo>(value);
  }

  ////! Convert to a type where we can not guarantee it can be done as a constexpr
  ////! This also throws a ConversionException on failure.
  // template <typename TTo, typename TFrom>
  // inline TTo ChangeTo(const TFrom& value) = delete;


  // Alternative implementation

  // template <typename TTarget, typename TSource, typename = std::enable_if<std::is_integral<TTarget>::value && std::is_integral<TSource>::value>>
  //  struct Converter
  //  {
  //    using result_type = TTarget;

  //    constexpr static result_type Convert(const TSource& value) noexcept
  //    {
  //      return Fsl::FastConvert::ConvertIntegral<TTarget>(value);
  //    }
  //  };

  //  template <>
  //  struct Converter<PxPoint2, PxExtent2D>
  //  {
  //    using source_type = PxExtent2D;
  //    using result_type = PxPoint2;

  //    constexpr static result_type Convert(const PxExtent2D& value) noexcept
  //    {
  //      return {Converter<result_type::value_type, source_type::value_type>::Convert(value.Width),
  //              Converter<result_type::value_type, source_type::value_type>::Convert(value.Height)};
  //    }
  //  };


  //  template <typename TTarget, typename TSource>
  //  constexpr inline TTarget NumericCast(const TSource& value) noexcept
  //  {
  //    return Converter<TTarget, TSource>::Convert(value);
  //  }

}

#endif
