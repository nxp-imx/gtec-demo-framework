#ifndef FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICMATERIALRECORD_HPP
#define FSLGRAPHICS3D_BASICRENDER_MATERIAL_BASICMATERIALRECORD_HPP
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

#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/BasicCullMode.hpp>
#include <FslGraphics/Render/Basic/BasicFrontFace.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclaration.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <FslGraphics/Render/Basic/Shader/BasicShaderHandle.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicMaterialTracker.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicNativeMaterialRecord.hpp>
#include <memory>
#include <utility>

namespace Fsl::Graphics3D
{
  struct BasicMaterialDetailsRecord
  {
    BasicMaterialInfo MaterialInfo;
    BasicMaterialVariableDeclaration MaterialDecl;
    BasicShaderHandle VertexShaderHandle;
    BasicShaderHandle FragmentShaderHandle;
    VertexDeclaration VertexDecl;

    BasicMaterialDetailsRecord() = default;
    BasicMaterialDetailsRecord(const BasicMaterialInfo& materialInfo, const BasicMaterialVariableDeclarationSpan& materialDecl,
                               const BasicShaderHandle hVertexShader, const BasicShaderHandle hFragmentShader,
                               const VertexDeclarationSpan& vertexDeclaration)
      : MaterialInfo(materialInfo)
      , MaterialDecl(materialDecl)
      , VertexShaderHandle(hVertexShader)
      , FragmentShaderHandle(hFragmentShader)
      , VertexDecl(vertexDeclaration)
    {
    }
  };

  struct BasicMaterialRecord
  {
    bool IsDynamic{false};
    BasicMaterialDetailsRecord Details;
    std::shared_ptr<INativeTexture2D> Texture;
    std::weak_ptr<BasicMaterialTracker> BasicUserObjectTracker;

    /// <summary>
    /// Only valid while a native material is allocated
    /// </summary>
    BasicNativeMaterialRecord Native;

    BasicMaterialRecord() = default;

    BasicMaterialRecord(const BasicMaterialCreateInfo& createInfo, const BasicShaderHandle hVertexShader, const BasicShaderHandle hFragmentShader,
                        std::shared_ptr<INativeTexture2D> texture, const bool isDynamic,
                        const BasicMaterialVariableDeclarationSpan& materialDeclaration, std::weak_ptr<BasicMaterialTracker> basicUserObject)
      : IsDynamic(isDynamic)
      , Details(createInfo.MaterialInfo, materialDeclaration, hVertexShader, hFragmentShader, createInfo.VertexDeclaration)
      , Texture(std::move(texture))
      , BasicUserObjectTracker(std::move(basicUserObject))
    {
      if (!createInfo.IsValid())
      {
        throw std::invalid_argument("Invalid VertexDeclaration");
      }
    }
  };
}

#endif
