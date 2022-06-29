#ifndef FSLDATABINDING_BASE_INTERNAL_CONVERTERUTIL_HPP
#define FSLDATABINDING_BASE_INTERNAL_CONVERTERUTIL_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/IPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/PropertyGetInfo.hpp>
#include <FslDataBinding/Base/Internal/PropertyMethodsImplType.hpp>
#include <functional>

namespace Fsl::DataBinding::Internal::ConverterUtil
{
  namespace InternalHelper
  {
    template <typename TSource>
    TSource DoGet(const PropertyGetInfo& getter)
    {
      switch (getter.ImplType)
      {
      case PropertyMethodsImplType::ATypedDependencyProperty:
        {    // Try the normal get method
          const auto* const pTypedGetOperation = dynamic_cast<const ATypedDependencyPropertyMethods<TSource>*>(getter.pGet);
          if (pTypedGetOperation != nullptr)
          {
            return pTypedGetOperation->Get();
          }
          break;
        }
      case PropertyMethodsImplType::ATypedDependencyPropertyRef:
        {    // Try the ref get method
          const auto* const pTypedGetOperation = dynamic_cast<const ATypedDependencyPropertyRefMethods<TSource>*>(getter.pGet);
          if (pTypedGetOperation != nullptr)
          {
            return pTypedGetOperation->Get();
          }
          break;
        }
      case PropertyMethodsImplType::NotAvailable:
      case PropertyMethodsImplType::Undefined:
      case PropertyMethodsImplType::ObserverDependency:
        break;
      }
      throw NotSupportedException("Unsupported get type (this should not occur, usage error?)");
    }

    template <typename TTarget, typename... TSource, std::size_t... TIndices>
    static TTarget DoConvert(const std::function<TTarget(TSource... value)>& fnConvert, const ReadOnlySpan<PropertyGetInfo> getters,
                             [[maybe_unused]] std::index_sequence<TIndices...> const& /*unused*/)
    {
      return fnConvert(DoGet<TSource>(getters[TIndices])...);
    }
  }


  //! @brief Invoke the converter function 'fnConvert' with the right getter
  template <typename TTarget, typename... TSource>
  static TTarget InvokeConvert(const std::function<TTarget(TSource... value)>& fnConvert, const ReadOnlySpan<PropertyGetInfo> getters)
  {
    return Fsl::DataBinding::Internal::ConverterUtil::InternalHelper::DoConvert<TTarget, TSource...>(
      fnConvert, getters, std::make_integer_sequence<std::size_t, sizeof...(TSource)>());
  }
}

#endif
