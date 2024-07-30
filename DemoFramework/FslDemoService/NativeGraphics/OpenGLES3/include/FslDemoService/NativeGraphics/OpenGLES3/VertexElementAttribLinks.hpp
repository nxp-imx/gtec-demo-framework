#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_VERTEXELEMENTATTRIBLINKS_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_VERTEXELEMENTATTRIBLINKS_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/VertexAttribConfig.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES3/GLVertexElementAttribConfig.hpp>
#include <array>

namespace Fsl
{
  class VertexDeclarationSpan;

  namespace GLES3
  {
    class VertexElementAttribLinks
    {
      struct InlineArray
      {
        std::array<GLVertexElementAttribConfig, VertexAttribConfig::MaxAttribCount> Entries{};
        uint32_t Count{0};
      };

      InlineArray m_array;
      uint32_t m_vertexStride{};

    public:
      VertexElementAttribLinks() noexcept = default;

      VertexElementAttribLinks(const VertexDeclarationSpan& vertexDeclaration, const ReadOnlySpan<GLVertexAttribLink> attribLinks);
      VertexElementAttribLinks(const ReadOnlySpan<GLVertexElementAttribConfig>& vertexElementAttribConfigs, const uint32_t vertexStride);

      bool IsCompatible(const VertexElementAttribLinks& instance) const noexcept;

      ReadOnlySpan<GLVertexElementAttribConfig> AsSpan() const noexcept
      {
        return SpanUtil::UncheckedAsReadOnlySpan(m_array.Entries, 0, m_array.Count);
      }

      uint32_t VertexStride() const noexcept
      {
        return m_vertexStride;
      }

    private:
      static InlineArray ToArray(const VertexDeclarationSpan& vertexDeclaration, const ReadOnlySpan<GLVertexAttribLink> attribLinks);
      static InlineArray ToArray(const ReadOnlySpan<GLVertexElementAttribConfig>& vertexElementAttribConfigs);
    };
  }
}

#endif
