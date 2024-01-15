#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSTEXTURE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSTEXTURE_HPP
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

#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsTextureScopedDescriptorSet.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTexture.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <cassert>

namespace Fsl::Vulkan
{
  class NativeGraphicsTexture final : public Graphics3D::INativeTexture
  {
    PxExtent3D m_extentPx{};
    VUTexture m_texture;
    NativeGraphicsTextureScopedDescriptorSet m_descriptorSet;

  public:
    NativeGraphicsTexture() = default;
    NativeGraphicsTexture(const PxExtent3D& extentPx, VUTexture texture, NativeGraphicsTextureScopedDescriptorSet&& descriptorSet);

    void Destroy();

    bool IsValid() const noexcept
    {
      return m_texture.IsValid();
    }

    inline VkDescriptorSet GetDescriptorSet() const noexcept
    {
      assert(IsValid());
      return m_descriptorSet.Get();
    }


    inline VUTextureInfo ToTextureInfo() const noexcept
    {
      assert(IsValid());
      return {Get(), VkExtent2D{m_extentPx.Width.Value, m_extentPx.Height.Value}};
    }

    //! @brief Get the native texture handle
    VkDescriptorImageInfo Get() const noexcept
    {
      assert(IsValid());
      return m_texture.GetDescriptorImageInfo();
    }

    // Graphics3D::INativeTexture
    void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags) final;
  };
}

#endif
