#ifndef SHARED_ANTIALIASING_SUPPORTEDSAMPLECOUNTFLAGS_HPP
#define SHARED_ANTIALIASING_SUPPORTEDSAMPLECOUNTFLAGS_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  enum class SupportedSampleCountFlags : uint32_t
  {
    NoFlags = 0,
    Samples1 = 1,
    Samples2 = 2,
    Samples4 = 4,
    Samples8 = 8,
    Samples16 = 16,
    Samples32 = 32,
    Samples64 = 64
  };

  constexpr inline SupportedSampleCountFlags operator|(const SupportedSampleCountFlags lhs, const SupportedSampleCountFlags rhs) noexcept
  {
    return static_cast<SupportedSampleCountFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline SupportedSampleCountFlags operator&(const SupportedSampleCountFlags lhs, const SupportedSampleCountFlags rhs) noexcept
  {
    return static_cast<SupportedSampleCountFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }

  constexpr inline SupportedSampleCountFlags& operator|=(SupportedSampleCountFlags& rLhs, const SupportedSampleCountFlags rhs)
  {
    rLhs = rLhs | rhs;
    return rLhs;
  }

  constexpr inline SupportedSampleCountFlags& operator&=(SupportedSampleCountFlags& rLhs, const SupportedSampleCountFlags rhs)
  {
    rLhs = rLhs & rhs;
    return rLhs;
  }

  namespace SupportedSampleCountFlagsUtil
  {
    constexpr inline bool IsEnabled(const SupportedSampleCountFlags srcFlag, SupportedSampleCountFlags flag) noexcept
    {
      return (srcFlag & flag) == flag;
    }

    constexpr inline void Enable(SupportedSampleCountFlags& rDstFlag, SupportedSampleCountFlags flag) noexcept
    {
      rDstFlag = rDstFlag | flag;
    }


    constexpr inline void Disable(SupportedSampleCountFlags& rDstFlag, SupportedSampleCountFlags flag) noexcept
    {
      rDstFlag = rDstFlag & (static_cast<SupportedSampleCountFlags>(~static_cast<uint32_t>(flag)));
    }

    constexpr inline void Set(SupportedSampleCountFlags& rDstFlag, SupportedSampleCountFlags flag, const bool enabled) noexcept
    {
      rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (static_cast<SupportedSampleCountFlags>(~static_cast<uint32_t>(flag))));
    }
  }
}

#endif
