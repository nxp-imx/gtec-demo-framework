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

#include <FslGraphics3D/BasicRender/Material/BasicNativeMaterialManager.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeMaterialFactory.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Graphics3D
  {
    namespace
    {
      namespace LocalConfig
      {
        constexpr auto LogType = Fsl::LogType::Verbose3;
      }

      bool HasValidHandle(const TypedFlexSpan<BasicNativeMaterialRecord>& span)
      {
        for (std::size_t i = 0; i < span.size(); ++i)
        {
          const BasicNativeMaterialRecord& rEntry = span[i];
          if (rEntry.InternalHandle.IsValid() || rEntry.NativeHandle.IsValid())
          {
            return true;
          }
        }
        return false;
      }

#ifndef NDEBUG
      bool HasValidHandle(const Span<BasicNativeMaterialHandle>& span)
      {
        for (std::size_t i = 0; i < span.size(); ++i)
        {
          if (span[i].IsValid())
          {
            return true;
          }
        }
        return false;
      }
#endif

      void ClearCreateSpan(Span<BasicNativeMaterialCreateInfo> createSpan)
      {
        for (uint32_t i = 0; i < createSpan.size(); ++i)
        {
          createSpan[i] = {};
        }
      }


      void CreateNativeMaterials(Span<BasicNativeMaterialHandle> dstSpan, INativeMaterialFactory& factory,
                                 Span<BasicNativeMaterialCreateInfo> createSpan, const ReadOnlyTypedFlexSpan<BasicMaterialDetailsRecord>& recordSpan)
      {
        if (createSpan.size() != recordSpan.size() || createSpan.size() != dstSpan.size())
        {
          throw std::invalid_argument("createSpan, recordSSpan and dstSpan size must be equal");
        }
        if (createSpan.empty())
        {
          return;
        }

        {    // Create a create request for each entry in the record span
          for (uint32_t i = 0; i < recordSpan.size(); ++i)
          {
            const BasicMaterialDetailsRecord& record = recordSpan[i];
            createSpan[i] = BasicNativeMaterialCreateInfo(record.MaterialInfo, record.MaterialDecl.AsSpan(), record.VertexDecl.AsSpan());
          }
        }

        {    // Do the request
          try
          {
            // We dont expect the dst span to contain any valid handles
            assert(!HasValidHandle(dstSpan));

            factory.CreateMaterials(dstSpan, createSpan);
            ClearCreateSpan(createSpan);
          }
          catch (const std::exception& ex)
          {
            // If a exception is thrown we don't expect the dst span to contain any valid handles
            assert(!HasValidHandle(dstSpan));
            ClearCreateSpan(createSpan);
            FSLLOG3_ERROR("CreateNativeMaterials threw exception: {}", ex.what());
            throw;
          }
        }
      }
    }

    BasicNativeMaterialManager::~BasicNativeMaterialManager()
    {
      Shutdown();
    }


    void BasicNativeMaterialManager::Init(const std::shared_ptr<INativeMaterialFactory>& factory, const uint32_t capacity,
                                          const uint32_t maxFramesInFlight)
    {
      assert(!IsValid());
      assert(factory);
      m_factory = factory;
      m_maxFramesInFlight = maxFramesInFlight;
      EnsureCapacity(capacity);
    }


    void BasicNativeMaterialManager::Shutdown()
    {
      if (!m_factory)
      {
        return;
      }

      CollectGarbage(true);

      for (uint32_t i = 0; i < m_nativeTextures.Count(); ++i)
      {
        BasicNativeMaterialHandle& rHandle = m_nativeTextures[i];
        m_factory->DestroyMaterial(rHandle);
        rHandle = {};
      }
      m_nativeTextures.Clear();
      m_factory.reset();
      m_maxFramesInFlight = 0;
    }


    void BasicNativeMaterialManager::PreUpdate()
    {
      if (m_factory)
      {
        CollectGarbage(false);
      }
    }


    void BasicNativeMaterialManager::CreateMaterials(TypedFlexSpan<BasicNativeMaterialRecord> dst,
                                                     ReadOnlyTypedFlexSpan<BasicMaterialDetailsRecord> src)
    {
      if (!m_factory)
      {
        throw UsageErrorException("Not initialized");
      }
      if (dst.size() != src.size())
      {
        throw std::invalid_argument("dst.size() must be equal to src.size()");
      }
      if (HasValidHandle(dst))
      {
        throw std::invalid_argument("dst can not contain valid handles");
      }

      if (!src.empty())
      {
        EnsureCapacity(src.size());

        // Extract material creation info so we can request them all at once
        auto createSpan = SpanUtil::AsSpan(m_materialCreationScratchpad).subspan(0, src.size());
        auto tmpSpan = SpanUtil::AsSpan(m_nativeMaterialsScratchpad).subspan(0, src.size());
        CreateNativeMaterials(tmpSpan, *m_factory, createSpan, src);
        try
        {
          // Assign a local handle to each allocated native texture
          for (std::size_t i = 0; i < dst.size(); ++i)
          {
            BasicNativeMaterialRecord& rDst = dst[i];
            auto nativeHandle = tmpSpan[i];
            auto localHandle = m_nativeTextures.Add(nativeHandle);
            rDst = BasicNativeMaterialRecord(InternalMaterialHandle(localHandle), nativeHandle);
            tmpSpan[i] = {};
          }
        }
        catch (const std::exception& ex)
        {
          FSLLOG3_ERROR("Exception occurred: {}", ex.what());
          for (std::size_t i = 0; i < dst.size(); ++i)
          {
            BasicNativeMaterialRecord& rDst = dst[i];
            if (rDst.InternalHandle.IsValid())
            {
              m_nativeTextures.Remove(rDst.InternalHandle.Value);
              m_factory->DestroyMaterial(rDst.NativeHandle);
              rDst = {};
            }
            else if (tmpSpan[i].IsValid())
            {
              m_factory->DestroyMaterial(tmpSpan[i]);
              tmpSpan[i] = {};
            }
          }
          throw;
        }
      }
    }


    BasicNativeMaterialRecord BasicNativeMaterialManager::CreateMaterial(const BasicMaterialDetailsRecord& src)
    {
      BasicNativeMaterialRecord result;
      CreateMaterials(TypedFlexSpan<BasicNativeMaterialRecord>(&result, 1, sizeof(BasicNativeMaterialRecord)),
                      ReadOnlyTypedFlexSpan<BasicMaterialDetailsRecord>(&src, 1, sizeof(BasicMaterialDetailsRecord)));
      return result;
    }


    bool BasicNativeMaterialManager::ScheduleRemove(const InternalMaterialHandle handle)
    {
      if (!m_factory)
      {
        throw UsageErrorException("Not initialized");
      }
      FSLLOG3(LocalConfig::LogType, "BasicNativeMaterialManager::ScheduleRemove({})", handle.Value);

      BasicNativeMaterialHandle* pHandle = m_nativeTextures.TryGet(handle.Value);
      if (pHandle == nullptr)
      {
        FSLLOG3_DEBUG_WARNING("Tried to ScheduleRemove of unknown handle");
        return false;
      }
      m_deferredRemove.emplace_back(m_maxFramesInFlight, *pHandle);
      m_nativeTextures.Remove(handle.Value);
      return true;
    }


    void BasicNativeMaterialManager::CollectGarbage(const bool force)
    {
      assert(m_factory);

      auto itr = m_deferredRemove.begin();
      while (itr != m_deferredRemove.end())
      {
        if (!force && itr->DeferCount > 0)
        {
          FSLLOG3(LocalConfig::LogType, "BasicNativeMaterialManager::CollectGarbage({}) Deferring remove {}", itr->NativeHandle.Value,
                  itr->DeferCount);
          if (itr->DeferCount <= m_maxFramesInFlight)
          {
            --itr->DeferCount;
          }
          else
          {
            itr->DeferCount = m_maxFramesInFlight;
          }
          ++itr;
        }
        else
        {
          FSLLOG3(LocalConfig::LogType, "BasicNativeMaterialManager::CollectGarbage({}) Destroying", itr->NativeHandle.Value);
          m_factory->DestroyMaterial(itr->NativeHandle);
          itr = m_deferredRemove.erase(itr);
        }
      }
    }


    void BasicNativeMaterialManager::EnsureCapacity(const std::size_t minCapacity)
    {
      if (minCapacity > m_materialCreationScratchpad.size())
      {
        FSLLOG3_VERBOSE5("BasicNativeMaterialManager::CreateDependentResources resizing MaterialCreationScratchpad");
        m_materialCreationScratchpad.resize(minCapacity);
      }
      if (minCapacity > m_nativeMaterialsScratchpad.size())
      {
        FSLLOG3_VERBOSE5("BasicNativeMaterialManager::CreateDependentResources resizing NativeMaterials");
        m_nativeMaterialsScratchpad.resize(minCapacity);
      }
    }

  }
}
