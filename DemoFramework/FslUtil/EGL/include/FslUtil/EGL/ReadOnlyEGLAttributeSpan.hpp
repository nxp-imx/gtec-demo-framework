#ifndef FSLUTIL_EGL_READONLYEGLATTRIBUTESPAN_HPP
#define FSLUTIL_EGL_READONLYEGLATTRIBUTESPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <EGL/egl.h>
#include <exception>

namespace Fsl::EGL
{
  class ReadOnlyEGLAttributeSpan
  {
    static const EGLint g_emptySpan = EGL_NONE;

    const ReadOnlySpan<EGLint> m_content;

    bool m_isHDRRequest{false};
    bool m_hasAlphaChannelRequest{false};

  public:
    explicit ReadOnlyEGLAttributeSpan()
      : m_content(&g_emptySpan, 1)
    {
    }

    explicit ReadOnlyEGLAttributeSpan(const ReadOnlySpan<EGLint> span)
      : m_content(span)
    {
      ValidateSpanContent(span);
      CacheChecks();
    }

    bool HasAlphaChannelRequest() const noexcept
    {
      return m_hasAlphaChannelRequest;
    }

    bool IsHDRRequest() const noexcept
    {
      return m_isHDRRequest;
    }

    const EGLint* data() const noexcept
    {
      return m_content.data();
    }

  private:
    void CacheChecks();

    static void ValidateSpanContent(const ReadOnlySpan<EGLint> span)
    {
      if (span.empty() || (span.size() % 2) == 0 || span[span.size() - 1] != EGL_NONE)
      {
        throw std::invalid_argument("EGLAttribute span was not in the expected format");
      }
    }
  };
}

#endif
