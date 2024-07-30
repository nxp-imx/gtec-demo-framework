#ifndef FSLDATABINDING_BASE_INTERNAL_TYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
#define FSLDATABINDING_BASE_INTERNAL_TYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
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
#include <FslDataBinding/Base/Internal/ATypedDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/ATypedReadOnlyDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/TypedDependencyPropertyRefMethodsDefinition.hpp>
#include <FslDataBinding/Base/Internal/TypedReadOnlyDependencyPropertyRefMethodsDefinition.hpp>
#include <cassert>
#include <memory>
#include <utility>

namespace Fsl::DataBinding::Internal
{
  template <typename T>
  class TypedDependencyPropertyRefMethods final : public ATypedDependencyPropertyRefMethods<T>
  {
  public:
    using property_value_type = typename ATypedDependencyPropertyRefMethods<T>::property_value_type;


  private:
    void* m_pPropertyOwner{nullptr};
    std::shared_ptr<TypedDependencyPropertyRefMethodsDefinition<property_value_type>> m_propertyMethods;

  public:
    explicit TypedDependencyPropertyRefMethods(void* pPropertyOwner,
                                               std::shared_ptr<TypedDependencyPropertyRefMethodsDefinition<property_value_type>> propertyMethods)
      : m_pPropertyOwner(pPropertyOwner)
      , m_propertyMethods(std::move(propertyMethods))
    {
      assert(m_pPropertyOwner != nullptr);
      assert(m_propertyMethods);
    }


    const property_value_type& Get() const final
    {
      assert(m_propertyMethods);
      assert(m_pPropertyOwner != nullptr);
      return m_propertyMethods->Get(m_pPropertyOwner);
    }

    bool Set(const property_value_type& value) final
    {
      assert(m_propertyMethods);
      assert(m_pPropertyOwner != nullptr);
      return m_propertyMethods->Set(m_pPropertyOwner, value);
    }

    PropertySetResult TrySet(const IPropertyMethods* const pGet) final
    {
      {    // Try a known get type
        const auto* const pTypedGetOperation = dynamic_cast<const ATypedDependencyPropertyRefMethods<property_value_type>*>(pGet);
        if (pTypedGetOperation != nullptr)
        {
          return Set(pTypedGetOperation->Get()) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
      }
      {    // Try a known get type
        const auto* const pTypedGetOperation = dynamic_cast<const ATypedReadOnlyDependencyPropertyRefMethods<property_value_type>*>(pGet);
        if (pTypedGetOperation != nullptr)
        {
          return Set(pTypedGetOperation->Get()) ? PropertySetResult::ValueChanged : PropertySetResult::ValueUnchanged;
        }
      }
      return PropertySetResult::UnsupportedGetType;
    }
  };
}

#endif
