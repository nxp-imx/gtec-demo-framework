#ifndef FSLGRAPHICS_BITMAP_RAWBITMAPUTIL_HPP
#define FSLGRAPHICS_BITMAP_RAWBITMAPUTIL_HPP
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

#include <FslGraphics/Bitmap/RawBitmap.hpp>

namespace Fsl
{
  class RawBitmapEx;

  //! @brief Contains helper methods for working on bitmaps
  //         They exist to provide functionality but they are not performance optimized!
  class RawBitmapUtil
  {
  public:
    //! @brief Given a raw bitmap calculate the row alignment that fits the stride or throw a UnsupportedStrideException if
    //         none fits (the alignment is the minimum padding necessary after a pixel row to start the next one at the given alignment).
    static int CalcAlignment(const RawBitmap& rawBitmap, const uint32_t bytesPerPixel)
    {
      return CalcAlignment(rawBitmap.GetPixelFormat(), rawBitmap.Width(), rawBitmap.Stride(), bytesPerPixel);
    }

    //! @brief Given a raw bitmap calculate the row alignment that fits the stride or throw a UnsupportedStrideException if
    //         none fits (the alignment is the minimum padding necessary after a pixel row to start the next one at the given alignment).
    static int CalcAlignment(const PixelFormat pixelFormat, const uint32_t width, const uint32_t stride, const uint32_t bytesPerPixel);

    //! @brief Check if the given bitmap uses the minimum stride for the requested alignment
    //! @throw a UnsupportedStrideException if not
    static void CheckIsUsingMinimumStrideForAlignment(const RawBitmap& rawBitmap, const uint32_t alignment);

    //! @brief Swizzle the color channels in a 24 or 32bpp image according to the supplied rules
    //! @param rBitmap the bitmap to swizzle.
    //! @param srcIdx0 the location to read 'byte0' from (0-3 32bpp) (0-2 24bpp)
    //! @param srcIdx1 the location to read 'byte1' from (0-3 32bpp) (0-2 24bpp)
    //! @param srcIdx2 the location to read 'byte2' from (0-3 32bpp) (0-2 24bpp)
    //! @param srcIdx3 the location to read 'byte3' from (0-3 32bpp) (ignored for 24bpp)
    //! @note Only R8G8B8_UINT, B8G8R8_UINT, R8G8B8A8_UINT and B8G8R8A8_UINT formats are supported at the moment, throws
    //! UnsupportedPixelFormatException if the pixel format is unsupported
    static void Swizzle(RawBitmapEx& rBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1, const uint32_t srcIdx2, const uint32_t srcIdx3);

    //! @brief Swizzle the color channels in a 24bpp image according to the supplied rules
    //! @param rBitmap the bitmap to swizzle.
    //! @note Works on any 3 bytes per pixel format, but the validity is up to the caller to ensure.
    static void Swizzle24From012To210(RawBitmapEx& rBitmap);

    //! @brief Swizzle the color channels in a 24bpp image according to the supplied rules
    //! @param rBitmap the bitmap to swizzle.
    //! @param srcIdx0 the location to read 'byte0' from (0-2)
    //! @param srcIdx1 the location to read 'byte1' from (0-2)
    //! @param srcIdx2 the location to read 'byte2' from (0-2)
    //! @note Works on any 3 bytes per pixel format, but the validity is up to the caller to ensure.
    static void Swizzle24(RawBitmapEx& rBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1, const uint32_t srcIdx2);


    //! @brief Swizzle the color channels in a 32bpp image according to the supplied rules
    //! @param rBitmap the bitmap to swizzle.
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @param srcIdx3 the location to read 'byte3' from (0-3)
    //! @note Works on any 4 bytes per pixel format, but the validity is up to the caller to ensure.
    static void Swizzle32(RawBitmapEx& rBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1, const uint32_t srcIdx2, const uint32_t srcIdx3);

    //! @brief Swizzle the color channels in a 32bpp image into a 8bit format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @note Only R8G8B8A8_UINT and B8G8R8A8_UINT formats are supported at the moment, throws UnsupportedPixelFormatException if the pixel format is
    //! unsupported
    static void Swizzle32To8(RawBitmapEx& rBitmap, const PixelFormat dstPixelFormat, const uint32_t dstStride, const uint32_t srcIdx0);

    //! @brief Swizzle the color channels in a 32bpp image into a 24bit three channel format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 4 bytes per pixel format to 3 bytes per pixel formats, but the validity is up to the caller to ensure.
    static void Swizzle32To24(RawBitmapEx& rBitmap, const PixelFormat dstPixelFormat, const uint32_t dstStride, const uint32_t srcIdx0,
                              const uint32_t srcIdx1, const uint32_t srcIdx2);

    //! @brief Average the content of three of the 32bpp images four channels into a 8bit format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 3 bytes per pixel format to 1 bytes per pixel formats, but the validity is up to the caller to ensure.
    static void Average24To8(RawBitmapEx& rBitmap, const PixelFormat dstPixelFormat, const uint32_t dstStride, const uint32_t srcIdx0,
                             const uint32_t srcIdx1, const uint32_t srcIdx2);

    //! @brief Average the content of three of the 32bpp images four channels into a 8bit format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 4 bytes per pixel format to 1 bytes per pixel formats, but the validity is up to the caller to ensure.
    static void Average32To8(RawBitmapEx& rBitmap, const PixelFormat dstPixelFormat, const uint32_t dstStride, const uint32_t srcIdx0,
                             const uint32_t srcIdx1, const uint32_t srcIdx2);

    //! @brief Zero fill the padding area if any exist (the padding area is the area minimum stride that is possible and stride that is being used)
    static void ClearPadding(RawBitmapEx& rBitmap);

    //! @brief Flip the bitmap about the y-axis
    static void FlipHorizontal(RawBitmapEx& rBitmap);

    //! @brief Copies the pixels from srcBitmap to rDstBitmap.
    //!        This just copies the memory from one bitmap to the other no transformations are done.
    //!        The only verification this does is that the bytes per pixel matches and the bitmaps are valid and that width, height and origin
    //!        matches.
    //! @note  If the memory buffers of the bitmaps overlap the result is undefined.
    static void MemoryCopy(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);

    //! @brief Swizzle the color channels in a 24bpp image according to the supplied rules
    //! @param rDstBitmap the bitmap to swizzle to.
    //! @param srcBitmap the bitmap to swizzle from.
    //! @note Works on any 3 bytes per pixel format, but the validity is up to the caller to ensure.
    //        This works on overlapping buffers as long as rDstBitmap.Content == srcBitmap.Content and srcBitmap.Stride >= rDstBitmap.Stride
    static void Swizzle24From012To210(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);


    //! @brief Swizzle the color channels in a 24bpp image according to the supplied rules
    //! @param rDstBitmap the bitmap to swizzle to.
    //! @param srcBitmap the bitmap to swizzle from.
    //! @param srcIdx0 the location to read 'byte0' from (0-2)
    //! @param srcIdx1 the location to read 'byte1' from (0-2)
    //! @param srcIdx2 the location to read 'byte2' from (0-2)
    //! @note Works on any 3 bytes per pixel format, but the validity is up to the caller to ensure.
    //        This works on overlapping buffers as long as rDstBitmap.Content == srcBitmap.Content and srcBitmap.Stride >= rDstBitmap.Stride
    static void Swizzle24(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1,
                          const uint32_t srcIdx2);

    //! @brief Swizzle the color channels in a 32bpp image according to the supplied rules
    //! @param rBitmap the bitmap to swizzle.
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @param srcIdx3 the location to read 'byte3' from (0-3)
    //! @note Works on any 4 bytes per pixel format, but the validity is up to the caller to ensure.
    //        This works on overlapping buffers as long as rDstBitmap.Content == srcBitmap.Content and srcBitmap.Stride >= rDstBitmap.Stride
    static void Swizzle32(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1, const uint32_t srcIdx2,
                          const uint32_t srcIdx3);

    //! @brief Swizzle the color channels in a 32bpp image into a 24bit three channel format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 4 bytes per pixel format to 3 bytes per pixel formats, but the validity is up to the caller to ensure.
    //        This does not work on overlapping buffers at all!
    static void Swizzle32To24(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1,
                              const uint32_t srcIdx2);

    //! @brief Swizzle the color channels in a 32bpp image into a 24bit three channel format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstIdx0 the location to write 'byte0' to (0-3)
    //! @param dstIdx1 the location to write 'byte1' to (0-3)
    //! @param dstIdx2 the location to write 'byte2' to (0-3)
    //! @param dstIdx3 the location to write value3 to (0-3)
    //! @note Works on any 4 bytes per pixel format to 3 bytes per pixel formats, but the validity is up to the caller to ensure.
    //        This does not work on overlapping buffers at all!
    static void Swizzle24To32(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t dstIdx0, const uint32_t dstIdx1,
                              const uint32_t dstIdx2, const uint32_t dstIdx3, const uint8_t value3);

    //! @brief Average the content of three of the 32bpp images four channels into a 8bit format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 3 byte format, but the validity is up to the caller to ensure.
    //        This works on overlapping buffers as long as rDstBitmap.Content == srcBitmap.Content and srcBitmap.Stride >= rDstBitmap.Stride
    static void Average24To8(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1,
                             const uint32_t srcIdx2);

    //! @brief Average the content of three of the 32bpp images four channels into a 8bit format using the dstStride to write the new image
    //! @param rBitmap the bitmap to swizzle.
    //! @param dstStride must be >= 0 and <= rBitmap.Stride()
    //! @param srcIdx0 the location to read 'byte0' from (0-3)
    //! @param srcIdx1 the location to read 'byte1' from (0-3)
    //! @param srcIdx2 the location to read 'byte2' from (0-3)
    //! @note Works on any 4 byte format, but the validity is up to the caller to ensure.
    //        This works on overlapping buffers as long as rDstBitmap.Content == srcBitmap.Content and srcBitmap.Stride >= rDstBitmap.Stride
    static void Average32To8(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap, const uint32_t srcIdx0, const uint32_t srcIdx1,
                             const uint32_t srcIdx2);

    //! @brief Expands a 1byte format into the number of bytes that the rDstBitmap pixel format uses.
    //! @note Works on any 1 byte format, but the validity is up to the caller to ensure.
    //        This does not work on overlapping buffers.
    static void Expand1ByteToNBytes(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);

    //! @brief Downscales a image using nearest neighbor
    static void DownscaleNearest(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);
    static void DownscaleNearest32(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);

    //! @brief Downscale a image using a simple box filter
    //! @note  The rDstBitmap is expected to be half the size of the srcBitmap and must use the same pixel format and origin.
    static void DownscaleBoxFilter(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);

    //! @brief Downscale a image using a simple box filter (only works on 32bit images with four 8bit channels)
    //! @note  The rDstBitmap is expected to be half the size of the srcBitmap and must use the same pixel format and origin.
    static void DownscaleBoxFilter32(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap);
  };
}

#endif
