#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSMATERIALATTRIBMANAGER_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES3_NATIVEGRAPHICSMATERIALATTRIBMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeMaterialAttribHandle.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/VertexElementAttribLinks.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <array>

namespace Fsl
{
  class VertexDeclarationSpan;

  namespace GLES3
  {
    class NativeGraphicsMaterialAttribManager
    {
      struct AttribConfigRecord
      {
        uint32_t RefCount{0};
        VertexElementAttribLinks AttribLinks;

        AttribConfigRecord() = default;

        AttribConfigRecord(const VertexDeclarationSpan& vertexDeclaration, const ReadOnlySpan<GLVertexAttribLink>& attribLinks)
          : RefCount(1)
          , AttribLinks(vertexDeclaration, attribLinks)
        {
        }
      };

      HandleVector<AttribConfigRecord> m_records;

    public:
      NativeGraphicsMaterialAttribManager();

      NativeMaterialAttribHandle AcquireConfig(const GLint locVertexPosition, const GLint locVertexColor, const GLint locVertexTextureCoord,
                                               const VertexDeclarationSpan& vertexDeclaration);

      bool ReleaseConfig(const NativeMaterialAttribHandle handle) noexcept;

      const VertexElementAttribLinks& GetVertexElementAttribLinks(const NativeMaterialAttribHandle handle) const
      {
        return m_records.Get(handle.Value).AttribLinks;
      }

      const VertexElementAttribLinks* TryGetVertexElementAttribLinks(const NativeMaterialAttribHandle handle) const noexcept
      {
        const AttribConfigRecord* const pRecords = m_records.TryGet(handle.Value);
        return pRecords != nullptr ? &pRecords->AttribLinks : nullptr;
      }

    private:
      static NativeMaterialAttribHandle TryAcquireExisting(HandleVector<AttribConfigRecord>& rRecords, const VertexDeclarationSpan& vertexDeclaration,
                                                           const ReadOnlySpan<GLVertexAttribLink> attribLinks);
    };
  }
}

#endif
