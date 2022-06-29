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

#include "RenderSystemBase.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/BasicWindowMetrics.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
//#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemCreateInfo.hpp>
#include "MeshManager.hpp"

namespace Fsl::UI::RenderIMBatch
{
  RenderSystemBase::RenderSystemBase(const RenderSystemCreateInfo& createInfo)
    : m_renderSystem(createInfo.RenderSystem)
    , m_meshManager(std::make_shared<MeshManager>(createInfo.DefaultMaterialInfo))
    , m_windowMetrics(createInfo.WindowMetrics)
    , m_allowDepthBuffer(createInfo.AllowDepthBuffer)
  {
    // m_batcher2.SetLimitOnlyOneEntryPerBatch(true);
    // m_batcher2.SetLimitOnlyOneBatchPerSegment(true);

    OnConfigurationChanged(createInfo.WindowMetrics);
  }


  RenderSystemBase::~RenderSystemBase() = default;


  void RenderSystemBase::OnConfigurationChanged(const BasicWindowMetrics& windowMetrics)
  {
    m_windowMetrics = windowMetrics;
    {    // Setup the orthographic matrix for 2d rendering
      const PxSize2D sizePx = windowMetrics.GetSizePx();

      const auto screenWidth = static_cast<float>(sizePx.Width());
      const auto screenHeight = static_cast<float>(sizePx.Height());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      m_matrixProjection = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                           Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10000.0f);
    }
    m_meshManager->OnConfigurationChanged();
  }


  std::shared_ptr<IMeshManager> RenderSystemBase::GetMeshManager() const
  {
    return m_meshManager;
  }


  DrawCommandBuffer& RenderSystemBase::AcquireDrawCommandBuffer()
  {
    m_meshManager->PreDraw();
    return m_commandBuffer;
  }

  void RenderSystemBase::ReleaseDrawCommandBuffer()
  {
  }

  void RenderSystemBase::PreDraw()
  {
    assert(m_renderSystem);
    m_stats = {};
    m_renderSystem->BeginCache();
  }


  void RenderSystemBase::PostDraw()
  {
    assert(m_renderSystem);
    m_renderSystem->EndCache();
  }
}
