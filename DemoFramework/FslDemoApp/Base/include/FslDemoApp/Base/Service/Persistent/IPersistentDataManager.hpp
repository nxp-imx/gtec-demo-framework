#ifndef FSLDEMOAPP_BASE_SERVICE_PERSISTENT_IPERSISTENTDATAMANAGER_HPP
#define FSLDEMOAPP_BASE_SERVICE_PERSISTENT_IPERSISTENTDATAMANAGER_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <vector>

namespace Fsl
{

  // FIX: add easy way to chose if a existing file should be overwritten
  class IPersistentDataManager
  {
  public:
    virtual ~IPersistentDataManager() {}

    //! @brief Get the root path of all persistent data (use this if you want to manually open persistent files)
    //! @return the content path.
    virtual IO::Path GetPersistentDataPath() const = 0;

    //! @brief Check if a persistent file exists
    virtual bool Exists(const IO::Path& relativePath) const = 0;

    //! @brief Get the length of the persistent file.
    //! @throws IOException if the file isn't found.
    virtual uint64_t GetLength(const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a string.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual std::string ReadAllText(const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void ReadAllBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
    //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @return the number of bytes that was read.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual uint64_t ReadAllBytes(void* pDstArray, const uint64_t cbDstArray, const IO::Path& relativePath) const = 0;

    //! @brief Read the requested content of the given file into a binary array.
    //! @param rTargetArray the array to load the content into. The array will be resized to fit the file content
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the file this throws a exception)
    //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void ReadBytes(std::vector<uint8_t>& rTargetArray, const IO::Path& relativePath, const uint64_t fileOffset, const uint64_t bytesToRead) const = 0;

    //! @brief Read the entire content of the given file into a binary array.
    //! @param pDstArray the array to load the content into (if == nullptr a exception will be thrown)
    //! @param cbDstArray the size of pDstArray in bytes (if too small to fit the fill a exception will be thrown).
    //! @param dstStartIndex the index inside pDstArray the write should start  (if < 0 or >= cbDstArray a exception will be thrown).
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @param fileOffset the offset from the beginning of the file where the read should occur (if less than zero or greater than the length of the file this throws a exception)
    //! @param bytesToRead the number of bytes to read (if the end of file is encountered the read operation will fail with a exception)
    //! @return the number of bytes that was read.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual uint64_t ReadBytes(void* pDstArray, const uint64_t cbDstArray, const uint64_t dstStartIndex, const IO::Path& relativePath, const uint64_t fileOffset, const uint64_t bytesToRead) const = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined, const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const = 0;

    //! @brief Read the content of the file as a texture.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the texture should be using. If this is PixelFormat::Undefined then the source image's format is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Texture& rTexture, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined, const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const = 0;

    //! @brief Write the content to a file
    //! @param relativePath the path to save the file to
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @note If the target file exists its overwritten
    //! @throws IOException if the file isn't found or something goes wrong writing it.
    virtual void WriteAlltext(const IO::Path& relativePath, const std::string& content) = 0;

    //! @brief Write the content to a file
    //! @param relativePath the path to save the file to
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @note If the target file exists its overwritten
    //! @throws IOException if the file isn't found or something goes wrong writing it.
    virtual void WriteAllBytes(const IO::Path& relativePath, const std::vector<uint8_t>& content) = 0;

    //! @brief Save the bitmap to a file of ImageFormat type and
    //!        the pixel format stored in the file is the one best matching the the bitmap pixel format.
    //! @param relativePath the path to load the file from
    //         (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to. If ImageFormat::Undefined then the filename extension is used.
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving If its PixelFormat::Undefined the bitmap pixelformat is used. If any conversion fails a exception is thrown.
    //! @note If the target file exists its overwritten
    virtual void Write(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat = ImageFormat::Undefined, const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;

    //! @brief Save the bitmap to a file of ImageFormat type using the specified pixel format.
    //!        Use this for exact control of what is saved.
    //!        For most use cases the standard 'Write' is a better match.
    //! @param relativePath the path to load the file from
    //         (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to.
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving. If its PixelFormat::Undefined the bitmap pixelformat is used. If any conversion fails a exception is thrown.
    //! @note If the target file exists its overwritten
    //!       If the ImageFormat doesn't support the bitmap pixel format a exception is thrown.
    virtual void WriteExactImage(const IO::Path& relativePath, const Bitmap& bitmap, const ImageFormat imageFormat, const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;

    //! @brief Write the content to a file
    //! @param relativePath the path to load the file from
    //         (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @param pContent the array to write to a file. (if == nullptr a exception will be thrown)
    //! @param cbContent the size of the pContent array in bytes.
    //! @throws IOException if the file isn't found or something goes wrong writing it.
    //virtual void WriteFile(const IO::Path& relativePath, const void*const pContent, const uint64_t cbContent) = 0;

    //! @brief Try to read the entire content of the given file into a string.
    //! @param relativePath the relative path to load the content from
    //!        (the path is expected to be relative and will be concatenated with the GetPersistentDataPath automatically)
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual bool TryReadAllText(std::string& rDst, const IO::Path& relativePath) const = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @return true if the bitmap was loaded, false otherwise
    virtual bool TryRead(Bitmap& rBitmap, const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined, const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const = 0;

    //! @brief Read the content of the file as a bitmap.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual Bitmap ReadBitmap(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined, const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const = 0;

    //! @brief Read the content of the file as a texture.
    //! @param relativePath the relative path to load the content from
    //         (the path is expected to be relative and will be concatenated with the GetContentPath automatically)
    //! @param desiredPixelFormat the pixel format that the texture should be using. If this is PixelFormat::Undefined then the source image's format is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined hosts default is used (see GetPreferredBitmapOrigin).
    //         (if the source image uses a different origin it will be converted).
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual Texture ReadTexture(const IO::Path& relativePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined, const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined) const = 0;
  };
}

#endif
