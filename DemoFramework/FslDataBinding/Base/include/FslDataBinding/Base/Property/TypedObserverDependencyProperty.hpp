#ifndef FSLDATABINDING_BASE_PROPERTY_TYPEDOBSERVERDEPENDENCYPROPERTY_HPP
#define FSLDATABINDING_BASE_PROPERTY_TYPEDOBSERVERDEPENDENCYPROPERTY_HPP
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

#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingInstanceHandle.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/IObservableObject.hpp>
#include <FslDataBinding/Base/Internal/ObserverDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/TypedDependencyPropertyRefMethods.hpp>
#include <FslDataBinding/Base/Internal/TypedDependencyPropertyRefMethodsDefinition.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>
#include <cassert>
#include <memory>
#include <type_traits>
#include <typeindex>


namespace Fsl::DataBinding
{
  class Binding;

  template <typename T>
  class TypedObserverDependencyProperty final
  {
  public:
    using value_type = T;
    using element_type = typename value_type::element_type;

  private:
    value_type m_value{};
    mutable DataBindingInstanceHandle m_hInstance;
    mutable DataBindingInstanceHandle m_hObserverInstance;
    mutable DataBindingInstanceHandle m_cachedSourceHandle;

  public:
    TypedObserverDependencyProperty()
    {
      static_assert(std::is_same<T, std::shared_ptr<element_type>>::value, "T must be a std::shared_ptr");
      static_assert(std::is_base_of<IObservableObject, element_type>::value, "T must be derived from Fsl::DataBinding::IObservableObject");
    }

    std::type_index Type() const
    {
      return typeid(T);
    }

    bool IsInstance(const DataBindingInstanceHandle hInstance) const noexcept
    {
      return hInstance == m_hInstance;
    }

    bool IsObserverInstance(const DataBindingInstanceHandle hInstance) const noexcept
    {
      return hInstance == m_hObserverInstance;
    }

    bool IsChangeFromSource(const DataBindingInstanceHandle hInstance) const noexcept
    {
      return m_value && hInstance == m_cachedSourceHandle;
    }


    bool IsReadOnly(const ScopedDependencyObject& dependencyObject) const noexcept
    {
      return !m_hInstance.IsValid() ? false : dependencyObject.IsPropertyReadOnly(m_hInstance);
    }

    const value_type& Get() const noexcept
    {
      return m_value;
    }

    const element_type* TryGetElement() const noexcept
    {
      return m_value.get();
    }

    bool Set(void* const pOwnerThis, IObjectObserver* const pObserver, ScopedDependencyObject& rDependencyObject,
             const DependencyPropertyDefinition& thisPropertyDef, const value_type& value)
    {
      assert(pOwnerThis != nullptr);
      assert(pObserver != nullptr);
      bool changed = false;
      if (value != m_value)
      {
        if (!m_hInstance.IsValid())
        {
          GetInstanceHandleOnDemand(pOwnerThis, pObserver, rDependencyObject, thisPropertyDef);
          assert(m_hInstance.IsValid());
        }
        if (rDependencyObject.PropertyChanged(m_hInstance, PropertyChangeReason::Modified))
        {
          assert(m_hObserverInstance.IsValid());
          m_value = value;
          changed = true;

          if (m_value)
          {    // The new value is valid, so we request a new binding to observe it
            auto hCachedSourceInstance = m_value->GetSourceInstanceHandle();
            rDependencyObject.SetObserverBinding(m_hObserverInstance, hCachedSourceInstance);
            m_cachedSourceHandle = hCachedSourceInstance;
          }
          else
          {
            rDependencyObject.ClearBinding(m_hObserverInstance);
            m_cachedSourceHandle = {};
          }
        }
      }
      return changed;
    }

    DataBindingInstanceHandle GetInstanceHandleOnDemand(void* const pOwnerThis, IObjectObserver* const pObserver,
                                                        ScopedDependencyObject& rDependencyObject,
                                                        const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      assert(pObserver != nullptr);
      if (!m_hInstance.IsValid())
      {
        // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        CreateProperties(pOwnerThis, pObserver, rDependencyObject, thisPropertyDef);
        assert(m_hInstance.IsValid());
        assert(m_hObserverInstance.IsValid());
      }
      return m_hInstance;
    }

    bool SetBinding(void* const pOwnerThis, IObjectObserver* const pObserver, ScopedDependencyObject& rDependencyObject,
                    const DependencyPropertyDefinition& thisPropertyDef, const Binding& binding)
    {
      assert(pOwnerThis != nullptr);
      assert(pObserver != nullptr);
      if (!m_hInstance.IsValid())
      {
        if (!binding.HasValidSourceHandles())
        {
          // Nothing bound and nothing requested to be bound -> quick exit
          return false;
        }
        // Create the properties on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
        CreateProperties(pOwnerThis, pObserver, rDependencyObject, thisPropertyDef);
        assert(m_hInstance.IsValid());
        assert(m_hObserverInstance.IsValid());
      }
      // As the property is valid, we just need to request the final binding to be performed
      assert(m_hInstance.IsValid());
      return rDependencyObject.SetBinding(m_hInstance, binding);
    }

  private:
    void CreateProperties(void* const pOwnerThis, IObjectObserver* const pObserver, ScopedDependencyObject& rDependencyObject,
                          const DependencyPropertyDefinition& thisPropertyDef)
    {
      assert(pOwnerThis != nullptr);
      auto typedMethods = std::dynamic_pointer_cast<Internal::TypedDependencyPropertyRefMethodsDefinition<value_type>>(thisPropertyDef.GetMethods());
      if (!typedMethods)
      {
        throw DependencyPropertyDefinitionException(
          "the dependency property was not configured properly as the 'methods' is not of the expected type");
      }
      assert(typedMethods->GetValueType() == typeid(value_type));

      // Create the property on demand. (properties are linked to their dependency object and will be automatically destroyed with it)
      DataBindingInstanceHandle hInstance;
      DataBindingInstanceHandle hInstanceObserver;
      assert(!hInstance.IsValid());
      assert(!hInstanceObserver.IsValid());
      try
      {
        hInstance = rDependencyObject.CreateProperty(
          thisPropertyDef, std::make_unique<Internal::TypedDependencyPropertyRefMethods<value_type>>(pOwnerThis, typedMethods));

        hInstanceObserver = rDependencyObject.CreateObserverProperty(hInstance, thisPropertyDef,
                                                                     std::make_unique<Internal::ObserverDependencyPropertyMethods>(pObserver));
      }
      catch (const std::exception&)
      {
        if (hInstanceObserver.IsValid())
        {
          rDependencyObject.DestroyProperty(hInstanceObserver);
        }
        if (hInstance.IsValid())
        {
          rDependencyObject.DestroyProperty(m_hInstance);
        }
        throw;
      }

      // Finally if value is valid we bind to the observer

      DataBindingInstanceHandle hCachedSourceInstance;
      if (m_value)
      {    // The new value is valid, so we request a new binding to observe it
        hCachedSourceInstance = m_value->GetSourceInstanceHandle();
        rDependencyObject.SetObserverBinding(hInstanceObserver, hCachedSourceInstance);
      }
      m_hInstance = hInstance;
      m_hObserverInstance = hInstanceObserver;
      m_cachedSourceHandle = hCachedSourceInstance;
    }
  };
}

#endif
