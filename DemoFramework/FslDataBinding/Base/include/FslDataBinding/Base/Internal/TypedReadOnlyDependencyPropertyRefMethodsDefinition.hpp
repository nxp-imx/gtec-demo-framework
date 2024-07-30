#ifndef FSLDATABINDING_BASE_INTERNAL_TYPEDREADONLYDEPENDENCYPROPERTYREFMETHODSDEFINITION_HPP
#define FSLDATABINDING_BASE_INTERNAL_TYPEDREADONLYDEPENDENCYPROPERTYREFMETHODSDEFINITION_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslDataBinding/Base/Internal/IPropertyMethodsDefinition.hpp>
#include <FslDataBinding/Base/Property/PropertyMethodUtil.hpp>
#include <typeindex>

namespace Fsl::DataBinding::Internal
{
  template <typename TPropertyValueType>
  class TypedReadOnlyDependencyPropertyRefMethodsDefinition final : public IPropertyMethodsDefinition
  {
    using FnTypedGetMemberCallProxy = const TPropertyValueType& (*)(void* pObjectThis);

    FnTypedGetMemberCallProxy m_fnGetProxy{};
    std::type_index m_targetClass;

    TypedReadOnlyDependencyPropertyRefMethodsDefinition(FnTypedGetMemberCallProxy fnGetProxy, std::type_index targetClass) noexcept
      : m_fnGetProxy(fnGetProxy)
      , m_targetClass(targetClass)
    {
    }

  public:
    using value_type = TPropertyValueType;

    // constexpr TypedReadOnlyDependencyPropertyRefMethodsDefinition() noexcept = default;

    template <typename T, const value_type& (T::*TGetMethod)() const, bool (T::*TSetMethod)(const value_type&)>
    static constexpr TypedReadOnlyDependencyPropertyRefMethodsDefinition Create() noexcept
    {
      return {PropertyMethodUtil::TypedGetRefMemberCallProxy<value_type, T, TGetMethod>, typeid(T)};
    }

    std::type_index GetTargetClass() const
    {
      return m_targetClass;
    }

    //! WARNING: only use this with pointers to objects that the methods are based on (so if X::Get was stored pObjectThis can only be of type X)
    //! We don't store the original type as a template parameter because we want to generate less 'template instances',
    //! instead its stored in m_targetClass so we can use this for multiple class types.
    const value_type& Get(void* pObjectThis) const
    {
      return (*m_fnGetProxy)(pObjectThis);
    }

    // IDependencyPropertyGetSetMethods

    std::type_index GetTargetClassType() const final
    {
      return m_targetClass;
    }

    std::type_index GetValueType() const final
    {
      return typeid(TPropertyValueType);
    }
  };
}

#endif
