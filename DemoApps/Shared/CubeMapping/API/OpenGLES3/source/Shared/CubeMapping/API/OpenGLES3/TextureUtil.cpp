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

#include <Shared/CubeMapping/API/OpenGLES3/TextureUtil.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <array>

namespace Fsl
{
  using namespace GLES3;

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


  GLTexture TextureUtil::CreateCubemapTextureFromSix(const std::shared_ptr<IContentManager>& contentManager, const IO::Path& basePath,
                                                     const PixelFormat pixelFormat)
  {
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

    GLTextureParameters3 texParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    return GLTexture(texPosX, texNegX, texPosY, texNegY, texPosZ, texNegZ, texParams, TextureFlags::AllowAnyBitmapOrigin);
  }
}
