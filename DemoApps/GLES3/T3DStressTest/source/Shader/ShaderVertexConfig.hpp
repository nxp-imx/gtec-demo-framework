#ifndef SHADER_GLES3_T3DSTRESSTEST_SHADERVERTEXCONFIG_HPP
#define SHADER_GLES3_T3DSTRESSTEST_SHADERVERTEXCONFIG_HPP
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

#include <GLES3/gl3.h>
#include <FslUtil/OpenGLES3/GLValues.hpp>

namespace Fsl
{
  struct ShaderVertexConfig
  {
    GLint Position;
    GLint Normal;
    GLint TexCoord;

    ShaderVertexConfig()
      : Position(GLES3::GLValues::INVALID_LOCATION)
      , Normal(GLES3::GLValues::INVALID_LOCATION)
      , TexCoord(GLES3::GLValues::INVALID_LOCATION)
    {
    }

    ShaderVertexConfig(const GLint position)
      : Position(position)
      , Normal(GLES3::GLValues::INVALID_LOCATION)
      , TexCoord(GLES3::GLValues::INVALID_LOCATION)
    {
    }

    ShaderVertexConfig(const GLint position, const GLint normal)
      : Position(position)
      , Normal(normal)
      , TexCoord(GLES3::GLValues::INVALID_LOCATION)
    {
    }

    ShaderVertexConfig(const GLint position, const GLint normal, const GLint texCoord)
      : Position(position)
      , Normal(normal)
      , TexCoord(texCoord)
    {
    }

  };
}

#endif
