#ifndef FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTDEPENDENCYOBJECT_HPP
#define FSLDATABINDING_UNITTEST_FSLDATABINDING_BASE_UNITTEST_UTDEPENDENCYOBJECT_HPP
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

#include <FslBase/Math/ConstrainedValue.hpp>
#include <FslDataBinding/Base/Object/DependencyObject.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>

namespace Fsl
{
  class UTDependencyObject : public DataBinding::DependencyObject
  {
    using prop0_type = uint32_t;
    using prop1_type = float;
    DataBinding::TypedDependencyProperty<prop0_type> m_property0;
    DataBinding::TypedDependencyProperty<prop1_type> m_property1;

    ConstrainedValue<uint32_t> m_constraints0;

  public:
    explicit UTDependencyObject(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding);

    ConstrainedValue<uint32_t> GetProperty0ValueConstraints() const;
    void SetProperty0ValueConstraints(ConstrainedValue<uint32_t> constraints);

    uint32_t GetProperty0Value() const noexcept
    {
      return m_property0.Get();
    }

    bool SetProperty0Value(const uint32_t value);

    float GetProperty1Value() const noexcept
    {
      return m_property1.Get();
    }

    bool SetProperty1Value(const float value)
    {
      return m_property1.Set(ThisDependencyObject(), value, DataBinding::PropertyChangeReason::Modified);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property0;
    // NOLINTNEXTLINE(readability-identifier-naming)
    static DataBinding::DependencyPropertyDefinition Property1;

  protected:
    bool DoSetProperty0Value(const uint32_t value, const DataBinding::PropertyChangeReason changeReason);
    DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
    DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                           const DataBinding::Binding& binding) override;
  };
}

#endif
