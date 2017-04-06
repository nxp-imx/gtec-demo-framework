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
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphicsGLES2/Common.hpp>
#include <FslGraphicsGLES2/GLCheck.hpp>
#include <FslGraphicsGLES2/GLBatch2DQuadRenderer.hpp>
#include <FslGraphicsGLES2/NativeTexture2D.hpp>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace GLES2
  {
    namespace
    {
      const int32_t QUAD_BATCH_SIZE = 2048;
      const int32_t QUAD_VERTEX_COUNT = 4;


      const char* g_vertexShader =
        "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
        "precision highp float;\n"
        "#else\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform mat4 g_matModelViewProj;\n"
        "\n"
        "attribute vec4 VertexPosition;\n"
        "attribute vec4 VertexColor;\n"
        "attribute vec2 VertexTextureCoord;\n"
        "\n"
        "varying vec4 FragColor;\n"
        "varying vec2 FragTextureCoord;\n"
        "\n"
        "void main()\n"
        "{"
        "  gl_Position = g_matModelViewProj * VertexPosition;\n"
        "  FragColor = VertexColor;\n"
        "  FragTextureCoord = VertexTextureCoord;\n"
        "}";


      const char* g_fragmentShader =
        "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
        "precision highp float;\n"
        "#else\n"
        "precision mediump float;\n"
        "#endif\n"
        "\n"
        "uniform sampler2D Texture;\n"
        "\n"
        "varying vec4 FragColor;\n"
        "varying vec2 FragTextureCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = texture2D(Texture, FragTextureCoord) * FragColor;\n"
        "}\n";


      void FillIndices(std::vector<uint16_t>& rIndices)
      {
        assert(((rIndices.size() - 4) % 6) == 0);
        rIndices[0] = 0;
        rIndices[1] = 2;
        rIndices[2] = 1;
        rIndices[3] = 3;

        // Prepare a index buffer once for 'N' formatted quads with degenerated triangles in between
        // 0-1  4-5  8-9
        // |/|  |/|  |/|
        // 2-3  6-7  A-B

        // 0,2,1 - 2,1,3
        // 1,3,3 - 3,3,4 - 3,4,4 - 4,4,6
        // 4,6,5 - 6,5,7

        int vertexIndex = 3;
        for (std::size_t i = 4; i < rIndices.size(); i += 6)
        {
          rIndices[i + 0] = vertexIndex;
          rIndices[i + 1] = vertexIndex + 1;
          rIndices[i + 2] = vertexIndex + 1;
          rIndices[i + 3] = vertexIndex + 3;
          rIndices[i + 4] = vertexIndex + 2;
          rIndices[i + 5] = vertexIndex + 4;
          vertexIndex += 4;
        }
      }


      inline void RestoreState(const GLenum oldState, const GLboolean oldValue)
      {
        if (oldValue)
          glEnable(oldState);
        else
          glDisable(oldState);
      }
    }


    GLBatch2DQuadRenderer::GLBatch2DQuadRenderer(const int32_t quadCapacityHint)
      : m_screenResolution()
      , m_indexBuffer()
      , m_vertexBuffer()
      , m_program(g_vertexShader, g_fragmentShader)
      , m_vertexOffset(0)
      , m_indexOffset(0)
      , m_oldState()
      , m_currentState()
      , m_locMatModelViewProj(GLValues::INVALID_LOCATION)
      , m_locTexture(GLValues::INVALID_LOCATION)
    {
      const int32_t quadCapacity = std::max(quadCapacityHint, QUAD_BATCH_SIZE);
      // (4*(quadCapacity-1) because we need four degenerated triangles to bind the quads together
      const int32_t vertexCapacity = 4 * quadCapacity;
      const int32_t indexCapacity = 4 + (((vertexCapacity / QUAD_VERTEX_COUNT) - 1) * 6);

      // Prepare the vertex buffer
      std::vector<VertexPositionColorTexture> emptyVertices(vertexCapacity);
      m_vertexBuffer.Reset(emptyVertices.data(), vertexCapacity, GL_DYNAMIC_DRAW);

      std::vector<uint16_t> indices(indexCapacity);
      FillIndices(indices);

      m_indexBuffer.Reset(indices.data(), indexCapacity, GL_STATIC_DRAW);


      // Configure the program to vertex links
      {
        const GLint hProgram = m_program.Get();
        const GLint locPosition = GL_CHECK(glGetAttribLocation(hProgram, "VertexPosition"));
        const GLint locColor = GL_CHECK(glGetAttribLocation(hProgram, "VertexColor"));
        const GLint locTexCoord = GL_CHECK(glGetAttribLocation(hProgram, "VertexTextureCoord"));
        assert(locPosition >= 0);
        assert(locColor >= 0);
        assert(locPosition >= 0);

        m_link[0] = GLVertexAttribLink(locPosition, m_vertexBuffer.GetVertexElementIndex(VertexElementUsage::Position, 0));
        m_link[1] = GLVertexAttribLink(locColor, m_vertexBuffer.GetVertexElementIndex(VertexElementUsage::Color, 0));
        m_link[2] = GLVertexAttribLink(locTexCoord, m_vertexBuffer.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0));
      }

      const GLint hProgram = m_program.Get();
      m_locMatModelViewProj = GL_CHECK(glGetUniformLocation(hProgram, "g_matModelViewProj"));
      m_locTexture = GL_CHECK(glGetUniformLocation(hProgram, "Texture"));
    }


    void GLBatch2DQuadRenderer::Begin(const Point2& screenResolution, const BlendState blendState, const bool restoreState)
    {
      // Fix saving of state should be controlled by a parameter
      {
        m_oldState.Blend = glIsEnabled(GL_BLEND);
        m_oldState.CullFace = glIsEnabled(GL_CULL_FACE);
        m_oldState.DepthTest = glIsEnabled(GL_DEPTH_TEST);
        m_oldState.ScissorTest = glIsEnabled(GL_SCISSOR_TEST);

        glGetIntegerv(GL_BLEND_SRC_RGB, &m_oldState.BlendSrcRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_oldState.BlendSrcAlpha);
        glGetIntegerv(GL_BLEND_DST_RGB, &m_oldState.BlendDstRGB);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &m_oldState.BlendDstAlpha);

        glGetIntegerv(GL_ACTIVE_TEXTURE, &m_oldState.ActiveTexture);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_oldState.CurrentTexture);
        glGetIntegerv(GL_CURRENT_PROGRAM, &m_oldState.CurrentProgram);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_oldState.VertexBuffer);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_oldState.IndexBuffer);
        // store the attrib state before we modify it
        for (int i = 0; i < 3; ++i)
        {
          glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &m_oldState.Attrib[i].Enabled);
          glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &m_oldState.Attrib[i].Size);
          glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &m_oldState.Attrib[i].Type);
          glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &m_oldState.Attrib[i].Normalized);
          glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &m_oldState.Attrib[i].Stride);
          glGetVertexAttribPointerv(m_link[i].AttribIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &m_oldState.Attrib[i].Pointer);
        }
      }

      const GLint hProgram = m_program.Get();

      switch (blendState)
      {
      case BlendState::Additive:
        m_currentState.Blend = GL_TRUE;
        m_currentState.BlendSrcRGB = GL_ONE;
        m_currentState.BlendDstRGB = GL_ONE;
        m_currentState.BlendSrcAlpha = GL_ONE;
        m_currentState.BlendDstAlpha = GL_ONE;
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        break;
      case BlendState::AlphaBlend:
        m_currentState.Blend = GL_TRUE;
        m_currentState.BlendSrcRGB = GL_ONE;
        m_currentState.BlendDstRGB = GL_ONE_MINUS_SRC_ALPHA;
        m_currentState.BlendSrcAlpha = GL_ONE;
        m_currentState.BlendDstAlpha = GL_ONE_MINUS_SRC_ALPHA;
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
      case BlendState::NonPremultiplied:
        m_currentState.Blend = GL_TRUE;
        m_currentState.BlendSrcRGB = GL_SRC_ALPHA;
        m_currentState.BlendDstRGB = GL_ONE_MINUS_SRC_ALPHA;
        m_currentState.BlendSrcAlpha = GL_SRC_ALPHA;
        m_currentState.BlendDstAlpha = GL_ONE_MINUS_SRC_ALPHA;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
      case BlendState::Opaque:
      default:
        m_currentState.Blend = GL_FALSE;
        m_currentState.BlendSrcRGB = m_oldState.BlendSrcRGB;
        m_currentState.BlendDstRGB = m_oldState.BlendDstRGB;
        m_currentState.BlendSrcAlpha = m_oldState.BlendSrcAlpha;
        m_currentState.BlendDstAlpha = m_oldState.BlendDstAlpha;
        glDisable(GL_BLEND);
        break;
      }

      if (m_oldState.DepthTest != GL_FALSE)
        glDisable(GL_DEPTH_TEST);
      if (m_oldState.CullFace != GL_FALSE)
        glDisable(GL_CULL_FACE);
      if (m_oldState.ScissorTest != GL_FALSE)
        glDisable(GL_SCISSOR_TEST);
      if (m_oldState.ActiveTexture != GL_TEXTURE0)
        glActiveTexture(GL_TEXTURE0);
      glUseProgram(hProgram);
      glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
      glBindBuffer(m_indexBuffer.GetTarget(), m_indexBuffer.Get());

      m_vertexBuffer.EnableAttribArrays(m_link, 3);

      if (screenResolution != m_screenResolution)
      {
        m_screenResolution = screenResolution;

        // Setup the shader
        const float screenWidth = static_cast<float>(m_screenResolution.X);
        const float screenHeight = static_cast<float>(m_screenResolution.Y);
        const float screenOffsetX = std::floor(screenWidth / 2.0f);
        const float screenOffsetY = std::floor(screenHeight / 2.0f);

        const Matrix matViewProj = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) *
                                   Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                   Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
        glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());
        glUniform1i(m_locTexture, 0);
      }

      m_vertexOffset = 0;
      m_indexOffset = 0;
    }


    void GLBatch2DQuadRenderer::End()
    {
      // Restore the initial state
      if (m_oldState.Blend != m_currentState.Blend)
        RestoreState(GL_BLEND, m_oldState.Blend);

      // Restore the blend func if necessary
      if (m_oldState.BlendSrcRGB != m_currentState.BlendSrcRGB || m_oldState.BlendDstRGB != m_currentState.BlendDstRGB ||
          m_oldState.BlendSrcAlpha != m_currentState.BlendSrcAlpha || m_oldState.BlendDstAlpha != m_currentState.BlendDstAlpha)
      {
        glBlendFuncSeparate(m_oldState.BlendSrcRGB, m_oldState.BlendDstRGB, m_oldState.BlendSrcAlpha, m_oldState.BlendDstAlpha);
      }

      if (m_oldState.CullFace != GL_FALSE)
        RestoreState(GL_CULL_FACE, m_oldState.CullFace);
      if (m_oldState.DepthTest != GL_FALSE)
        RestoreState(GL_DEPTH_TEST, m_oldState.DepthTest);
      if (m_oldState.ScissorTest != GL_FALSE)
        RestoreState(GL_SCISSOR_TEST, m_oldState.ScissorTest);
      if (m_oldState.ActiveTexture != GL_TEXTURE0)
        glActiveTexture(m_oldState.ActiveTexture);
      glBindTexture(GL_TEXTURE_2D, m_oldState.CurrentTexture);
      glUseProgram(m_oldState.CurrentProgram);
      glBindBuffer(GL_ARRAY_BUFFER, m_oldState.VertexBuffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oldState.IndexBuffer);

      // restore the attrib state
      for (int i = 0; i < 3; ++i)
      {
        glVertexAttribPointer(i, m_oldState.Attrib[i].Size, m_oldState.Attrib[i].Type, m_oldState.Attrib[i].Normalized, m_oldState.Attrib[i].Stride, m_oldState.Attrib[i].Pointer);
        if (m_oldState.Attrib[i].Enabled)
          glEnableVertexAttribArray(i);
        else
          glDisableVertexAttribArray(i);
      }
    }


    void GLBatch2DQuadRenderer::DrawQuads(const VertexPositionColorTexture*const pVertices, const uint32_t length, const GLuint hTexture)
    {
      assert(pVertices != nullptr);
      assert(length >= 1);
      assert(length <= std::numeric_limits<uint32_t>::max());

      // bind the texture
      glBindTexture(GL_TEXTURE_2D, hTexture);

      const VertexPositionColorTexture* pSrcVertices = pVertices;
      uint32_t verticesLeft = length * QUAD_VERTEX_COUNT;
      const auto maxCapacity = m_vertexBuffer.GetCapacity();
      assert(m_vertexOffset <= maxCapacity);
      auto capacityLeft = maxCapacity - m_vertexOffset;

      auto verticesToAdd = (verticesLeft <= capacityLeft ? verticesLeft : capacityLeft);
      if (capacityLeft > 0)
      {
        assert((m_vertexOffset + verticesToAdd) <= m_vertexBuffer.GetCapacity());
        m_vertexBuffer.SetDataFast(m_vertexOffset, pSrcVertices, verticesToAdd);
        pSrcVertices += verticesToAdd;
        m_vertexOffset += verticesToAdd;

        const auto numIndices = 4 + (((verticesToAdd / QUAD_VERTEX_COUNT) - 1) * 6);
        assert((m_indexOffset + numIndices) <= m_indexBuffer.GetCapacity());
        glDrawElements(GL_TRIANGLE_STRIP, numIndices, m_indexBuffer.GetType(), (const void*)(m_indexOffset*sizeof(uint16_t)));
        m_indexOffset += numIndices + 2;
        verticesLeft -= verticesToAdd;
      }

      if (verticesLeft >= maxCapacity)
      {
        do
        {
          m_vertexBuffer.SetDataFast(0, pSrcVertices, maxCapacity);
          pSrcVertices += maxCapacity;
          glDrawElements(GL_TRIANGLE_STRIP, m_indexBuffer.GetCapacity(), m_indexBuffer.GetType(), nullptr);
          verticesLeft -= maxCapacity;
        } while (verticesLeft >= maxCapacity);
        m_vertexOffset = 0;
        m_indexOffset = 0;
      }

      if ( verticesLeft > 0)
      {
        m_vertexOffset = 0;
        m_indexOffset = 0;

        assert(verticesLeft <= m_vertexBuffer.GetCapacity());
        m_vertexBuffer.SetDataFast(m_vertexOffset, pSrcVertices, verticesLeft);
        pSrcVertices += verticesLeft;
        m_vertexOffset += verticesLeft;

        const int32_t numIndices = 4 + (((verticesLeft / QUAD_VERTEX_COUNT) - 1) * 6);
        assert((m_indexOffset + numIndices) <= m_indexBuffer.GetCapacity());
        glDrawElements(GL_TRIANGLE_STRIP, numIndices, m_indexBuffer.GetType(), (const void*)(m_indexOffset*sizeof(uint16_t)));
        m_indexOffset += numIndices + 2;
        verticesLeft -= verticesLeft;
        assert(verticesLeft == 0);
      }

    }


  }
}
