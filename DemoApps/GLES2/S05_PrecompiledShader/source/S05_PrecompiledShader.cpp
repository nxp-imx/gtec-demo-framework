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
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include "S05_PrecompiledShader.hpp"
#include "OptionParser.hpp"
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <vector>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    void DumpDebugInformation(const GLuint hShader)
    {
      GLint length;
      GL_ON_ERROR_LOG_AND_RETURN(glGetShaderiv(hShader, GL_SHADER_SOURCE_LENGTH, &length));
      std::vector<GLchar> source(std::max(length, 1));
      source[0] = 0;
      GL_ON_ERROR_LOG_AND_RETURN(glGetShaderSource(hShader, source.size(), nullptr, &source[0]));

      FSLLOG("*** Source start ***\n" << &source[0] << "\n*** Source end ***\n\n");

      // Fetch the log
      GL_ON_ERROR_LOG_AND_RETURN(glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &length));
      std::vector<GLchar> errorLog(std::max(length, 1));
      errorLog[0] = 0;
      GL_ON_ERROR_LOG_AND_RETURN(glGetShaderInfoLog(hShader, errorLog.size(), nullptr, &errorLog[0]));

      FSLLOG("*** Error log start ***\n" << &errorLog[0] << "\n*** Error Log End ***\n\n");
    }
    typedef enum
    {
      GC400T = 0,
      GC880,
      GC2000
    } gpuType_t;
    gpuType_t g_gpuType;
  }


  S05_PrecompiledShader::S05_PrecompiledShader(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_width(0)
    , m_height(0)
    , m_hVertexShader(0)
    , m_hFragmentShader(0)
    , m_hProgram(0)
  {
    const Point2 currentSize = GetScreenResolution();
    m_width = currentSize.X;
    m_height = currentSize.Y;
    const std::shared_ptr<OptionParser> options = config.GetOptions<OptionParser>();

    m_hProgram = PrepareProgram(options->GetSeparateShader());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  }


  S05_PrecompiledShader::~S05_PrecompiledShader()
  {
    if (m_hVertexShader != 0)
    {
      glDeleteShader(m_hVertexShader);
    }
    if (m_hFragmentShader != 0)
    {
      glDeleteShader(m_hFragmentShader);
    }
    if (m_hProgram != 0)
    {
      glDeleteProgram(m_hProgram);
    }
  }


  void S05_PrecompiledShader::Update(const DemoTime& demoTime)
  {
  }


  void S05_PrecompiledShader::Draw(const DemoTime& demoTime)
  {
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};

    // set viewport
    glViewport(0, 0, m_width, m_height);

    // clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_hProgram);

    // load vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
  }


  GLuint S05_PrecompiledShader::LoadProgram(const uint8_t* buf, const int length)
  {
    GL_CHECK(const GLuint hProgram = glCreateProgram());
    if (hProgram == 0)
      throw GraphicsException("glCreateProgram failed");

    try
    {
      GL_CHECK(glProgramBinaryOES(hProgram, GL_PROGRAM_BINARY_VIV, buf, length));
      return hProgram;
    }
    catch (const std::exception&)
    {
      if (hProgram != 0)
      {
        GL_CHECK(glDeleteProgram(hProgram));
      }
      throw;
    }
  }


  GLuint S05_PrecompiledShader::LoadShader(const uint8_t* buf, const GLenum type, const int length)
  {
    // Create the shader object
    GL_CHECK(const GLuint hShader = glCreateShader(type));
    if (hShader == 0)
    {
      throw GraphicsException("glCreateShader failed");
    }

    try
    {
      // Load the precompiled shader binary into the newly created shader object
      // std::vector<uint8_t> alignBuf(length);
      // std::copy(buf, buf+length, alignBuf.begin());
      GL_CHECK(glShaderBinary(1, &hShader, GL_SHADER_BINARY_VIV, buf, length));
      return hShader;
    }
    catch (const std::exception&)
    {
      if (hShader != 0)
        glDeleteShader(hShader);
      throw;
    }
  }


  GLuint S05_PrecompiledShader::PrepareProgram(const bool useSeparateShaders)
  {
    GLuint hProgram = 0;
    const char* localRenderer;
    GL_CHECK(localRenderer = (const char*)glGetString(GL_RENDERER));

    std::string rendererString(localRenderer);
    if (rendererString.compare("Vivante GC2000"))
    {
      if (rendererString.compare("Vivante GC880"))
      {
        g_gpuType = GC400T;
        FSLLOG("GPU = GC400T");
      }
      else
      {
        g_gpuType = GC880;
        FSLLOG("GPU = GC880");
      }
    }
    else
    {
      g_gpuType = GC2000;
      FSLLOG("GPU = GC2000");
    }
    if (useSeparateShaders)
    {
      hProgram = glCreateProgram();
      if (hProgram == 0)
      {
        throw GraphicsException("glCreateProgram failed");
      }

      GLuint hVertexShader = 0;
      GLuint hFragmentShader = 0;
      try
      {
        std::string vertexShaderName("flatES2gc400t.vgcSL");
        std::string fragmentShaderName("flatES2gc400t.pgcSL");

        if (GC2000 == g_gpuType)
        {
          vertexShaderName.assign("flatES2gc2000.vgcSL");
          fragmentShaderName.assign("flatES2gc2000.pgcSL");
        }
        else if (GC880 == g_gpuType)
        {
          vertexShaderName.assign("flatES2gc880.vgcSL");
          fragmentShaderName.assign("flatES2gc880.pgcSL");
        }
        // load vertex/frag shader
        std::vector<uint8_t> buf;
        GetContentManager()->ReadAllBytes(buf, vertexShaderName);
        hVertexShader = LoadShader(buf.data(), GL_VERTEX_SHADER, buf.size());
        GetContentManager()->ReadAllBytes(buf, fragmentShaderName);
        hFragmentShader = LoadShader(buf.data(), GL_FRAGMENT_SHADER, buf.size());

        GL_CHECK(glAttachShader(hProgram, hVertexShader));
        GL_CHECK(glAttachShader(hProgram, hFragmentShader));

        // bind vPosition to attrib 0
        GL_CHECK(glBindAttribLocation(hProgram, 0, "vPosition"));

        // linkage
        GL_CHECK(glLinkProgram(hProgram));

        // check status
        GLint linked;
        GL_CHECK(glGetProgramiv(hProgram, GL_LINK_STATUS, &linked));

        if (!linked)
        {
          DumpDebugInformation(hProgram);
          throw GraphicsException("glLinkProgram failed");
        }

        // If we have no use for the vertex and fragment shader we are allowed to delete them here
        // since they have been attached to a program.
        // But for this example we store the handles instead allowing us to reuse them later.
        m_hVertexShader = hVertexShader;
        m_hFragmentShader = hFragmentShader;
      }
      catch (const std::exception&)
      {
        if (hVertexShader != 0)
        {
          GL_CHECK(glDeleteShader(hVertexShader));
        }
        if (hFragmentShader != 0)
        {
          GL_CHECK(glDeleteShader(hFragmentShader));
        }
        if (hProgram != 0)
        {
          GL_CHECK(glDeleteProgram(hProgram));
        }
        throw;
      }
    }
    else
    {
      std::string shaderProgramName("es20gc400t.gcPGM");
      if (GC2000 == g_gpuType)
      {
        shaderProgramName.assign("es20gc2000.gcPGM");
      }
      else if (GC880 == g_gpuType)
      {
        shaderProgramName.assign("es20gc880.gcPGM");
      }
      std::vector<uint8_t> buf;
      GetContentManager()->ReadAllBytes(buf, shaderProgramName);
      hProgram = LoadProgram(buf.data(), buf.size());
    }
    return hProgram;
  }
}
