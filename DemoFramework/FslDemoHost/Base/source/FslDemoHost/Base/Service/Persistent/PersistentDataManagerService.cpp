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
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslDemoApp/Base/Service/Image/IImageService.hpp>
#include <FslDemoHost/Base/Service/Persistent/PersistentDataManagerService.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace
  {
    const IO::Path ToAbsolutePath(const IO::Path& trustedAbsPath, const IO::Path& notTrustedRelativePath)
    {
      assert(!trustedAbsPath.IsEmpty());

      // Do a lot of extra validation
      if (notTrustedRelativePath.IsEmpty())
      {
        throw std::invalid_argument("path is invalid");
      }
      if (IO::Path::IsPathRooted(notTrustedRelativePath))
      {
        throw std::invalid_argument("not a relative path");
      }
      if (notTrustedRelativePath.Contains(".."))
      {
        throw std::invalid_argument("\"..\" not allowed in the relative path");
      }

      return IO::Path::Combine(trustedAbsPath, notTrustedRelativePath);
    }
  }


  PersistentDataManagerService::PersistentDataManagerService(const ServiceProvider& serviceProvider, const IO::Path& persistentDataPath)
    : ThreadLocalService(serviceProvider)
    , m_persistentDataPath(persistentDataPath)
    , m_imageService(serviceProvider.TryGet<IImageService>())
  {
  }


  PersistentDataManagerService::~PersistentDataManagerService() = default;


  IO::Path PersistentDataManagerService::GetPersistentDataPath() const
  {
    return m_persistentDataPath;
  }


  bool PersistentDataManagerService::Exists(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return IO::File::Exists(absPath);
  }


  uint64_t PersistentDataManagerService::GetLength(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    const auto length = IO::File::GetLength(absPath);
    if (length > std::numeric_limits<uint32_t>::max())
    {
      std::stringstream strstream;
      strstream << "File '" << absPath.ToAsciiString() << "' was larger than 4GB, which is unsupported";
      throw IOException(strstream.str());
    }
    return length;
  }


  std::string PersistentDataManagerService::ReadAllText(const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return IO::File::ReadAllText(absPath);
  }


  void PersistentDataManagerService::ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    IO::File::ReadAllBytes(rTargetArray, absPath);
  }


  uint64_t PersistentDataManagerService::ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return IO::File::ReadAllBytes(pDstArray, cbDstArray, absPath);
  }


  void PersistentDataManagerService::ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const uint64_t fileOffset,
                                               const uint64_t bytesToRead) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    IO::File::ReadBytes(rTargetArray, absPath, fileOffset, bytesToRead);
  }


  uint64_t PersistentDataManagerService::ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex,
                                                   const IO::Path& relativePath, const uint64_t fileOffset, const uint64_t bytesToRead) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return IO::File::ReadBytes(pDstArray, cbDstArray, dstStartIndex, absPath, fileOffset, bytesToRead);
  }


  void PersistentDataManagerService::Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat,
                                          const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    m_imageService->Read(rBitmap, absPath, desiredPixelFormat, desiredOrigin);
  }


  void PersistentDataManagerService::Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat,
                                          const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    m_imageService->Read(rTexture, absPath, desiredPixelFormat, desiredOrigin);
  }


  void PersistentDataManagerService::WriteAlltext(const IO::Path& relativePath, const std::string& content)
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    IO::File::WriteAllText(relativePath, content);
  }


  void PersistentDataManagerService::WriteAllBytes(const IO::Path& relativePath, const std::vector<uint8_t>& content)
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    IO::File::WriteAllBytes(relativePath, content);
  }


  void PersistentDataManagerService::Write(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                           const PixelFormat desiredPixelFormat)
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return m_imageService->Write(absPath, bitmap, imageFormat, desiredPixelFormat);
  }


  void PersistentDataManagerService::WriteExactImage(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                     const PixelFormat desiredPixelFormat)
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return m_imageService->WriteExactImage(absPath, bitmap, imageFormat, desiredPixelFormat);
  }


  bool PersistentDataManagerService::TryReadAllText(std::string& rDst, const IO::Path& relativePath) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return IO::File::TryReadAllText(rDst, absPath);
  }


  bool PersistentDataManagerService::TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat,
                                             const BitmapOrigin desiredOrigin) const
  {
    const IO::Path absPath(ToAbsolutePath(m_persistentDataPath, relativePath));
    return m_imageService->TryRead(rBitmap, absPath, desiredPixelFormat, desiredOrigin);
  }


  Bitmap PersistentDataManagerService::ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat,
                                                  const BitmapOrigin desiredOrigin) const
  {
    Bitmap bitmap;
    Read(bitmap, relativePath, desiredPixelFormat, desiredOrigin);
    return bitmap;
  }


  Texture PersistentDataManagerService::ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat,
                                                    const BitmapOrigin desiredOrigin) const
  {
    Texture texture;
    Read(texture, relativePath, desiredPixelFormat, desiredOrigin);
    return texture;
  }
}
