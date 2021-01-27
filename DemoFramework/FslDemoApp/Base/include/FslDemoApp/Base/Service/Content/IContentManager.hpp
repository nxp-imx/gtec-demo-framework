#ifndef FSLDEMOAPP_BASE_SERVICE_CONTENT_ICONTENTMANAGER_HPP
#define FSLDEMOAPP_BASE_SERVICE_CONTENT_ICONTENTMANAGER_HPP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  class BitmapFont;
  class BasicFontKerning;
  class BasicTextureAtlas;

  class IContentManager
  {
  public:
    virtual ~IContentManager() = default;

    //! @brief Get the hosts preferred bitmap origin.
    virtual BitmapOrigin GetPreferredBitmapOrigin() const = 0;

    //! @brief Get the root path of all content (use this if you want to manually open content files)
    //! @return the content path.
    virtual IO::Path GetContentPath() const = 0;

    //! @brief Check if a content file exists
    virtual bool Exists(const IO::Path& relativePath) const = 0;

    //! @brief Get the length of the content file.
    //! @throws IOException if the file isn't found.
    virtual uint64_t GetLength(const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a string.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual std::string ReadAllText(const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual std::vector<uint8_t> ReadAllBytes(const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
    //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @return the number of bytes that was read.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual uint64_t ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual std::vector<uint8_t> ReadBytes(const IO::Path& relativePath) const = 0;

    //! @brief Read the requested content of the given file into a binary array.
    //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the
    //! file this throws a exception)
    //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const uint64_t fileOffset,
                           const uint64_t bytesToRead) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
    //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
    //! @param dstStartIndex the index inside pDstArray the write should start  (if < 0 or >= cbDstArray a exception will be thrown).
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the
    //! file this throws a exception)
    //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
    //! @return the number of bytes that was read.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual uint64_t ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& relativePath,
                               const uint64_t fileOffset, const uint64_t bytesToRead) const = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see
    //! GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and the image service is free to ignore it.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                      const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const = 0;

    //! @brief Read the content of the file as a texture.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the texture should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //!        (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see
    //! GetPreferredBitmapOrigin).
    //!        (if the source image uses a different origin it will be converted).
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //!        Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //!        The channel order is just a hint and the image service is free to ignore it.
    //! @param generateMipMapsHint = if true mip maps will be generated for the texture.
    //!                              The request is just a hint and the service is allowed to ignore it. This can easy be the case if
    //!                              the source texture is compressed or using a unsupported texture format.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                      const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined, const bool generateMipMapsHint = false) const = 0;

    //! @brief Try to read the content of the file as a BasicTextureAtlas.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    //! @throws if its a unsupported format.
    virtual void Read(BasicTextureAtlas& rTextureAtlas, const IO::Path& relativePath) const = 0;

    //! @brief Try to read the content of the file as a BasicFontKerning.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    //! @throws if its a unsupported format.
    virtual void Read(BasicFontKerning& rFontKerning, const IO::Path& relativePath) const = 0;


    //! @brief Try to read the content of the file as a BitmapFont.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    //! @throws if its a unsupported format.
    virtual void Read(BitmapFont& rBitmapFont, const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a string.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @return true if the file was read, false otherwise
    virtual bool TryReadAllText(std::string& rText, const IO::Path& relativePath) const FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //!        (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see
    //! GetPreferredBitmapOrigin).
    //!        (if the source image uses a different origin it will be converted).
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //!        Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //!        The channel order is just a hint and the image service is free to ignore it.
    //! @return true if the bitmap was loaded, false otherwise
    virtual bool TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                         const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                         const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //!        (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see
    //! GetPreferredBitmapOrigin).
    //!        (if the source image uses a different origin it will be converted).
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //!        Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //!        The channel order is just a hint and the image service is free to ignore it.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual Bitmap ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                              const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                              const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const = 0;

    //! @brief Read the content of the file as a texture.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the texture should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //!        (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see
    //! GetPreferredBitmapOrigin).
    //!        (if the source image uses a different origin it will be converted).
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //!        Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //!        The channel order is just a hint and the image service is free to ignore it.
    //! @param generateMipMapsHint = if true mip maps will be generated for the texture.
    //!                              The request is just a hint and the service is allowed to ignore it. This can easy be the case if
    //!                              the source texture is compressed or using a unsupported texture format.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual Texture ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                                const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                                const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined,
                                const bool generateMipMapsHint = false) const = 0;

    //! @brief Try to read the content of the file as a BitmapFont.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    //! @throws if its a unsupported format.
    virtual BitmapFont ReadBitmapFont(const IO::Path& relativePath) const = 0;
  };
}

#endif
