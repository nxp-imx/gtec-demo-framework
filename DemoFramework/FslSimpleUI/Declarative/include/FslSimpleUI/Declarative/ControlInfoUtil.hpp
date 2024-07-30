#ifndef FSLSIMPLEUI_DECLARATIVE_CONTROLINFOUTIL_HPP
#define FSLSIMPLEUI_DECLARATIVE_CONTROLINFOUTIL_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Control/ContentControl.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>

namespace Fsl::UI::Declarative::ControlInfoUtil
{
  inline bool IsContainer(UI::BaseWindow* const pWindow)
  {
    // Really crude, a better solution would get enough meta data from the factory

    // Check known containers
    assert(pWindow != nullptr);
    {
      auto* const pContainer = dynamic_cast<UI::Layout*>(pWindow);
      if (pContainer != nullptr)
      {
        return true;
      }
    }
    return false;
  }

  inline bool IsContentControl(UI::BaseWindow* const pWindow)
  {
    // Really crude, a better solution would get enough meta data from the factory

    // Check known containers
    assert(pWindow != nullptr);
    {
      auto* const pContentControl = dynamic_cast<UI::ContentControl*>(pWindow);
      if (pContentControl != nullptr)
      {
        return true;
      }
    }
    return false;
  }
}

#endif
