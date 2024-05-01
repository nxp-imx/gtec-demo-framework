#ifndef FSLSIMPLEUI_RENDER_IMBATCH_RENDERSYSTEMBASE_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_RENDERSYSTEMBASE_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Math/BasicWindowMetrics.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Render/Basic/IBasicDynamicBuffer.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialHandle.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBufferEx.hpp>
#include <FslSimpleUI/Render/Base/IRenderSystem.hpp>
#include <cassert>
#include <memory>
#include <utility>
#include "Preprocess/ProcessedCommandRecord.hpp"
#include "RenderSystemBufferRecord.hpp"

namespace Fsl
{
  struct BasicCameraInfo;
  struct BasicWindowMetrics;
  class IBasicRenderSystem;

  namespace UI
  {
    struct RenderSystemCreateInfo;
  }
}

namespace Fsl::UI::RenderIMBatch
{
  class MeshManager;

  class RenderSystemBase : public IRenderSystem
  {
    std::shared_ptr<IBasicRenderSystem> m_renderSystem;
    std::shared_ptr<MeshManager> m_meshManager;
    BasicWindowMetrics m_windowMetrics;
    bool m_allowDepthBuffer{false};

    std::vector<RenderSystemBufferRecord> m_buffers;

    DrawCommandBufferEx m_commandBuffer;
    bool m_commandBufferCleared{false};
    std::size_t m_commandBufferSizeLastFrame{0};

    Matrix m_matrixProjection;
    RenderSystemStats m_stats;

  protected:
    std::vector<ProcessedCommandRecord> m_processedCommandRecords;

    bool GetAllowDepthBuffer() const noexcept
    {
      return m_allowDepthBuffer;
    }

  public:
    explicit RenderSystemBase(const RenderSystemCreateInfo& createInfo);
    ~RenderSystemBase() override;

    void OnConfigurationChanged(const BasicWindowMetrics& windowMetrics) override;
    std::shared_ptr<IMeshManager> GetMeshManager() const final;

    DrawCommandBuffer& AcquireDrawCommandBuffer(const bool clear) final;
    void ReleaseDrawCommandBuffer() final;

    // Normal operation
    void PreDraw() final;
    void PostDraw() final;

    RenderSystemStats GetStats() const noexcept final
    {
      return m_stats;
    }

    VertexDeclarationSpan AsVertexDeclarationSpan() const final
    {
      return GetVertexDeclarationSpan();
    }

    static VertexDeclarationSpan GetVertexDeclarationSpan()
    {
      return VertexPositionColorTexture::AsVertexDeclarationSpan();
    }

  protected:
    const BasicWindowMetrics& GetWindowMetrics() const noexcept
    {
      return m_windowMetrics;
    }

    const Matrix& GetMatrixProjection() const noexcept
    {
      return m_matrixProjection;
    }

    IBasicRenderSystem& GetRenderSystem() noexcept
    {
      assert(m_renderSystem);
      return *m_renderSystem;
    }

    MeshManager& DoGetMeshManager() noexcept
    {
      assert(m_meshManager);
      return *m_meshManager;
    }

    std::vector<RenderSystemBufferRecord>& GetBuffers() noexcept
    {
      return m_buffers;
    }

    /// <summary>
    /// Check if the command buffer was modified since the last frame
    /// </summary>
    /// <returns></returns>
    bool IsNewCommandBuffer() const noexcept
    {
      return m_commandBufferCleared || m_commandBuffer.Count() != m_commandBufferSizeLastFrame;
    }

    DrawCommandBufferEx& GetCommandBuffer() noexcept
    {
      return m_commandBuffer;
    }

    RenderSystemStats& DoGetStats() noexcept
    {
      return m_stats;
    }

    //! Ensure that the draw cache is invalid so the render operation is done from scratch
    void InvalidateDrawCache() noexcept
    {
      m_commandBufferSizeLastFrame = 0;
    }
  };
}

#endif
