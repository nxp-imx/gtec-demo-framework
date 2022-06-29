#ifndef FSLDEMOHOST_BASE_SERVICE_CONTENT_CONTENTMANAGERSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_CONTENT_CONTENTMANAGERSERVICE_HPP
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

#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <memory>

namespace Fsl
{
  class IImageService;
  class ITextureService;

  class ContentManagerService final
    : public ThreadLocalService
    , public IContentManager
  {
    IO::Path m_contentPath;
    std::shared_ptr<IImageService> m_imageService;
    std::shared_ptr<ITextureService> m_textureService;

  public:
    ContentManagerService(const ServiceProvider& serviceProvider, const IO::Path& contentPath);
    ~ContentManagerService() final;

    // From IContentManager
    IO::Path GetContentPath() const final;
    BitmapOrigin GetPreferredBitmapOrigin() const final;
    bool Exists(const IO::Path& relativePath) const final;
    uint64_t GetLength(const IO::Path& relativePath) const final;
    std::string ReadAllText(const IO::Path& relativePath) const final;
    void ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const final;
    std::vector<uint8_t> ReadAllBytes(const IO::Path& relativePath) const final;
    uint64_t ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const IO::Path& relativePath) const final;
    std::vector<uint8_t> ReadBytes(const IO::Path& relativePath) const final;
    void ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const uint64_t fileOffset,
                   const uint64_t bytesToRead) const final;
    uint64_t ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& relativePath,
                       const uint64_t fileOffset, const uint64_t bytesToRead) const final;
    void Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
              const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
              const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const final;
    void Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
              const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
              const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined, const bool generateMipMapsHint = false) const final;
    void Read(BasicTextureAtlas& rTextureAtlas, const IO::Path& relativePath) const final;
    void Read(BasicFontKerning& rFontKerning, const IO::Path& relativePath) const final;
    void Read(BitmapFont& rBitmapFont, const IO::Path& relativePath) const final;
    bool TryReadAllText(std::string& rText, const IO::Path& relativePath) const final;
    bool TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                 const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                 const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const final;
    Bitmap ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                      const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const final;
    Texture ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                        const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                        const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined,
                        const bool generateMipMapsHint = false) const final;
    BitmapFont ReadBitmapFont(const IO::Path& relativePath) const final;

  private:
  };
}

#endif
