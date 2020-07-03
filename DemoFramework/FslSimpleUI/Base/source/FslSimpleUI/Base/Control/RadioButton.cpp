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

#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/Control/Logic/RadioGroup.hpp>
#include <string>

namespace Fsl
{
  namespace UI
  {
    bool RadioButton::SetRadioGroup(const std::shared_ptr<RadioGroup>& radioGroup)
    {
      if (radioGroup != m_radioGroup)
      {
        if (m_radioGroup)
        {
          m_radioGroup->Remove(this);
        }
        if (radioGroup && !radioGroup->Contains(this))
        {
          throw std::invalid_argument("radio button must be a member of the group that is being set");
        }

        m_radioGroup = radioGroup;
        PropertyUpdated(PropertyType::Other);
        return true;
      }
      return false;
    }

    void RadioButton::SetIsChecked(const bool value)
    {
      if (ToggleButton::IsChecked() || !value)
      {
        // The radio button is already checked or trying to set a radiobutton to false
        // -> do nothing
        return;
      }

      // Force uncheck all other members of the group
      auto* group = m_radioGroup.get();
      if (group != nullptr)
      {
        const std::size_t count = group->Count();
        for (std::size_t i = 0; i < count; ++i)
        {
          const std::weak_ptr<RadioButton>& weakEntry = group->At(i);
          const auto entry = weakEntry.lock();
          if (entry.get() != this)
          {
            entry->ForceUnchecked();
          }
        }
      }
      ToggleButton::SetIsChecked(value);
    }


    void RadioButton::ForceUnchecked()
    {
      ToggleButton::SetIsChecked(false);
    }

  }
}
