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

#include <FslBase/NumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexElementAttribLinks.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslUtil/OpenGLES2/GLVertexElements.hpp>
#include <algorithm>

namespace Fsl::GLES2
{
  VertexElementAttribLinks::VertexElementAttribLinks(const VertexDeclarationSpan& vertexDeclaration,
                                                     const ReadOnlySpan<GLVertexAttribLink> attribLinks)
    : m_array(ToArray(vertexDeclaration, attribLinks))
    , m_vertexStride(vertexDeclaration.VertexStride())
  {
  }

  VertexElementAttribLinks::VertexElementAttribLinks(const ReadOnlySpan<GLVertexElementAttribConfig>& vertexElementAttribConfigs,
                                                     const uint32_t vertexStride)
    : m_array(ToArray(vertexElementAttribConfigs))
    , m_vertexStride(vertexStride)
  {
  }

  bool VertexElementAttribLinks::IsCompatible(const VertexElementAttribLinks& instance) const noexcept
  {
    if (&instance != this)
    {
      if (m_array.Entries.size() != instance.m_array.Entries.size())
      {
        return false;
      }
      for (std::size_t i = 0; i < m_array.Entries.size(); ++i)
      {
        if (m_array.Entries[i] != instance.m_array.Entries[i])
        {
          return false;
        }
      }
    }
    return true;
  }

  VertexElementAttribLinks::InlineArray VertexElementAttribLinks::ToArray(const VertexDeclarationSpan& vertexDeclaration,
                                                                          const ReadOnlySpan<GLVertexAttribLink> attribLinks)
  {
    // This is not optimal but it works for now
    auto configuration = GLVertexElements(vertexDeclaration).ExtractConfiguration(attribLinks);
    return ToArray(ReadOnlySpanUtil::AsSpan(configuration));
  }

  VertexElementAttribLinks::InlineArray VertexElementAttribLinks::ToArray(const ReadOnlySpan<GLVertexElementAttribConfig>& vertexElementAttribConfigs)
  {
    VertexElementAttribLinks::InlineArray result;
    const std::size_t convertedSize = vertexElementAttribConfigs.size();
    if (convertedSize > result.Entries.size())
    {
      throw NotSupportedException("VertexElementAttribLinks max supported capacity exceeded");
    }
    // Copy it to the array
    for (std::size_t i = 0; i < convertedSize; ++i)
    {
      result.Entries[i] = vertexElementAttribConfigs[i];
    }
    // sort the entries based on attrib index (low to high)
    std::sort(result.Entries.begin(), result.Entries.begin() + UncheckedNumericCast<std::ptrdiff_t>(convertedSize),
              [](const GLVertexElementAttribConfig& lhs, const GLVertexElementAttribConfig& rhs) -> bool
              { return lhs.AttribIndex < rhs.AttribIndex; });

    result.Count = NumericCast<uint32_t>(convertedSize);
    assert(result.Count <= result.Entries.size());
    return result;
  }
}
