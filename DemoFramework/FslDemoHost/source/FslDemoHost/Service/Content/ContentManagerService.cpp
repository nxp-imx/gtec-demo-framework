/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslDemoApp/Service/Image/IImageService.hpp>
#include <FslDemoHost/Service/Content/ContentManagerService.hpp>
#include <FslGraphics/Font/BinaryFontBasicKerningLoader.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/BinaryTextureAtlasLoader.hpp>
#include <cassert>
#include <limits>
#include <sstream>

namespace Fsl
{
  namespace
  {
    const IO::Path ToAbsolutePath(const IO::Path& trustedAbsPath, const IO::Path& notTrustedRelativePath)
    {
      assert(! trustedAbsPath.IsEmpty());

      // Do a lot of extra validation
      if (notTrustedRelativePath.IsEmpty())
        throw std::invalid_argument(std::string("path is invalid: ") + notTrustedRelativePath.ToAsciiString());
      if (IO::Path::IsPathRooted(notTrustedRelativePath))
        throw std::invalid_argument(std::string("not a relative path: ") + notTrustedRelativePath.ToAsciiString());
      if (notTrustedRelativePath.Contains(".."))
        throw std::invalid_argument(std::string("\"..\" not allowed in the relative path: ") + notTrustedRelativePath.ToAsciiString());

      return IO::Path::Combine(trustedAbsPath, notTrustedRelativePath);
    }
  }

  ContentManagerService::ContentManagerService(const ServiceProvider& serviceProvider, const IO::Path& contentPath)
    : ThreadLocalService(serviceProvider)
    , m_contentPath(contentPath)
    , m_imageService(serviceProvider.TryGet<IImageService>()) // Try to acquire the image service so we can use it if its available.
  {
    if (! IO::Path::IsPathRooted(m_contentPath))
    {
      FSLLOG_WARNING("The supplied path is not rooted '" << contentPath.ToAsciiString() << "'");
    }
  }


  ContentManagerService::~ContentManagerService()
  {
  }


  IO::Path ContentManagerService::GetContentPath() const
  {
    return m_contentPath;
  }


  BitmapOrigin ContentManagerService::GetPreferredBitmapOrigin() const
  {
    return m_imageService->GetPreferredBitmapOrigin();
  }


  bool ContentManagerService::Exists(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return IO::File::Exists(absPath);
  }


  int32_t ContentManagerService::GetLength(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    const int64_t length = IO::File::GetLength(absPath);
    if (length > std::numeric_limits<int32_t>::max())
    {
      std::stringstream strstream;
      strstream << "File '" << absPath.ToAsciiString() << "' was larger than 2GB, which is unsupported";
      throw IOException(strstream.str());
    }
    return static_cast<int32_t>(length);
  }


  std::string ContentManagerService::ReadAllText(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return IO::File::ReadAllText(absPath);
  }


  void ContentManagerService::ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    IO::File::ReadAllBytes(rTargetArray, absPath);
  }


  int32_t ContentManagerService::ReadAllBytes(void* pDstArray, const int32_t cbDstArray, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return (int32_t) IO::File::ReadAllBytes(pDstArray, cbDstArray, absPath);
  }


  std::vector<uint8_t> ContentManagerService::ReadBytes(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return IO::File::ReadBytes(absPath);
  }


  void ContentManagerService::ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const int32_t fileOffset, const int32_t bytesToRead) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    IO::File::ReadBytes(rTargetArray, absPath, fileOffset, bytesToRead);
  }


  int32_t ContentManagerService::ReadBytes(void* pDstArray, const int32_t cbDstArray, const int32_t dstStartIndex, const IO::Path& relativePath, const int32_t fileOffset, const int32_t bytesToRead) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return (int32_t)IO::File::ReadBytes(pDstArray, cbDstArray, dstStartIndex, absPath, fileOffset, bytesToRead);
  }


  void ContentManagerService::Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    m_imageService->Read(rBitmap, absPath, desiredPixelFormat, desiredOrigin);
  }


  void ContentManagerService::Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    m_imageService->Read(rTexture, absPath, desiredPixelFormat, desiredOrigin);
  }


  void ContentManagerService::Read(BasicTextureAtlas& rTextureAtlas, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    BinaryTextureAtlasLoader::Load(rTextureAtlas, absPath);
  }


  void ContentManagerService::Read(BasicFontKerning& rFontKerning, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    BinaryFontBasicKerningLoader::Load(rFontKerning, absPath);
  }


  bool ContentManagerService::TryReadAllText(std::string& rText, const IO::Path& relativePath) const
  {
    // For now we implement it via the exception throwing method
    try
    {
      rText = ReadAllText(relativePath);
      return true;
    }
    catch (std::exception&)
    {
      rText.clear();
      return false;
    }
  }


  bool ContentManagerService::TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_contentPath, relativePath));
    return m_imageService->TryRead(rBitmap, absPath, desiredPixelFormat, desiredOrigin);
  }


  Bitmap ContentManagerService::ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    Bitmap bitmap;
    Read(bitmap, relativePath, desiredPixelFormat, desiredOrigin);
    return bitmap;
  }


  Texture ContentManagerService::ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin) const
  {
    Texture texture;
    Read(texture, relativePath, desiredPixelFormat, desiredOrigin);
    return texture;
  }

}
