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

#include <FslGraphics3D/BasicRender/Buffer/BasicBufferManager.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeBufferFactory.hpp>
#include <FslGraphics3D/BasicRender/Buffer/BasicDynamicBufferLink.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Graphics3D
  {
    BasicBufferManager::BasicBufferManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeBufferFactory> factory)
      : m_maxFramesInFlight(maxFramesInFlight)
      , m_factory(std::move(factory))
    {
      FSLLOG3_VERBOSE5("BasicBufferManager::BasicBufferManager({})", maxFramesInFlight);
      if (maxFramesInFlight < 1)
      {
        throw std::invalid_argument("maxFramesInFlight needs to be at least 1");
      }
      if (!m_factory)
      {
        throw std::invalid_argument("factory can not be null");
      }
      m_factoryCaps = m_factory->GetBufferCaps();
    }


    BasicBufferManager::~BasicBufferManager()
    {
      // Force free all buffers
      ForceFreeAllBuffers();
      FSLLOG3_WARNING_IF(!m_staticRecords.empty(), "BasicBufferManager: There are still {} BasicStaticBuffer objects allocated",
                         m_staticRecords.size());
      FSLLOG3_WARNING_IF(!m_dynamicRecords.empty(), "BasicBufferManager: There are still {} BasicDynamicBuffer objects allocated",
                         m_dynamicRecords.size());
    }


    void BasicBufferManager::CreateDependentResources()
    {
      FSLLOG3_VERBOSE5("BasicBufferManager::CreateDependentResources()");
      m_dependentResources = DependentResources(true);
      // As we are currently creating dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
      CollectGarbage(0);
    }

    void BasicBufferManager::DestroyDependentResources()
    {
      FSLLOG3_VERBOSE5("BasicBufferManager::DestroyDependentResources");
      m_dependentResources = {};
      // As we are currently destroying dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
      CollectGarbage(0, true);
    }


    void BasicBufferManager::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
    {
      FSLLOG3_VERBOSE5("BasicBufferManager::OnRenderSystemEvent({})", static_cast<int>(theEvent));
      switch (theEvent)
      {
      case BasicRenderSystemEvent::SwapchainLost:
        // We know the device is idle when this occurs so we can just force free everything (and therefore also use a defer count of zero)
        CollectGarbage(0, true);
        break;
      case BasicRenderSystemEvent::SwapchainRecreated:
        break;
      default:
        FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
        break;
      }

      for (auto& rEntry : m_dynamicRecords)
      {
        assert(rEntry.Link);
        rEntry.Link->OnRenderSystemEvent(theEvent);
      }
    }


    std::shared_ptr<IBasicStaticBuffer> BasicBufferManager::CreateStaticBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData)
    {
      assert(m_factory);


      auto hNative = m_factory->CreateBuffer(bufferType, bufferData, NumericCast<uint32_t>(bufferData.size()), false);

      auto basic = std::make_shared<BasicStaticBufferTracker>(bufferType, hNative, NumericCast<uint32_t>(bufferData.size()));
      m_staticRecords.emplace_back(bufferType, hNative, basic, m_maxFramesInFlight);

      FSLLOG3_VERBOSE5("BasicBufferManager: CreateStaticBuffer ({}) count: {}", hNative.Value, m_staticRecords.size());
      return basic;
    }


    std::shared_ptr<IBasicDynamicBuffer> BasicBufferManager::CreateDynamicBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData,
                                                                                 const uint32_t capacity)
    {
      assert(m_factory);

      const bool setDataSupported = NativeBufferFactoryCapsUtil::IsEnabled(m_factoryCaps, NativeBufferFactoryCaps::Dynamic);

      auto link = std::make_shared<BasicDynamicBufferLink>(m_maxFramesInFlight, m_factory, bufferType, bufferData, capacity, setDataSupported);
      auto basic = std::make_shared<BasicDynamicBufferTracker>(link);
      m_dynamicRecords.emplace_back(basic, link, m_maxFramesInFlight);

      FSLLOG3_VERBOSE5("BasicBufferManager: CreateDynamicBuffer ({}) count: {} capacity: {}", reinterpret_cast<intptr_t>(link.get()),
                       m_dynamicRecords.size(), capacity);
      return basic;
    }

    void BasicBufferManager::PreUpdate()
    {
      // If the dependent resources are invalid then we can instantly collect all garbage
      const uint32_t deferCount = m_dependentResources.IsValid ? m_maxFramesInFlight : 0;
      CollectGarbage(deferCount);
    }


    void BasicBufferManager::CollectGarbage(const uint32_t deferCount, const bool force)
    {
      bool freedBuffer = false;
      {
        auto itr = m_staticRecords.begin();
        while (itr != m_staticRecords.end())
        {
          if (!itr->BasicUserObjectTracker.expired())
          {
            itr->DeferCount = deferCount;
            ++itr;
          }
          else if (!force && itr->DeferCount > 1u)
          {
            FSLLOG3_VERBOSE5("Deferring buffer destruction ({}) numFrames {}", itr->NativeHandle.Value, (itr->DeferCount - 1));
            --itr->DeferCount;
            ++itr;
          }
          else
          {
            FSLLOG3_VERBOSE5("Destroying buffer ({})", itr->NativeHandle.Value);
            m_factory->DestroyBuffer(itr->NativeHandle);
            itr = m_staticRecords.erase(itr);
            freedBuffer = true;
          }
        }
      }
      {
        auto itr = m_dynamicRecords.begin();
        while (itr != m_dynamicRecords.end())
        {
          if (!itr->BasicUserObject.expired())
          {
            itr->Link->CollectGarbage();
            itr->DeferCount = deferCount;
            ++itr;
          }
          else if (!force && itr->DeferCount > 0)
          {
            FSLLOG3_VERBOSE5("Deferring dynamic buffer destruction ({}) numFrames {}", reinterpret_cast<intptr_t>(itr->Link.get()), itr->DeferCount);
            --itr->DeferCount;
            ++itr;
          }
          else
          {
            FSLLOG3_VERBOSE5("Destroying buffer ({})", reinterpret_cast<intptr_t>(itr->Link.get()));
            itr->Link->Destroy();
            itr = m_dynamicRecords.erase(itr);
            freedBuffer = true;
          }
        }
      }

      FSLLOG3_VERBOSE4_IF(freedBuffer, "BasicBufferManager: After GC {} normal, {} dynamic", m_staticRecords.size(), m_dynamicRecords.size());
    }

    void BasicBufferManager::ForceFreeAllBuffers()
    {
      if (m_dependentResources.IsValid)
      {
        FSLLOG3_WARNING("Force destroying dependent resources");
        m_dependentResources = {};
      }

      FSLLOG3_VERBOSE5("BasicBufferManager: ForceFreeAllBuffers begin {} normal, {} dynamic", m_staticRecords.size(), m_dynamicRecords.size());
      {
        auto itr = m_staticRecords.begin();
        while (itr != m_staticRecords.end())
        {
          FSLLOG3_VERBOSE5("Destroying buffer ({})", itr->NativeHandle.Value);
          auto basic = itr->BasicUserObjectTracker.lock();
          if (basic)
          {
            basic->Dispose();
          }
          m_factory->DestroyBuffer(itr->NativeHandle);
          itr = m_staticRecords.erase(itr);
        }
      }
      {
        auto itr = m_dynamicRecords.begin();
        while (itr != m_dynamicRecords.end())
        {
          auto basic = itr->BasicUserObject.lock();
          if (basic)
          {
            basic->Dispose();
          }
          itr->Link->Destroy();
          itr = m_dynamicRecords.erase(itr);
        }
      }
      FSLLOG3_VERBOSE5("BasicBufferManager::ForceFreeAllBuffers done {} normal, {} dynamic", m_staticRecords.size(), m_dynamicRecords.size());
    }
  }
}
