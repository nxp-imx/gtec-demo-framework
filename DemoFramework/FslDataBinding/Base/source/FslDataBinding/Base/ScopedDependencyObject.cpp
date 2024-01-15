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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Internal/IDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/Internal/ObserverDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>
#include <utility>

namespace Fsl::DataBinding
{
  ScopedDependencyObject::ScopedDependencyObject(std::shared_ptr<DataBindingService> dataBinding)
    : m_dataBinding(std::move(dataBinding))
  {
  }

  ScopedDependencyObject::ScopedDependencyObject(std::shared_ptr<DataBindingService> dataBinding, const bool createInstanceNow)
    : m_dataBinding(std::move(dataBinding))
  {
    if (createInstanceNow)
    {
      GetInstanceHandleOnDemand();
    }
  }

  ScopedDependencyObject::~ScopedDependencyObject() noexcept
  {
    if (m_hInstance.IsValid())
    {
      m_dataBinding->DestroyInstance(m_hInstance);
    }
  }


  DataBindingInstanceHandle ScopedDependencyObject::GetInstanceHandleOnDemand()
  {
    if (!m_hInstance.IsValid())
    {
      // Create the dependency object on demand.
      m_hInstance = m_dataBinding->CreateDependencyObject();
    }
    return m_hInstance;
  }


  DataBindingInstanceHandle ScopedDependencyObject::CreateProperty(const DependencyPropertyDefinition& propertyDefinition,
                                                                   std::unique_ptr<DataBinding::Internal::IDependencyPropertyMethods> methods)
  {
    auto hInstance = GetInstanceHandleOnDemand();
    return m_dataBinding->CreateDependencyObjectProperty(hInstance, propertyDefinition, std::move(methods));
  }


  DataBindingInstanceHandle ScopedDependencyObject::CreateObserverProperty(const DataBindingInstanceHandle hOwner,
                                                                           const DependencyPropertyDefinition& propertyDefinition,
                                                                           std::unique_ptr<Internal::ObserverDependencyPropertyMethods> methods)
  {
    return m_dataBinding->CreateDependencyObjectObserverProperty(hOwner, propertyDefinition, std::move(methods));
  }


  bool ScopedDependencyObject::ClearBinding(const DataBindingInstanceHandle hTarget)
  {
    return m_dataBinding->ClearBinding(hTarget);
  }


  bool ScopedDependencyObject::SetBinding(const DataBindingInstanceHandle hTarget, const Binding& binding)
  {
    return m_dataBinding->SetBinding(hTarget, binding);
  }


  bool ScopedDependencyObject::SetObserverBinding(const DataBindingInstanceHandle hTarget, const DataBindingInstanceHandle hSource)
  {
    return m_dataBinding->SetBinding(hTarget, Binding(hSource));
  }

  bool ScopedDependencyObject::PropertyChanged(const DataBindingInstanceHandle hInstance, const PropertyChangeReason changeReason)
  {
    return m_dataBinding->Changed(hInstance, changeReason);
  }


  bool ScopedDependencyObject::IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept
  {
    return m_dataBinding->IsPropertyReadOnly(hInstance);
  }


  bool ScopedDependencyObject::DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept
  {
    return m_dataBinding->DestroyProperty(hInstance);
  }
}
