/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/Base/Service/Keyboard/KeyboardState.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    int IndexOf(const VirtualKey::Enum*const pKeys, const int32_t entries, const VirtualKey::Enum key)
    {
      assert(pKeys != nullptr);
      assert(entries >= 0);

      for (int i = 0; i < entries; ++i)
      {
        if (pKeys[i] == key)
          return i;
      }
      return -1;
    }

  }

  KeyboardState::KeyboardState(const VirtualKey::Enum*const pKeys, const int entries)
    : m_entries(0)
  {
    if (pKeys == nullptr)
      throw std::invalid_argument("pKeys can not be null");
    if (entries < 0 || entries >= 256)
      throw std::invalid_argument("invalid entries count");

    for (int i = 0; i < entries; ++i)
      m_keys[i] = pKeys[i];
    m_entries = entries;
  }


  bool KeyboardState::IsKeyUp(const VirtualKey::Enum key) const
  {
    // If we don't find the key its up
    return IndexOf(m_keys, m_entries, key) < 0;
  }


  bool KeyboardState::IsKeyDown(const VirtualKey::Enum key) const
  {
    // If we find the key its down
    return IndexOf(m_keys, m_entries, key) >= 0;
  }

  //bool KeyboardState::GetPressedKeys() const;
}
