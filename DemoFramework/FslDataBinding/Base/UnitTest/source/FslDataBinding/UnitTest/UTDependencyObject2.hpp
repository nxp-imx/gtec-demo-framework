#ifndef FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTDEPENDENCYOBJECT2_HPP
#define FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTDEPENDENCYOBJECT2_HPP
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

#include <FslDataBinding/Base/IObservableObject.hpp>
#include <FslDataBinding/Base/Property/TypedObserverDependencyProperty.hpp>
#include <memory>
#include "UTDependencyObject.hpp"

namespace Fsl
{
  class UTDependencyObject2
    : public UTDependencyObject
    , public DataBinding::IObjectObserver
  {
    using prop2_type = uint32_t;
    using prop3_type = float;
    using prop4_type = int32_t;
    using prop5_type = std::shared_ptr<DataBinding::IObservableObject>;
    using prop6_type = std::shared_ptr<DataBinding::IObservableObject>;
    using prop7_type = uint32_t;
    DataBinding::TypedDependencyProperty<prop2_type> m_property2;
    DataBinding::TypedDependencyProperty<prop3_type> m_property3;
    DataBinding::TypedDependencyProperty<prop4_type> m_property4;
    DataBinding::TypedObserverDependencyProperty<prop5_type> m_property5;
    DataBinding::TypedObserverDependencyProperty<prop6_type> m_property6;
    DataBinding::TypedDependencyProperty<prop7_type> m_property7;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property2;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property3;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property4;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property5;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property6;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property7;

    uint32_t TestProperty5ChangedCallCount{0};
    uint32_t TestProperty6ChangedCallCount{0};

    explicit UTDependencyObject2(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
      : UTDependencyObject(dataBinding)
    {
    }

    uint32_t GetProperty2Value() const noexcept
    {
      return m_property2.Get();
    }

    bool SetProperty2Value(const uint32_t value)
    {
      return m_property2.Set(ThisDependencyObject(), value, DataBinding::PropertyChangeReason::Modified);
    }

    float GetProperty3Value() const noexcept
    {
      return m_property3.Get();
    }

    bool SetProperty3Value(const float value)
    {
      return m_property3.Set(ThisDependencyObject(), value, DataBinding::PropertyChangeReason::Modified);
    }

    int32_t GetProperty4Value() const noexcept
    {
      return m_property4.Get();
    }

    bool SetProperty4Value(const int32_t value)
    {
      return m_property4.Set(ThisDependencyObject(), value, DataBinding::PropertyChangeReason::Modified);
    }

    const prop5_type& GetProperty5Value() const noexcept
    {
      return m_property5.Get();
    }

    bool SetProperty5Value(const prop5_type& value)
    {
      return m_property5.Set(this, this, ThisDependencyObject(), Property5, value);
    }

    const prop6_type& GetProperty6Value() const noexcept
    {
      return m_property6.Get();
    }

    bool SetProperty6Value(const prop6_type& value)
    {
      return m_property6.Set(this, this, ThisDependencyObject(), Property6, value);
    }

    uint32_t GetProperty7Value() const noexcept
    {
      return m_property7.Get();
    }

    bool SetProperty7Value(const uint32_t value)
    {
      return m_property7.Set(ThisDependencyObject(), value, DataBinding::PropertyChangeReason::Modified);
    }


    void OnChanged(const DataBinding::DataBindingInstanceHandle hInstance) override;

  protected:
    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
  };
}

#endif
