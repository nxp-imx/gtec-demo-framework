#ifndef FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVEDEVICE_HPP
#define FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVEDEVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeTextureHandle.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeBufferFactory.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeMaterialFactory.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeShaderFactory.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeTextureFactory.hpp>

namespace Fsl
{
  struct BasicCameraInfo;
  struct BasicNativeBeginFrameInfo;
  struct BasicNativeDependentCreateInfo;
  class BasicMaterialVariables;

  namespace Graphics3D
  {
    /// <summary>
    /// use it.
    /// </summary>
    class INativeDevice
      : public INativeBufferFactory
      , public INativeShaderFactory
      , public INativeMaterialFactory
      , public INativeTextureFactory
    {
    public:
      virtual void CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo) = 0;
      virtual void DestroyDependentResources() = 0;

      virtual void BeginFrame(const BasicNativeBeginFrameInfo& frameInfo) = 0;
      virtual void EndFrame() noexcept = 0;
      virtual void BeginCache() = 0;
      virtual void EndCache() noexcept = 0;
      virtual void BeginCmds() = 0;
      virtual void EndCmds() noexcept = 0;
      virtual void CmdSetCamera(const BasicCameraInfo& cameraInfo) = 0;
      virtual void CmdBindIndexBuffer(const BasicNativeBufferHandle indexBuffer) = 0;
      virtual void CmdBindMaterial(const BasicNativeMaterialHandle material, const BasicMaterialVariables& materialVariables,
                                   const ReadOnlySpan<BasicNativeTextureHandle> textures) = 0;
      virtual void CmdBindVertexBuffer(const BasicNativeBufferHandle vertexBuffer) = 0;
      virtual void CmdDraw(const uint32_t vertexCount, const uint32_t firstVertex) noexcept = 0;
      virtual void CmdDrawIndexed(const uint32_t indexCount, const uint32_t firstIndex) noexcept = 0;
    };
  }
}

#endif
