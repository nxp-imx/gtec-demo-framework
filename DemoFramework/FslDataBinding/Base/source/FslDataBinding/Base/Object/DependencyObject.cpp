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

#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <FslDataBinding/Base/Object/DependencyObject.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <fmt/format.h>
#include <utility>

namespace Fsl::DataBinding
{
  DependencyObject::DependencyObject(std::shared_ptr<DataBindingService> dataBinding)
    : m_dependencyObject(std::move(dataBinding))
  {
  }


  DataBindingInstanceHandle DependencyObject::GetPropertyHandle(const DependencyPropertyDefinition& sourceDef)
  {
    auto res = TryGetPropertyHandleNow(sourceDef);
    if (!res.IsValid())
    {
      throw DataBinding::UnknownPropertyInstanceException(fmt::format("Unknown property instance {}", sourceDef.Name()));
    }
    return res;
  }


  bool DependencyObject::ClearBinding(const DependencyPropertyDefinition& targetDef)
  {
    return SetBinding(targetDef, Binding(DataBindingInstanceHandle()));
  }


  bool DependencyObject::SetBinding(const DependencyPropertyDefinition& targetDef, const DataBindingInstanceHandle hSource)
  {
    return SetBinding(targetDef, Binding(hSource));
  }

  bool DependencyObject::SetBinding(const DependencyPropertyDefinition& targetDef, const Binding& binding)
  {
    auto res = TrySetBindingNow(targetDef, binding);
    switch (res)
    {
    case PropertySetBindingResult::Unchanged:
      return false;
    case PropertySetBindingResult::Changed:
      return true;
    default:
      throw DataBinding::UnknownPropertyInstanceException(fmt::format("Unknown property instance {}", targetDef.Name()));
    }
  }

  PropertySetBindingResult DependencyObject::TrySetBinding(const DependencyPropertyDefinition& targetDef, const Binding& binding)
  {
    return TrySetBindingNow(targetDef, binding);
  }


  void DependencyObject::ExtractProperties(DependencyPropertyDefinitionVector& rProperties)
  {
    return ExtractAllProperties(rProperties);
  }

}
