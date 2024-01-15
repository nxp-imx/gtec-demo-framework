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
#include <FslGraphics3D/BasicRender/Adapter/INativeTextureFactory.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicDynamicTextureLink.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicTextureManager.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl::Graphics3D
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto LogType = Fsl::LogType::Verbose5;
    }
  }

  BasicTextureManager::BasicTextureManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeTextureFactory> factory)
    : m_maxFramesInFlight(maxFramesInFlight)
    , m_factory(std::move(factory))
  {
    FSLLOG3(LocalConfig::LogType, "BasicTextureManager::BasicTextureManager({})", maxFramesInFlight);
    if (maxFramesInFlight <= 0)
    {
      throw std::invalid_argument("maxFramesInFlight needs to be at least 1");
    }
    if (!m_factory)
    {
      throw std::invalid_argument("textureFactory can not be null");
    }
    m_factoryCaps = m_factory->GetTextureCaps();
  }


  BasicTextureManager::~BasicTextureManager()
  {
    // Force free all textures
    ForceFreeAllTextures();
    FSLLOG3_WARNING_IF(!m_staticRecords.empty(), "BasicTextureManager: There are still {} StaticNativeTexture objects allocated",
                       m_staticRecords.size());
    FSLLOG3_WARNING_IF(!m_dynamicRecords.empty(), "BasicTextureManager: There are still {} DynamicNativeTexture objects allocated",
                       m_dynamicRecords.size());
  }


  void BasicTextureManager::CreateDependentResources()
  {
    FSLLOG3(LocalConfig::LogType, "BasicTextureManager::CreateDependentResources()");
    m_dependentResources = DependentResources(true);
    // As we are currently creating dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
    DoCollectGarbage(0);
  }

  void BasicTextureManager::DestroyDependentResources()
  {
    FSLLOG3(LocalConfig::LogType, "BasicTextureManager::DestroyDependentResources");
    // As we are currently destroying dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
    m_dependentResources = {};
    DoCollectGarbage(0, true);
  }


  void BasicTextureManager::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
  {
    FSLLOG3(LocalConfig::LogType, "BasicTextureManager::OnRenderSystemEvent({})", static_cast<int>(theEvent));
    switch (theEvent)
    {
    case BasicRenderSystemEvent::SwapchainLost:
      // We know the device is idle when this occurs so we can just force free everything (and therefore also use a defer count of zero)
      DoCollectGarbage(0, true);
      break;
    case BasicRenderSystemEvent::SwapchainRecreated:
      break;
    default:
      FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
      break;
    }

    for (auto& rEntry : m_dynamicRecords)
    {
      assert(rEntry.LinkTexture);
      rEntry.LinkTexture->OnRenderSystemEvent(theEvent);
    }
  }


  std::shared_ptr<INativeTexture2D> BasicTextureManager::CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                         const TextureFlags textureFlags)
  {
    assert(m_factory);
    BasicNativeTextureHandle hNative = m_factory->CreateTexture(texture, filterHint, textureFlags, false);

    const bool textureCoordinatesFlipY = NativeTextureFactoryCapsUtil::IsEnabled(m_factoryCaps, NativeTextureFactoryCaps::TextureCoordinatesFlipY);
    const auto hTexture = BasicTextureHandle(m_textures.Add(TextureRecord(false)));
    auto tracker = std::make_shared<BasicStaticTextureTracker>(hTexture, texture.GetExtent(), textureCoordinatesFlipY, hNative);


    m_staticRecords.emplace_back(hTexture, tracker, hNative, m_maxFramesInFlight);

    FSLLOG3(LocalConfig::LogType, "BasicTextureManager: CreateTexture2D ({},{}) count: {}", hTexture.Value, hNative.Value, m_staticRecords.size());
    return tracker;
  }


  std::shared_ptr<IDynamicNativeTexture2D>
    BasicTextureManager::CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    assert(m_factory);

    const bool setDataSupported = NativeTextureFactoryCapsUtil::IsEnabled(m_factoryCaps, NativeTextureFactoryCaps::Dynamic);

    auto linkTexture = std::make_shared<BasicDynamicTextureLink>(m_maxFramesInFlight, m_factory, texture, filterHint, textureFlags, setDataSupported);
    const bool textureCoordinatesFlipY = NativeTextureFactoryCapsUtil::IsEnabled(m_factoryCaps, NativeTextureFactoryCaps::TextureCoordinatesFlipY);
    const auto hTexture = BasicTextureHandle(m_textures.Add(TextureRecord(true)));
    auto tracker = std::make_shared<BasicDynamicTextureTracker>(hTexture, texture.GetExtent(), textureCoordinatesFlipY, linkTexture);
    m_dynamicRecords.emplace_back(hTexture, tracker, linkTexture, m_maxFramesInFlight);

    FSLLOG3(LocalConfig::LogType, "BasicTextureManager: CreateDynamicTexture2D ({}) count: {}", hTexture.Value, m_dynamicRecords.size());
    return tracker;
  }


  PxExtent2D BasicTextureManager::GetTextureExtentPx(const std::shared_ptr<INativeTexture2D>& texture) const noexcept
  {
    if (!texture)
    {
      FSLLOG3_DEBUG_WARNING("texture can not be null");
      return {};
    }
    // Check if its a static texture
    const auto* pStatic = dynamic_cast<const BasicStaticTextureTracker*>(texture.get());
    if (pStatic != nullptr)
    {
      return pStatic->GetExtent2D();
    }
    // Check if its a dynamic texture
    const auto* pDynamic = dynamic_cast<const BasicDynamicTextureTracker*>(texture.get());
    if (pDynamic != nullptr)
    {
      return pDynamic->GetExtent2D();
    }
    FSLLOG3_DEBUG_WARNING("Unknown texture type");
    return {};
  }

  const IBasicNativeTexture* BasicTextureManager::TryGetNativeTexture(const BasicNativeTextureHandle hTexture) const noexcept
  {
    return m_factory->TryGetTexture(hTexture);
  }


  void BasicTextureManager::PreUpdate()
  {
    // If the dependent resources are invalid then we can instantly collect all garbage
    const uint32_t deferCount = m_dependentResources.IsValid ? m_maxFramesInFlight : 0;
    DoCollectGarbage(deferCount);
  }

  void BasicTextureManager::DoCollectGarbage(const uint32_t deferCount, const bool force) noexcept
  {
    bool freedTexture = false;
    {
      auto itr = m_staticRecords.begin();
      while (itr != m_staticRecords.end())
      {
        if (!itr->Texture.expired())
        {
          itr->DeferCount = deferCount;
          ++itr;
        }
        else if (!force && itr->DeferCount > 1u)
        {
          FSLLOG3(LocalConfig::LogType, "BasicTextureManager: Deferring texture destruction ({},{}) numFrames {}", itr->Handle.Value,
                  itr->NativeHandle.Value, (itr->DeferCount - 1));
          --itr->DeferCount;
          ++itr;
        }
        else
        {
          FSLLOG3(LocalConfig::LogType, "BasicTextureManager: Destroying texture ({},{})", itr->Handle.Value, itr->NativeHandle.Value);
          {
            m_factory->DestroyTexture(itr->NativeHandle);
            m_textures.Remove(itr->Handle.Value);
            auto tex = itr->Texture.lock();
            if (tex)
            {
              tex->Dispose();
            }
          }
          itr = m_staticRecords.erase(itr);
          freedTexture = true;
        }
      }
    }
    {
      auto itr = m_dynamicRecords.begin();
      while (itr != m_dynamicRecords.end())
      {
        if (!itr->Texture.expired())
        {
          itr->LinkTexture->CollectGarbage();
          itr->DeferCount = deferCount;
          ++itr;
        }
        else if (!force && itr->DeferCount > 0)
        {
          FSLLOG3(LocalConfig::LogType, "BasicTextureManager: Deferring dynamic texture destruction ({}) numFrames {}",
                  reinterpret_cast<intptr_t>(itr->LinkTexture.get()), itr->DeferCount);
          --itr->DeferCount;
          ++itr;
        }
        else
        {
          FSLLOG3(LocalConfig::LogType, "BasicTextureManager: Destroying texture ({})", reinterpret_cast<intptr_t>(itr->LinkTexture.get()));
          {
            itr->LinkTexture->Destroy();
            auto tex = itr->Texture.lock();
            if (tex)
            {
              tex->Dispose();
            }
          }
          itr = m_dynamicRecords.erase(itr);
          freedTexture = true;
        }
      }
    }

    FSLLOG3_VERBOSE4_IF(freedTexture, "BasicTextureManager: After GC {} normal, {} dynamic", m_staticRecords.size(), m_dynamicRecords.size());
  }

  void BasicTextureManager::ForceFreeAllTextures() noexcept
  {
    if (m_dependentResources.IsValid)
    {
      FSLLOG3_WARNING("Force destroying dependent resources");
      m_dependentResources = {};
    }

    FSLLOG3(LocalConfig::LogType, "BasicTextureManager: ForceFreeAllTextures begin {} normal, {} dynamic", m_staticRecords.size(),
            m_dynamicRecords.size());
    {
      auto itr = m_staticRecords.begin();
      while (itr != m_staticRecords.end())
      {
        FSLLOG3(LocalConfig::LogType, "BasicTextureManager: Destroying texture ({},{})", itr->Handle.Value, itr->NativeHandle.Value);
        {
          m_factory->DestroyTexture(itr->NativeHandle);
          m_textures.Remove(itr->Handle.Value);
          auto tex = itr->Texture.lock();
          if (tex)
          {
            tex->Dispose();
          }
        }
        itr = m_staticRecords.erase(itr);
      }
    }
    {
      auto itr = m_dynamicRecords.begin();
      while (itr != m_dynamicRecords.end())
      {
        {
          itr->LinkTexture->Destroy();
          auto tex = itr->Texture.lock();
          if (tex)
          {
            tex->Dispose();
          }
        }
        itr = m_dynamicRecords.erase(itr);
      }
    }
    FSLLOG3(LocalConfig::LogType, "BasicTextureManager: ForceFreeAllTextures done {} normal, {} dynamic", m_staticRecords.size(),
            m_dynamicRecords.size());
  }
}
