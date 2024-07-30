#ifndef FSLDATABINDING_BASE_OBJECT_OBSERVABLEDATASOURCEOBJECT_HPP
#define FSLDATABINDING_BASE_OBJECT_OBSERVABLEDATASOURCEOBJECT_HPP
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

#include <FslDataBinding/Base/IObservableObject.hpp>
#include <FslDataBinding/Base/ScopedDataSourceObject.hpp>

namespace Fsl::DataBinding
{
  class DependencyPropertyDefinitionVector;

  class ObservableDataSourceObject : public DataBinding::IObservableObject
  {
    mutable DataBinding::ScopedDataSourceObject m_dataSourceObject;

  protected:
    const DataBinding::ScopedDataSourceObject& ThisDataSourceObject() const noexcept
    {
      return m_dataSourceObject;
    }

    DataBinding::ScopedDataSourceObject& ThisDataSourceObject() noexcept
    {
      return m_dataSourceObject;
    }

  public:
    explicit ObservableDataSourceObject(const std::shared_ptr<DataBinding::DataBindingService>& dataBinding);

    DataBinding::DataBindingInstanceHandle GetSourceInstanceHandle() const final;

    DataBindingInstanceHandle GetPropertyHandle(const DependencyPropertyDefinition& sourceDef);
    void ExtractProperties(DependencyPropertyDefinitionVector& rProperties);

  protected:
    bool MarkAsChangedNow();

    virtual DataBindingInstanceHandle TryGetPropertyHandleNow(const DependencyPropertyDefinition& sourceDef)
    {
      FSL_PARAM_NOT_USED(sourceDef);
      return {};
    }

    virtual void ExtractAllProperties(DependencyPropertyDefinitionVector& rProperties)
    {
      FSL_PARAM_NOT_USED(rProperties);
    }
  };
}

#endif
