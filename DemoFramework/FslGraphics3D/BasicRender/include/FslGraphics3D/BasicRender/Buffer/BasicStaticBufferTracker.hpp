#ifndef FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICSTATICBUFFERTRACKER_HPP
#define FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICSTATICBUFFERTRACKER_HPP
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

#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics/Render/Basic/IBasicStaticBuffer.hpp>
#include <FslGraphics3D/BasicRender/Buffer/ABasicBufferTracker.hpp>
#include <stdexcept>

namespace Fsl
{
  namespace Graphics3D
  {
    class BasicStaticBufferTracker final
      : public ABasicBufferTracker
      , public IBasicStaticBuffer
    {
      BasicBufferType m_type;
      BasicNativeBufferHandle m_nativeHandle;
      uint32_t m_capacity;

    public:
      explicit BasicStaticBufferTracker(const BasicBufferType type, const BasicNativeBufferHandle nativeHandle, const uint32_t capacity)
        : m_type(type)
        , m_nativeHandle(nativeHandle)
        , m_capacity(capacity)
      {
        if (!nativeHandle.IsValid())
        {
          throw std::invalid_argument("BasicStaticBufferLink can not be null");
        }
      }

      ~BasicStaticBufferTracker() final = default;


      void Dispose() noexcept
      {
        m_nativeHandle = {};
      }

      // IBasicStaticBuffer
      BasicBufferType GetType() const noexcept final
      {
        return m_type;
      }

      uint32_t Capacity() const noexcept final
      {
        return m_capacity;
      }

      BasicNativeBufferHandle TryGetNativeHandle() const noexcept final
      {
        return m_nativeHandle;
      }
    };
  }
}

#endif
