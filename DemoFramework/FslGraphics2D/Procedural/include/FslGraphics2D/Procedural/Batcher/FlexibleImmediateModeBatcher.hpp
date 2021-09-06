#ifndef FSLGRAPHICS2D_PROCEDURAL_BATCHER_FLEXIBLEIMMEDIATEMODEBATCHER_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BATCHER_FLEXIBLEIMMEDIATEMODEBATCHER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchContentType.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatcherAddMeshFlags.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchInfo.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchRecord.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialId.hpp>
#include <FslGraphics2D/Procedural/Batcher/ImmediateModeBatcherTypes.hpp>
#include <FslGraphics2D/Procedural/Builder/UIRawBasicMeshBuilder2D.hpp>
#include <FslGraphics2D/Procedural/Builder/UIRawMeshBuilder2D.hpp>
#include <cassert>
#include <utility>
#include <vector>

namespace Fsl
{
  /// <summary>
  /// This is basically the exact same code as ImmediateModeBatcher but with a fex extra configuration parameters.
  /// </summary>
  class FlexibleImmediateModeBatcher
  {
  public:
    using size_type = uint32_t;

  private:
    struct MeshLimitsRecord
    {
      //! This is the maximum vertex capacity of a segment when operating on pure vertex based meshes
      uint32_t MaxPureVertexCapacity{0};
      //! This is the maximum vertex capacity of a segment when operating on vertex+index based meshes
      uint32_t MaxVertexCapacity{0};
      //! This is the maximum index capacity of a segment when operating on vertex+index based meshes
      uint32_t MaxIndexCapacity{0};
      //! This can be used to force start a segment once a certain vertex threshold has been reached.
      uint32_t TriggerNewSegmentPureVertexCapacity{0};
      //! This can be used to force start a segment once a certain vertex threshold has been reached.
      uint32_t TriggerNewSegmentVertexCapacity{0};

      bool OnlyOneEntryPerBatch{false};
      bool OnlyOneBatchPerSegment{false};

      constexpr MeshLimitsRecord() = default;
      MeshLimitsRecord(const uint32_t maxPureVertexCapacity, const uint32_t maxVertexCapacity, const uint32_t maxIndexCapacity,
                       const uint32_t triggerNewSegmentPureVertexCapacity, const uint32_t triggerNewSegmentVertexCapacity,
                       const bool onlyOneEntryPerBatch, const bool onlyOneBatchPerSegment);
    };

    MeshLimitsRecord m_meshLimits;
    ImmediateModeBatcherTypes::BatchBuildStatus m_batchBuildStatus;

    std::vector<ImmediateModeBatcherTypes::SegmentRecord> m_segments;
    std::vector<BatchRecord> m_batches;
    std::vector<VertexPositionColorTexture> m_vertices;
    std::vector<uint16_t> m_indices;
    float m_zPosAdd{1.0f};
    uint32_t m_batchBuildId{0};

  public:
    struct Stats
    {
      size_type SegmentCount{0};
      size_type BatchCount{0};
      size_type VertexCount{0};
      size_type IndexCount{0};

      constexpr Stats() noexcept = default;
      constexpr Stats(const size_type segmentCount, const size_type batchCount, const size_type vertexCount, const size_type indexCount) noexcept
        : SegmentCount(segmentCount)
        , BatchCount(batchCount)
        , VertexCount(vertexCount)
        , IndexCount(indexCount)
      {
      }
    };

    struct SegmentSpans
    {
      ReadOnlySpan<VertexPositionColorTexture> Vertices;
      ReadOnlySpan<uint16_t> Indices;
    };

    struct SegmentBatchInfo
    {
      SpanRange<size_type> BatchRange;
    };

    FlexibleImmediateModeBatcher();
    FlexibleImmediateModeBatcher(const size_type vertexCapacity, const size_type indexCapacity);
    ~FlexibleImmediateModeBatcher();

    bool GetLimitOnlyOneEntryPerBatch() const
    {
      return m_meshLimits.OnlyOneEntryPerBatch;
    }

    bool SetLimitOnlyOneEntryPerBatch(const bool onlyOneEntryPerBatch)
    {
      bool changed = onlyOneEntryPerBatch != m_meshLimits.OnlyOneEntryPerBatch;
      m_meshLimits.OnlyOneEntryPerBatch = onlyOneEntryPerBatch;
      return changed;
    }

    bool GetLimitOnlyOneBatchPerSegment() const
    {
      return m_meshLimits.OnlyOneBatchPerSegment;
    }

    bool SetLimitOnlyOneBatchPerSegment(const bool enabled)
    {
      bool changed = enabled != m_meshLimits.OnlyOneBatchPerSegment;
      m_meshLimits.OnlyOneBatchPerSegment = enabled;
      return changed;
    }

    std::size_t VertexCapacity()
    {
      return m_vertices.size();
    }

    std::size_t IndexCapacity()
    {
      return m_indices.size();
    }

    bool IsInBatchBuild() const
    {
      return m_batchBuildStatus.Building;
    }

    //! @brief Start a new batch operation (during batching we are allowed to modify the meshes)
    //! @return Both true and false indicates a successful begin. But true means operation was started without any issues,
    //!         if false it means we force ended a existing build before we started the new one
    bool BeginBatch();

    //! @brief Apply all changes the occurred since the last time this was called
    //! @return true if this call ended a build that was begun with BeginMeshBuild, false if there was no build to end
    bool EndBatch();

    bool ForceEndBatch();

    void EnsureCapacity(const size_type vertexCapacity, const size_type indexCapacity)
    {
      EnsureVertexCapacity(vertexCapacity);
      EnsureIndexCapacity(indexCapacity);
    }

    void EnsureVertexCapacity(const size_type vertexCapacity);
    void EnsureIndexCapacity(const size_type indexCapacity);

    UIRawBasicMeshBuilder2D BeginBasicMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity, const Color color = Color::White(),
                                                const BatcherAddMeshFlags flags = BatcherAddMeshFlags::NoFlags);
    UIRawBasicMeshBuilder2D BeginBasicMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity, const float manualZPos,
                                                       const Color color = Color::White());
    void EndBasicMeshBuild(UIRawBasicMeshBuilder2D& rBuilder);


    UIRawMeshBuilder2D BeginMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity, const size_type indexCapacity,
                                      const Color color = Color::White(), const BatcherAddMeshFlags flags = BatcherAddMeshFlags::NoFlags);

    UIRawMeshBuilder2D BeginMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity, const size_type indexCapacity,
                                             const float manualZPos, const Color color = Color::White());

    void EndMeshBuild(UIRawMeshBuilder2D& rBuilder)
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


    inline size_type GetSegmentCount() const noexcept
    {
      return m_batchBuildStatus.SegmentCount;
    }


    inline SegmentSpans GetSegmentSpans(const size_type index) const noexcept
    {
      assert(index < m_batchBuildStatus.SegmentCount);
      const ImmediateModeBatcherTypes::SegmentRecord& record = m_segments[index];
      return {ReadOnlySpanUtil::AsSpan(m_vertices, record.VertexSpanRange.Start, record.VertexSpanRange.Length, OptimizationCheckFlag::NoCheck),
              ReadOnlySpanUtil::AsSpan(m_indices, record.IndexSpanRange.Start, record.IndexSpanRange.Length, OptimizationCheckFlag::NoCheck)};
    }


    inline SegmentBatchInfo GetSegmentBatchInfo(const size_type index) const noexcept
    {
      assert(index < m_batchBuildStatus.SegmentCount);
      return {m_segments[index].BatchRange};
    }

    inline const BatchRecord& GetBatchRecord(const size_type index) const noexcept
    {
      assert(index < m_batchBuildStatus.BatchCount);
      return m_batches[index];
    }

    Stats GetStats() const noexcept
    {
      return {m_batchBuildStatus.SegmentCount, m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount};
    }
  };
}

#endif
