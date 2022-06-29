#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVESHADERCOMBOKEY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVESHADERCOMBOKEY_HPP
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

#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>

namespace Fsl::GLES2
{
  struct NativeShaderComboKey
  {
    uint64_t Value{0};

    constexpr NativeShaderComboKey() noexcept
      : NativeShaderComboKey(BasicNativeShaderHandle::Invalid(), BasicNativeShaderHandle::Invalid())
    {
    }

    constexpr NativeShaderComboKey(BasicNativeShaderHandle key0, BasicNativeShaderHandle key1) noexcept
      : Value(((static_cast<uint64_t>(key0.Value) & 0xFFFFFFFF) << 32) | (static_cast<uint64_t>(key1.Value) & 0xFFFFFFFF))
    {
      static_assert(sizeof(BasicNativeShaderHandle::value_type) <= 4, "value does not fit");
    }

    constexpr bool operator==(const NativeShaderComboKey& rhs) const noexcept
    {
      return Value == rhs.Value;
    }

    constexpr bool operator!=(const NativeShaderComboKey& rhs) const noexcept
    {
      return !(*this == rhs);
    }

    constexpr bool operator<(const NativeShaderComboKey& rhs) const noexcept
    {
      return Value < rhs.Value;
    }

    constexpr bool operator<=(const NativeShaderComboKey& rhs) const noexcept
    {
      return Value <= rhs.Value;
    }

    constexpr bool operator>=(const NativeShaderComboKey& rhs) const noexcept
    {
      return Value >= rhs.Value;
    }

    constexpr bool operator>(const NativeShaderComboKey& rhs) const noexcept
    {
      return Value > rhs.Value;
    }
  };
}

#endif
