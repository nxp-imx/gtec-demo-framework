#ifndef FSLBASE_SYSTEM_PLATFORM_QNX_SCOPEDSYSTEMMEMORYACCESSQNX_HPP
#define FSLBASE_SYSTEM_PLATFORM_QNX_SCOPEDSYSTEMMEMORYACCESSQNX_HPP
#ifdef __QNXNTO__
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Noncopyable.hpp>
#include <cstddef>

namespace Fsl
{
  class ScopedSystemMemoryAccessQNX : private Noncopyable
  {
    int m_fd;
    void* m_pMem;
    void* m_pVirtAddress;
    int m_lastValue;
  public:
    ScopedSystemMemoryAccessQNX(const std::size_t targetAddress);
    ~ScopedSystemMemoryAccessQNX();
    uint8_t GetUInt8() const;
    uint16_t GetUInt16() const;
    uint32_t GetUInt32() const;
    void SetUInt8(const uint8_t value);
    void SetUInt16(const uint16_t value);
    void SetUInt32(const uint32_t value);
  };
}

#endif
#endif
