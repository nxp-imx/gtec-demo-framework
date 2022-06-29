#ifndef FSLUTIL_OPENVG_VGIMAGEBUFFER_HPP
#define FSLUTIL_OPENVG_VGIMAGEBUFFER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslUtil/OpenVG/Common.hpp>
#include <VG/openvg.h>

namespace Fsl
{
  class Bitmap;

  namespace OpenVG
  {
    // A very simple wrapper for a VGImage
    class VGImageBuffer
    {
      VGImage m_handle;
      PxSize2D m_size;

    public:
      VGImageBuffer(const VGImageBuffer&) = delete;
      VGImageBuffer& operator=(const VGImageBuffer&) = delete;

      //! @brief Create a uninitialized VGImageBuffer (use Reset to add texture data to it)
      VGImageBuffer();

      //! @brief Let this VGImageBuffer object assume control over the given VGImage handle.
      //! @param handle the VGImage handle
      //! @param size the size of the image
      VGImageBuffer(const VGImage handle, const PxSize2D& size);

      //! @brief Fill it with the provided bitmap.
      VGImageBuffer(const Bitmap& bitmap, const VGbitfield quality);

      //! @brief Fill it with the provided bitmap.
      VGImageBuffer(const RawBitmap& bitmap, const VGbitfield quality);

      ~VGImageBuffer();

      //! @brief Check if this contains a valid handle.
      bool IsValid() const
      {
        return m_handle != VG_INVALID_HANDLE;
      }


      //! @brief If a VGImage is allocated this will releases it.
      void Reset();

      //! @brief Let this VGImageBuffer object assume control over the given vgimage handle.
      //! @param handle the VGImage handle
      //! @param size the size of the image
      void Reset(const VGImage handle, const PxSize2D& size);

      //! @brief Free any existing image, then reallocate and fill it with the provided bitmap
      void Reset(const Bitmap& bitmap, const VGbitfield quality);

      //! @brief Free any existing image, then reallocate and fill it with the provided bitmap
      void Reset(const RawBitmap& bitmap, const VGbitfield quality);

      //! @brief Get the vgimage handle associated with the VGImageBuffer
      //! @return the handle or VG_INVALID_HANDLE if the VGImageBuffer is unallocated.
      VGImage GetHandle() const;

      //! @brief Get the image size.
      PxSize2D GetSize() const;
    };
  }
}

#endif
