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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/SpanRangeUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatcherAddMeshFlags.hpp>
#include <FslGraphics2D/Procedural/Batcher/ImmediateModeBatcher.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint32_t MaxVertexSegmentCapacity = 0x7FFFFFFF;
      constexpr const uint32_t MaxIndexSegmentCapacity = 0x7FFFFFFF;
      // The max vertex capacity for index based meshes
      constexpr const uint32_t MaxIndexBasedMeshVertexCapacity = 0xFFFF;

      constexpr const uint32_t DefaultVertexCapacity = 4 * 20000;
      constexpr const uint32_t DefaultIndexCapacity = 6 * 20000;
      constexpr const ImmediateModeBatcher::size_type DefaultSegmentCapacity = 32;
      constexpr const ImmediateModeBatcher::size_type DefaultBatchCapacity = 128;
      constexpr const std::size_t GrowSegmentsBy = 32;
      constexpr const std::size_t GrowBatchesBy = 32;
      constexpr const std::size_t GrowVerticesBy = 4 * 5000;
      constexpr const std::size_t GrowIndicesBy = 6 * 5000;
    }

    //! brief Finalize the previous batch info by writing it
    inline void FinalizeBatchInfo(std::vector<BatchRecord>& rBatches, ImmediateModeBatcherTypes::BatchBuildStatus& rBatchBuildStatus)
    {
      // Ensure we have capacity
      if (rBatchBuildStatus.BatchCount >= rBatches.size())
      {
        // Reserve room for more batches
        rBatches.resize(rBatchBuildStatus.BatchCount + LocalConfig::GrowBatchesBy);
      }
      rBatches[rBatchBuildStatus.BatchCount] =
        BatchRecord(rBatchBuildStatus.CurrentBatch.Info,
                    SpanRangeUtil::AsSpanRange(rBatchBuildStatus.CurrentBatch.SegmentVertexOffset, rBatchBuildStatus.CurrentBatch.VertexCount,
                                               OptimizationCheckFlag::NoCheck),
                    SpanRangeUtil::AsSpanRange(rBatchBuildStatus.CurrentBatch.SegmentIndexOffset, rBatchBuildStatus.CurrentBatch.IndexCount,
                                               OptimizationCheckFlag::NoCheck));
      ++rBatchBuildStatus.BatchCount;
    }


    inline void FinalizeSegmentInfo(std::vector<ImmediateModeBatcherTypes::SegmentRecord>& rSegments,
                                    ImmediateModeBatcherTypes::BatchBuildStatus& rBatchBuildStatus)
    {
      if (rBatchBuildStatus.SegmentCount >= rSegments.size())
      {
        // Reserve room for more segments
        rSegments.resize(rBatchBuildStatus.SegmentCount + LocalConfig::GrowSegmentsBy);
      }

      {    // Finalize the previous segment by writing it
        assert(rBatchBuildStatus.CurrentSegment.BatchStartIndex < rBatchBuildStatus.BatchCount);

        rSegments[rBatchBuildStatus.SegmentCount] = ImmediateModeBatcherTypes::SegmentRecord(
          SpanRangeUtil::FromStartAndEnd(rBatchBuildStatus.CurrentSegment.BatchStartIndex, rBatchBuildStatus.BatchCount,
                                         OptimizationCheckFlag::NoCheck),
          SpanRangeUtil::AsSpanRange(rBatchBuildStatus.CurrentSegment.VertexOffset, rBatchBuildStatus.CurrentSegment.VertexCount,
                                     OptimizationCheckFlag::NoCheck),
          SpanRangeUtil::AsSpanRange(rBatchBuildStatus.CurrentSegment.IndexOffset, rBatchBuildStatus.CurrentSegment.IndexCount,
                                     OptimizationCheckFlag::NoCheck));
      }
      ++rBatchBuildStatus.SegmentCount;
    }
  }

  ImmediateModeBatcher::MeshLimitsRecord::MeshLimitsRecord(const uint32_t maxPureVertexCapacity, const uint32_t maxVertexCapacity,
                                                           const uint32_t maxIndexCapacity, const uint32_t triggerNewSegmentPureVertexCapacity,
                                                           const uint32_t triggerNewSegmentVertexCapacity)
    : MaxPureVertexCapacity(maxPureVertexCapacity)
    , MaxVertexCapacity(maxVertexCapacity)
    , MaxIndexCapacity(maxIndexCapacity)
    , TriggerNewSegmentPureVertexCapacity(std::min(triggerNewSegmentPureVertexCapacity, maxPureVertexCapacity))
    , TriggerNewSegmentVertexCapacity(std::min(triggerNewSegmentVertexCapacity, maxVertexCapacity))
  {
  }


  ImmediateModeBatcher::ImmediateModeBatcher()
    : ImmediateModeBatcher(LocalConfig::DefaultVertexCapacity, LocalConfig::DefaultIndexCapacity)
  {
  }


  ImmediateModeBatcher::ImmediateModeBatcher(const size_type vertexCapacity, const size_type indexCapacity)
    : m_meshLimits(LocalConfig::MaxVertexSegmentCapacity, LocalConfig::MaxIndexBasedMeshVertexCapacity, LocalConfig::MaxIndexSegmentCapacity,
                   LocalConfig::MaxVertexSegmentCapacity, LocalConfig::MaxIndexBasedMeshVertexCapacity)
    , m_segments(std::max(LocalConfig::DefaultSegmentCapacity, 1u))
    , m_batches(std::max(LocalConfig::DefaultBatchCapacity, 1u))
    , m_vertices(vertexCapacity > 0 ? vertexCapacity : LocalConfig::DefaultVertexCapacity)
    , m_indices(indexCapacity > 0 ? indexCapacity : LocalConfig::DefaultIndexCapacity)
  {
  }


  ImmediateModeBatcher::~ImmediateModeBatcher()
  {
    FSLLOG3_VERBOSE4("ImmediateModeBatcher final vertexCapacity: {} indexCapacity: {}", m_vertices.size(), m_indices.size());
  }

  bool ImmediateModeBatcher::BeginBatch()
  {
    bool noIssues = true;
    if (m_batchBuildStatus.Building)
    {
      FSLLOG3_WARNING("Already in a mesh building section, force ending");
      EndBatch();
      noIssues = false;
    }
    m_batchBuildStatus = ImmediateModeBatcherTypes::BatchBuildStatus(true);
    m_batchBuildStatus.ZPos += m_zPosAdd;
    ++m_batchBuildId;
    return noIssues;
  }

  bool ImmediateModeBatcher::EndBatch()
  {
    if (!m_batchBuildStatus.Building)
    {
      FSLLOG3_WARNING("Not in a mesh change state");
      return false;
    }
    m_batchBuildStatus.Building = false;
    if (m_batchBuildStatus.CurrentBatch.VertexCount > 0)
    {    // We have a batch that was not written yet, so lets write it
      FinalizeBatchInfo(m_batches, m_batchBuildStatus);
      // Ensure we don't detect it twice
      m_batchBuildStatus.CurrentBatch = {};
    }
    if (m_batchBuildStatus.CurrentSegment.VertexCount > 0)
    {
      FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
      // Ensure we don't detect it twice
      m_batchBuildStatus.CurrentSegment = {};
    }
    return true;
  }

  bool ImmediateModeBatcher::ForceEndBatch()
  {
    if (!m_batchBuildStatus.Building)
    {
      return false;
    }

    FSLLOG3_WARNING("Force ending build operation");
    m_batchBuildStatus = {};
    return true;
  }

  void ImmediateModeBatcher::EnsureVertexCapacity(const size_type vertexCapacity)
  {
    if (vertexCapacity > m_vertices.size())
    {
      m_vertices.resize(vertexCapacity + LocalConfig::GrowVerticesBy);
    }
  }

  void ImmediateModeBatcher::EnsureIndexCapacity(const size_type indexCapacity)
  {
    if (indexCapacity > m_indices.size())
    {
      m_indices.resize(indexCapacity + LocalConfig::GrowIndicesBy);
    }
  }


  UIRawBasicMeshBuilder2D ImmediateModeBatcher::BeginBasicMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity,
                                                                    const Color color, const BatcherAddMeshFlags flags)
  {
    assert(vertexCapacity <= m_meshLimits.MaxPureVertexCapacity);
    assert(m_meshLimits.TriggerNewSegmentPureVertexCapacity <= m_meshLimits.MaxPureVertexCapacity);
    if (vertexCapacity > m_meshLimits.MaxPureVertexCapacity)
    {
      throw NotSupportedException(fmt::format("this type of meshes can not exceed {} vertices", m_meshLimits.MaxPureVertexCapacity));
    }

    assert(m_batchBuildStatus.Building);

    {    // Decide how we should batch this specific mesh
      if (m_batchBuildStatus.CurrentBatch.VertexCount > 0 &&
          (m_batchBuildStatus.CurrentBatch.Info.ContentType != BatchContentType::Vertices ||
           m_batchBuildStatus.CurrentBatch.Info.MaterialId != materialId ||
           (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity))
      {    // Create a new batch
        FinalizeBatchInfo(m_batches, m_batchBuildStatus);

        // Check if we need to force generate a new segment
        if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity)
        {
          FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
          // Prepare to track a new segment
          m_batchBuildStatus.CurrentSegment = ImmediateModeBatcherTypes::CurrentSegementRecord(
            m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount);
        }

        if (!BatcherAddMeshFlagsUtil::IsEnabled(flags, BatcherAddMeshFlags::DontIncreaseZPos))
        {
          m_batchBuildStatus.ZPos += m_zPosAdd;
        }

        // Prepare the new batch
        m_batchBuildStatus.CurrentBatch = ImmediateModeBatcherTypes::CurrentBatchRecord(
          BatchContentType::Vertices, materialId, m_batchBuildStatus.CurrentSegment.VertexCount, m_batchBuildStatus.CurrentSegment.IndexCount);
      }
      else if (m_batchBuildStatus.CurrentBatch.VertexCount <= 0)
      {
        // Patch the material
        m_batchBuildStatus.CurrentBatch.Info = BatchInfo(BatchContentType::Vertices, materialId);
      }
    }

    // finally create and return the mesh builder for the active batch
    assert((vertexCapacity + m_batchBuildStatus.VertexCount) <= m_vertices.size());
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::Vertices);
    return UIRawBasicMeshBuilder2D(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, m_batchBuildStatus.ZPos, color);
  }


  UIRawBasicMeshBuilder2D ImmediateModeBatcher::BeginBasicMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity,
                                                                           const float manualZPos, const Color color)
  {
    assert(vertexCapacity <= m_meshLimits.MaxPureVertexCapacity);
    assert(m_meshLimits.TriggerNewSegmentPureVertexCapacity <= m_meshLimits.MaxPureVertexCapacity);
    if (vertexCapacity > m_meshLimits.MaxPureVertexCapacity)
    {
      throw NotSupportedException(fmt::format("this type of meshes can not exceed {} vertices", m_meshLimits.MaxPureVertexCapacity));
    }

    assert(m_batchBuildStatus.Building);

    {    // Decide how we should batch this specific mesh
      if (m_batchBuildStatus.CurrentBatch.VertexCount > 0 &&
          (m_batchBuildStatus.CurrentBatch.Info.ContentType != BatchContentType::Vertices ||
           m_batchBuildStatus.CurrentBatch.Info.MaterialId != materialId ||
           (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity))
      {    // Create a new batch
        FinalizeBatchInfo(m_batches, m_batchBuildStatus);

        // Check if we need to force generate a new segment
        if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity)
        {
          FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
          // Prepare to track a new segment
          m_batchBuildStatus.CurrentSegment = ImmediateModeBatcherTypes::CurrentSegementRecord(
            m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount);
        }

        // Prepare the new batch
        m_batchBuildStatus.CurrentBatch = ImmediateModeBatcherTypes::CurrentBatchRecord(
          BatchContentType::Vertices, materialId, m_batchBuildStatus.CurrentSegment.VertexCount, m_batchBuildStatus.CurrentSegment.IndexCount);
      }
      else if (m_batchBuildStatus.CurrentBatch.VertexCount <= 0)
      {
        // Patch the material
        m_batchBuildStatus.CurrentBatch.Info = BatchInfo(BatchContentType::Vertices, materialId);
      }
    }

    // finally create and return the mesh builder for the active batch
    assert((vertexCapacity + m_batchBuildStatus.VertexCount) <= m_vertices.size());
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::Vertices);
    return UIRawBasicMeshBuilder2D(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, manualZPos, color);
  }

  void ImmediateModeBatcher::EndBasicMeshBuild(UIRawBasicMeshBuilder2D& rBuilder)
  {
    const uint32_t builderVertexCount = rBuilder.GetVertexCount();
    assert(m_batchBuildStatus.Building);

    // Increase the current position in the vertex and index buffer scratch pads
    m_batchBuildStatus.VertexCount += builderVertexCount;

    // Update the active batch
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::Vertices);
    assert(m_batchBuildStatus.CurrentBatch.IndexCount == 0u);
    m_batchBuildStatus.CurrentBatch.VertexCount += builderVertexCount;

    // Update the active segment
    m_batchBuildStatus.CurrentSegment.VertexCount += builderVertexCount;
  }


  UIRawMeshBuilder2D ImmediateModeBatcher::BeginMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity,
                                                          const size_type indexCapacity, const Color color, const BatcherAddMeshFlags flags)
  {
    assert(m_batchBuildStatus.Building);
    assert(vertexCapacity <= m_meshLimits.MaxVertexCapacity);
    assert(m_meshLimits.TriggerNewSegmentVertexCapacity <= m_meshLimits.MaxVertexCapacity);
    if (vertexCapacity > m_meshLimits.MaxVertexCapacity)
    {
      throw NotSupportedException(fmt::format("this type of meshes can not exceed {} vertices", m_meshLimits.MaxVertexCapacity));
    }

    {    // Decide how we should batch this specific mesh
      if (m_batchBuildStatus.CurrentBatch.VertexCount > 0 &&
          (m_batchBuildStatus.CurrentBatch.Info.ContentType != BatchContentType::VerticesAndIndices ||
           m_batchBuildStatus.CurrentBatch.Info.MaterialId != materialId ||
           //(m_batchBuildStatus.CurrentBatch.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
           (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
           (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity))
      {
        FinalizeBatchInfo(m_batches, m_batchBuildStatus);

        // Check if we need to force generate a new segment
        if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
            (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity)
        {
          FinalizeSegmentInfo(m_segments, m_batchBuildStatus);

          // Prepare to track a new segment
          m_batchBuildStatus.CurrentSegment = ImmediateModeBatcherTypes::CurrentSegementRecord(
            m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount);
        }


        // Create a new batch
        if (!BatcherAddMeshFlagsUtil::IsEnabled(flags, BatcherAddMeshFlags::DontIncreaseZPos))
        {
          m_batchBuildStatus.ZPos += m_zPosAdd;
        }


        // Prepare the new batch
        m_batchBuildStatus.CurrentBatch =
          ImmediateModeBatcherTypes::CurrentBatchRecord(BatchContentType::VerticesAndIndices, materialId,
                                                        m_batchBuildStatus.CurrentSegment.VertexCount, m_batchBuildStatus.CurrentSegment.IndexCount);
      }
      else if (m_batchBuildStatus.CurrentBatch.VertexCount <= 0)
      {
        assert(m_batchBuildStatus.CurrentBatch.IndexCount <= 0);
        // Patch the material
        m_batchBuildStatus.CurrentBatch.Info = BatchInfo(BatchContentType::VerticesAndIndices, materialId);
      }
    }

    // finally create and return the mesh builder for the active batch
    assert((vertexCapacity + m_batchBuildStatus.VertexCount) <= m_vertices.size());
    assert((indexCapacity + m_batchBuildStatus.IndexCount) <= m_indices.size());
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::VerticesAndIndices);

    return UIRawMeshBuilder2D(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, m_indices.data() + m_batchBuildStatus.IndexCount,
                              indexCapacity, m_batchBuildStatus.CurrentSegment.VertexCount, m_batchBuildStatus.ZPos, color);
  }


  UIRawMeshBuilder2D ImmediateModeBatcher::BeginMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity,
                                                                 const size_type indexCapacity, const float manualZPos, const Color color)
  {
    assert(m_batchBuildStatus.Building);
    assert(vertexCapacity <= m_meshLimits.MaxVertexCapacity);
    assert(m_meshLimits.TriggerNewSegmentVertexCapacity <= m_meshLimits.MaxVertexCapacity);
    if (vertexCapacity > m_meshLimits.MaxVertexCapacity)
    {
      throw NotSupportedException(fmt::format("this type of meshes can not exceed {} vertices", m_meshLimits.MaxVertexCapacity));
    }

    {    // Decide how we should batch this specific mesh
      if (m_batchBuildStatus.CurrentBatch.VertexCount > 0 &&
          (m_batchBuildStatus.CurrentBatch.Info.ContentType != BatchContentType::VerticesAndIndices ||
           m_batchBuildStatus.CurrentBatch.Info.MaterialId != materialId ||
           //(m_batchBuildStatus.CurrentBatch.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
           (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
           (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity))
      {
        FinalizeBatchInfo(m_batches, m_batchBuildStatus);

        // Check if we need to force generate a new segment
        if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
            (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity)
        {
          FinalizeSegmentInfo(m_segments, m_batchBuildStatus);

          // Prepare to track a new segment
          m_batchBuildStatus.CurrentSegment = ImmediateModeBatcherTypes::CurrentSegementRecord(
            m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount);
        }

        // Prepare the new batch
        m_batchBuildStatus.CurrentBatch =
          ImmediateModeBatcherTypes::CurrentBatchRecord(BatchContentType::VerticesAndIndices, materialId,
                                                        m_batchBuildStatus.CurrentSegment.VertexCount, m_batchBuildStatus.CurrentSegment.IndexCount);
      }
      else if (m_batchBuildStatus.CurrentBatch.VertexCount <= 0)
      {
        assert(m_batchBuildStatus.CurrentBatch.IndexCount <= 0);
        // Patch the material
        m_batchBuildStatus.CurrentBatch.Info = BatchInfo(BatchContentType::VerticesAndIndices, materialId);
      }
    }

    // finally create and return the mesh builder for the active batch
    assert((vertexCapacity + m_batchBuildStatus.VertexCount) <= m_vertices.size());
    assert((indexCapacity + m_batchBuildStatus.IndexCount) <= m_indices.size());
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::VerticesAndIndices);

    return UIRawMeshBuilder2D(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, m_indices.data() + m_batchBuildStatus.IndexCount,
                              indexCapacity, m_batchBuildStatus.CurrentSegment.VertexCount, manualZPos, color);
  }


  void ImmediateModeBatcher::EndMeshBuild(UIRawMeshBuilder2D& rBuilder)
  {
    const uint32_t builderVertexCount = rBuilder.GetVertexCount();
    const uint32_t builderIndexCount = rBuilder.GetIndexCount();
    assert(m_batchBuildStatus.Building);
    // A valid build requires that we either have "vertices and indices" or "no vertices and no indices".
    assert((builderVertexCount > 0 && builderIndexCount > 0) || (builderVertexCount == 0 && builderIndexCount == 0));


    // Increase the current position in the vertex and index buffer scratch pads
    m_batchBuildStatus.VertexCount += builderVertexCount;
    m_batchBuildStatus.IndexCount += builderIndexCount;

    // Update the active batch
    assert(m_batchBuildStatus.CurrentBatch.Info.ContentType == BatchContentType::VerticesAndIndices);
    m_batchBuildStatus.CurrentBatch.VertexCount += builderVertexCount;
    m_batchBuildStatus.CurrentBatch.IndexCount += builderIndexCount;

    // Update the active segment
    m_batchBuildStatus.CurrentSegment.VertexCount += builderVertexCount;
    m_batchBuildStatus.CurrentSegment.IndexCount += builderIndexCount;
  }
}
