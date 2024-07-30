#ifndef FSLDATABINDING_BASE_BIND_TWOWAYMULTICONVERTERBINDING_HPP
#define FSLDATABINDING_BASE_BIND_TWOWAYMULTICONVERTERBINDING_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDataBinding/Base/Bind/AMultiConverterBinding.hpp>
#include <FslDataBinding/Base/Internal/ConverterChecks.hpp>
#include <FslDataBinding/Base/Internal/ConverterUtil.hpp>
#include <FslDataBinding/Base/Internal/TypedPropertyMethodsUtil.hpp>
#include <array>
#include <cassert>
#include <functional>
#include <tuple>
#include <typeindex>
#include <utility>


namespace Fsl::DataBinding
{
  template <typename TTarget, typename... TSource>
  class TwoWayMultiConverterBinding final : public AMultiConverterBinding
  {
  public:
    using target_value_type = TTarget;
    using convert_function_type = std::function<target_value_type(TSource... value)>;
    using convert_back_function_type = std::function<std::tuple<TSource...>(const TTarget&)>;

  private:
    static_assert(sizeof...(TSource) > 1, "TwoWayMultiConverterBinding should only be used when there are multiple bindings");
    // NOLINTNEXTLINE(google-readability-casting)
    static inline const std::array<PropertyTypeInfo, sizeof...(TSource)> Types{PropertyTypeInfo(typeid(TSource))...};

    convert_function_type m_fnConvert;
    convert_back_function_type m_fnConvertBack;

  public:
    explicit TwoWayMultiConverterBinding(convert_function_type fnConvert, convert_back_function_type fnConvertBack)
      : m_fnConvert(std::move(fnConvert))
      , m_fnConvertBack(std::move(fnConvertBack))
    {
      if (!m_fnConvert)
      {
        throw std::invalid_argument("fnConvert can not be empty");
      }
      if (!m_fnConvertBack)
      {
        throw std::invalid_argument("fnConvert can not be empty");
      }
    }

    BindingCapabilityFlags GetCaps() const noexcept final
    {
      return BindingCapabilityFlags::ConvertBack;
    }

    ReadOnlySpan<PropertyTypeInfo> GetSourceTypes() const final
    {
      return SpanUtil::AsReadOnlySpan(Types);
    }

    std::type_index GetTargetType() const final
    {
      return typeid(target_value_type);
    }


    Internal::PropertySetResult Convert(const Internal::PropertyMethodsImplType setPropertyMethodsImplType, Internal::IPropertyMethods* const pSet,
                                        const ReadOnlySpan<Internal::PropertyGetInfo> getters) final
    {
      if (getters.size() != (sizeof...(TSource)))
      {
        FSLLOG3_DEBUG_WARNING("Incorrect amount of getters supplied");
        return Internal::PropertySetResult::UnsupportedGetType;
      }
      if (Fsl::DataBinding::Internal::ConverterChecks::IsGettersValid(getters, typeid(TSource)...))
      {
        // We know that the getters are valid (the right type and the right amount), so lets execute the converter function.
        // So its time to extract each of the multi binding parameters and call the converter function with them.
        // This requires a bit of template magic.
        return Internal::TypedPropertyMethodsUtil::SetByValue(
          setPropertyMethodsImplType, pSet,
          Fsl::DataBinding::Internal::ConverterUtil::InvokeConvert<target_value_type, TSource...>(m_fnConvert, getters));
      }
      return Internal::PropertySetResult::UnsupportedGetType;
    }


    bool TryConvertBack(Span<Internal::PropertySetResult> resultSpan, const ReadOnlySpan<Internal::PropertySetInfo> setters,
                        const Internal::PropertyGetInfo getter) final
    {
      if (setters.size() != (sizeof...(TSource)))
      {
        FSLLOG3_DEBUG_WARNING("Incorrect amount of getters supplied");
        return false;
      }
      if (resultSpan.size() < (sizeof...(TSource)))
      {
        FSLLOG3_DEBUG_WARNING("Incorrect resultSpan size supplied");
        return false;
      }
      if (!Fsl::DataBinding::Internal::ConverterChecks::IsSettersValid(setters, typeid(TSource)...))
      {
        FSLLOG3_DEBUG_WARNING("Incorrect setter types");
        return false;
      }

      // We know that the setters are valid (the right type and the right amount), so lets execute the back converter function.
      // So its time to extract each of the multi binding parameters and call the converter function with them.
      // This requires a bit of template magic.

      Fsl::DataBinding::Internal::ConverterUtil::InvokeConvertBack<target_value_type, TSource...>(resultSpan, m_fnConvertBack, setters, getter);
      return true;
    }


  private:
  };
}

#endif
