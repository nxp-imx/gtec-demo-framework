#ifndef SHADER_GLES3_T3DSTRESSTEST_FURSHADERBASE_HPP
#define SHADER_GLES3_T3DSTRESSTEST_FURSHADERBASE_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/IO/Path.hpp>
#include <GLES3/gl3.h>
#include <vector>
#include "ShaderBase.hpp"

namespace Fsl
{
  class IContentManager;

  class FurShaderBase : public ShaderBase
  {
    int m_lightCount;
    GLint m_locWorld;
    GLint m_locView;
    GLint m_locProjection;
    GLint m_locTexture0;
    GLint m_locTexture1;
    std::vector<GLint> m_locLightDirection;
    std::vector<GLint> m_locLightColor;
    GLint m_locLightCount;
    GLint m_locAmbientColor;
    GLint m_locMaxHairLength;
    GLint m_locDisplacement;

    std::vector<GLfloat> m_lightDirection;
    std::vector<GLfloat> m_lightColor;
  public:
    FurShaderBase(const IContentManager& contentManager, const IO::Path& shaderPath, const bool useHighPrecision, const int lightCount);
    FurShaderBase(const IContentManager& contentManager, const IO::Path& vertShaderPath, const IO::Path& fragShaderPath, const int lightCount);

    void SetWorld(const Matrix& matrix);
    void SetView(const Matrix& matrix);
    void SetProjection(const Matrix& matrix);

    void SetDisplacement(const Vector3& displacement);

    int GetLightCount() const;
    void SetLightDirection(const int index, const Vector3& lightDirection);
    void SetLightColor(const int index, const Vector3& lightColor);
    void SetLightAmbientColor(const Vector3& ambientColor);

    void SetMaxHairLength(const float maxHairLength);

    void SetTexture0(const int unit);
    void SetTexture1(const int unit);

  private:
    void Construct(const int32_t lightCount);
    void SetLightCount(const int lightCount);
  };
}

#endif
