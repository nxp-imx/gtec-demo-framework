#ifndef FSLSIMPLEUI_RENDER_IMBATCH_FLEXRENDERSYSTEM_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_FLEXRENDERSYSTEM_HPP
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

#include <FslGraphics2D/Procedural/Batcher/FlexibleImmediateModeBatcher.hpp>
#include <FslSimpleUI/Render/Base/RenderSystemCreateInfo.hpp>
#include <FslSimpleUI/Render/IMBatch/DrawReorderMethod.hpp>
#include <FslSimpleUI/Render/IMBatch/FlexRenderSystemConfig.hpp>
#include <FslSimpleUI/Render/IMBatch/IFlexRenderSystemConfig.hpp>
#include "Preprocess/Linear/LinearPreprocessor.hpp"
#include "Preprocess/SpatialGrid/SpatialGridPreprocessor.hpp"
#include "RenderSystemBase.hpp"

namespace Fsl::UI::RenderIMBatch
{
  class FlexRenderSystem final
    : public RenderSystemBase
    , public IFlexRenderSystemConfig
  {
    FlexibleImmediateModeBatcher m_batcher;
    FlexRenderSystemConfig m_config;
    LinearPreprocessor m_preprocessor;
    SpatialGridPreprocessor m_spatialGridPreprocessor;
    uint32_t m_maxDrawCalls{0xFFFFFFFF};

  public:
    FlexRenderSystem(const FlexRenderSystem&) = delete;
    FlexRenderSystem& operator=(const FlexRenderSystem&) = delete;

    explicit FlexRenderSystem(const RenderSystemCreateInfo& createInfo);

    void OnConfigurationChanged(const BasicWindowMetrics& windowMetrics) final;

    const FlexRenderSystemConfig& GetConfig() const final
    {
      return m_config;
    }

    void SetConfig(const FlexRenderSystemConfig& config) final
    {
      m_batcher.SetLimitOnlyOneEntryPerBatch(!config.Batch);
      m_batcher.SetLimitOnlyOneBatchPerSegment(!config.FillBuffers);
      m_config = config;
      InvalidateDrawCache();
    }

    uint32_t GetMaxDrawCalls() const final
    {
      return m_maxDrawCalls;
    }

    void SetMaxDrawCalls(const uint32_t maxDrawCalls) final
    {
      m_maxDrawCalls = maxDrawCalls;
    }

    void Draw(RenderPerformanceCapture* const pPerformanceCapture) final;
  };
}

#endif
