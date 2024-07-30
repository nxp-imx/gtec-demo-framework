#ifndef FSLDATABINDING_BASE_INTERNAL_TYPEDPROPERTYMETHODSUTIL_HPP
#define FSLDATABINDING_BASE_INTERNAL_TYPEDPROPERTYMETHODSUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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
#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/ATypedReadOnlyDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ATypedReadOnlyDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/PropertyGetInfo.hpp>
#include <FslDataBinding/Base/Internal/PropertyMethodsImplType.hpp>
#include <FslDataBinding/Base/Internal/PropertySetResult.hpp>

namespace Fsl::DataBinding::Internal::TypedPropertyMethodsUtil
{
  template <typename TSource>
  TSource Get(const PropertyGetInfo& getter)
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
    case PropertyMethodsImplType::ATypedReadOnlyDependencyProperty:
      {    // Try the normal get method
        const auto* const pTypedGetOperation = dynamic_cast<const ATypedReadOnlyDependencyPropertyMethods<TSource>*>(getter.pGet);
        if (pTypedGetOperation != nullptr)
        {
          return pTypedGetOperation->Get();
        }
        break;
      }
    case PropertyMethodsImplType::ATypedReadOnlyDependencyPropertyRef:
      {    // Try the ref get method
        const auto* const pTypedGetOperation = dynamic_cast<const ATypedReadOnlyDependencyPropertyRefMethods<TSource>*>(getter.pGet);
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

  // template <typename TTarget>
  // inline bool TryGetByValue(const IPropertyMethods* const pGet, TTarget& rValue)
  //{
  //   const auto* const pTypedGetOperation = dynamic_cast<const ATypedDependencyPropertyMethods<TTarget>*>(pGet);
  //   if (pTypedGetOperation != nullptr)
  //   {
  //     rValue = pTypedGetOperation->Get(value);
  //     return true;
  //   }
  //   rValue = {};
  //   return false
  // }


  template <typename TTarget>
  inline PropertySetResult SetByValue(IPropertyMethods* const pSet, const TTarget value)
  {
    {    // normal set
      auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyMethods<TTarget>*>(pSet);
      if (pTypedSetOperation != nullptr)
      {
        return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
      }
    }
    {    // ref set
      auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyRefMethods<TTarget>*>(pSet);
      if (pTypedSetOperation != nullptr)
      {
        return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
      }
    }
    return PropertySetResult::NotSupported;
  }

  template <typename TTarget>
  inline PropertySetResult SetByRef(IPropertyMethods* const pSet, const TTarget& value)
  {
    {    // ref set
      auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyRefMethods<TTarget>*>(pSet);
      if (pTypedSetOperation != nullptr)
      {
        return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
      }
    }
    {    // normal set
      auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyMethods<TTarget>*>(pSet);
      if (pTypedSetOperation != nullptr)
      {
        return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
      }
    }
    return PropertySetResult::NotSupported;
  }


  template <typename TTarget>
  inline PropertySetResult SetByValue(const PropertyMethodsImplType propertyMethodsImplType, IPropertyMethods* const pSet, const TTarget value)
  {
    switch (propertyMethodsImplType)
    {
    case PropertyMethodsImplType::ATypedDependencyProperty:
      {    // normal set
        auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyMethods<TTarget>*>(pSet);
        if (pTypedSetOperation != nullptr)
        {
          return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
        break;
      }
    case PropertyMethodsImplType::ATypedDependencyPropertyRef:
      {    // ref set
        auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyRefMethods<TTarget>*>(pSet);
        if (pTypedSetOperation != nullptr)
        {
          return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
        break;
      }
    default:
      break;
    }
    return PropertySetResult::NotSupported;
  }

  template <typename TTarget>
  inline PropertySetResult SetByRef(const PropertyMethodsImplType propertyMethodsImplType, IPropertyMethods* const pSet, const TTarget& value)
  {
    switch (propertyMethodsImplType)
    {
    case PropertyMethodsImplType::ATypedDependencyProperty:
      {    // normal set
        auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyMethods<TTarget>*>(pSet);
        if (pTypedSetOperation != nullptr)
        {
          return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
        break;
      }
    case PropertyMethodsImplType::ATypedDependencyPropertyRef:
      {    // ref set
        auto* const pTypedSetOperation = dynamic_cast<ATypedDependencyPropertyRefMethods<TTarget>*>(pSet);
        if (pTypedSetOperation != nullptr)
        {
          return pTypedSetOperation->Set(value) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
        break;
      }
    default:
      break;
    }
    return PropertySetResult::NotSupported;
  }

}

#endif
