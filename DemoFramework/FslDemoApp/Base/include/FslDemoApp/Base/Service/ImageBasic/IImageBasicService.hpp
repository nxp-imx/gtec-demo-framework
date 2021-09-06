#ifndef FSLDEMOAPP_BASE_SERVICE_IMAGEBASIC_IIMAGEBASICSERVICE_HPP
#define FSLDEMOAPP_BASE_SERVICE_IMAGEBASIC_IIMAGEBASICSERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslBase/IO/Path.hpp>

namespace Fsl
{
  class Bitmap;
  class Texture;

  //! @brief The image basic service is always present. It uses various ImageLibraryServices to do the actual loading
  class IImageBasicService
  {
  public:
    virtual ~IImageBasicService() = default;

    //! @brief Read the content of the file as a bitmap.
    //! @param absolutePath the absolute path to load the content from (a relative path will be treated as a error)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined the source image origin will be used.
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and the image service is free to ignore it.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                      const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const = 0;


    //! @brief Read the content of the file as a texture.
    //! @param absolutePath the absolute path to load the content from (a relative path will be treated as a error)
    //! @param desiredPixelFormat the pixel format that the texture should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined the source image origin will be used.
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and the image service is free to ignore it.
    //! @throws IOException if the file isn't found or something goes wrong reading it.
    virtual void Read(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                      const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                      const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const = 0;

    //! @brief Save the bitmap to a file of ImageFormat type and
    //!        the pixel format stored in the file is the one best matching the the bitmap pixel format.
    //! @param absolutePath the absolute path to save the content to (a relative path will be treated as a error)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to. If ImageFormat::Undefined then the filename extension is used.
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving If its PixelFormat::Undefined the bitmap pixelformat is used.
    //! If any conversion fails a exception is thrown.
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined the source image origin will be used.
    //! @note If the target file exists its overwritten
    virtual void Write(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat = ImageFormat::Undefined,
                       const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;

    //! @brief Save the bitmap to a file of ImageFormat type using the specified pixel format.
    //!        Use this for exact control of what is saved.
    //!        For most use cases the standard 'Write' is a better match.
    //! @param absolutePath the absolute path to save the content to (a relative path will be treated as a error)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to.
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving. If its PixelFormat::Undefined the bitmap pixelformat is used.
    //! If any conversion fails a exception is thrown.
    //! @note If the target file exists its overwritten
    //!       If the ImageFormat doesn't support the bitmap pixel format a exception is thrown.
    virtual void WriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                 const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;


    //! @brief Read the content of the file as a bitmap.
    //! @param absolutePath the absolute path to load the content from (a relative path will be treated as a error)
    //! @param desiredPixelFormat the pixel format that the bitmap should be using. If this is PixelFormat::Undefined then the source image's format
    //! is used.
    //         (if the source image uses a different pixel format it will be converted to the desiredPixelFormat if possible).
    //! @param desiredOrigin the origin that should be used for the bitmap. If this is BitmapOrigin::Undefined the source image origin will be used.
    //! @param preferredChannelOrder this is only used if desiredPixelFormat is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and the image service is free to ignore it.
    //! @return true if the bitmap was loaded, false otherwise
    FSL_FUNC_WARN_UNUSED_RESULT virtual bool TryRead(Bitmap& rBitmap, const IO::Path& absolutePath,
                                                     const PixelFormat desiredPixelFormat = PixelFormat::Undefined,
                                                     const BitmapOrigin desiredOrigin = BitmapOrigin::Undefined,
                                                     const PixelChannelOrder preferredChannelOrder = PixelChannelOrder::Undefined) const = 0;


    //! @brief Save the bitmap to a file of ImageFormat type and
    //!        the pixel format stored in the file is the one best matching the the bitmap pixel format.
    //! @param absolutePath the absolute path to save the content to (a relative path will be treated as a error)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to. If ImageFormat::Undefined then the filename extension is used
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving. If its PixelFormat::Undefined the bitmap pixelformat is used.
    //! @return true if the bitmap was saved, false otherwise
    //! @note If the target file exists its overwritten
    FSL_FUNC_WARN_UNUSED_RESULT virtual bool TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap,
                                                      const ImageFormat imageFormat = ImageFormat::Undefined,
                                                      const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;

    //! @brief Save the bitmap to a file of ImageFormat type using the specified pixel format.
    //!        Use this for exact control of what is saved.
    //!        For most use cases the standard 'Write' is a better match.
    //! @param absolutePath the absolute path to save the content to (a relative path will be treated as a error)
    //! @param bitmap the bitmap to save.
    //! @param imageFormat the desired image format to save to.
    //! @param desiredPixelFormat the desired pixel format of the bitmap before saving. If its PixelFormat::Undefined the bitmap pixelformat is used.
    //! @return true if the bitmap was saved, false otherwise
    //! @note If the target file exists its overwritten
    FSL_FUNC_WARN_UNUSED_RESULT virtual bool TryWriteExactImage(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                                                                const PixelFormat desiredPixelFormat = PixelFormat::Undefined) = 0;
  };
}

#endif
