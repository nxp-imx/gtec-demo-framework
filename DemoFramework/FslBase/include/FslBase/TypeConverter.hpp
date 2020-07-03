#ifndef FSLBASE_TYPECONVERTER_HPP
#define FSLBASE_TYPECONVERTER_HPP
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

#include <FslBase/NumericCast.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Fsl
{
  //! Provides type conversion.
  //!
  //! The main methods are
  //! - To<type>        Convert the input type to the given type with proper checks. This can be done as a constexpr.
  //! - ChangeTo<type>  Convert the input type to the given type with proper checks. This can not be done as a constexpr.
  //!                   ChangeTo is primarily needed becasue std::round isn't a constexpr yet.
  namespace TypeConverter
  {
    //! Convert to a type where we can guarantee it can be done as a constexpr
    //! Will validate that the conversion is valid in and if its not it throws a ConversionException.
    template <typename TTo, typename TFrom>
    constexpr inline TTo To(const TFrom& value) = delete;

    //! Convert to a type where we can not guarantee it can be done as a constexpr
    //! Will validate that the conversion is valid in and if its not it throws a ConversionException.
    template <typename TTo, typename TFrom>
    inline TTo ChangeTo(const TFrom& value) = delete;

    //! Convert to a type where we can not guarantee it can be done as a constexpr
    //! Will validate that the conversion is valid in debug builds, but will do nothing in release builds.
    template <typename TTo, typename TFrom>
    constexpr inline TTo UncheckedTo(const TFrom& value) noexcept = delete;

    //! Convert to a type where we can not guarantee it can be done as a constexpr
    //! Will validate that the conversion is valid in debug builds, but will do nothing in release builds.
    template <typename TTo, typename TFrom>
    inline TTo UncheckedChangeTo(const TFrom& value) = delete;
  }
}

#endif
