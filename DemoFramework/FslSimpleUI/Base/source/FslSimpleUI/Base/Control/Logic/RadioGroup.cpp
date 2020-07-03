/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/Base/Control/Logic/RadioGroup.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      std::vector<std::weak_ptr<RadioButton>>::iterator Find(std::vector<std::weak_ptr<RadioButton>>& members,
                                                             const std::shared_ptr<RadioButton>& value)
      {
        auto itrFind = members.begin();
        while (itrFind != members.end())
        {
          auto entry = itrFind->lock();
          if (entry == value)
          {
            return itrFind;
          }
          ++itrFind;
        }
        return itrFind;
      }

      std::vector<std::weak_ptr<RadioButton>>::iterator Find(std::vector<std::weak_ptr<RadioButton>>& members, const RadioButton* const value)
      {
        if (value == nullptr)
        {
          return members.end();
        }

        auto itrFind = members.begin();
        while (itrFind != members.end())
        {
          auto entry = itrFind->lock();
          if (entry.get() == value)
          {
            return itrFind;
          }
          ++itrFind;
        }
        return itrFind;
      }

      std::vector<std::weak_ptr<RadioButton>>::const_iterator Find(const std::vector<std::weak_ptr<RadioButton>>& members,
                                                                   const RadioButton* const value)
      {
        if (value == nullptr)
        {
          return members.end();
        }

        auto itrFind = members.begin();
        while (itrFind != members.end())
        {
          auto entry = itrFind->lock();
          if (entry.get() == value)
          {
            return itrFind;
          }
          ++itrFind;
        }
        return itrFind;
      }
    }


    RadioGroup::RadioGroup(const StringViewLite& name)
      : m_name(StringViewLiteUtil::ToString(name))
    {
    }

    StringViewLite RadioGroup::GetName() const
    {
      return StringViewLiteUtil::AsStringViewLite(m_name);
    }

    void RadioGroup::Add(const std::shared_ptr<RadioButton>& entry)
    {
      if (!entry)
      {
        throw std::invalid_argument("Can not add a null entry");
      }
      auto itrFind = Find(m_members, entry);
      if (itrFind != m_members.end())
      {
        throw std::invalid_argument("entry already added");
      }
      m_members.push_back(entry);
    }


    bool RadioGroup::Remove(const std::shared_ptr<RadioButton>& entry)
    {
      auto itrFind = Find(m_members, entry);
      if (itrFind == m_members.end())
      {
        return false;
      }
      m_members.erase(itrFind);
      return true;
    }

    bool RadioGroup::Contains(const RadioButton* const pButton) const
    {
      return Find(m_members, pButton) != m_members.end();
    }

    bool RadioGroup::Remove(const RadioButton* const pButton)
    {
      auto itrFind = Find(m_members, pButton);
      if (itrFind == m_members.end())
      {
        return false;
      }
      m_members.erase(itrFind);
      return true;
    }

  }
}
