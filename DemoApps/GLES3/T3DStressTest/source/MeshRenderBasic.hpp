#ifndef GLES3_T3DSTRESSTEST_MESHRENDERBASIC_HPP
#define GLES3_T3DSTRESSTEST_MESHRENDERBASIC_HPP
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

#include "MeshRender.hpp"
#include <FslBase/BasicTypes.hpp>
#include <FslGraphics3D/Procedural/BasicMesh.hpp>

namespace Fsl
{
  // Render the model using basic client side arrays (slow but easy)
  class MeshRenderBasic : public MeshRender
  {
    const int m_vertexCount;
    const int m_indexCount;
    float* m_pVertices;
    float* m_pNormals;
    float* m_pTextureCoords;
    uint16_t* m_pIndices;
    GLenum m_primitiveType;
  public:
    MeshRenderBasic(const Procedural::BasicMesh& mesh);
    virtual ~MeshRenderBasic();

    virtual void Bind(const ShaderBase& shader);
    virtual void Draw();
    virtual void Unbind();
  };

}
#endif
