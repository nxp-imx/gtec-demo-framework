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

#include "UTDependencyObject2.hpp"
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <fmt/format.h>

namespace Fsl
{
  using TClass = UTDependencyObject2;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::Property2 = TFactory::Create<TClass::prop2_type, TClass, &TClass::GetProperty2Value, &TClass::SetProperty2Value>("Property2");
  TDef TClass::Property3 = TFactory::Create<TClass::prop3_type, TClass, &TClass::GetProperty3Value, &TClass::SetProperty3Value>("Property3");
  TDef TClass::Property4 = TFactory::Create<TClass::prop4_type, TClass, &TClass::GetProperty4Value, &TClass::SetProperty4Value>("Property4");
  TDef TClass::Property5 = TFactory::Create<TClass::prop5_type, TClass, &TClass::GetProperty5Value, &TClass::SetProperty5Value>("Property5");
  TDef TClass::Property6 = TFactory::Create<TClass::prop6_type, TClass, &TClass::GetProperty6Value, &TClass::SetProperty6Value>("Property6");
  TDef TClass::Property7 = TFactory::Create<TClass::prop7_type, TClass, &TClass::GetProperty7Value, &TClass::SetProperty7Value>("Property7");
}

namespace Fsl
{
  void UTDependencyObject2::OnChanged(const DataBinding::DataBindingInstanceHandle hInstance)
  {
    if (m_property5.IsChangeFromSource(hInstance))
    {
      ++TestProperty5ChangedCallCount;
    }
    else if (m_property6.IsChangeFromSource(hInstance))
    {
      ++TestProperty6ChangedCallCount;
      SetProperty7Value(TestProperty6ChangedCallCount);
    }
  }


  DataBinding::DataBindingInstanceHandle UTDependencyObject2::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef, PropLinkRefs(Property2, m_property2),
                                                            PropLinkRefs(Property3, m_property3), PropLinkRefs(Property4, m_property4),
                                                            PropLinkRefs(Property5, m_property5), PropLinkRefs(Property6, m_property6),
                                                            PropLinkRefs(Property7, m_property7));
    return res.IsValid() ? res : UTDependencyObject::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult UTDependencyObject2::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                              const DataBinding::Binding& binding)
  {
    using namespace DataBinding;
    auto res = DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding, PropLinkRefs(Property2, m_property2),
                                                     PropLinkRefs(Property3, m_property3), PropLinkRefs(Property4, m_property4),
                                                     PropLinkRefs(Property5, m_property5), PropLinkRefs(Property6, m_property6),
                                                     PropLinkRefs(Property7, m_property7));
    return res != PropertySetBindingResult::NotFound ? res : UTDependencyObject::TrySetBindingNow(targetDef, binding);
  }
}
