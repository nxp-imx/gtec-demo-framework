#ifndef FSLDEMOAPP_BASE_SERVICE_IMAGELIBRARY_IIMAGELIBRARYSERVICE_HPP
#define FSLDEMOAPP_BASE_SERVICE_IMAGELIBRARY_IIMAGELIBRARYSERVICE_HPP
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
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/ImageFormat.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslBase/IO/Path.hpp>
#include <deque>

namespace Fsl
{
  class Bitmap;
  class Texture;

  //! @brief A image service is a optional service and there can be multiple image library services.
  //! @note Future incarnations of this should improve the save interface so its possible to query the supported image formats and
  //        the pixel formats and bitmap origin that these formats support.
  class IImageLibraryService
  {
  public:
    virtual ~IImageLibraryService() = default;

    //! @brief Get the name of the library service
    virtual std::string GetName() const = 0;

    //! @brief Get a list of formats that this library supports.
    //! @note  The library is not required to list all supported formats here, but it can help optimize things a bit.
    virtual void ExtractSupportedImageFormats(std::deque<ImageFormat>& rFormats) = 0;

    //! @brief Try to read the content of the file as a bitmap.
    //! @param path the path to load the file from
    //! @param pixelFormatHint the pixel format that we would prefer to get the image in (but the load does not fail if the pixel format couldn't be
    //! obeyed).
    //!        If this is set to PixelFormat::Undefined it means we prefer to get it in the source format.
    //! @param originHint the bitmap origin that we would prefer to get the image in (but the load does not fail if the origin couldn't be obeyed).
    //!        If this is set to BitmapOrigin::Undefined it means we prefer to get it in the source origin
    //! @param preferredChannelOrderHint this is only used if pixelFormatHint is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and a library is free to ignore it.
    //! @return true on success, false if the image failed to load (for any reason)
    virtual bool TryRead(Bitmap& rBitmap, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                         const PixelChannelOrder preferredChannelOrderHint) FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT = 0;

    //! @brief Try to read the content of the file as a texture.
    //! @param path the path to load the file from
    //! @param pixelFormatHint the pixel format that we would prefer to get the image in (but the load does not fail if the pixel format couldn't be
    //! obeyed).
    //!        If this is set to PixelFormat::Undefined it means we prefer to get it in the source format.
    //! @param originHint the bitmap origin that we would prefer to get the image in (but the load does not fail if the origin couldn't be obeyed).
    //!        If this is set to BitmapOrigin::Undefined it means we prefer to get it in the source origin
    //! @param preferredChannelOrderHint this is only used if pixelFormatHint is PixelFormat::Undefined.
    //         Informs the image library of the preferred channel ordering when loading content using a undefined pixel-format.
    //         The channel order is just a hint and a library is free to ignore it.
    //! @return true on success, false if the image failed to load (for any reason)
    virtual bool TryRead(Texture& rTexture, const IO::Path& absolutePath, const PixelFormat pixelFormatHint, const BitmapOrigin originHint,
                         const PixelChannelOrder preferredChannelOrderHint) FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT = 0;

    //! @brief Try to write the bitmap to the file.
    //! @param path the file the bitmap should be saved to.
    //! @param bitmap the bitmap to write
    //! @param imageFormat the desired image format (if imageFormat == ImageFormat::Undefied the service can try to determine the format from the
    //! filename extension, but it does not have to)
    //! @param allowOverwrite if true any existing file is overwritten if false the write fails
    //! @return true if the bitmap was saved.
    virtual bool TryWrite(const IO::Path& absolutePath, const Bitmap& bitmap, const ImageFormat imageFormat,
                          const bool allowOverwrite) FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT = 0;
  };
}

#endif
