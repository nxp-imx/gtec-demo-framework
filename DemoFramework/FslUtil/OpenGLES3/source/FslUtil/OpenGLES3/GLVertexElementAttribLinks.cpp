/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES3/GLVertexElementAttribLinks.hpp>
#include <FslUtil/OpenGLES3/GLVertexElements.hpp>
#include <algorithm>

namespace Fsl::GLES3
{
  GLVertexElementAttribLinks::GLVertexElementAttribLinks(const VertexDeclarationSpan& vertexDeclaration,
                                                         const ReadOnlySpan<GLVertexAttribLink> attribLinks)
    : m_entries(GLVertexElements(vertexDeclaration).ExtractConfiguration(attribLinks))
    , m_vertexStride(vertexDeclaration.VertexStride())
  {
  }

  GLVertexElementAttribLinks::GLVertexElementAttribLinks(const ReadOnlySpan<GLVertexElementAttribConfig>& vertexElementAttribConfigs,
                                                         const uint32_t vertexStride)
    : m_entries(SpanUtil::ToVector(vertexElementAttribConfigs))
    , m_vertexStride(vertexStride)
  {
  }


  bool GLVertexElementAttribLinks::IsCompatible(const GLVertexElementAttribLinks& instance) const
  {
    if (m_entries.size() != instance.m_entries.size())
    {
      return false;
    }
    for (std::size_t i = 0; i < m_entries.size(); ++i)
    {
      if (m_entries[i] != instance.m_entries[i])
      {
        return false;
      }
    }
    return true;
  }

  void GLVertexElementAttribLinks::EnableAttribArrays() const noexcept
  {
    for (const auto& entry : m_entries)
    {
      glVertexAttribPointer(entry.AttribIndex, entry.Size, entry.Type, entry.Normalized, UncheckedNumericCast<GLsizei>(m_vertexStride),
                            entry.Pointer);
      glEnableVertexAttribArray(entry.AttribIndex);
    }
  }


  ReadOnlySpan<GLVertexElementAttribConfig> GLVertexElementAttribLinks::AsSpan() const
  {
    return SpanUtil::AsReadOnlySpan(m_entries);
  }
}
