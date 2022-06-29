#ifndef FSLDATABINDING_BASE_OBJECT_DEPENDENCYOBJECT_HPP
#define FSLDATABINDING_BASE_OBJECT_DEPENDENCYOBJECT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDataBinding/Base/Property/PropertySetBindingResult.hpp>
#include <FslDataBinding/Base/ScopedDependencyObject.hpp>
#include <memory>

namespace Fsl::DataBinding
{
  class Binding;
  class DependencyPropertyDefinition;
  class DependencyPropertyDefinitionVector;
  class ISingleBinding;

  class DependencyObject
  {
    ScopedDependencyObject m_dependencyObject;

  protected:
    inline const ScopedDependencyObject& ThisDependencyObject() const noexcept
    {
      return m_dependencyObject;
    }

    inline ScopedDependencyObject& ThisDependencyObject() noexcept
    {
      return m_dependencyObject;
    }

  public:
    DependencyObject(const DependencyObject&) = delete;
    DependencyObject& operator=(const DependencyObject&) = delete;

    explicit DependencyObject(std::shared_ptr<DataBindingService> dataBinding);
    virtual ~DependencyObject() = default;

    DataBindingInstanceHandle GetPropertyHandle(const DependencyPropertyDefinition& sourceDef);
    bool ClearBinding(const DependencyPropertyDefinition& targetDef);

    bool SetBinding(const DependencyPropertyDefinition& targetDef, const DataBindingInstanceHandle hSource);
    bool SetBinding(const DependencyPropertyDefinition& targetDef, const Binding& binding);
    PropertySetBindingResult TrySetBinding(const DependencyPropertyDefinition& targetDef, const Binding& binding);

    void ExtractProperties(DependencyPropertyDefinitionVector& rProperties);

  protected:
    virtual DataBindingInstanceHandle TryGetPropertyHandleNow(const DependencyPropertyDefinition& sourceDef)
    {
      FSL_PARAM_NOT_USED(sourceDef);
      return {};
    }

    virtual PropertySetBindingResult TrySetBindingNow(const DependencyPropertyDefinition& targetDef, const Binding& binding)
    {
      FSL_PARAM_NOT_USED(targetDef);
      FSL_PARAM_NOT_USED(binding);
      return PropertySetBindingResult::NotFound;
    }

    virtual void ExtractAllProperties(DependencyPropertyDefinitionVector& rProperties)
    {
      FSL_PARAM_NOT_USED(rProperties);
    }
  };
}

#endif
