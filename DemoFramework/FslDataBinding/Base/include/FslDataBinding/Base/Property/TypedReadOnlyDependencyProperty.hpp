#ifndef FSLDATABINDING_BASE_PROPERTY_TYPEDREADONLYDEPENDENCYPROPERTY_HPP
#define FSLDATABINDING_BASE_PROPERTY_TYPEDREADONLYDEPENDENCYPROPERTY_HPP
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

#include <FslBase/String/StringViewLite.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Internal/TypedReadOnlyDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/TypedReadOnlyDependencyPropertyMethodsDefinition.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/PropertyChangeReason.hpp>
#include <FslDataBinding/Base/ScopedDataSourceObject.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>
#include <cassert>
#include <memory>
#include <string>
#include <typeindex>

namespace Fsl::DataBinding
{
  class Binding;

  template <typename T>
  class TypedReadOnlyDependencyProperty final
  {
  public:
    using value_type = T;

  private:
    value_type m_value{};
    mutable DataBindingInstanceHandle m_hInstance;

  public:
    TypedReadOnlyDependencyProperty() = default;

    explicit TypedReadOnlyDependencyProperty(const T value)
      : m_value(value)
    {
    }

    std::type_index Type() const
    {
      return typeid(T);
    }

    bool IsInstance(const DataBindingInstanceHandle hInstance) const noexcept
    {
      return hInstance == m_hInstance;
    }

    bool IsReadOnly(const ScopedDependencyObject& dependencyObject) const noexcept
    {
      return !m_hInstance.IsValid() ? false : dependencyObject.IsPropertyReadOnly(m_hInstance);
    }

    value_type Get() const noexcept
    {
      return m_value;
    }

    bool Set(ScopedDependencyObject& rDependencyObject, const value_type value,
             const PropertyChangeReason changeReason = PropertyChangeReason::Modified)
    {
      bool changed = false;
      if (value != m_value)
      {
        bool allowChanges = true;
        if (m_hInstance.IsValid())
        {
          allowChanges = rDependencyObject.PropertyChanged(m_hInstance, changeReason);
        }
        if (allowChanges)
        {
          changed = true;
          m_value = value;
        }
      }
      return changed;
    }

    bool Set(ScopedDataSourceObject& rDependencyObject, const value_type value,
             const PropertyChangeReason changeReason = PropertyChangeReason::Modified)
    {
      bool changed = false;
      if (value != m_value)
      {
        bool allowChanges = true;
        if (m_hInstance.IsValid())
        {
          allowChanges = rDependencyObject.PropertyChanged(m_hInstance, changeReason);
        }
        if (allowChanges)
        {
          changed = true;
          m_value = value;
        }
      }
      return changed;
    }

    DataBindingInstanceHandle GetInstanceHandleOnDemand(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject,
                                                        const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      if (!m_hInstance.IsValid())
      {
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      return m_hInstance;
    }

    DataBindingInstanceHandle GetInstanceHandleOnDemand(void* const pOwnerThis, ScopedDataSourceObject& rDependencyObject,
                                                        const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      if (!m_hInstance.IsValid())
      {
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      return m_hInstance;
    }

    bool SetBinding(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject, const DependencyPropertyDefinition& thisPropertyDef,
                    const Binding& binding)
    {
      if (!m_hInstance.IsValid())
      {
        if (!binding.HasValidSourceHandles())
        {
          // Nothing bound and nothing requested to be bound -> quick exit
          return false;
        }
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      // As the property is valid, we just need to request the final binding to be performed
      assert(m_hInstance.IsValid());
      return rDependencyObject.SetBinding(m_hInstance, binding);
    }

  private:
    static DataBindingInstanceHandle CreateProperty(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject,
                                                    const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      auto typedMethods =
        std::dynamic_pointer_cast<Internal::TypedReadOnlyDependencyPropertyMethodsDefinition<value_type>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      assert(typedMethods->GetValueType() == typeid(value_type));

      // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
      return rDependencyObject.CreateReadOnlyProperty(
        thisPropertyDef, std::make_unique<Internal::TypedReadOnlyDependencyPropertyMethods<value_type>>(pOwnerThis, typedMethods));
    }

    static DataBindingInstanceHandle CreateProperty(void* const pOwnerThis, ScopedDataSourceObject& rDependencyObject,
                                                    const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      auto typedMethods =
        std::dynamic_pointer_cast<Internal::TypedReadOnlyDependencyPropertyMethodsDefinition<value_type>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      assert(typedMethods->GetValueType() == typeid(value_type));

      // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
      return rDependencyObject.CreateReadOnlyProperty(
        thisPropertyDef, std::make_unique<Internal::TypedReadOnlyDependencyPropertyMethods<value_type>>(pOwnerThis, typedMethods));
    }
  };


  template <>
  class TypedReadOnlyDependencyProperty<StringViewLite> final
  {
  private:
    std::string m_value;
    mutable DataBindingInstanceHandle m_hInstance;

  public:
    TypedReadOnlyDependencyProperty() = default;

    explicit TypedReadOnlyDependencyProperty(const StringViewLite value)
      : m_value(value)
    {
    }

    std::type_index Type() const
    {
      return typeid(StringViewLite);
    }

    bool IsInstance(const DataBindingInstanceHandle hInstance) const noexcept
    {
      return hInstance == m_hInstance;
    }

    bool IsReadOnly(const ScopedDependencyObject& dependencyObject) const noexcept
    {
      return !m_hInstance.IsValid() ? false : dependencyObject.IsPropertyReadOnly(m_hInstance);
    }

    StringViewLite Get() const noexcept
    {
      return std::string_view(m_value);
    }


    bool Set(ScopedDependencyObject& rDependencyObject, const StringViewLite value,
             const PropertyChangeReason changeReason = PropertyChangeReason::Modified)
    {
      bool changed = false;
      if (value != m_value)
      {
        bool allowChanges = true;
        if (m_hInstance.IsValid())
        {
          allowChanges = rDependencyObject.PropertyChanged(m_hInstance, changeReason);
        }
        if (allowChanges)
        {
          changed = true;
          m_value = value;
        }
      }
      return changed;
    }

    bool Set(ScopedDependencyObject& rDependencyObject, const std::string& value,
             const PropertyChangeReason changeReason = PropertyChangeReason::Modified)
    {
      return Set(rDependencyObject, StringViewLite(value), changeReason);
    }

    bool Set(ScopedDependencyObject& rDependencyObject, std::string&& value, const PropertyChangeReason changeReason = PropertyChangeReason::Modified)
    {
      bool changed = false;
      if (value != m_value)
      {
        bool allowChanges = true;
        if (m_hInstance.IsValid())
        {
          allowChanges = rDependencyObject.PropertyChanged(m_hInstance, changeReason);
        }
        if (allowChanges)
        {
          changed = true;
          m_value = std::move(value);
        }
      }
      return changed;
    }

    DataBindingInstanceHandle GetInstanceHandleOnDemand(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject,
                                                        const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      if (!m_hInstance.IsValid())
      {
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      return m_hInstance;
    }

    DataBindingInstanceHandle GetInstanceHandleOnDemand(void* const pOwnerThis, ScopedDataSourceObject& rDependencyObject,
                                                        const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      if (!m_hInstance.IsValid())
      {
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      return m_hInstance;
    }

    bool SetBinding(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject, const DependencyPropertyDefinition& thisPropertyDef,
                    const Binding& binding)
    {
      if (!m_hInstance.IsValid())
      {
        if (!binding.HasValidSourceHandles())
        {
          // Nothing bound and nothing requested to be bound -> quick exit
          return false;
        }
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        m_hInstance = CreateProperty(pOwnerThis, rDependencyObject, thisPropertyDef);
      }
      // As the property is valid, we just need to request the final binding to be performed
      assert(m_hInstance.IsValid());
      return rDependencyObject.SetBinding(m_hInstance, binding);
    }

  private:
    static DataBindingInstanceHandle CreateProperty(void* const pOwnerThis, ScopedDependencyObject& rDependencyObject,
                                                    const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      auto typedMethods =
        std::dynamic_pointer_cast<Internal::TypedReadOnlyDependencyPropertyMethodsDefinition<StringViewLite>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      assert(typedMethods->GetValueType() == typeid(StringViewLite));

      // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
      return rDependencyObject.CreateReadOnlyProperty(
        thisPropertyDef, std::make_unique<Internal::TypedReadOnlyDependencyPropertyMethods<StringViewLite>>(pOwnerThis, typedMethods));
    }

    static DataBindingInstanceHandle CreateProperty(void* const pOwnerThis, ScopedDataSourceObject& rDependencyObject,
                                                    const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      auto typedMethods =
        std::dynamic_pointer_cast<Internal::TypedReadOnlyDependencyPropertyMethodsDefinition<StringViewLite>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      assert(typedMethods->GetValueType() == typeid(StringViewLite));

      // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
      return rDependencyObject.CreateReadOnlyProperty(
        thisPropertyDef, std::make_unique<Internal::TypedReadOnlyDependencyPropertyMethods<StringViewLite>>(pOwnerThis, typedMethods));
    }
  };

}

#endif
