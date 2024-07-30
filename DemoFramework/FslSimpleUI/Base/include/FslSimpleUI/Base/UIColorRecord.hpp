#ifndef FSLSIMPLEUI_BASE_UICOLORRECORD_HPP
#define FSLSIMPLEUI_BASE_UICOLORRECORD_HPP
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

#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>

namespace Fsl::UI
{
  class UIColorRecord
  {
    UIColor m_externalColor;
    UIRenderColor m_internalColor;

  public:
    constexpr UIColorRecord() noexcept = default;

    UIColorRecord(const UIColorConverter converter, const UIColor value) noexcept
      : m_externalColor(value)
      , m_internalColor(converter.Convert(value))
    {
    }

    UIColor Get() const noexcept
    {
      return m_externalColor;
    }

    bool Set(const UIColorConverter converter, const UIColor value) noexcept
    {
      bool changed = value != m_externalColor;
      m_externalColor = value;
      m_internalColor = converter.Convert(value);
      return changed;
    }

    UIRenderColor GetInternalColor() const noexcept
    {
      return m_internalColor;
    }
  };
}

#endif
