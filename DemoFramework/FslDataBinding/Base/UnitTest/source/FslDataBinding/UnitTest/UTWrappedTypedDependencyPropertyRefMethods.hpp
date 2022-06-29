#ifndef FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTWRAPPEDTYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
#define FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTWRAPPEDTYPEDDEPENDENCYPROPERTYREFMETHODS_HPP
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

#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Internal/TypedDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <memory>

namespace Fsl
{
  template <typename TPropertyValue>
  class UTWrappedTypedDependencyPropertyRefMethods
  {
    std::shared_ptr<DataBinding::Internal::TypedDependencyPropertyRefMethodsDefinition<TPropertyValue>> m_propertyDef;
    TPropertyValue m_value{};

    static DataBinding::DependencyPropertyDefinition CreateDefinition()
    {
      return DataBinding::DependencyPropertyDefinitionFactory::Create<TPropertyValue, UTWrappedTypedDependencyPropertyRefMethods<TPropertyValue>,
                                                                      &UTWrappedTypedDependencyPropertyRefMethods<TPropertyValue>::Get,
                                                                      &UTWrappedTypedDependencyPropertyRefMethods<TPropertyValue>::Set>(
        "TestProperty");
    }

    static std::shared_ptr<DataBinding::Internal::TypedDependencyPropertyRefMethodsDefinition<TPropertyValue>> CreateTypedDefinitionPtr()
    {
      auto thisPropertyDef = CreateDefinition();
      auto typedMethods =
        std::dynamic_pointer_cast<DataBinding::Internal::TypedDependencyPropertyRefMethodsDefinition<TPropertyValue>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DataBinding::DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      return typedMethods;
    }

  public:
    DataBinding::Internal::TypedDependencyPropertyRefMethods<TPropertyValue> Property;


    UTWrappedTypedDependencyPropertyRefMethods()
      : m_propertyDef(CreateTypedDefinitionPtr())
      , Property(this, m_propertyDef)
    {
    }

    explicit UTWrappedTypedDependencyPropertyRefMethods(const TPropertyValue value)
      : m_propertyDef(CreateTypedDefinitionPtr())
      , m_value(value)
      , Property(this, m_propertyDef)
    {
    }


    const TPropertyValue& Get() const
    {
      return m_value;
    }


    bool Set(const TPropertyValue& value)
    {
      const bool changed = value != m_value;
      m_value = value;
      return changed;
    }
  };

}

#endif
