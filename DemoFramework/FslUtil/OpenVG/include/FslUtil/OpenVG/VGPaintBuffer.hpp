#ifndef FSLUTIL_OPENVG_VGPAINTBUFFER_HPP
#define FSLUTIL_OPENVG_VGPAINTBUFFER_HPP
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
#include <FslUtil/OpenVG/Common.hpp>
#include <FslBase/Noncopyable.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <VG/openvg.h>

namespace Fsl
{
  class Bitmap;

  namespace OpenVG
  {
    // A very simple wrapper for a VGPaint
    // FIX: this misses a constructor+reset method for creating the paint directly
    class VGPaintBuffer : private Noncopyable
    {
      VGPaint m_handle;

    public:
      //! @brief Create a uninitialized VGPaint
      VGPaintBuffer();

      //! @brief Let this VGPaintBuffer object assume control over the given VGPaint handle.
      //! @param handle the VGPaint handle
      VGPaintBuffer(const VGPaint handle);

      ~VGPaintBuffer();

      //! @brief Check if this contains a valid handle.
      bool IsValid() const
      {
        return m_handle != VG_INVALID_HANDLE;
      }

      //! @brief If a VGPaint is allocated this will releases it.
      void Reset();

      //! @brief Let this VGPaintBuffer object assume control over the given vgPaint handle.
      //! @param handle the VGPaint handle
      //! @param size the size of the paint
      void Reset(const VGPaint handle);

      //! @brief Get the vgpaint handle associated with the VGPaint Buffer
      //! @return the handle or VG_INVALID_HANDLE if the VGImageBuffer is unallocated.
      VGPaint GetHandle() const;
    };
  }
}

#endif
