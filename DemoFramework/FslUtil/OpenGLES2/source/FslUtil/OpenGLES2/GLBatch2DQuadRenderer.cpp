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

#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Font/SdfFontUtil.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <FslUtil/OpenGLES2/GLBatch2DQuadRenderer.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::GLES2
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const int32_t QuadBatchSize = 2048;
      constexpr const int32_t QuadVertexCount = 4;
    }

    const char* const g_vertexShader =
      "precision mediump float;\n"
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


    const char* const g_fragmentShader =
      "precision mediump float;\n"
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

    const char* const g_fragmentSdfShader =
      "precision mediump float;\n"
      "\n"
      "uniform sampler2D Texture;\n"
      "uniform float g_smoothing;\n"
      "\n"
      "varying vec4 FragColor;\n"
      "varying vec2 FragTextureCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  float distance = texture2D(Texture, FragTextureCoord).a;\n"
      "  float alpha = smoothstep(0.5 - g_smoothing, 0.5 + g_smoothing, distance);\n"
      "  gl_FragColor = vec4(FragColor.rgb, FragColor.a * alpha);\n"
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

      uint16_t vertexIndex = 3;
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
      if (oldValue != 0u)
      {
        glEnable(oldState);
      }
      else
      {
        glDisable(oldState);
      }
    }
  }


  GLBatch2DQuadRenderer::GLBatch2DQuadRenderer(const int32_t quadCapacityHint)
    : m_vertexOffset(0)
    , m_indexOffset(0)
  {
    const int32_t quadCapacity = std::max(quadCapacityHint, LocalConfig::QuadBatchSize);
    // (4*(quadCapacity-1) because we need four degenerated triangles to bind the quads together
    const int32_t vertexCapacity = 4 * quadCapacity;
    const int32_t indexCapacity = 4 + (((vertexCapacity / LocalConfig::QuadVertexCount) - 1) * 6);

    // Prepare the vertex buffer
    std::vector<VertexPositionColorTexture> emptyVertices(vertexCapacity);
    m_vertexBuffer.Reset(emptyVertices.data(), vertexCapacity, GL_DYNAMIC_DRAW);

    std::vector<uint16_t> indices(indexCapacity);
    FillIndices(indices);

    m_indexBuffer.Reset(indices.data(), indexCapacity, GL_STATIC_DRAW);


    m_normalInfo = PrepareProgram(m_vertexBuffer, false);
    m_sdfInfo = PrepareProgram(m_vertexBuffer, true);
  }


  void GLBatch2DQuadRenderer::Begin(const PxSize2D& sizePx, const BlendState blendState, const BatchSdfRenderConfig& sdfRenderConfig,
                                    const bool restoreState)
  {
    m_cachedBlendState = blendState;
    auto& activeInfo = (blendState != BlendState::Sdf ? m_normalInfo : m_sdfInfo);

    FSL_PARAM_NOT_USED(restoreState);
    // FIX: saving of state should be controlled by a parameter
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
      for (uint32_t i = 0; i < activeInfo.AttribLink.size(); ++i)
      {
        const auto attribIndex = activeInfo.AttribLink[i].AttribIndex;
        m_oldState.Attrib[i] = {};
        glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &m_oldState.Attrib[i].Enabled);
        glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &m_oldState.Attrib[i].Size);
        glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_TYPE, &m_oldState.Attrib[i].Type);
        GLint normalized = 0;
        glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
        m_oldState.Attrib[i].Normalized = (normalized != GL_FALSE ? GL_TRUE : GL_FALSE);
        glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &m_oldState.Attrib[i].Stride);
        glGetVertexAttribPointerv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &m_oldState.Attrib[i].Pointer);
      }
    }

    const auto hProgram = activeInfo.Program.Get();

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
    case BlendState::Sdf:
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
    {
      glDisable(GL_DEPTH_TEST);
    }
    if (m_oldState.CullFace != GL_FALSE)
    {
      glDisable(GL_CULL_FACE);
    }
    if (m_oldState.ScissorTest != GL_FALSE)
    {
      glDisable(GL_SCISSOR_TEST);
    }
    if (m_oldState.ActiveTexture != GL_TEXTURE0)
    {
      glActiveTexture(GL_TEXTURE0);
    }
    glUseProgram(hProgram);
    glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
    glBindBuffer(m_indexBuffer.GetTarget(), m_indexBuffer.Get());

    m_vertexBuffer.EnableAttribArrays(activeInfo.AttribLink);

    if (sizePx != activeInfo.CachedSizePx)
    {
      activeInfo.CachedSizePx = sizePx;

      // Setup the shader
      const auto screenWidth = static_cast<float>(sizePx.RawWidth());
      const auto screenHeight = static_cast<float>(sizePx.RawHeight());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      const Matrix matViewProj = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) *
                                 Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                                 Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
      glUniformMatrix4fv(activeInfo.LocMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());
      glUniform1i(activeInfo.LocTexture, 0);
    }

    if (activeInfo.LocSmoothing != GLValues::InvalidLocation)
    {
      float smoothing = SdfFontUtil::CalcSmooth(sdfRenderConfig.Spread, sdfRenderConfig.Scale);
      glUniform1f(activeInfo.LocSmoothing, smoothing);
    }

    m_vertexOffset = 0;
    m_indexOffset = 0;
  }


  void GLBatch2DQuadRenderer::End()
  {
    // Restore the initial state
    if (m_oldState.Blend != m_currentState.Blend)
    {
      RestoreState(GL_BLEND, m_oldState.Blend);
    }

    // Restore the blend func if necessary
    if (m_oldState.BlendSrcRGB != m_currentState.BlendSrcRGB || m_oldState.BlendDstRGB != m_currentState.BlendDstRGB ||
        m_oldState.BlendSrcAlpha != m_currentState.BlendSrcAlpha || m_oldState.BlendDstAlpha != m_currentState.BlendDstAlpha)
    {
      glBlendFuncSeparate(m_oldState.BlendSrcRGB, m_oldState.BlendDstRGB, m_oldState.BlendSrcAlpha, m_oldState.BlendDstAlpha);
    }

    if (m_oldState.CullFace != GL_FALSE)
    {
      RestoreState(GL_CULL_FACE, m_oldState.CullFace);
    }
    if (m_oldState.DepthTest != GL_FALSE)
    {
      RestoreState(GL_DEPTH_TEST, m_oldState.DepthTest);
    }
    if (m_oldState.ScissorTest != GL_FALSE)
    {
      RestoreState(GL_SCISSOR_TEST, m_oldState.ScissorTest);
    }
    if (m_oldState.ActiveTexture != GL_TEXTURE0)
    {
      glActiveTexture(m_oldState.ActiveTexture);
    }

    glBindTexture(GL_TEXTURE_2D, m_oldState.CurrentTexture);
    glUseProgram(m_oldState.CurrentProgram);
    glBindBuffer(GL_ARRAY_BUFFER, m_oldState.VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oldState.IndexBuffer);

    // restore the attrib state
    {
      auto& activeInfo = (m_cachedBlendState != BlendState::Sdf ? m_normalInfo : m_sdfInfo);

      for (int i = 0; i < 3; ++i)
      {
        const auto attribIndex = activeInfo.AttribLink[i].AttribIndex;
        glVertexAttribPointer(attribIndex, m_oldState.Attrib[i].Size, m_oldState.Attrib[i].Type, m_oldState.Attrib[i].Normalized,
                              m_oldState.Attrib[i].Stride, m_oldState.Attrib[i].Pointer);
        if (m_oldState.Attrib[i].Enabled != 0)
        {
          glEnableVertexAttribArray(attribIndex);
        }
        else
        {
          glDisableVertexAttribArray(attribIndex);
        }
      }
    }
    m_stats = {};
  }


  void GLBatch2DQuadRenderer::DrawQuads(const VertexPositionColorTexture* const pVertices, const uint32_t length, const GLTextureInfo& textureInfo)
  {
    assert(pVertices != nullptr);
    assert(length >= 1);
    assert(length <= std::numeric_limits<uint32_t>::max());

    // bind the texture
    glBindTexture(GL_TEXTURE_2D, textureInfo.Handle);

    const VertexPositionColorTexture* pSrcVertices = pVertices;
    uint32_t verticesLeft = length * LocalConfig::QuadVertexCount;
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

      const auto numIndices = 4 + (((verticesToAdd / LocalConfig::QuadVertexCount) - 1) * 6);
      assert((m_indexOffset + numIndices) <= m_indexBuffer.GetCapacity());
      glDrawElements(GL_TRIANGLE_STRIP, UncheckedNumericCast<GLsizei>(numIndices), m_indexBuffer.GetType(),
                     reinterpret_cast<const void*>(m_indexOffset * sizeof(uint16_t)));
      ++m_stats.DrawCalls;
      m_indexOffset += numIndices + 2;
      verticesLeft -= verticesToAdd;
    }

    if (verticesLeft >= maxCapacity)
    {
      do
      {
        m_vertexBuffer.SetDataFast(0, pSrcVertices, maxCapacity);
        pSrcVertices += maxCapacity;
        glDrawElements(GL_TRIANGLE_STRIP, m_indexBuffer.GetGLCapacity(), m_indexBuffer.GetType(), nullptr);
        verticesLeft -= maxCapacity;
      } while (verticesLeft >= maxCapacity);
      m_vertexOffset = 0;
      m_indexOffset = 0;
    }

    if (verticesLeft > 0)
    {
      m_vertexOffset = 0;
      m_indexOffset = 0;

      assert(verticesLeft <= m_vertexBuffer.GetCapacity());
      m_vertexBuffer.SetDataFast(m_vertexOffset, pSrcVertices, verticesLeft);
      pSrcVertices += verticesLeft;
      m_vertexOffset += verticesLeft;

      const int32_t numIndices = 4 + (((UncheckedNumericCast<int32_t>(verticesLeft) / LocalConfig::QuadVertexCount) - 1) * 6);
      assert((m_indexOffset + numIndices) <= m_indexBuffer.GetCapacity());
      glDrawElements(GL_TRIANGLE_STRIP, numIndices, m_indexBuffer.GetType(), reinterpret_cast<const void*>(m_indexOffset * sizeof(uint16_t)));
      ++m_stats.DrawCalls;
      m_indexOffset += numIndices + 2;
      verticesLeft = 0;
      assert(verticesLeft == 0);
    }
  }


  GLBatch2DQuadRenderer::ProgramInfo GLBatch2DQuadRenderer::PrepareProgram(const GLVertexBuffer& vertexBuffer, const bool sdf)
  {
    ProgramInfo info;
    info.Program.Reset(g_vertexShader, !sdf ? g_fragmentShader : g_fragmentSdfShader);

    // Configure the program to vertex links
    {
      const GLint locPosition = info.Program.GetAttribLocation("VertexPosition");
      const GLint locColor = info.Program.GetAttribLocation("VertexColor");
      const GLint locTexCoord = info.Program.GetAttribLocation("VertexTextureCoord");
      info.AttribLink[0] = GLVertexAttribLink(locPosition, vertexBuffer.GetVertexElementIndex(VertexElementUsage::Position, 0));
      info.AttribLink[1] = GLVertexAttribLink(locColor, vertexBuffer.GetVertexElementIndex(VertexElementUsage::Color, 0));
      info.AttribLink[2] = GLVertexAttribLink(locTexCoord, vertexBuffer.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0));
    }

    info.LocMatModelViewProj = info.Program.GetUniformLocation("g_matModelViewProj");
    info.LocTexture = info.Program.GetUniformLocation("Texture");
    info.LocSmoothing = sdf ? info.Program.GetUniformLocation("g_smoothing") : GLValues::InvalidLocation;
    return info;
  }
}
