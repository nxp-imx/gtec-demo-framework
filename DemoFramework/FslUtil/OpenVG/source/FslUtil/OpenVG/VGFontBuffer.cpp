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
#include <FslUtil/OpenVG/VGCheck.hpp>
#include <FslUtil/OpenVG/VGFontBuffer.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace OpenVG
  {



    VGFontBuffer::VGFontBuffer()
      : m_font(VG_INVALID_HANDLE)
    {
    }


    VGFontBuffer::VGFontBuffer(const int32_t capacityHint)
      : m_font(VG_INVALID_HANDLE)
    {
      Reset(capacityHint);
    }


    VGFontBuffer::~VGFontBuffer()
    {
      Reset();
    }


    void VGFontBuffer::Reset()
    {
      if (m_font != VG_INVALID_HANDLE)
      {
        vgDestroyFont(m_font);
        m_font = VG_INVALID_HANDLE;
      }
    }


    void VGFontBuffer::Reset(const int32_t capacityHint)
    {
      if (m_font != VG_INVALID_HANDLE)
      {
        vgDestroyFont(m_font);
        m_font = VG_INVALID_HANDLE;
      }

      m_font = vgCreateFont(capacityHint);
      FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
    }


    VGFont VGFontBuffer::GetHandle() const
    {
      return m_font;
    }
  }
}
