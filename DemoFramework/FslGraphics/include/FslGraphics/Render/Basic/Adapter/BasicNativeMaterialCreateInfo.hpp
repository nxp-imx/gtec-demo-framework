#ifndef FSLGRAPHICS_RENDER_BASIC_ADAPTER_BASICNATIVEMATERIALCREATEINFO_HPP
#define FSLGRAPHICS_RENDER_BASIC_ADAPTER_BASICNATIVEMATERIALCREATEINFO_HPP
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

#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>

namespace Fsl
{
  struct BasicNativeMaterialCreateInfo
  {
    BasicMaterialInfo MaterialInfo;
    BasicMaterialVariableDeclarationSpan MaterialDeclaration;
    VertexDeclarationSpan VertexDeclaration;
    BasicNativeShaderHandle VertexShaderHandle;
    BasicNativeShaderHandle FragmentShaderHandle;

    constexpr BasicNativeMaterialCreateInfo() noexcept = default;

    constexpr BasicNativeMaterialCreateInfo(const BasicMaterialInfo& materialInfo, BasicMaterialVariableDeclarationSpan materialDeclaration,
                                            VertexDeclarationSpan vertexDeclaration, const BasicNativeShaderHandle vertexShaderHandle,
                                            const BasicNativeShaderHandle fragmentShaderHandle) noexcept
      : MaterialInfo(materialInfo)
      , MaterialDeclaration(materialDeclaration)
      , VertexDeclaration(vertexDeclaration)
      , VertexShaderHandle(vertexShaderHandle)
      , FragmentShaderHandle(fragmentShaderHandle)
    {
    }

    constexpr bool IsValid() const noexcept
    {
      return !MaterialDeclaration.empty() && !VertexDeclaration.Empty() && VertexShaderHandle.IsValid() && FragmentShaderHandle.IsValid();
    }

    constexpr bool IsCompatible(const BasicMaterialInfo& materialInfo, const VertexDeclarationSpan& vertexDecl,
                                const BasicNativeShaderHandle vertexShaderHandle, const BasicNativeShaderHandle fragmentShaderHandle) const noexcept
    {
      return MaterialInfo == materialInfo && VertexDeclaration == vertexDecl && VertexShaderHandle == vertexShaderHandle &&
             FragmentShaderHandle == fragmentShaderHandle;
    }
  };
}

#endif
