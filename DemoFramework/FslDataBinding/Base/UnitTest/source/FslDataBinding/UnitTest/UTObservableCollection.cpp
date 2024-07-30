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

#include "UTObservableCollection.hpp"
#include <FslDataBinding/Base/Object/ObservableDataSourceObjectHelper.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>

namespace Fsl
{
  using TClass = UTObservableCollection;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::Property0 = TFactory::CreateReadOnly<TClass::prop0_type, TClass, &TClass::GetProperty0Value>("Property0");
  TDef TClass::Property1 = TFactory::CreateReadOnly<TClass::prop1_type, TClass, &TClass::GetProperty1Value>("Property1");
}

namespace Fsl
{
  UTObservableCollection::UTObservableCollection(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding)
    : DataBinding::ObservableDataSourceObject(dataBinding)
  {
  }

  bool UTObservableCollection::MarkAsChanged()
  {
    return MarkAsChangedNow();
  }

  bool UTObservableCollection::SetProperty0Value(const uint32_t value)
  {
    return m_property0.Set(ThisDataSourceObject(), value, DataBinding::PropertyChangeReason::Modified);
  }


  bool UTObservableCollection::SetProperty1Value(const float value)
  {
    return m_property1.Set(ThisDataSourceObject(), value, DataBinding::PropertyChangeReason::Modified);
  }

  DataBinding::DataBindingInstanceHandle UTObservableCollection::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    using namespace DataBinding;
    auto res = ObservableDataSourceObjectHelper::TryGetPropertyHandle(this, ThisDataSourceObject(), sourceDef, PropLinkRefs(Property0, m_property0),
                                                                      PropLinkRefs(Property1, m_property1));
    return res.IsValid() ? res : ObservableDataSourceObject::TryGetPropertyHandleNow(sourceDef);
  }

}
