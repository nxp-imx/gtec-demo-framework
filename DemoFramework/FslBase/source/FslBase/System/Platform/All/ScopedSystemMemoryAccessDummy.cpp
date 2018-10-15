#ifndef __linux__
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

#include <FslBase/System/Platform/All/ScopedSystemMemoryAccessDummy.hpp>
#include <FslBase/Log/Log.hpp>

namespace Fsl
{
  ScopedSystemMemoryAccessDummy::ScopedSystemMemoryAccessDummy(const std::size_t targetAddress)
  //: m_fd(0)
  //, m_pMem(nullptr)
  //, m_pVirtAddress(nullptr)
  //, m_lastValue(0)
  {
    FSLLOG_WARNING("ScopedSystemMemoryAccess is a dummy implementation that does nothing");
  }

  ScopedSystemMemoryAccessDummy::~ScopedSystemMemoryAccessDummy() = default;

  uint8_t ScopedSystemMemoryAccessDummy::GetUInt8() const
  {
    return static_cast<uint8_t>(0);
  }

  uint16_t ScopedSystemMemoryAccessDummy::GetUInt16() const
  {
    return static_cast<uint16_t>(0);
  }

  uint32_t ScopedSystemMemoryAccessDummy::GetUInt32() const
  {
    return static_cast<uint32_t>(0);
  }

  void ScopedSystemMemoryAccessDummy::SetUInt8(const uint8_t value)
  {
  }

  void ScopedSystemMemoryAccessDummy::SetUInt16(const uint16_t value)
  {
  }

  void ScopedSystemMemoryAccessDummy::SetUInt32(const uint32_t value)
  {
  }
}
#endif
