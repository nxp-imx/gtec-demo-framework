#ifndef FSLUTIL_OPENGLES2_GLBATCH2DQUADRENDERER_HPP
#define FSLUTIL_OPENGLES2_GLBATCH2DQUADRENDERER_HPP
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
#include <FslUtil/OpenGLES2/Common.hpp>

#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <FslUtil/OpenGLES2/GLIndexBuffer.hpp>
#include <FslUtil/OpenGLES2/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLTextureInfo.hpp>
#include <FslGraphics/Render/BlendState.hpp>

namespace Fsl
{
  struct VertexPositionColorTexture;

  namespace GLES2
  {
    class GLBatch2DQuadRenderer
    {
      struct OldVertexAttrib
      {
        GLint Enabled{0};
        GLint Size{0};
        GLint Type{0};
        GLint Normalized{GL_FALSE};
        GLint Stride{0};
        GLvoid* Pointer{nullptr};

        OldVertexAttrib() = default;
      };

      struct StateCache
      {
        GLboolean Blend{GL_FALSE};
        GLint BlendSrcRGB{0};
        GLint BlendSrcAlpha{0};
        GLint BlendDstRGB{0};
        GLint BlendDstAlpha{0};

        StateCache() = default;
      };

      struct OldState
      {
        GLboolean Blend{GL_FALSE};
        GLboolean CullFace{GL_FALSE};
        GLboolean DepthTest{GL_FALSE};
        GLboolean ScissorTest{GL_FALSE};
        GLint BlendSrcRGB{0};
        GLint BlendSrcAlpha{0};
        GLint BlendDstRGB{0};
        GLint BlendDstAlpha{0};
        GLint ActiveTexture;
        GLint CurrentTexture;
        GLint CurrentProgram;
        GLint VertexBuffer;
        GLint IndexBuffer;
        OldVertexAttrib Attrib[3];

        OldState()
          : ActiveTexture(GLValues::INVALID_HANDLE)
          , CurrentTexture(GLValues::INVALID_HANDLE)
          , CurrentProgram(GLValues::INVALID_HANDLE)
          , VertexBuffer(GLValues::INVALID_HANDLE)
          , IndexBuffer(GLValues::INVALID_HANDLE)
        {
        }
      };

      Point2 m_screenResolution;
      GLIndexBuffer m_indexBuffer;
      GLVertexBuffer m_vertexBuffer;
      GLProgram m_program;
      GLVertexAttribLink m_link[3];
      uint32_t m_vertexOffset;
      uint32_t m_indexOffset;
      OldState m_oldState;
      StateCache m_currentState;
      GLint m_locMatModelViewProj;
      GLint m_locTexture;

    public:
      GLBatch2DQuadRenderer(const int32_t quadCapacityHint);
      void Begin(const Point2& screenResolution, const BlendState blendState, const bool restoreState);
      void End();
      void DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const GLTextureInfo& textureInfo);
    };
  }
}

#endif
