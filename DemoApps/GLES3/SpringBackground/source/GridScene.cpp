/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include "GridScene.hpp"
#include "GridRender/GridRenderVBGeometry1.hpp"
#include <GLES3/gl3.h>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;


  // Geometry Wars had 60.000 points in its spring grid :)
  GridScene::GridScene(const DemoAppConfig& config, const AtlasTexture2D& texFill, const Point2& gridResolution)
    : m_screenSize(static_cast<float>(config.ScreenResolution.X), static_cast<float>(config.ScreenResolution.Y))
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_texFill(texFill)
    , m_grid(Rect(0, 0, m_screenSize.X, m_screenSize.Y), gridResolution)
    , m_gridRenderNativeBatchBasic(m_grid.GetGridSize())
    , m_gridRenderNativeBatchCRSpline1(m_grid.GetGridSize())
    , m_gridRenderNativeBatchCRSpline2(m_grid.GetGridSize())
    , m_gridRenderNativeBatchCRSpline3(m_grid.GetGridSize())
    , m_gridRenderVB1(m_grid.GetGridSize(), config.ScreenResolution, config.DemoServiceProvider.Get<IContentManager>())
    , m_gridRenderVB2(m_grid.GetGridSize(), config.ScreenResolution, config.DemoServiceProvider.Get<IContentManager>())
    , m_activeRenderQueueIndex(0)
  {
#ifdef FSL_ENABLE_OPENGLES3_1
    m_gridRenderGeometry1 =
      std::make_shared<GridRenderVBGeometry1>(m_grid.GetGridSize(), config.ScreenResolution, config.DemoServiceProvider.Get<IContentManager>());
#endif

    m_renderDeque.push_back(&m_gridRenderNativeBatchBasic);
    m_renderDeque.push_back(&m_gridRenderNativeBatchCRSpline1);
    m_renderDeque.push_back(&m_gridRenderNativeBatchCRSpline2);
    m_renderDeque.push_back(&m_gridRenderNativeBatchCRSpline3);
    m_renderDeque.push_back(&m_gridRenderVB1);
    m_renderDeque.push_back(&m_gridRenderVB2);
    if (m_gridRenderGeometry1)
    {
      m_renderDeque.push_back(m_gridRenderGeometry1.get());
    }
    m_activeRenderQueueIndex = static_cast<int32_t>(m_renderDeque.size() - 1);
  }


  GridScene::~GridScene() = default;


  const char* GridScene::GetRenderName() const
  {
    return GetActiveRender()->GetName();
  }


  void GridScene::PrevGridRender()
  {
    --m_activeRenderQueueIndex;
    if (m_activeRenderQueueIndex < 0)
    {
      m_activeRenderQueueIndex = static_cast<int32_t>(m_renderDeque.size()) - 1;
    }
  }


  void GridScene::NextGridRender()
  {
    ++m_activeRenderQueueIndex;
    if (m_activeRenderQueueIndex >= static_cast<int32_t>(m_renderDeque.size()))
    {
      m_activeRenderQueueIndex = 0;
    }
  }


  void GridScene::SetRenderId(const int32_t renderId)
  {
    m_activeRenderQueueIndex = std::max(std::min(renderId, static_cast<int32_t>(m_renderDeque.size()) - 1), 0);
  }


  void GridScene::ApplyDirectedForce(const Vector3& force, const Vector3& position, const float radius)
  {
    m_grid.ApplyDirectedForce(force, position, radius);
  }


  void GridScene::ApplyImplosiveForce(const float force, const Vector3& position, const float radius)
  {
    m_grid.ApplyImplosiveForce(force, position, radius);
  }


  void GridScene::ApplyExplosiveForce(const float force, const Vector3& position, const float radius)
  {
    m_grid.ApplyExplosiveForce(force, position, radius);
  }


  void GridScene::FixedUpdate(const DemoTime& demoTime)
  {
    m_grid.FixedUpdate();
  }

  void GridScene::Update(const DemoTime& demoTime)
  {
    assert(m_activeRenderQueueIndex >= 0 && m_activeRenderQueueIndex <= static_cast<int32_t>(m_renderDeque.size()));
    auto pActiveGridRender = m_renderDeque[m_activeRenderQueueIndex];
    assert(pActiveGridRender != nullptr);

    m_grid.Update(demoTime, m_screenSize, *pActiveGridRender);
  }


  void GridScene::Draw()
  {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    assert(m_activeRenderQueueIndex >= 0 && m_activeRenderQueueIndex <= static_cast<int32_t>(m_renderDeque.size()));
    auto pActiveGridRender = m_renderDeque[m_activeRenderQueueIndex];
    assert(pActiveGridRender != nullptr);

    GridRenderDrawContext gridDrawContext(m_batch.get(), m_texFill, m_screenSize);

    m_batch->Begin(BlendState::Additive);
    m_grid.Draw(gridDrawContext, *pActiveGridRender);
    m_batch->End();
  }


  IGridRender* GridScene::GetActiveRender() const
  {
    assert(m_activeRenderQueueIndex >= 0 && m_activeRenderQueueIndex <= static_cast<int32_t>(m_renderDeque.size()));
    auto pActiveGridRender = m_renderDeque[m_activeRenderQueueIndex];
    assert(pActiveGridRender != nullptr);
    return pActiveGridRender;
  }
}
