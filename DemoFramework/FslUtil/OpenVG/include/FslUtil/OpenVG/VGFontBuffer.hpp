#ifndef FSLUTIL_OPENVG_VGFONTBUFFER_HPP
#define FSLUTIL_OPENVG_VGFONTBUFFER_HPP
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
#include <VG/openvg.h>
#include <FslBase/Noncopyable.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <vector>

namespace Fsl
{
  namespace OpenVG
  {
    class VGFontBuffer : private Noncopyable
    {
      VGFont m_font;
    public:
      //! @brief Create a uninitialized path buffer
      VGFontBuffer();

      //! @brief Create a initialized path buffer
      VGFontBuffer(const int32_t capacityHint);

      ~VGFontBuffer();

      //! @brief If a buffer is allocated this will releases it.
      void Reset();

      //! @brief Reset the buffer to the suggested capacity
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer internally)
      void Reset(const int32_t capacityHint);

      VGPath GetHandle() const;
    };
  }
}

#endif
