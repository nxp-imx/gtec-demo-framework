#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_SAVEGLSTATE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_SAVEGLSTATE_HPP
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

#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/GLVertexAttribStateFunctor.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribStateCache.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexElementAttribLinks.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <GLES2/gl2.h>
#include <array>
#include <cstdint>

namespace Fsl::GLES2
{
  class SaveGLState
  {
    struct State
    {
      GLboolean Blend{GL_FALSE};
      GLboolean CullFace{GL_FALSE};
      GLboolean DepthTest{GL_FALSE};
      GLboolean DepthMask{GL_FALSE};
      GLboolean ScissorTest{GL_FALSE};
      GLenum BlendSrcRGB{0};
      GLenum BlendSrcAlpha{0};
      GLenum BlendDstRGB{0};
      GLenum BlendDstAlpha{0};
      GLenum CullFaceMode{0};
      GLenum CullFrontFace{0};
      GLenum DepthFunc{0};
      GLuint ActiveTexture{GLValues::InvalidHandle};
      GLuint CurrentTexture{GLValues::InvalidHandle};
      GLuint CurrentProgram{GLValues::InvalidHandle};
      GLuint VertexBuffer{GLValues::InvalidHandle};
      GLuint IndexBuffer{GLValues::InvalidHandle};
      PxViewport Viewport;
    };

    bool m_hasSavedState{false};
    State m_oldState;
    State m_newState;
    VertexAttribStateCache<GLVertexAttribStateFunctor> m_attribCache;

  public:
    SaveGLState(const SaveGLState&) = delete;
    SaveGLState& operator=(const SaveGLState&) = delete;

    SaveGLState& operator=(SaveGLState&& other) noexcept
    {
      if (this != &other)
      {
        if (m_hasSavedState)
        {
          Reset();
        }
        m_hasSavedState = other.m_hasSavedState;
        m_oldState = other.m_oldState;
        m_newState = other.m_newState;
        m_attribCache = std::move(other.m_attribCache);
        other.m_hasSavedState = false;
        other.m_oldState = {};
        other.m_newState = {};
      }
      return *this;
    }

    SaveGLState(SaveGLState&& other) noexcept
      : m_hasSavedState(other.m_hasSavedState)
      , m_oldState(other.m_oldState)
      , m_newState(other.m_newState)
      , m_attribCache(std::move(other.m_attribCache))
    {
      other.m_hasSavedState = false;
      other.m_oldState = {};
      other.m_newState = {};
    }


    SaveGLState() = default;
    explicit SaveGLState(const bool /*dummy*/) noexcept
      : m_hasSavedState(true)
    {
      m_oldState.Blend = glIsEnabled(GL_BLEND);
      m_oldState.CullFace = glIsEnabled(GL_CULL_FACE);
      m_oldState.DepthTest = glIsEnabled(GL_DEPTH_TEST);
      Get(GL_DEPTH_WRITEMASK, m_oldState.DepthMask);
      m_oldState.ScissorTest = glIsEnabled(GL_SCISSOR_TEST);

      m_oldState.BlendSrcRGB = GetEnum(GL_BLEND_SRC_RGB);
      m_oldState.BlendSrcAlpha = GetEnum(GL_BLEND_SRC_ALPHA);
      m_oldState.BlendDstRGB = GetEnum(GL_BLEND_DST_RGB);
      m_oldState.BlendDstAlpha = GetEnum(GL_BLEND_DST_ALPHA);
      m_oldState.CullFaceMode = GetEnum(GL_CULL_FACE_MODE);
      m_oldState.CullFrontFace = GetEnum(GL_FRONT_FACE);
      m_oldState.DepthFunc = GetEnum(GL_DEPTH_FUNC);
      Get(GL_ACTIVE_TEXTURE, m_oldState.ActiveTexture);
      Get(GL_TEXTURE_BINDING_2D, m_oldState.CurrentTexture);
      Get(GL_CURRENT_PROGRAM, m_oldState.CurrentProgram);
      Get(GL_ARRAY_BUFFER_BINDING, m_oldState.VertexBuffer);
      Get(GL_ELEMENT_ARRAY_BUFFER_BINDING, m_oldState.IndexBuffer);

      m_oldState.Viewport = GetViewport();
      m_newState = m_oldState;
    }


    ~SaveGLState() noexcept
    {
      Reset();
    }

    void EnableDepthTest()
    {
      assert(m_hasSavedState);
      if (m_newState.DepthTest == GL_FALSE)
      {
        glEnable(GL_DEPTH_TEST);
        m_newState.DepthTest = GL_TRUE;
      }
    }


    void DisableDepthTest()
    {
      assert(m_hasSavedState);
      if (m_newState.DepthTest != GL_FALSE)
      {
        glDisable(GL_DEPTH_TEST);
        m_newState.DepthTest = GL_FALSE;
      }
    }

    void EnableDepthWrite()
    {
      assert(m_hasSavedState);
      if (m_newState.DepthMask == GL_FALSE)
      {
        glDepthMask(GL_TRUE);
        m_newState.DepthMask = GL_TRUE;
      }
    }

    void DisableDepthWrite()
    {
      assert(m_hasSavedState);
      if (m_newState.DepthMask != GL_FALSE)
      {
        glDepthMask(GL_FALSE);
        m_newState.DepthMask = GL_FALSE;
      }
    }

    void EnableCullFace()
    {
      assert(m_hasSavedState);
      if (m_newState.CullFace == GL_FALSE)
      {
        glEnable(GL_CULL_FACE);
        m_newState.CullFace = GL_TRUE;
      }
    }

    void EnableBlend()
    {
      assert(m_hasSavedState);
      if (m_newState.Blend == GL_FALSE)
      {
        glEnable(GL_BLEND);
        m_newState.Blend = GL_TRUE;
      }
    }

    void DisableBlend()
    {
      assert(m_hasSavedState);
      if (m_newState.Blend != GL_FALSE)
      {
        glDisable(GL_BLEND);
        m_newState.Blend = GL_FALSE;
      }
    }


    void BlendFunc(const GLenum srcFactor, const GLenum dstFactor)
    {
      assert(m_hasSavedState);
      if (srcFactor != m_newState.BlendSrcRGB || srcFactor != m_newState.BlendSrcAlpha || dstFactor != m_newState.BlendDstRGB ||
          dstFactor != m_newState.BlendDstAlpha)
      {
        glBlendFunc(srcFactor, dstFactor);
        m_newState.BlendSrcRGB = srcFactor;
        m_newState.BlendSrcAlpha = srcFactor;
        m_newState.BlendDstRGB = dstFactor;
        m_newState.BlendDstAlpha = dstFactor;
      }
    }

    void DisableCullFace()
    {
      assert(m_hasSavedState);
      if (m_newState.CullFace != GL_FALSE)
      {
        glDisable(GL_CULL_FACE);
        m_newState.CullFace = GL_FALSE;
      }
    }

    void DisableScissor()
    {
      assert(m_hasSavedState);
      if (m_newState.ScissorTest != GL_FALSE)
      {
        glDisable(GL_SCISSOR_TEST);
        m_newState.ScissorTest = GL_FALSE;
      }
    }

    void CullFace(const GLenum mode)
    {
      assert(m_hasSavedState);
      if (mode != m_newState.CullFaceMode)
      {
        glCullFace(mode);
        m_newState.CullFaceMode = mode;
      }
    }

    void CullFrontFace(const GLenum mode)
    {
      assert(m_hasSavedState);
      if (mode != m_newState.CullFrontFace)
      {
        glFrontFace(mode);
        m_newState.CullFrontFace = mode;
      }
    }


    void DepthFunc(const GLenum func)
    {
      assert(m_hasSavedState);
      if (func != m_newState.DepthFunc)
      {
        glDepthFunc(func);
        m_newState.DepthFunc = func;
      }
    }

    void SetViewport(const PxViewport& viewportPx)
    {
      assert(m_hasSavedState);
      if (m_newState.Viewport != viewportPx)
      {
        m_newState.Viewport = viewportPx;
        glViewport(m_newState.Viewport.X(), m_newState.Viewport.Y(), m_newState.Viewport.Width(), m_newState.Viewport.Height());
      }
    }

    void ActiveTexture0()
    {
      assert(m_hasSavedState);
      if (m_newState.ActiveTexture != GL_TEXTURE0)
      {
        glActiveTexture(GL_TEXTURE0);
        m_newState.ActiveTexture = GL_TEXTURE0;
      }
    }

    void UseProgram(const GLuint program)
    {
      assert(m_hasSavedState);
      if (program != m_newState.CurrentProgram)
      {
        glUseProgram(program);
        m_newState.CurrentProgram = program;
      }
    }

    void BindTexture(const GLuint handle)
    {
      assert(m_hasSavedState);
      if (handle != m_newState.CurrentTexture)
      {
        glBindTexture(GL_TEXTURE_2D, handle);
        m_newState.CurrentTexture = handle;
      }
    }

    void BindIndexBuffer(const GLuint handle)
    {
      assert(m_hasSavedState);
      if (handle != m_newState.IndexBuffer)
      {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
        m_newState.IndexBuffer = handle;
      }
    }

    void BindVertexBuffer(const GLuint handle)
    {
      assert(m_hasSavedState);
      if (handle != m_newState.VertexBuffer)
      {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        m_newState.VertexBuffer = handle;
      }
    }

    void ChangeVertexAttribsLinks(const VertexElementAttribLinks& vertexElementAttribLinks)
    {
      assert(m_hasSavedState);
      m_attribCache.ChangeAttribs(vertexElementAttribLinks);
    }

  private:
    inline void RestoreState(const GLenum oldState, const GLboolean oldValue)
    {
      if (oldValue != GL_FALSE)
      {
        glEnable(oldState);
      }
      else
      {
        glDisable(oldState);
      }
    }

    static inline GLenum GetEnum(GLenum target)
    {
      GLint value = 0;
      glGetIntegerv(target, &value);
      return UncheckedNumericCast<GLenum>(value);
    }

    static inline void Get(GLenum target, GLboolean& rValue)
    {
      glGetBooleanv(target, &rValue);
    }

    static inline void Get(GLenum target, GLint& rValue)
    {
      glGetIntegerv(target, &rValue);
    }

    static inline void Get(GLenum target, GLuint& rValue)
    {
      GLint value = 0;
      glGetIntegerv(target, &value);
      rValue = UncheckedNumericCast<GLuint>(value);
    }

    static inline PxViewport GetViewport()
    {
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      GLint value[4]{};
      glGetIntegerv(GL_VIEWPORT, value);
      return {value[0], value[1], value[2], value[3]};
    }


    void Reset() noexcept
    {
      if (m_hasSavedState)
      {
        m_hasSavedState = false;
        // Restore the initial state
        if (m_oldState.Blend != m_newState.Blend)
        {
          RestoreState(GL_BLEND, m_oldState.Blend);
        }

        // Restore the blend func if necessary
        if (m_oldState.BlendSrcRGB != m_newState.BlendSrcRGB || m_oldState.BlendDstRGB != m_newState.BlendDstRGB ||
            m_oldState.BlendSrcAlpha != m_newState.BlendSrcAlpha || m_oldState.BlendDstAlpha != m_newState.BlendDstAlpha)
        {
          if (m_oldState.BlendSrcRGB != m_oldState.BlendSrcAlpha || m_oldState.BlendDstRGB != m_oldState.BlendDstAlpha)
          {
            glBlendFuncSeparate(m_oldState.BlendSrcRGB, m_oldState.BlendDstRGB, m_oldState.BlendSrcAlpha, m_oldState.BlendDstAlpha);
          }
          else
          {
            glBlendFunc(m_oldState.BlendSrcRGB, m_oldState.BlendDstRGB);
          }
        }

        if (m_oldState.CullFace != m_newState.CullFace)
        {
          RestoreState(GL_CULL_FACE, m_oldState.CullFace);
        }
        if (m_oldState.DepthTest != m_newState.DepthTest)
        {
          RestoreState(GL_DEPTH_TEST, m_oldState.DepthTest);
        }
        if (m_oldState.DepthMask != m_newState.DepthMask)
        {
          glDepthMask(m_oldState.DepthMask);
        }
        if (m_oldState.ScissorTest != m_newState.ScissorTest)
        {
          RestoreState(GL_SCISSOR_TEST, m_oldState.ScissorTest);
        }
        if (m_oldState.CullFaceMode != m_newState.CullFaceMode)
        {
          glCullFace(m_oldState.CullFaceMode);
        }
        if (m_oldState.CullFrontFace != m_newState.CullFrontFace)
        {
          glFrontFace(m_oldState.CullFrontFace);
        }
        if (m_oldState.DepthFunc != m_newState.DepthFunc)
        {
          glDepthFunc(m_oldState.DepthFunc);
        }
        if (m_oldState.ActiveTexture != m_newState.ActiveTexture)
        {
          glActiveTexture(m_oldState.ActiveTexture);
        }
        if (m_oldState.CurrentTexture != m_newState.CurrentTexture)
        {
          glBindTexture(GL_TEXTURE_2D, m_oldState.CurrentTexture);
        }
        if (m_oldState.CurrentProgram != m_newState.CurrentProgram)
        {
          glUseProgram(m_oldState.CurrentProgram);
        }
        if (m_oldState.VertexBuffer != m_newState.VertexBuffer)
        {
          glBindBuffer(GL_ARRAY_BUFFER, m_oldState.VertexBuffer);
        }
        if (m_oldState.IndexBuffer != m_newState.IndexBuffer)
        {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oldState.IndexBuffer);
        }
        if (m_oldState.Viewport != m_newState.Viewport)
        {
          glViewport(m_oldState.Viewport.X(), m_oldState.Viewport.Y(), m_oldState.Viewport.Width(), m_oldState.Viewport.Height());
        }
        m_attribCache = {};
      }
    }
  };
}

#endif
