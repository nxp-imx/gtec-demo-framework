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

#include "UTDependencyObject.hpp"
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <limits>
#include <utility>


// FIX: consider if we need something like:
//      BindingOperations.SetBinding(myText, TextBlock.TextProperty, myBinding);


namespace Fsl
{
  using TClass = UTDependencyObject;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::Property0 = TFactory::Create<TClass::prop0_type, TClass, &TClass::GetProperty0Value, &TClass::SetProperty0Value>("Property0");
  TDef TClass::Property1 = TFactory::Create<TClass::prop1_type, TClass, &TClass::GetProperty1Value, &TClass::SetProperty1Value>("Property1");
}

namespace Fsl
{
  UTDependencyObject::UTDependencyObject(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
    : DataBinding::DependencyObject(dataBinding)
    , m_contraints0(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max())
  {
  }


  ConstrainedValue<uint32_t> UTDependencyObject::GetProperty0ValueConstaints() const
  {
    return m_contraints0;
  }


  void UTDependencyObject::SetProperty0ValueConstaints(ConstrainedValue<uint32_t> constaints)
  {
    if (constaints != m_contraints0)
    {
      m_contraints0 = constaints;
      SetProperty0Value(GetProperty0Value());
    }
  }


  bool UTDependencyObject::SetProperty0Value(const uint32_t value)
  {
    const uint32_t constrainedValue = std::clamp(value, m_contraints0.Min(), m_contraints0.Max());
    return m_property0.Set(ThisDependencyObject(), constrainedValue);
  }


  DataBinding::DataBindingInstanceHandle UTDependencyObject::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef, PropLinkRefs(Property0, m_property0),
                                                            PropLinkRefs(Property1, m_property1));
    return res.IsValid() ? res : DependencyObject::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult UTDependencyObject::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                             const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(Property0, m_property0),
                                                     PropLinkRefs(Property1, m_property1));
    return res != PropertySetBindingResult::NotFound ? res : DependencyObject::TrySetBindingNow(targetDef, binding);
  }

}
