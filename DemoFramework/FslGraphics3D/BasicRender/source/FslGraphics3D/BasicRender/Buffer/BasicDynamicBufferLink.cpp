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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeBufferFactory.hpp>
#include <FslGraphics3D/BasicRender/Buffer/BasicDynamicBufferLink.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <utility>

namespace Fsl::Graphics3D
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto LogType = Fsl::LogType::Verbose6;
    }
  }

  BasicDynamicBufferLink::BasicDynamicBufferLink(const uint32_t maxFramesInFlight, std::shared_ptr<INativeBufferFactory> factory,
                                                 const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity,
                                                 const bool setDataSupported)
    : m_factory(std::move(factory))
    , m_bufferType(bufferType)
    , m_buffers(std::max(maxFramesInFlight, 2u))
    , m_setDataSupported(setDataSupported)
  {
    FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink::Construct");
    if (!m_factory)
    {
      throw std::invalid_argument("factory can not be null");
    }
    if (bufferData.size() > bufferElementCapacity)
    {
      throw std::invalid_argument(
        fmt::format("Current buffer capacity of {} can not contain the requested buffer data of size {}", bufferElementCapacity, bufferData.size()));
    }

    m_buffers.front().NativeHandle = m_factory->CreateBuffer(bufferType, bufferData, bufferElementCapacity, setDataSupported);
    m_buffers.front().IsInUse = true;
    m_bufferElementCapacity = NumericCast<uint32_t>(bufferElementCapacity);
  }


  BasicDynamicBufferLink::~BasicDynamicBufferLink()
  {
    uint32_t useCount = 0;
    for (const Record& record : m_buffers)
    {
      if (record.NativeHandle.IsValid())
      {
        ++useCount;
      }
    }
    FSLLOG3_ERROR_IF(useCount > 0, "Found {} unfreed native buffers", useCount);
  }


  void BasicDynamicBufferLink::Destroy()
  {
    FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink::Destroy");

    for (Record& rRecord : m_buffers)
    {
      if (rRecord.NativeHandle.IsValid())
      {
        FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink::Destroyed");
        m_factory->DestroyBuffer(rRecord.NativeHandle);
        rRecord.NativeHandle = {};
      }
    }
    m_buffers.clear();
    m_bufferElementCapacity = 0u;
    m_activeIndex = 0u;
    m_isDestroyed = true;
  }


  void BasicDynamicBufferLink::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
  {
    switch (theEvent)
    {
    case BasicRenderSystemEvent::SwapchainLost:
      m_swapchainValid = false;
      break;
    case BasicRenderSystemEvent::SwapchainRecreated:
      m_swapchainValid = true;
      break;
    default:
      FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
      break;
    }
  }


  void BasicDynamicBufferLink::CollectGarbage()
  {
    for (std::size_t i = 0; i < m_buffers.size(); ++i)
    {
      auto& rEntry = m_buffers[i];
      if (rEntry.DeferredReuse)
      {
        if (rEntry.DeferCount <= 1u)
        {
          // Make the buffer available for reuse
          rEntry.IsInUse = false;
          rEntry.DeferredReuse = false;
          rEntry.DeferCount = 0u;
          FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink: InternalBuffer at #{} ready for reuse", i);
        }
        else
        {
          --rEntry.DeferCount;
        }
      }
    }
  }

  // two buffers
  // 1234
  // A
  // AB
  // ABC
  //  BCA
  //   CAB
  //    ABC


  void BasicDynamicBufferLink::SetData(ReadOnlyFlexSpan bufferData)
  {
    if (m_isDestroyed)
    {
      FSLLOG3_ERROR("Can not SetData on a destroyed buffer");
      return;
    }
    if (bufferData.size() > m_bufferElementCapacity)
    {
      throw UsageErrorException("bufferData can not exceed the capacity");
    }

    if (!m_swapchainValid)
    {
      if (m_activeIndex > m_buffers.size())
      {
        throw NotFoundException("Could not find a free buffer (SetData internal error)");
      }
      SetData(m_buffers[m_activeIndex], *m_factory, m_bufferType, bufferData, m_bufferElementCapacity, m_setDataSupported);
      // m_bufferElementCapacity = NumericCast<uint32_t>(bufferData.size());
      return;
    }

    // 1. locate a free buffer / create a free buffer
    auto itrFind = std::find_if(m_buffers.begin(), m_buffers.end(), [](const Record& entry) { return !entry.IsInUse; });
    if (itrFind == m_buffers.end())
    {
      throw NotFoundException("Could not find a free buffer (SetData internal error)");
    }

    assert(!itrFind->IsInUse);
    assert(!itrFind->DeferredReuse);
    assert(itrFind->DeferCount == 0u);

    // 2. set data
    SetData(*itrFind, *m_factory, m_bufferType, bufferData, m_bufferElementCapacity, m_setDataSupported);
    // m_bufferElementCapacity = NumericCast<uint32_t>(bufferData.size());

    // 3. tag current as 'deferred free'
    const auto deferCount = static_cast<uint32_t>(m_buffers.size());
    m_buffers[m_activeIndex].DeferredReuse = true;
    m_buffers[m_activeIndex].DeferCount = deferCount > 0u ? deferCount - 1u : 1u;

    // 4. mark new buffer as being active
    itrFind->IsInUse = true;
    m_activeIndex = static_cast<uint32_t>(std::distance(m_buffers.begin(), itrFind));

    FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink: InternalBuffer at #{} marked as active", m_activeIndex);
  }

  void BasicDynamicBufferLink::SetData(Record& rRecord, INativeBufferFactory& factory, const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData,
                                       const uint32_t bufferElementCapacity, const bool setDataSupported)
  {
    if (!rRecord.NativeHandle.IsValid())
    {
      // The buffer is not valid, so we need to allocate it
      rRecord.NativeHandle = factory.CreateBuffer(bufferType, bufferData, bufferElementCapacity, setDataSupported);
      FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink::SetData (create)");
    }
    else
    {
      // The buffer is valid, so we need to update it
      if (setDataSupported)
      {
        factory.SetBufferData(rRecord.NativeHandle, 0, bufferData);
        FSLLOG3(LocalConfig::LogType, "DynamicNativeBufferLink::SetData (optimized)");
      }
      else
      {
        // this is extremely inefficient, but SetData is not supported so its the only way
        factory.DestroyBuffer(rRecord.NativeHandle);
        rRecord.NativeHandle = factory.CreateBuffer(bufferType, bufferData, bufferElementCapacity, false);
        FSLLOG3(LocalConfig::LogType, "BasicDynamicBufferLink::SetData (destroy/create)");
      }
    }
  }
}
