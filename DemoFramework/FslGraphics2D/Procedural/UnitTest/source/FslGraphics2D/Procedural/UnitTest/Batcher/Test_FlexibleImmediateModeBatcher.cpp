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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialId.hpp>
#include <FslGraphics2D/Procedural/Batcher/FlexibleImmediateModeBatcher.hpp>

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr float StartZ = 1.0f;
    constexpr float ZAdd = 1.0f;
  }

  // const BatchMaterialId g_materialInfoDefault(0);
  const BatchMaterialId g_materialInfoOpaque0(1);
  const BatchMaterialId g_materialInfoOpaque1(2);
  const BatchMaterialId g_materialInfoTransp0(3);

  class TestBacher_FlexibleImmediateModeBatcher : public TestFixtureFslGraphics
  {
  public:
    FlexibleImmediateModeBatcher m_batcher;

    TestBacher_FlexibleImmediateModeBatcher()
      : m_batcher(1, 1)
    {
    }
  };


  void CheckSpanRect(const ReadOnlySpan<Fsl::VertexPositionColorTexture>& vertexSpan, const float x0, const float y0, const float x1, const float y1,
                     const Fsl::Color& color, const Fsl::NativeTextureArea& textureArea, const float zPos)
  {
    ASSERT_EQ(4u, vertexSpan.size());
    EXPECT_EQ(Vector3(x0, y0, zPos), vertexSpan[0].Position);
    EXPECT_EQ(Vector3(x0, y1, zPos), vertexSpan[1].Position);
    EXPECT_EQ(Vector3(x1, y0, zPos), vertexSpan[2].Position);
    EXPECT_EQ(Vector3(x1, y1, zPos), vertexSpan[3].Position);

    EXPECT_EQ(color, vertexSpan[0].Color);
    EXPECT_EQ(color, vertexSpan[1].Color);
    EXPECT_EQ(color, vertexSpan[2].Color);
    EXPECT_EQ(color, vertexSpan[3].Color);

    EXPECT_EQ(Vector2(textureArea.X0, textureArea.Y0), vertexSpan[0].TextureCoordinate);
    EXPECT_EQ(Vector2(textureArea.X0, textureArea.Y1), vertexSpan[1].TextureCoordinate);
    EXPECT_EQ(Vector2(textureArea.X1, textureArea.Y0), vertexSpan[2].TextureCoordinate);
    EXPECT_EQ(Vector2(textureArea.X1, textureArea.Y1), vertexSpan[3].TextureCoordinate);
  }

  void CheckSpanRect(const ReadOnlySpan<uint16_t>& indexSpan, const uint16_t indexOffset)
  {
    ASSERT_EQ(6u, indexSpan.size());

    EXPECT_EQ(indexOffset + 1u, indexSpan[0]);
    EXPECT_EQ(indexOffset + 0u, indexSpan[1]);
    EXPECT_EQ(indexOffset + 3u, indexSpan[2]);

    EXPECT_EQ(indexOffset + 3u, indexSpan[3]);
    EXPECT_EQ(indexOffset + 0u, indexSpan[4]);
    EXPECT_EQ(indexOffset + 2u, indexSpan[5]);
  }

  void CheckSpanRect(const ReadOnlySpan<Fsl::VertexPositionColorTexture>& vertexSpan, const ReadOnlySpan<VertexPositionColorTexture> expectedSpan,
                     const float zPos)
  {
    ASSERT_EQ(expectedSpan.size(), vertexSpan.size());
    for (std::size_t i = 0; i < expectedSpan.size(); ++i)
    {
      EXPECT_EQ(Vector3(expectedSpan[i].Position.X, expectedSpan[i].Position.Y, zPos), vertexSpan[i].Position);
      EXPECT_EQ(expectedSpan[i].Color, vertexSpan[i].Color);
      EXPECT_EQ(expectedSpan[i].TextureCoordinate, vertexSpan[i].TextureCoordinate);
    }
  }

  void CheckSpanRect(const ReadOnlySpan<uint16_t>& indexSpan, const ReadOnlySpan<uint16_t> expectedIndexSpan, const uint16_t indexOffset)
  {
    ASSERT_EQ(expectedIndexSpan.size(), indexSpan.size());
    for (std::size_t i = 0; i < expectedIndexSpan.size(); ++i)
    {
      EXPECT_EQ(expectedIndexSpan[i] + indexOffset, indexSpan[i]);
    }
  }

  void CheckSpanRect(const InlineRawMeshBuilder2D<Fsl::VertexPositionColorTexture, uint16_t>& meshBuilder,
                     const ReadOnlySpan<VertexPositionColorTexture> expectedVertexSpan, const ReadOnlySpan<uint16_t> expectedIndexSpan,
                     const uint16_t indexOffset)
  {
    CheckSpanRect(meshBuilder.VerticesAsReadOnlySpan(), expectedVertexSpan, meshBuilder.GetZPos());
    CheckSpanRect(meshBuilder.IndicesAsReadOnlySpan(), expectedIndexSpan, indexOffset);
  }

  void CheckSpanRect(const InlineRawMeshBuilder2D<Fsl::VertexPositionColorTexture, uint16_t>& meshBuilder, const float x0, const float y0,
                     const float x1, const float y1, const Fsl::Color& color, const Fsl::NativeTextureArea& textureArea, const uint16_t indexOffset)
  {
    CheckSpanRect(meshBuilder.VerticesAsReadOnlySpan(), x0, y0, x1, y1, color, textureArea, meshBuilder.GetZPos());
    CheckSpanRect(meshBuilder.IndicesAsReadOnlySpan(), indexOffset);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, Construct_Default)
{
  EXPECT_FALSE(m_batcher.IsInBatchBuild());
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, BeginBatch)
{
  EXPECT_TRUE(m_batcher.BeginBatch());
  EXPECT_TRUE(m_batcher.IsInBatchBuild());
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, BeginBatch_2x)
{
  EXPECT_TRUE(m_batcher.BeginBatch());
  EXPECT_TRUE(m_batcher.IsInBatchBuild());
  EXPECT_FALSE(m_batcher.BeginBatch());
  EXPECT_TRUE(m_batcher.IsInBatchBuild());
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, EndBatch_NoBegin)
{
  EXPECT_FALSE(m_batcher.IsInBatchBuild());
  EXPECT_FALSE(m_batcher.EndBatch());
  EXPECT_FALSE(m_batcher.IsInBatchBuild());
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, EndBatch)
{
  EXPECT_TRUE(m_batcher.BeginBatch());
  EXPECT_TRUE(m_batcher.IsInBatchBuild());
  EXPECT_TRUE(m_batcher.EndBatch());
  EXPECT_FALSE(m_batcher.IsInBatchBuild());
}

TEST_F(TestBacher_FlexibleImmediateModeBatcher, ForceEndBatch_NoBegin)
{
  EXPECT_FALSE(m_batcher.ForceEndBatch());
}

TEST_F(TestBacher_FlexibleImmediateModeBatcher, ForceEndBatch)
{
  EXPECT_TRUE(m_batcher.BeginBatch());
  EXPECT_TRUE(m_batcher.IsInBatchBuild());
  EXPECT_TRUE(m_batcher.ForceEndBatch());
  EXPECT_FALSE(m_batcher.IsInBatchBuild());
}

TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_NoMesh)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;

  m_batcher.EnsureCapacity(vertexCount, indexCount);

  EXPECT_TRUE(m_batcher.BeginBatch());

  auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount);

  EXPECT_EQ(0u, meshBuilder.GetVertexCount());
  EXPECT_EQ(0u, meshBuilder.GetIndexCount());
  EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
  EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());

  m_batcher.EndMeshBuild(meshBuilder);

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(0u, m_batcher.GetSegmentCount());
  EXPECT_EQ(0u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(0u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(0u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(0u, m_batcher.GetStats().IndexCount);
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_NormalBasicNormal_NoMeshes)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;

  m_batcher.EnsureCapacity(vertexCount, indexCount);

  EXPECT_TRUE(m_batcher.BeginBatch());

  {
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(0u, meshBuilder.GetIndexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
    m_batcher.EndMeshBuild(meshBuilder);
  }

  {
    auto meshBuilder = m_batcher.BeginBasicMeshBuild(g_materialInfoOpaque0, vertexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    m_batcher.EndBasicMeshBuild(meshBuilder);
  }

  {
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(0u, meshBuilder.GetIndexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(0u, m_batcher.GetSegmentCount());
  EXPECT_EQ(0u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(0u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(0u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(0u, m_batcher.GetStats().IndexCount);
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_BasicNormalBasic_NoMeshes)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;

  m_batcher.EnsureCapacity(vertexCount, indexCount);

  EXPECT_TRUE(m_batcher.BeginBatch());

  {
    auto meshBuilder = m_batcher.BeginBasicMeshBuild(g_materialInfoOpaque0, vertexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    m_batcher.EndBasicMeshBuild(meshBuilder);
  }

  {
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(0u, meshBuilder.GetIndexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
    m_batcher.EndMeshBuild(meshBuilder);
  }

  {
    auto meshBuilder = m_batcher.BeginBasicMeshBuild(g_materialInfoOpaque0, vertexCount);
    EXPECT_EQ(0u, meshBuilder.GetVertexCount());
    EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
    m_batcher.EndBasicMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(0u, m_batcher.GetSegmentCount());
  EXPECT_EQ(0u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(0u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(0u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(0u, m_batcher.GetStats().IndexCount);
}

TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  m_batcher.EnsureCapacity(vertexCount, indexCount);

  EXPECT_TRUE(m_batcher.BeginBatch());

  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoTransp0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());


      const uint16_t indexOffset = meshBuilder.GetIndexVertexOffset();

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(1u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices, 0);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoTransp0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect_2X)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  constexpr float quad2X0 = 11.0f;
  constexpr float quad2X1 = 12.0f;
  constexpr float quad2Y0 = 13.0f;
  constexpr float quad2Y1 = 14.0f;
  constexpr NativeTextureArea quad2TextureArea(0.15f, 0.16f, 0.17f, 0.18f);


  uint16_t indexOffset = 0;
  m_batcher.EnsureCapacity(vertexCount * 2, indexCount * 2);
  EXPECT_TRUE(m_batcher.BeginBatch());
  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  indexOffset += 4;
  {    // Mesh1
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad2X0, quad2Y0, quad2X1, quad2Y1, quad2TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(1u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u * 2u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u * 2u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u * 2u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u * 2u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices.subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Vertices.subspan(4, 4), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices.subspan(0, 6), 0);
    CheckSpanRect(segmentSpans.Indices.subspan(6, 6), 4);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoOpaque0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4 * 2), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6 * 2), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect_2XDiffMaterial)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  constexpr float quad2X0 = 11.0f;
  constexpr float quad2X1 = 12.0f;
  constexpr float quad2Y0 = 13.0f;
  constexpr float quad2Y1 = 14.0f;
  constexpr NativeTextureArea quad2TextureArea(0.15f, 0.16f, 0.17f, 0.18f);

  uint16_t indexOffset = 0;
  m_batcher.EnsureCapacity(vertexCount * 2, indexCount * 2);
  EXPECT_TRUE(m_batcher.BeginBatch());
  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  // Since this is a new material it starts a new batch but it does not start a new segment
  indexOffset += 4;
  {    // Mesh1
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque1, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad2X0, quad2Y0, quad2X1, quad2Y1, quad2TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(2u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u * 2u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u * 2u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(2u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u * 2u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u * 2u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices.subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Vertices.subspan(4, 4), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea,
                  LocalConfig::StartZ + LocalConfig::ZAdd);
    CheckSpanRect(segmentSpans.Indices.subspan(0, 6), 0);
    CheckSpanRect(segmentSpans.Indices.subspan(6, 6), 4);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoOpaque0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6), batchRecord.IndexSpanRange);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(1);
    EXPECT_EQ(g_materialInfoOpaque1, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(4, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(6, 6), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect_3X_2xDiffMaterial)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  constexpr float quad2X0 = 11.0f;
  constexpr float quad2X1 = 12.0f;
  constexpr float quad2Y0 = 13.0f;
  constexpr float quad2Y1 = 14.0f;
  constexpr NativeTextureArea quad2TextureArea(0.15f, 0.16f, 0.17f, 0.18f);

  constexpr float quad3X0 = 15.0f;
  constexpr float quad3X1 = 16.0f;
  constexpr float quad3Y0 = 17.0f;
  constexpr float quad3Y1 = 18.0f;
  constexpr NativeTextureArea quad3TextureArea(0.19f, 0.20f, 0.21f, 0.22f);

  uint16_t indexOffset = 0;
  m_batcher.EnsureCapacity(vertexCount * 3, indexCount * 3);
  EXPECT_TRUE(m_batcher.BeginBatch());
  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0, meshBuilder.GetVertexCount());
      EXPECT_EQ(0, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  indexOffset += 4;
  {    // Mesh1
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad2X0, quad2Y0, quad2X1, quad2Y1, quad2TextureArea);

      EXPECT_EQ(4, meshBuilder.GetVertexCount());
      EXPECT_EQ(6, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  // This starts a new batch but it does not start a new segment
  indexOffset += 4;
  {    // Mesh2
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque1, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0, meshBuilder.GetVertexCount());
      EXPECT_EQ(0, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad3X0, quad3Y0, quad3X1, quad3Y1, quad3TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad3X0, quad3Y0, quad3X1, quad3Y1, color, quad3TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(2u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u * 3u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u * 3u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(2u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u * 3u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u * 3u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices.subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Vertices.subspan(4, 4), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices.subspan(0, 6), 0);
    CheckSpanRect(segmentSpans.Indices.subspan(6, 6), 4);
    // New batch inside the same segment
    CheckSpanRect(segmentSpans.Vertices.subspan(8, 4), quad3X0, quad3Y0, quad3X1, quad3Y1, color, quad3TextureArea,
                  LocalConfig::StartZ + LocalConfig::ZAdd);
    CheckSpanRect(segmentSpans.Indices.subspan(6 * 2, 6), 8);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoOpaque0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4 * 2), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6 * 2), batchRecord.IndexSpanRange);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(1);
    EXPECT_EQ(g_materialInfoOpaque1, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(4 * 2, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(6 * 2, 6), batchRecord.IndexSpanRange);
  }
}

TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect_PxAreaRectangleF_2XDiffMaterial)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  constexpr float quad2X0 = 11.0f;
  constexpr float quad2X1 = 12.0f;
  constexpr float quad2Y0 = 13.0f;
  constexpr float quad2Y1 = 14.0f;
  constexpr NativeTextureArea quad2TextureArea(0.15f, 0.16f, 0.17f, 0.18f);

  uint16_t indexOffset = 0;
  m_batcher.EnsureCapacity(vertexCount * 2, indexCount * 2);
  EXPECT_TRUE(m_batcher.BeginBatch());
  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(quad1X0, quad1Y0, quad1X1, quad1Y1), quad1TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  // This starts a new batch but it does not start a new segment
  indexOffset += 4;
  {    // Mesh1
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque1, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(PxAreaRectangleF::CreateFromLeftTopRightBottom(quad2X0, quad2Y0, quad2X1, quad2Y1), quad2TextureArea);

      EXPECT_EQ(4u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4u, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6u, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(2u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u * 2u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u * 2u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(2u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u * 2u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u * 2u, segmentSpans.Indices.size());

    CheckSpanRect(segmentSpans.Vertices.subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices.subspan(0, 6), 0);

    CheckSpanRect(segmentSpans.Vertices.subspan(4, 4), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea,
                  LocalConfig::StartZ + LocalConfig::ZAdd);
    CheckSpanRect(segmentSpans.Indices.subspan(6, 6), 4);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoOpaque0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4 * 1), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6 * 1), batchRecord.IndexSpanRange);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(1);
    EXPECT_EQ(g_materialInfoOpaque1, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(4 * 1, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(6 * 1, 6), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect2X)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  constexpr float quad2X0 = 9.0f;
  constexpr float quad2X1 = 10.0f;
  constexpr float quad2Y0 = 11.0f;
  constexpr float quad2Y1 = 12.0f;
  constexpr NativeTextureArea quad2TextureArea(0.13f, 0.14f, 0.15f, 0.16f);

  const uint16_t indexOffset = 0;
  m_batcher.EnsureCapacity(vertexCount * 2, indexCount * 2);
  EXPECT_TRUE(m_batcher.BeginBatch());

  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoOpaque0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0u, meshBuilder.GetVertexCount());
      EXPECT_EQ(0u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());
      EXPECT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);
      meshBuilder.AddRect(quad2X0, quad2Y0, quad2X1, quad2Y1, quad2TextureArea);

      EXPECT_EQ(4u * 2u, meshBuilder.GetVertexCount());
      EXPECT_EQ(6u * 2u, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - (4 * 2), meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - (6 * 2), meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder.VerticesAsReadOnlySpan().subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea,
                    LocalConfig::StartZ);
      CheckSpanRect(meshBuilder.VerticesAsReadOnlySpan().subspan(4, 8), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea,
                    LocalConfig::StartZ);
      CheckSpanRect(meshBuilder.IndicesAsReadOnlySpan().subspan(0, 6), indexOffset);
      CheckSpanRect(meshBuilder.IndicesAsReadOnlySpan().subspan(6, 6), indexOffset + 4);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(1u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u * 2u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u * 2u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u * 2u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u * 2u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices.subspan(0, 4), quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Vertices.subspan(4, 4), quad2X0, quad2Y0, quad2X1, quad2Y1, color, quad2TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices.subspan(0, 6), 0);
    CheckSpanRect(segmentSpans.Indices.subspan(6, 6), 4);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoOpaque0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4 * 2), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6 * 2), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AddRect_SwitchBetween)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);

  m_batcher.EnsureCapacity(vertexCount, indexCount);

  EXPECT_TRUE(m_batcher.BeginBatch());

  {    // Mesh0
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoTransp0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0, meshBuilder.GetVertexCount());
      EXPECT_EQ(0, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());


      const uint16_t indexOffset = meshBuilder.GetIndexVertexOffset();

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4, meshBuilder.GetVertexCount());
      EXPECT_EQ(6, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }

  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(1u, m_batcher.GetSegmentCount());
  EXPECT_EQ(1u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(1u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(4u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(6u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(4u, segmentSpans.Vertices.size());
    EXPECT_EQ(6u, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices, 0);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoTransp0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6), batchRecord.IndexSpanRange);
  }
}


TEST_F(TestBacher_FlexibleImmediateModeBatcher, MeshBuild_AtOverVertexCapacity_AddRect)
{
  constexpr uint16_t vertexCount = 30;
  constexpr uint16_t indexCount = 60;
  constexpr Color color(0xFFFEFDFC);

  constexpr float quad1X0 = 1.0f;
  constexpr float quad1X1 = 2.0f;
  constexpr float quad1Y0 = 3.0f;
  constexpr float quad1Y1 = 4.0f;
  constexpr NativeTextureArea quad1TextureArea(0.5f, 0.6f, 0.7f, 0.8f);
  constexpr uint32_t mesh0VertexCount = 0xFFFF;
  constexpr uint32_t mesh0IndexCount = 0xFFFF;
  std::vector<VertexPositionColorTexture> vertices(mesh0VertexCount);
  std::vector<uint16_t> indices(mesh0IndexCount);

  m_batcher.EnsureCapacity(mesh0VertexCount + vertexCount, mesh0IndexCount + indexCount);
  EXPECT_EQ(0u, m_batcher.GetSegmentCount());

  uint32_t indexOffset = 0u;
  EXPECT_TRUE(m_batcher.BeginBatch());
  {
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoTransp0, mesh0VertexCount, mesh0IndexCount, color);
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
      auto val = static_cast<float>(i);
      vertices[i] = VertexPositionColorTexture(Vector3(val, val * 10.0f, LocalConfig::StartZ), color, Vector2(val * 20.0f, val * 30.0f));
      indices[i] = UncheckedNumericCast<uint16_t>(i);
      meshBuilder.AddVertex(vertices[i].Position.X, vertices[i].Position.Y, vertices[i].TextureCoordinate.X, vertices[i].TextureCoordinate.Y);
      meshBuilder.AddIndex(indices[i]);
    }
    EXPECT_EQ(mesh0VertexCount, meshBuilder.GetVertexCount());
    EXPECT_EQ(mesh0IndexCount, meshBuilder.GetIndexCount());
    EXPECT_EQ(0u, meshBuilder.GetRemainingVertexCapacity());
    EXPECT_EQ(0u, meshBuilder.GetRemainingIndexCapacity());
    CheckSpanRect(meshBuilder, ReadOnlySpanUtil::AsSpan(vertices), ReadOnlySpanUtil::AsSpan(indices), indexOffset);
    m_batcher.EndMeshBuild(meshBuilder);
  }

  {    // Mesh1
    // This starts a new segment as the vertex capacity exceeds what can be addressed with 16bit indices
    auto meshBuilder = m_batcher.BeginMeshBuild(g_materialInfoTransp0, vertexCount, indexCount, color);
    {
      // Add a simple rect
      EXPECT_EQ(0, meshBuilder.GetVertexCount());
      EXPECT_EQ(0, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount, meshBuilder.GetRemainingIndexCapacity());

      ASSERT_EQ(indexOffset, meshBuilder.GetIndexVertexOffset());

      meshBuilder.AddRect(quad1X0, quad1Y0, quad1X1, quad1Y1, quad1TextureArea);

      EXPECT_EQ(4, meshBuilder.GetVertexCount());
      EXPECT_EQ(6, meshBuilder.GetIndexCount());
      EXPECT_EQ(vertexCount - 4, meshBuilder.GetRemainingVertexCapacity());
      EXPECT_EQ(indexCount - 6, meshBuilder.GetRemainingIndexCapacity());
      CheckSpanRect(meshBuilder, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, indexOffset);
    }
    m_batcher.EndMeshBuild(meshBuilder);
  }
  EXPECT_TRUE(m_batcher.EndBatch());

  EXPECT_EQ(2u, m_batcher.GetSegmentCount());
  EXPECT_EQ(2u, m_batcher.GetStats().SegmentCount);
  EXPECT_EQ(2u, m_batcher.GetStats().BatchCount);
  EXPECT_EQ(mesh0VertexCount + 4u, m_batcher.GetStats().VertexCount);
  EXPECT_EQ(mesh0IndexCount + 6u, m_batcher.GetStats().IndexCount);

  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(0);
    EXPECT_EQ(0u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentBatchInfo = m_batcher.GetSegmentBatchInfo(1);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Start);
    EXPECT_EQ(1u, segmentBatchInfo.BatchRange.Length);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(0);
    EXPECT_EQ(mesh0VertexCount, segmentSpans.Vertices.size());
    EXPECT_EQ(mesh0IndexCount, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices, ReadOnlySpanUtil::AsSpan(vertices), LocalConfig::StartZ);
    CheckSpanRect(segmentSpans.Indices, ReadOnlySpanUtil::AsSpan(indices), 0);
  }
  {
    auto segmentSpans = m_batcher.GetSegmentSpans(1);
    EXPECT_EQ(4, segmentSpans.Vertices.size());
    EXPECT_EQ(6, segmentSpans.Indices.size());
    CheckSpanRect(segmentSpans.Vertices, quad1X0, quad1Y0, quad1X1, quad1Y1, color, quad1TextureArea, LocalConfig::StartZ + LocalConfig::ZAdd);
    CheckSpanRect(segmentSpans.Indices, 0);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(0);
    EXPECT_EQ(g_materialInfoTransp0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, mesh0VertexCount), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, mesh0IndexCount), batchRecord.IndexSpanRange);
  }
  {
    const auto& batchRecord = m_batcher.GetBatchRecord(1);
    EXPECT_EQ(g_materialInfoTransp0, batchRecord.Info.MaterialId);
    EXPECT_EQ(BatchContentType::VerticesAndIndices, batchRecord.Info.ContentType);
    EXPECT_EQ(SpanRange<uint32_t>(0, 4), batchRecord.VertexSpanRange);
    EXPECT_EQ(SpanRange<uint32_t>(0, 6), batchRecord.IndexSpanRange);
  }
}
