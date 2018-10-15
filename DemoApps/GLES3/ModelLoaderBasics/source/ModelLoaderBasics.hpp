#ifndef GLES3_MODELLOADERBASICS_MODELLOADERBASICS_HPP
#define GLES3_MODELLOADERBASICS_MODELLOADERBASICS_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLIndexBufferArray.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBufferArray.hpp>
#include <vector>

namespace Fsl
{
  namespace Graphics3D
  {
    class SceneNode;
  }

  class ModelLoaderBasics : public DemoAppGLES3
  {
    GLES3::GLProgram m_program;
    GLES3::GLTexture m_texture;
    GLES3::GLIndexBufferArray m_indexBuffers;
    GLES3::GLVertexBufferArray m_vertexBuffers;
    GLint m_locWorldView;
    GLint m_locWorldViewProjection;
    GLint m_locNormalMatrix;
    GLint m_locTexture0;
    GLint m_locLightDirection;
    GLint m_locLightColor;
    GLint m_locAmbientColor;

    std::vector<GLES3::GLVertexAttribLink> m_attribLink;

    std::shared_ptr<Graphics3D::SceneNode> m_rootNode;
    Matrix m_matrixWorld;
    Matrix m_matrixView;
    Matrix m_matrixProjection;
    Matrix m_matrixWorldView;
    Matrix m_matrixWorldViewProjection;
    Matrix3 m_matrixNormal;
    Vector3 m_rotationSpeed;
    Vector3 m_rotation;
    Vector3 m_lightDirection;
    Vector3 m_lightColor;
    Vector3 m_ambientColor;

  public:
    ModelLoaderBasics(const DemoAppConfig& config);
    ~ModelLoaderBasics() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void DrawMeshes();
  };
}

#endif
