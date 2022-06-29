/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

// Include gtest before anything that includes the polluted standard X11 header that cause all kind of issues withs its bad defines.
#include <gtest/gtest.h>
// Then include the rest
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatch.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchShaders.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/Common.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/TestFixtureFslUtil_Vulkan1_0.hpp>
#include <vector>

using namespace Fsl;
using namespace Fsl::Vulkan;

namespace
{
  class TestFixtureFslUtil_Vulkan1_0_TestQuadBatch : public TestFixtureFslUtil_Vulkan1_0
  {
  public:
    QuadBatch m_quadBatch;
    PxSize2D m_sizePx;

    std::vector<VertexPositionColorTexture> m_verticesEmpty;
    std::vector<VertexPositionColorTexture> m_verticesSimple = {
      VertexPositionColorTexture(),
      VertexPositionColorTexture(),
      VertexPositionColorTexture(),
      VertexPositionColorTexture(),
    };

    VUTextureInfo m_textureInfoInvalid;


    TestFixtureFslUtil_Vulkan1_0_TestQuadBatch()
      : m_quadBatch(ReadOnlySpanUtil::ToVector(QuadBatchShaders::GetVertexShader()),
                    ReadOnlySpanUtil::ToVector(QuadBatchShaders::GetFragmentShader()),
                    ReadOnlySpanUtil::ToVector(QuadBatchShaders::GetSdfFragmentShader()), 4096, true)
      , m_sizePx(640, 480)
    {
    }

    // const VertexPositionColorTexture *const pVertices, const uint32_t length, const VUTextureInfo &textureInfo
  };

  using TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized = TestFixtureFslUtil_Vulkan1_0_TestQuadBatch;
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, BeginFrame_NullHandle)
{
  m_quadBatch.BeginFrame(VK_NULL_HANDLE, 0);
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, EndFrame)
{
  ASSERT_THROW(m_quadBatch.EndFrame(), UsageErrorException);
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, BeginFrame_NullHandle_EndFrame)
{
  m_quadBatch.BeginFrame(VK_NULL_HANDLE, 0);
  m_quadBatch.EndFrame();
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, BeginFrame_NullHandle_DestroyDependentResources)
{
  m_quadBatch.BeginFrame(VK_NULL_HANDLE, 0);
  m_quadBatch.DestroyDependentResources();
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, BeginFrame_NullHandle_Begin_DestroyDependentResources)
{
  m_quadBatch.BeginFrame(VK_NULL_HANDLE, 0);
  m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true);
  m_quadBatch.DestroyDependentResources();
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, Construct_Destruct)
{
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, Begin_NoBeginFrame)
{
  ASSERT_THROW(m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true), UsageErrorException);
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, Begin_End)
{
  ASSERT_THROW(m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true), UsageErrorException);
  ASSERT_THROW(m_quadBatch.End(), UsageErrorException);
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, End)
{
  ASSERT_THROW(m_quadBatch.End(), UsageErrorException);
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_InvalidAllParams)
{
  ASSERT_THROW(m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true), UsageErrorException);
  ASSERT_THROW(m_quadBatch.DrawQuads(nullptr, 0, m_textureInfoInvalid), UsageErrorException);
  ASSERT_THROW(m_quadBatch.End(), UsageErrorException);
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_Empty_InvalidTexture)
{
  ASSERT_THROW(m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true), UsageErrorException);
  ASSERT_THROW(m_quadBatch.DrawQuads(m_verticesEmpty.data(), static_cast<uint32_t>(m_verticesEmpty.size()), m_textureInfoInvalid),
               UsageErrorException);
  ASSERT_THROW(m_quadBatch.End(), UsageErrorException);
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_InvalidTexture)
{
  ASSERT_THROW(m_quadBatch.Begin(m_sizePx, BlendState::Additive, BatchSdfRenderConfig(), true), UsageErrorException);
  ASSERT_THROW(m_quadBatch.DrawQuads(m_verticesSimple.data(), static_cast<uint32_t>(m_verticesSimple.size()), m_textureInfoInvalid),
               UsageErrorException);
  ASSERT_THROW(m_quadBatch.End(), UsageErrorException);
}


TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_OutsideBeginBlock_InvalidAll)
{
  ASSERT_THROW(m_quadBatch.DrawQuads(nullptr, 0, m_textureInfoInvalid), UsageErrorException);
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_OutsideBeginBlock_EmptyQuads_InvalidTexture)
{
  ASSERT_THROW(m_quadBatch.DrawQuads(m_verticesEmpty.data(), static_cast<uint32_t>(m_verticesEmpty.size()), m_textureInfoInvalid),
               UsageErrorException);
}

TEST_F(TestFixtureFslUtil_Vulkan1_0_TestQuadBatch_StateVulkanUninitialized, DrawQuads_NoBeginFrame_OutsideBeginBlock_InvalidTexture)
{
  ASSERT_THROW(m_quadBatch.DrawQuads(m_verticesSimple.data(), static_cast<uint32_t>(m_verticesSimple.size()), m_textureInfoInvalid),
               UsageErrorException);
}
