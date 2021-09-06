#ifndef FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICDYNAMICBUFFERLINK_HPP
#define FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICDYNAMICBUFFERLINK_HPP
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

#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics/Render/Basic/BasicBufferType.hpp>
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace Graphics3D
  {
    class INativeBufferFactory;

    class BasicDynamicBufferLink final
    {
      struct Record
      {
        BasicNativeBufferHandle NativeHandle;
        uint32_t DeferCount{0};
        bool IsInUse{false};
        bool DeferredReuse{false};
      };

      std::shared_ptr<INativeBufferFactory> m_factory;
      BasicBufferType m_bufferType;
      std::vector<Record> m_buffers;
      uint32_t m_activeIndex{0};
      bool m_setDataSupported{false};
      bool m_swapchainValid{true};
      bool m_isDestroyed{false};
      uint32_t m_bufferElementCapacity;

    public:
      BasicDynamicBufferLink(const uint32_t maxFramesInFlight, std::shared_ptr<INativeBufferFactory> factory, const BasicBufferType bufferType,
                             ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity, const bool setDataSupported);
      ~BasicDynamicBufferLink();
      void Destroy();

      void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent);

      void CollectGarbage();


      void SetData(ReadOnlyFlexSpan bufferData);


      //! @brief Get the content type
      BasicBufferType GetType() const noexcept
      {
        return m_bufferType;
      }

      uint32_t Capacity() const noexcept
      {
        return m_bufferElementCapacity;
      }

      //! @brief Try to get the native texture
      BasicNativeBufferHandle TryGetNativeHandle() const noexcept
      {
        return m_activeIndex < m_buffers.size() ? m_buffers[m_activeIndex].NativeHandle : BasicNativeBufferHandle::Invalid();
      }

    private:
      static void SetData(Record& rRecord, INativeBufferFactory& factory, const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData,
                          const uint32_t bufferElementCapacity, const bool setDataSupported);
    };
  }
}

#endif
