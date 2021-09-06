#ifndef FSLSIMPLEUI_RENDER_STUB_RENDERSYSTEM_HPP
#define FSLSIMPLEUI_RENDER_STUB_RENDERSYSTEM_HPP
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

#include <FslSimpleUI/Render/Base/IRenderSystem.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBufferEx.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    namespace RenderStub
    {
      class MeshManager;

      class RenderSystem final : public IRenderSystem
      {
        DrawCommandBufferEx m_commandBuffer;
        std::shared_ptr<MeshManager> m_meshManager;

      public:
        RenderSystem();
        ~RenderSystem() override;

        VertexDeclarationSpan AsVertexDeclarationSpan() const final
        {
          return GetVertexDeclarationSpan();
        }

        void OnConfigurationChanged(const BasicWindowMetrics& windowMetrics) final
        {
          FSL_PARAM_NOT_USED(windowMetrics);
        }

        std::shared_ptr<IMeshManager> GetMeshManager() const final;
        DrawCommandBuffer& AcquireDrawCommandBuffer() final;
        void ReleaseDrawCommandBuffer() final;

        void PreDraw() final{};
        void Draw(RenderPerformanceCapture* const pPerformanceCapture) final;
        void PostDraw() final{};

        RenderSystemStats GetStats() const final
        {
          return {};
        }

        static VertexDeclarationSpan GetVertexDeclarationSpan();
      };
    }
  }
}

#endif
