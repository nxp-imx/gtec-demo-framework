#ifndef FSLDEMOHOST_BASE_SERVICE_PERSISTENT_PERSISTENTDATAMANAGERSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_PERSISTENT_PERSISTENTDATAMANAGERSERVICE_HPP
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

#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <FslDemoApp/Base/Service/Persistent/IPersistentDataManager.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>

namespace Fsl
{
  class IImageService;

  class PersistentDataManagerService
    : public ThreadLocalService
    , public IPersistentDataManager
  {
    IO::Path m_persistentDataPath;
    std::shared_ptr<IImageService> m_imageService;

  public:
    PersistentDataManagerService(const ServiceProvider& serviceProvider, const IO::Path& persistentDataPath);
    ~PersistentDataManagerService() override;

    // From IPersistentDataManager
    IO::Path GetPersistentDataPath() const override;
    bool Exists(const IO::Path& relativePath) const override;
    uint64_t GetLength(const IO::Path& relativePath) const override;
    std::string ReadAllText(const IO::Path& relativePath) const override;
    void ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const override;
    uint64_t ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const IO::Path& relativePath) const override;
    void ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const uint64_t fileOffset,
                   const uint64_t bytesToRead) const override;
    uint64_t ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& relativePath,
                       const uint64_t fileOffset, const uint64_t bytesToRead) const override;
    void Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
              const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const override;
    void Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
              const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const override;
    void WriteAlltext(const IO::Path& relativePath, const std::string& content) override;
    void WriteAllBytes(const IO::Path& relativePath, const std::vector<uint8_t>& content) override;
    void Write(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat = ImageFormat::Undefined,
               const PixelFormat desiredPixelFormat = PixelFormat::Undefined) override;
    void WriteExactImage(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                         const PixelFormat desiredPixelFormat = PixelFormat::Undefined) override;
    bool TryReadAllText(std::string& rDst, const IO::Path& relativePath) const override;
    bool TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                 const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const override;
    Bitmap ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const override;
    Texture ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                        const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const override;

  private:
  };
}

#endif
