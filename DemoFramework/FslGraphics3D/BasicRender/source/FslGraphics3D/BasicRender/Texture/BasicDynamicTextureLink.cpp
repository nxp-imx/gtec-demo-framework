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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTexture.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTextureFactory.hpp>
#include <FslGraphics3D/BasicRender/Texture/BasicDynamicTextureLink.hpp>
#include <algorithm>
#include <utility>

namespace Fsl
{
  class ReadOnlyRawTexture;
  namespace Graphics3D
  {
    BasicDynamicTextureLink::BasicDynamicTextureLink(const uint32_t maxFramesInFlight, std::shared_ptr<INativeTextureFactory> factory,
                                                     const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint,
                                                     const TextureFlags textureFlags, const bool setDataSupported)
      : m_factory(std::move(factory))
      , m_textures(std::max(maxFramesInFlight, 2u))
      , m_setDataSupported(setDataSupported)
    {
      FSLLOG3_VERBOSE5("DynamicNativeTextureLink::Construct");
      if (!m_factory)
      {
        throw std::invalid_argument("factory can not be null");
      }

      m_textures.front().NativeHandle = m_factory->CreateTexture(texture, filterHint, textureFlags, false);
      m_textures.front().IsInUse = true;
    }


    BasicDynamicTextureLink::~BasicDynamicTextureLink() noexcept
    {
      uint32_t useCount = 0;
      for (const Record& record : m_textures)
      {
        if (record.NativeHandle.IsValid())
        {
          ++useCount;
        }
      }
      FSLLOG3_ERROR_IF(useCount > 0, "Found {} unfreed native textures", useCount);
    }


    void BasicDynamicTextureLink::Destroy() noexcept
    {
      FSLLOG3_VERBOSE5("DynamicNativeTextureLink::Destroy");

      for (Record& rRecord : m_textures)
      {
        if (rRecord.NativeHandle.IsValid())
        {
          FSLLOG3_VERBOSE5("DynamicNativeTextureLink::Destroyed");
          m_factory->DestroyTexture(rRecord.NativeHandle);
          rRecord.NativeHandle = {};
        }
      }
      m_textures.clear();
      m_activeIndex = 0u;
      m_isDestroyed = true;
    }


    void BasicDynamicTextureLink::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
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


    void BasicDynamicTextureLink::CollectGarbage() noexcept
    {
      for (std::size_t i = 0; i < m_textures.size(); ++i)
      {
        auto& rEntry = m_textures[i];
        if (rEntry.DeferredReuse)
        {
          if (rEntry.DeferCount <= 1u)
          {
            // Make the texture available for reuse
            rEntry.IsInUse = false;
            rEntry.DeferredReuse = false;
            rEntry.DeferCount = 0u;
            FSLLOG3_VERBOSE5("DynamicNativeTextureLink: InternalTexture at #{} ready for reuse", i);
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


    void BasicDynamicTextureLink::SetData(const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    {
      if (m_isDestroyed)
      {
        FSLLOG3_ERROR("Can not SetData on a destroyed texture");
        return;
      }

      if (!m_swapchainValid)
      {
        if (m_activeIndex > m_textures.size())
        {
          throw NotFoundException("Could not find a free texture (SetData internal error)");
        }
        SetData(m_textures[m_activeIndex], *m_factory, texture, filterHint, textureFlags, m_setDataSupported);
        return;
      }

      // 1. locate a free texture / create a free texture
      auto itrFind = std::find_if(m_textures.begin(), m_textures.end(), [](const Record& entry) { return !entry.IsInUse; });
      if (itrFind == m_textures.end())
      {
        throw NotFoundException("Could not find a free texture (SetData internal error)");
      }

      assert(!itrFind->IsInUse);
      assert(!itrFind->DeferredReuse);
      assert(itrFind->DeferCount == 0u);

      // 2. set data
      SetData(*itrFind, *m_factory, texture, filterHint, textureFlags, m_setDataSupported);

      // 3. tag current as 'deferred free'
      auto deferCount = static_cast<uint32_t>(m_textures.size());
      m_textures[m_activeIndex].DeferredReuse = true;
      m_textures[m_activeIndex].DeferCount = deferCount > 0u ? deferCount - 1u : 1u;

      // 4. mark new texture as being active
      itrFind->IsInUse = true;
      m_activeIndex = static_cast<uint32_t>(std::distance(m_textures.begin(), itrFind));

      FSLLOG3_VERBOSE5("DynamicNativeTextureLink: InternalTexture at #{} marked as active", m_activeIndex);
    }

    void BasicDynamicTextureLink::SetData(Record& rRecord, INativeTextureFactory& textureFactory, const ReadOnlyRawTexture& texture,
                                          const Texture2DFilterHint filterHint, const TextureFlags textureFlags, const bool setDataSupported)
    {
      if (!rRecord.NativeHandle.IsValid())
      {
        // The texture is not valid, so we need to allocate it
        rRecord.NativeHandle = textureFactory.CreateTexture(texture, filterHint, textureFlags, false);
        FSLLOG3_VERBOSE5("DynamicNativeTextureLink::SetData (create)");
      }
      else
      {
        // The texture is valid, so we need to update it
        if (setDataSupported)
        {
          textureFactory.SetTextureData(rRecord.NativeHandle, texture, filterHint, textureFlags);
          FSLLOG3_VERBOSE5("DynamicNativeTextureLink::SetData (optimized)");
        }
        else
        {
          // this is extremely inefficient, but SetData is not supported so its the only way
          textureFactory.DestroyTexture(rRecord.NativeHandle);
          rRecord.NativeHandle = textureFactory.CreateTexture(texture, filterHint, textureFlags, false);
          FSLLOG3_VERBOSE5("DynamicNativeTextureLink::SetData (destroy/create)");
        }
      }
    }
  }
}
