/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslSimpleUI/Declarative/PrimitiveTypeRegistry.hpp>

namespace Fsl::UI::Declarative
{
  void PrimitiveTypeRegistry::Register(const PrimitiveTypeData& data)
  {
    if (m_typeToData.find(data.Info.Type) != m_typeToData.end())
    {
      throw std::invalid_argument("type already registered");
    }
    if (m_nameToType.find(data.Info.Name) != m_nameToType.end())
    {
      throw std::invalid_argument("type name already registered");
    }

    m_typeToData.emplace(data.Info.Type, data);
    try
    {
      m_nameToType.emplace(data.Info.Name, data.Info.Type);
    }
    catch (const std::exception&)
    {
      m_typeToData.erase(data.Info.Type);
      throw;
    }
  }

  void PrimitiveTypeRegistry::Register(std::span<const PrimitiveTypeData> dataSpan)
  {
    for (const auto& entry : dataSpan)
    {
      Register(entry);
    }
  }

  std::string_view PrimitiveTypeRegistry::TryGetName(const std::type_index& type) const
  {
    const auto itrFind = m_typeToData.find(type);
    if (itrFind != m_typeToData.end())
    {
      return itrFind->second.Info.Name;
    }
    return {};
  }


  std::vector<PrimitiveTypeData> PrimitiveTypeRegistry::GetAllTypes() const
  {
    std::vector<PrimitiveTypeData> types;
    types.reserve(m_typeToData.size());

    for (const auto& pair : m_typeToData)
    {
      types.push_back(pair.second);
    }
    return types;
  }
}
