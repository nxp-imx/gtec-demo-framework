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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Service/Graphics/Basic2D.hpp>
#include <FslDemoHost/Service/NativeGraphics/INativeGraphicsBasic2D.hpp>
#include <cstring>
#include <limits>

namespace Fsl
{
  Basic2D::Basic2D(const std::shared_ptr<INativeGraphicsBasic2D>& native)
    : m_native(native)
    , m_fontSize()
    , m_inBegin(false)
  {
    if (!m_native)
      throw std::invalid_argument("the native implementation can not be null");
    m_fontSize = m_native->FontSize();
  }


  void Basic2D::Begin()
  {
    if (m_inBegin)
      throw UsageErrorException("Already in a begin block");

    m_native->Begin();
    m_inBegin = true;
  }


  void Basic2D::End()
  {
    if (! m_inBegin)
      throw UsageErrorException("Not in a begin block");

    m_inBegin = false;
    m_native->End();
  }


  void Basic2D::DrawPoints(const Vector2*const pSrc, const int32_t length, const Color& color)
  {
    if (!m_inBegin)
      throw UsageErrorException("Not in a begin block");

    m_native->DrawPoints(pSrc, length, color);
  }


  void Basic2D::DrawString(const char*const psz, const Vector2& dstPosition)
  {
    if (!m_inBegin)
      throw UsageErrorException("Not in a begin block");

    if (psz == nullptr)
    {
      FSLLOG_WARNING("psz is null");
      return;
    }
    const auto len = strlen(psz);
    if (len > std::numeric_limits<uint32_t>::max())
      throw NotSupportedException("string is too long");

    DrawString(psz, 0, static_cast<uint32_t>(len), dstPosition);
  }


  void Basic2D::DrawString(const std::string& str, const Vector2& dstPosition)
  {
    if (!m_inBegin)
      throw UsageErrorException("Not in a begin block");

    if (str.size() > std::numeric_limits<uint32_t>::max())
      throw NotSupportedException("string is too long");
    DrawString(str.c_str(), 0, static_cast<uint32_t>(str.size()), dstPosition);
  }


  void Basic2D::DrawString(const char*const characters, const uint32_t startIndex, const uint32_t length, const Vector2& dstPosition)
  {
    if (!m_inBegin)
      throw UsageErrorException("Not in a begin block");

    if (characters == nullptr)
    {
      FSLLOG_WARNING("invalid arguments");
      return;
    }
    m_native->DrawString(characters + startIndex, length, dstPosition);
  }


  const Point2 Basic2D::FontSize() const
  {
    return m_fontSize;
  }
}
