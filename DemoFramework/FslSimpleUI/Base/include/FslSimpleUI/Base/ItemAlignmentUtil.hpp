#ifndef FSLSIMPLEUI_BASE_ITEMALIGNMENTUTIL_HPP
#define FSLSIMPLEUI_BASE_ITEMALIGNMENTUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslSimpleUI/Base/ItemAlignment.hpp>

namespace Fsl
{
  namespace UI
  {
    namespace ItemAlignmentUtil
    {
      constexpr inline int32_t CalcAlignmentPx(const ItemAlignment alignment, const int32_t deltaPx) noexcept
      {
        switch (alignment)
        {
        case ItemAlignment::Center:
          // +1 or -1 before divide ensures that this is equal to doing a "round"
          return deltaPx >= 0 ? static_cast<int32_t>((static_cast<int64_t>(deltaPx) + 1) / 2)
                              : static_cast<int32_t>((static_cast<int64_t>(deltaPx) - 1) / 2);
        case ItemAlignment::Far:
          return deltaPx;
        case ItemAlignment::Near:
        default:
          return 0;
        }
      }


      constexpr inline int32_t CenterPx(const int32_t deltaPx) noexcept
      {
        // +1 or -1 before divide ensures that this is equal to doing a "round"
        return deltaPx >= 0 ? static_cast<int32_t>((static_cast<int64_t>(deltaPx) + 1) / 2)
                            : static_cast<int32_t>((static_cast<int64_t>(deltaPx) - 1) / 2);
      }


      constexpr inline uint16_t CenterPx(const uint16_t deltaPx) noexcept
      {
        // +1 or -1 before divide ensures that this is equal to doing a "round"
        return static_cast<uint16_t>((static_cast<int32_t>(deltaPx) + 1) / 2);
      }

    }
  }
}

#endif
