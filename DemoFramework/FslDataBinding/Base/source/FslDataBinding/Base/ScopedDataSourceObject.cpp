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

#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Internal/IDependencyPropertyMethods.hpp>
#include <FslDataBinding/Base/ScopedDataSourceObject.hpp>
#include <utility>

namespace Fsl::DataBinding
{
  ScopedDataSourceObject::ScopedDataSourceObject(std::shared_ptr<DataBindingService> dataBinding)
    : ScopedDataSourceObject(std::move(dataBinding), DataSourceFlags::NoFlags)
  {
  }

  ScopedDataSourceObject::ScopedDataSourceObject(std::shared_ptr<DataBindingService> dataBinding, const DataSourceFlags flags)
    : m_dataBinding(std::move(dataBinding))
    , m_flags(flags)
  {
  }

  ScopedDataSourceObject::ScopedDataSourceObject(std::shared_ptr<DataBindingService> dataBinding, const DataSourceFlags flags,
                                                 const bool createInstanceNow)
    : m_dataBinding(std::move(dataBinding))
    , m_flags(flags)
  {
    if (createInstanceNow)
    {
      GetInstanceHandleOnDemand();
    }
  }

  ScopedDataSourceObject::~ScopedDataSourceObject() noexcept
  {
    if (m_hInstance.IsValid())
    {
      m_dataBinding->DestroyInstance(m_hInstance);
    }
  }


  DataBindingInstanceHandle ScopedDataSourceObject::GetInstanceHandleOnDemand()
  {
    if (!m_hInstance.IsValid())
    {
      // Create the dependency object on demand.
      m_hInstance = m_dataBinding->CreateDataSourceObject(m_flags);
    }
    return m_hInstance;
  }

  bool ScopedDataSourceObject::Changed()
  {
    bool changed = m_hInstance.IsValid() && DataSourceFlagsUtil::IsEnabled(m_flags, DataSourceFlags::Observable);
    if (changed)
    {
      changed = m_dataBinding->Changed(m_hInstance, PropertyChangeReason::Modified);
    }
    return changed;
  }


  DataBindingInstanceHandle ScopedDataSourceObject::CreateReadOnlyProperty(const DependencyPropertyDefinition& propertyDefinition,
                                                                           std::unique_ptr<DataBinding::Internal::IDependencyPropertyMethods> methods)
  {
    auto hInstance = GetInstanceHandleOnDemand();
    return m_dataBinding->CreateReadOnlyDependencyObjectProperty(hInstance, propertyDefinition, std::move(methods));
  }


  bool ScopedDataSourceObject::PropertyChanged(const DataBindingInstanceHandle hInstance, const PropertyChangeReason changeReason)
  {
    return m_dataBinding->Changed(hInstance, changeReason);
  }


  bool ScopedDataSourceObject::IsPropertyReadOnly(const DataBindingInstanceHandle hInstance) const noexcept
  {
    return m_dataBinding->IsPropertyReadOnly(hInstance);
  }


  bool ScopedDataSourceObject::DestroyProperty(const DataBindingInstanceHandle hInstance) noexcept
  {
    return m_dataBinding->DestroyProperty(hInstance);
  }
}
