#ifndef FSLGRAPHICS_IO_BMPUTIL_HPP
#define FSLGRAPHICS_IO_BMPUTIL_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <fstream>

namespace Fsl
{
  //! The BMP loading capabilities are very limited!
  class BMPUtil : private Noncopyable
  {
  public:
    //! @brief Load bitmap from file
    //! @param rBitmap the bitmap that will be resized to fit the loaded bmp image.
    //! @param strFilename the file to load.
    static void Load(Bitmap& rBitmap, const IO::Path& strFilename);

    //! @brief Load bitmap from file
    //! @param rBitmap the bitmap that will be resized to fit the loaded bmp image.
    //! @param strFilename the file to load.
    //! @param originHint the bitmap origin that we would prefer to get the image in (but the load does not fail if the origin couldn't be obeyed).
    static void Load(Bitmap& rBitmap, const IO::Path& strFilename, const BitmapOrigin originHint);

    //! @brief Load bitmap from stream
    //! @param rBitmap the bitmap that will be resized to fit the loaded bmp image.
    //! @param stream the stream to load the bmp from
    static void Load(Bitmap& rBitmap, std::ifstream& rStream);

    //! @brief Load bitmap from stream
    //! @param rBitmap the bitmap that will be resized to fit the loaded bmp image.
    //! @param stream the stream to load the bmp from
    //! @param originHint the bitmap origin that we would prefer to get the image in (but the load does not fail if the origin couldn't be obeyed).
    static void Load(Bitmap& rBitmap, std::ifstream& rStream, const BitmapOrigin originHint);


    //! @brief Save bitmap to file
    //! @note Only supportes bitmaps in PixelFormat::B8G8R8A8_UINT format all other formats will
    //        throw a UnsupportedPixelFormatException.
    static void Save(const IO::Path& strFilename, const Bitmap& bitmap);

    //! @brief Save bitmap to file
    //! @note Only supportes bitmaps in PixelFormat::B8G8R8A8_UINT format all other formats will
    //        throw a UnsupportedPixelFormatException.
    static void Save(const IO::Path& strFilename, const RawBitmap& bitmap);

    //! @brief Save bitmap to file
    //! @note Only supportes bitmaps in PixelFormat::B8G8R8A8_UINT format all other formats will
    //        throw a UnsupportedPixelFormatException.
    static void Save(std::ofstream& stream, const Bitmap& bitmap);

    //! @brief Save bitmap to file
    //! @note Only supportes bitmaps in PixelFormat::B8G8R8A8_UINT format all other formats will
    //        throw a UnsupportedPixelFormatException.
    static void Save(std::ofstream& stream, const RawBitmap& bitmap);
  };
}

#endif
