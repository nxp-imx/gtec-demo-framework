#ifndef FSLDATABINDING_BASE_DATASOURCEFLAGS_HPP
#define FSLDATABINDING_BASE_DATASOURCEFLAGS_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl::DataBinding
{
  enum class DataSourceFlags : uint32_t
  {
    NoFlags = 0,
    Observable = 0x01,
  };

  constexpr inline DataSourceFlags operator|(const DataSourceFlags lhs, const DataSourceFlags rhs) noexcept
  {
    return static_cast<DataSourceFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline DataSourceFlags operator&(const DataSourceFlags lhs, const DataSourceFlags rhs) noexcept
  {
    return static_cast<DataSourceFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }

  namespace DataSourceFlagsUtil
  {
    constexpr inline bool IsEnabled(const DataSourceFlags srcFlag, DataSourceFlags flag) noexcept
    {
      return (srcFlag & flag) == flag;
    }

    constexpr inline void Enable(DataSourceFlags& rDstFlag, DataSourceFlags flag) noexcept
    {
      rDstFlag = rDstFlag | flag;
    }


    constexpr inline void Disable(DataSourceFlags& rDstFlag, DataSourceFlags flag) noexcept
    {
      rDstFlag = rDstFlag & (static_cast<DataSourceFlags>(~static_cast<uint32_t>(flag)));
    }

    constexpr inline void Set(DataSourceFlags& rDstFlag, DataSourceFlags flag, const bool enabled) noexcept
    {
      rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (static_cast<DataSourceFlags>(~static_cast<uint32_t>(flag))));
    }
  }
}

#endif
