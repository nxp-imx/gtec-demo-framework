/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
#include <Shared/CubeMapping/API/Vulkan/TextureUtil.hpp>
#include <array>
#include <cassert>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    void ApplyExtension(std::array<IO::Path, 6>& rFiles, const char* const pszExtension)
    {
      for (auto& rEntry : rFiles)
      {
        rEntry += pszExtension;
      }
    }
  }


  VUTexture TextureUtil::CreateCubemapTextureFromSix(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& basePath,
                                                     VulkanImageCreator& imageCreator, const PixelFormat pixelFormat)
  {
    assert(contentManager);

    std::array<IO::Path, 6> files = {"PosX", "NegX", "PosY", "NegY", "PosZ", "NegZ"};

    if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.png")))
    {
      ApplyExtension(files, ".png");
    }
    else if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.jpg")))
    {
      ApplyExtension(files, ".jpg");
    }
    else if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.exr")))
    {
      ApplyExtension(files, ".exr");
    }
    else if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.hdr")))
    {
      ApplyExtension(files, ".hdr");
    }
    else if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.dds")))
    {
      ApplyExtension(files, ".dds");
    }
    else if (contentManager->Exists(IO::Path::Combine(basePath, "PosX.ktx")))
    {
      ApplyExtension(files, ".ktx");
    }
    else
    {
      throw NotSupportedException(std::string("Could not find a supported image format at '") + basePath.ToUTF8String() + "'");
    }

    // Cubemaps defy the normal OpenGL ES convention of storing the images upside down
    const auto texPosX = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[0]), pixelFormat, BitmapOrigin::UpperLeft);
    const auto texNegX = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[1]), pixelFormat, BitmapOrigin::UpperLeft);
    const auto texPosY = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[2]), pixelFormat, BitmapOrigin::UpperLeft);
    const auto texNegY = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[3]), pixelFormat, BitmapOrigin::UpperLeft);
    const auto texPosZ = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[4]), pixelFormat, BitmapOrigin::UpperLeft);
    const auto texNegZ = contentManager->ReadBitmap(IO::Path::Combine(basePath, files[5]), pixelFormat, BitmapOrigin::UpperLeft);

    // GLTextureParameters3 texParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    // return VUTexture(texPosX, texNegX, texPosY, texNegY, texPosZ, texNegZ, texParams, TextureFlags::AllowAnyBitmapOrigin);

    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 1.0f;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

    return imageCreator.CreateTexture(texPosX, texNegX, texPosY, texNegY, texPosZ, texNegZ, samplerCreateInfo);
  }
}
