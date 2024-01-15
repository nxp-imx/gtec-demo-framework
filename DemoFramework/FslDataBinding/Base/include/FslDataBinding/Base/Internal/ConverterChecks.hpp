#ifndef FSLDATABINDING_BASE_INTERNAL_CONVERTERCHECKS_HPP
#define FSLDATABINDING_BASE_INTERNAL_CONVERTERCHECKS_HPP
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
#include <FslDataBinding/Base/Bind/PropertyTypeInfo.hpp>
#include <FslDataBinding/Base/Internal/PropertyGetInfo.hpp>
#include <FslDataBinding/Base/Internal/PropertySetInfo.hpp>
#include <typeinfo>


namespace Fsl::DataBinding::Internal::ConverterChecks
{
  namespace InternalHelper
  {
    inline bool IsSupported(const PropertyMethodsImplType getPropertyMethodsImplType) noexcept
    {
      switch (getPropertyMethodsImplType)
      {
      case PropertyMethodsImplType::ATypedDependencyProperty:
      case PropertyMethodsImplType::ATypedDependencyPropertyRef:
        return true;
      case PropertyMethodsImplType::NotAvailable:
      case PropertyMethodsImplType::Undefined:
      case PropertyMethodsImplType::ObserverDependency:
        break;
      }
      return false;
    }

    inline bool CheckIsGetValid(const Internal::PropertyGetInfo& getter, const std::type_info& typeInfo)
    {
      return getter.pGet != nullptr && getter.pGet->GetType() == typeInfo && IsSupported(getter.ImplType);
    }

    inline bool CheckIsSetValid(const Internal::PropertySetInfo& setter, const std::type_info& typeInfo)
    {
      return setter.pSet != nullptr && setter.pSet->GetType() == typeInfo && IsSupported(setter.ImplType);
    }

    inline bool CheckIsGettersValid(const ReadOnlySpan<Internal::PropertyGetInfo> getters, const std::size_t index, const std::type_info& typeInfo)
    {
      return index < getters.size() && CheckIsGetValid(getters[index], typeInfo);
    }

    inline bool CheckIsSettersValid(const ReadOnlySpan<Internal::PropertySetInfo> setters, const std::size_t index, const std::type_info& typeInfo)
    {
      return index < setters.size() && CheckIsSetValid(setters[index], typeInfo);
    }

    template <typename... TTypeIndices>
    bool CheckIsGettersValid(const ReadOnlySpan<Internal::PropertyGetInfo> getters, const std::size_t index, const std::type_info& typeInfo,
                             TTypeIndices const&... args)
    {
      return index < getters.size() && CheckIsGetValid(getters[index], typeInfo) ? CheckIsGettersValid(getters, index + 1, args...) : false;
    }

    template <typename... TTypeIndices>
    bool CheckIsSettersValid(const ReadOnlySpan<Internal::PropertySetInfo> setters, const std::size_t index, const std::type_info& typeInfo,
                             TTypeIndices const&... args)
    {
      return index < setters.size() && CheckIsSetValid(setters[index], typeInfo) ? CheckIsSettersValid(setters, index + 1, args...) : false;
    }
  }


  inline bool IsGettersValid(const ReadOnlySpan<Internal::PropertyGetInfo> getters, const std::type_info& typeInfo)
  {
    return InternalHelper::CheckIsGettersValid(getters, 0, typeInfo);
  }

  template <typename... TTypeIndices>
  bool IsGettersValid(const ReadOnlySpan<Internal::PropertyGetInfo> getters, const std::type_info& typeInfo, TTypeIndices const&... args)
  {
    return InternalHelper::CheckIsGettersValid(getters, 0, typeInfo, args...);
  }


  inline bool IsSettersValid(const ReadOnlySpan<Internal::PropertySetInfo> setters, const std::type_info& typeInfo)
  {
    return InternalHelper::CheckIsSettersValid(setters, 0, typeInfo);
  }

  template <typename... TTypeIndices>
  bool IsSettersValid(const ReadOnlySpan<Internal::PropertySetInfo> setters, const std::type_info& typeInfo, TTypeIndices const&... args)
  {
    return InternalHelper::CheckIsSettersValid(setters, 0, typeInfo, args...);
  }
}

#endif
