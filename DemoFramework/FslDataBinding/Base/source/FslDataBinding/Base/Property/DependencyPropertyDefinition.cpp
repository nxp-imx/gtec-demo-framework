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

#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinition.hpp>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl::DataBinding
{
  DependencyPropertyDefinition::DependencyPropertyDefinition(const uint64_t uniqueId, StringViewLite name, const std::type_index& type,
                                                             const std::type_index& ownerType,
                                                             std::shared_ptr<Internal::IPropertyMethodsDefinition> methods)
    : m_uniqueId(uniqueId)
    , m_name(StringViewLiteUtil::ToString(name))
    , m_type(type)
    , m_ownerType(ownerType)
    , m_methods(std::move(methods))
  {
    assert(m_methods);
    assert(type == m_methods->GetValueType());
    assert(ownerType == m_methods->GetTargetClassType());
  }


  StringViewLite DependencyPropertyDefinition::Name() const
  {
    return StringViewLiteUtil::AsStringViewLite(m_name);
  }


  DependencyPropertyDefinition DependencyPropertyDefinition::Create(StringViewLite name, const std::type_index& type,
                                                                    const std::type_index& ownerType,
                                                                    std::shared_ptr<Internal::IPropertyMethodsDefinition> methods)
  {
    if (name.empty())
    {
      throw std::invalid_argument("the name can not be empty");
    }
    if (!methods)
    {
      throw std::invalid_argument("methods can not be null");
    }
    if (type != methods->GetValueType())
    {
      throw std::invalid_argument("the supplied 'methods' do not match the supplied 'type'");
    }
    if (ownerType != methods->GetTargetClassType())
    {
      throw std::invalid_argument("the supplied 'methods' do not match the supplied 'ownerType'");
    }

    static uint64_t g_uniqueId{0};
    auto uniqueId = g_uniqueId;
    assert(g_uniqueId < std::numeric_limits<uint64_t>::max());
    ++g_uniqueId;
    return DependencyPropertyDefinition(uniqueId, name, type, ownerType, std::move(methods));
  }
}
