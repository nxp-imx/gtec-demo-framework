#ifndef FSLSIMPLEUI_BASE_RENDER_UIRENDERSYSTEM_HPP
#define FSLSIMPLEUI_BASE_RENDER_UIRENDERSYSTEM_HPP
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

#include <FslSimpleUI/Render/Base/DrawCommandBuffer.hpp>
#include <memory>

namespace Fsl
{
  struct BasicWindowMetrics;

  namespace UI
  {
    class IMeshManager;
    class IRenderSystem;
    class IRenderSystemBase;
    class RenderPerformanceCapture;

    class UIRenderSystem
    {
      std::unique_ptr<IRenderSystem> m_renderSystem;
      bool m_useYFlipTextureCoordinates;

    public:
      class ScopedDrawCommandBufferAccess final
      {
        UIRenderSystem& m_rSystem;
        DrawCommandBuffer& m_rDrawCommandBuffer;

      public:
        explicit ScopedDrawCommandBufferAccess(UIRenderSystem& rSystem)
          : m_rSystem(rSystem)
          , m_rDrawCommandBuffer(m_rSystem.AcquireDrawCommandBuffer())
        {
        }

        ~ScopedDrawCommandBufferAccess()
        {
          m_rSystem.ReleaseDrawCommandBuffer();
        }

        DrawCommandBuffer& GetDrawCommandBuffer() const
        {
          return m_rDrawCommandBuffer;
        }
      };


      explicit UIRenderSystem(std::unique_ptr<IRenderSystem> renderSystem, const bool useYFlipTextureCoordinates);
      ~UIRenderSystem();

      std::shared_ptr<IMeshManager> GetMeshManager() const;

      void ConfigurationChanged(const BasicWindowMetrics& windowMetrics);

      void PreDraw();
      void Draw(RenderPerformanceCapture* const pPerformanceCapture);
      void PostDraw();

      const UI::IRenderSystemBase& GetRenderSystem() const;
      UI::IRenderSystemBase* TryGetRenderSystem();

      bool SYS_GetUseYFlipTextureCoordinates() const
      {
        return m_useYFlipTextureCoordinates;
      }

    private:
      DrawCommandBuffer& AcquireDrawCommandBuffer();
      void ReleaseDrawCommandBuffer();
    };
  }
}

#endif
