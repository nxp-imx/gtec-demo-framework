#ifndef FSLDATABINDING_BASE_PROPERTY_DEPENDENCYPROPERTYDEFINITION_HPP
#define FSLDATABINDING_BASE_PROPERTY_DEPENDENCYPROPERTYDEFINITION_HPP
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

#include <FslBase/String/StringViewLite.hpp>
#include <FslDataBinding/Base/Internal/IPropertyMethodsDefinition.hpp>
#include <memory>
#include <string>
#include <typeindex>

namespace Fsl::DataBinding
{
  //! Intended to be static member variables only!!
  class DependencyPropertyDefinition final
  {
    uint64_t m_uniqueId{0};
    std::string m_name;
    std::type_index m_type;
    std::type_index m_ownerType;
    std::shared_ptr<Internal::IPropertyMethodsDefinition> m_methods;

    explicit DependencyPropertyDefinition(const uint64_t uniqueId, StringViewLite name, const std::type_index& type, const std::type_index& ownerType,
                                          std::shared_ptr<Internal::IPropertyMethodsDefinition> methods);

  public:
    static DependencyPropertyDefinition Create(StringViewLite name, const std::type_index& type, const std::type_index& ownerType,
                                               std::shared_ptr<Internal::IPropertyMethodsDefinition> methods);

    StringViewLite Name() const;

    std::type_index Type() const
    {
      return m_type;
    }

    std::type_index OwnerType() const
    {
      return m_ownerType;
    }

    std::shared_ptr<Internal::IPropertyMethodsDefinition> GetMethods() const
    {
      // If this returns null there would be a internal error as we don't allow the object to be constructed with a nullptr
      return m_methods;
    }


    bool operator==(const DependencyPropertyDefinition& rhs) const noexcept
    {
      return m_uniqueId == rhs.m_uniqueId;
    }

    bool operator!=(const DependencyPropertyDefinition& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
