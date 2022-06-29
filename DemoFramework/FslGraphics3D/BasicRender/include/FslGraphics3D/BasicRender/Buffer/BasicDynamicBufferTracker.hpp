#ifndef FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICDYNAMICBUFFERTRACKER_HPP
#define FSLGRAPHICS3D_BASICRENDER_BUFFER_BASICDYNAMICBUFFERTRACKER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslBase/Span/ReadOnlyFlexSpanUtil.hpp>
#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpan.hpp>
#include <FslGraphics3D/BasicRender/Buffer/ABasicBufferTracker.hpp>
#include <FslGraphics3D/BasicRender/Buffer/BasicDynamicBufferLink.hpp>
#include <memory>

namespace Fsl::Graphics3D
{
  class BasicDynamicBufferTracker final
    : public ABasicBufferTracker
    , public IBasicDynamicBuffer
  {
    std::shared_ptr<BasicDynamicBufferLink> m_link;
    BasicBufferType m_type;

  public:
    explicit BasicDynamicBufferTracker(std::shared_ptr<BasicDynamicBufferLink> link)
      : m_link(std::move(link))
    {
      if (!m_link)
      {
        throw std::invalid_argument("BasicDynamicBufferLink can not be null");
      }
      m_type = m_link->GetType();
    }

    ~BasicDynamicBufferTracker() final = default;


    void Dispose() noexcept
    {
      m_link.reset();
    }

    // IBasicDynamicBuffer
    BasicBufferType GetType() const noexcept final
    {
      return m_type;
    }

    BasicNativeBufferHandle TryGetNativeHandle() const noexcept final
    {
      const BasicDynamicBufferLink* const pLink = m_link.get();
      return pLink != nullptr ? pLink->TryGetNativeHandle() : BasicNativeBufferHandle();
    }

    uint32_t Capacity() const noexcept final
    {
      const BasicDynamicBufferLink* const pLink = m_link.get();
      return pLink != nullptr ? pLink->Capacity() : 0u;
    }

    void SetData(const ReadOnlySpan<uint16_t> indexSpan) final
    {
      BasicDynamicBufferLink* const pLink = m_link.get();
      if (pLink != nullptr)
      {
        pLink->SetData(ReadOnlyFlexSpanUtil::AsSpan(indexSpan));
      }
      else
      {
        throw UsageErrorException("SetData called on disposed object");
      }
    }

    void SetData(const ReadOnlyFlexVertexSpan& vertexSpan) final
    {
      BasicDynamicBufferLink* const pLink = m_link.get();
      if (pLink != nullptr)
      {
        pLink->SetData(ReadOnlyFlexSpan(vertexSpan.data(), vertexSpan.size(), vertexSpan.stride(), OptimizationCheckFlag::NoCheck));
      }
      else
      {
        throw UsageErrorException("SetData called on disposed object");
      }
    }


    void SetData(ReadOnlyFlexSpan bufferData) final
    {
      if (!m_link)
      {
        throw UsageErrorException("BasicDynamicBuffer has been marked as disposed");
      }
      m_link->SetData(bufferData);
    }
  };
}

#endif
