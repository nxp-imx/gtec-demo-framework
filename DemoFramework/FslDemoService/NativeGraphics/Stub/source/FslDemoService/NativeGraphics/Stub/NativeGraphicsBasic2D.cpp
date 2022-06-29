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

#include "NativeGraphicsBasic2D.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/EmbeddedFont8x8.hpp>
#include <cassert>

namespace Fsl::Stub
{
  NativeGraphicsBasic2D::NativeGraphicsBasic2D(const PxExtent2D& extentPx, const bool showWarning)
    : m_pxCurrentExtent(extentPx)
    , m_fontSize(EmbeddedFont8x8::CharacterSize())
    , m_inBegin(false)
  {
    FSLLOG3_WARNING_IF(showWarning, "NativeGraphicsBasic2D is a stub");
  }


  NativeGraphicsBasic2D::~NativeGraphicsBasic2D() = default;


  void NativeGraphicsBasic2D::SetScreenExtent(const PxExtent2D& extentPx)
  {
    assert(!m_inBegin);
    m_pxCurrentExtent = extentPx;
  }


  void NativeGraphicsBasic2D::Begin()
  {
    assert(!m_inBegin);
    m_inBegin = true;
  }


  void NativeGraphicsBasic2D::End()
  {
    assert(m_inBegin);
    m_inBegin = false;
  }


  void NativeGraphicsBasic2D::DrawPoints(const Vector2* const pDst, const uint32_t /*length*/, const Color& /*color*/)
  {
    FSL_PARAM_NOT_USED(pDst);
    assert(m_inBegin);
    assert(pDst != nullptr);
  }


  void NativeGraphicsBasic2D::DrawString(const StringViewLite& /*strView*/, const Vector2& /*dstPosition*/)
  {
    assert(m_inBegin);
  }


  PxSize2D NativeGraphicsBasic2D::FontSize() const
  {
    return m_fontSize;
  }
}
