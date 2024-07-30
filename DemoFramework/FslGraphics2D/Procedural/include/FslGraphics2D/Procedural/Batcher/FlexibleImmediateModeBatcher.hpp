#ifndef FSLGRAPHICS2D_PROCEDURAL_BATCHER_FLEXIBLEIMMEDIATEMODEBATCHER_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BATCHER_FLEXIBLEIMMEDIATEMODEBATCHER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslBase/Math/SpanRangeUtil.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchContentType.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchInfo.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchMaterialId.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchRecord.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatcherAddMeshFlags.hpp>
#include <FslGraphics2D/Procedural/Batcher/ImmediateModeBatcherTypes.hpp>
#include <cassert>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace Graphics2D::Internal
  {
    namespace FlexibleImmediateModeBatcherConfig
    {
      constexpr const uint32_t MaxVertexSegmentCapacity = 0x7FFFFFFF;
      constexpr const uint32_t MaxIndexSegmentCapacity = 0x7FFFFFFF;
      // The max vertex capacity for index based meshes
      constexpr const uint32_t MaxIndexBasedMeshVertexCapacity = 0xFFFF;

      constexpr const uint32_t DefaultVertexCapacity = 4 * 20000;
      constexpr const uint32_t DefaultIndexCapacity = 6 * 20000;
      constexpr const uint32_t DefaultSegmentCapacity = 32;
      constexpr const uint32_t DefaultBatchCapacity = 128;
      constexpr const std::size_t GrowSegmentsBy = 32;
      constexpr const std::size_t GrowBatchesBy = 32;
      constexpr const std::size_t GrowVerticesBy = 4 * 5000;
      constexpr const std::size_t GrowIndicesBy = 6 * 5000;
    }

    struct FlexibleImmediateModeBatcherMeshLimitsRecord
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

      constexpr FlexibleImmediateModeBatcherMeshLimitsRecord() noexcept = default;
      constexpr FlexibleImmediateModeBatcherMeshLimitsRecord(const uint32_t maxPureVertexCapacity, const uint32_t maxVertexCapacity,
                                                             const uint32_t maxIndexCapacity, const uint32_t triggerNewSegmentPureVertexCapacity,
                                                             const uint32_t triggerNewSegmentVertexCapacity, const bool onlyOneEntryPerBatch,
                                                             const bool onlyOneBatchPerSegment) noexcept
        : MaxPureVertexCapacity(maxPureVertexCapacity)
        , MaxVertexCapacity(maxVertexCapacity)
        , MaxIndexCapacity(maxIndexCapacity)
        , TriggerNewSegmentPureVertexCapacity(std::min(triggerNewSegmentPureVertexCapacity, maxPureVertexCapacity))
        , TriggerNewSegmentVertexCapacity(std::min(triggerNewSegmentVertexCapacity, maxVertexCapacity))
        , OnlyOneEntryPerBatch(onlyOneEntryPerBatch)
        , OnlyOneBatchPerSegment(onlyOneBatchPerSegment)
      {
      }
    };

    namespace FlexibleImmediateModeBatcherUtil
    {
      //! brief Finalize the previous batch info by writing it
      inline void FinalizeBatchInfo(std::vector<BatchRecord>& rBatches, ImmediateModeBatcherTypes::BatchBuildStatus& rBatchBuildStatus)
      {
        // Ensure we have capacity
        if (rBatchBuildStatus.BatchCount >= rBatches.size())
        {
          // Reserve room for more batches
          rBatches.resize(rBatchBuildStatus.BatchCount + Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::GrowBatchesBy);
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
          rSegments.resize(rBatchBuildStatus.SegmentCount + Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::GrowSegmentsBy);
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
  }

  struct FlexibleImmediateModeBatcherStats
  {
    uint32_t SegmentCount{0};
    uint32_t BatchCount{0};
    uint32_t VertexCount{0};
    uint32_t IndexCount{0};

    constexpr FlexibleImmediateModeBatcherStats() noexcept = default;
    constexpr FlexibleImmediateModeBatcherStats(const uint32_t segmentCount, const uint32_t batchCount, const uint32_t vertexCount,
                                                const uint32_t indexCount) noexcept
      : SegmentCount(segmentCount)
      , BatchCount(batchCount)
      , VertexCount(vertexCount)
      , IndexCount(indexCount)
    {
    }
  };


  /// <summary>
  /// This is basically the exact same code as ImmediateModeBatcher but with a fex extra configuration parameters.
  /// </summary>
  template <typename TRawBasicMeshBuilder2D, typename TRawMeshBuilder2D>
  class FlexibleImmediateModeBatcher
  {
  public:
    using size_type = uint32_t;
    using vertex_type = typename TRawMeshBuilder2D::vertex_element_type;
    using index_type = typename TRawMeshBuilder2D::index_element_type;
    using color_type = typename vertex_type::color_type;
    using raw_basic_mesh_builder_type = TRawBasicMeshBuilder2D;
    using raw_mesh_builder_type = TRawMeshBuilder2D;

    // The two builders must use the same vertex element type
    static_assert(std::is_same<typename TRawBasicMeshBuilder2D::vertex_element_type, typename TRawMeshBuilder2D::vertex_element_type>());

  private:
    Graphics2D::Internal::FlexibleImmediateModeBatcherMeshLimitsRecord m_meshLimits;
    ImmediateModeBatcherTypes::BatchBuildStatus m_batchBuildStatus;

    std::vector<ImmediateModeBatcherTypes::SegmentRecord> m_segments;
    std::vector<BatchRecord> m_batches;
    std::vector<vertex_type> m_vertices;
    std::vector<index_type> m_indices;
    float m_zPosAdd{1.0f};
    uint32_t m_batchBuildId{0};

  public:
    struct SegmentSpans
    {
      ReadOnlySpan<vertex_type> Vertices;
      ReadOnlySpan<index_type> Indices;
    };

    struct SegmentBatchInfo
    {
      SpanRange<size_type> BatchRange;
    };

    FlexibleImmediateModeBatcher()
      : FlexibleImmediateModeBatcher(Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultVertexCapacity,
                                     Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultIndexCapacity)
    {
    }

    FlexibleImmediateModeBatcher(const size_type vertexCapacity, const size_type indexCapacity)
      : m_meshLimits(Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::MaxVertexSegmentCapacity,
                     Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::MaxIndexBasedMeshVertexCapacity,
                     Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::MaxIndexSegmentCapacity,
                     Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::MaxVertexSegmentCapacity,
                     Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::MaxIndexBasedMeshVertexCapacity, false, false)
      , m_segments(std::max(Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultSegmentCapacity, 1u))
      , m_batches(std::max(Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultBatchCapacity, 1u))
      , m_vertices(vertexCapacity > 0 ? vertexCapacity : Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultVertexCapacity)
      , m_indices(indexCapacity > 0 ? indexCapacity : Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::DefaultIndexCapacity)
    {
    }

    ~FlexibleImmediateModeBatcher() noexcept
    {
      FSLLOG3_VERBOSE4("FlexibleImmediateModeBatcher final vertexCapacity: {} indexCapacity: {}", m_vertices.size(), m_indices.size());
    }


    bool GetLimitOnlyOneEntryPerBatch() const noexcept
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

    std::size_t VertexCapacity() const noexcept
    {
      return m_vertices.size();
    }

    std::size_t IndexCapacity() const noexcept
    {
      return m_indices.size();
    }

    bool IsInBatchBuild() const noexcept
    {
      return m_batchBuildStatus.Building;
    }

    //! @brief Start a new batch operation (during batching we are allowed to modify the meshes)
    //! @return Both true and false indicates a successful begin. But true means operation was started without any issues,
    //!         if false it means we force ended a existing build before we started the new one
    bool BeginBatch()
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

    //! @brief Apply all changes the occurred since the last time this was called
    //! @return true if this call ended a build that was begun with BeginMeshBuild, false if there was no build to end
    bool EndBatch()
    {
      if (!m_batchBuildStatus.Building)
      {
        FSLLOG3_WARNING("Not in a mesh change state");
        return false;
      }
      m_batchBuildStatus.Building = false;
      if (m_batchBuildStatus.CurrentBatch.VertexCount > 0)
      {    // We have a batch that was not written yet, so lets write it
        Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeBatchInfo(m_batches, m_batchBuildStatus);
        // Ensure we don't detect it twice
        m_batchBuildStatus.CurrentBatch = {};
      }
      if (m_batchBuildStatus.CurrentSegment.VertexCount > 0)
      {
        Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
        // Ensure we don't detect it twice
        m_batchBuildStatus.CurrentSegment = {};
      }
      return true;
    }

    bool ForceEndBatch() noexcept
    {
      if (!m_batchBuildStatus.Building)
      {
        return false;
      }

      FSLLOG3_WARNING("Force ending build operation");
      m_batchBuildStatus = {};
      return true;
    }

    void EnsureCapacity(const size_type vertexCapacity, const size_type indexCapacity)
    {
      EnsureVertexCapacity(vertexCapacity);
      EnsureIndexCapacity(indexCapacity);
    }

    void EnsureVertexCapacity(const size_type vertexCapacity)
    {
      if (vertexCapacity > m_vertices.size())
      {
        m_vertices.resize(vertexCapacity + Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::GrowVerticesBy);
      }
    }

    void EnsureIndexCapacity(const size_type indexCapacity)
    {
      if (indexCapacity > m_indices.size())
      {
        m_indices.resize(indexCapacity + Graphics2D::Internal::FlexibleImmediateModeBatcherConfig::GrowIndicesBy);
      }
    }

    raw_basic_mesh_builder_type BeginBasicMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity,
                                                    const color_type color = Colors::White(),
                                                    const BatcherAddMeshFlags flags = BatcherAddMeshFlags::NoFlags)
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
             (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity ||
             m_meshLimits.OnlyOneEntryPerBatch))
        {    // Create a new batch
          Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeBatchInfo(m_batches, m_batchBuildStatus);

          // Check if we need to force generate a new segment
          if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity ||
              m_meshLimits.OnlyOneBatchPerSegment)
          {
            Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
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
      return raw_basic_mesh_builder_type(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, m_batchBuildStatus.ZPos, color);
    }
    raw_basic_mesh_builder_type BeginBasicMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity, const float manualZPos,
                                                           const color_type color = Colors::White())
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
             (m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity ||
             m_meshLimits.OnlyOneEntryPerBatch))
        {    // Create a new batch
          Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeBatchInfo(m_batches, m_batchBuildStatus);

          // Check if we need to force generate a new segment
          if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentPureVertexCapacity ||
              m_meshLimits.OnlyOneBatchPerSegment)
          {
            Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeSegmentInfo(m_segments, m_batchBuildStatus);
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
      return raw_basic_mesh_builder_type(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity, manualZPos, color);
    }

    void EndBasicMeshBuild(raw_basic_mesh_builder_type& rBuilder)
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


    raw_mesh_builder_type BeginMeshBuild(const BatchMaterialId materialId, const size_type vertexCapacity, const size_type indexCapacity,
                                         const color_type color = Colors::White(), const BatcherAddMeshFlags flags = BatcherAddMeshFlags::NoFlags)
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
             (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity || m_meshLimits.OnlyOneEntryPerBatch))
        {
          Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeBatchInfo(m_batches, m_batchBuildStatus);

          // Check if we need to force generate a new segment
          if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
              (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity || m_meshLimits.OnlyOneBatchPerSegment)
          {
            Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeSegmentInfo(m_segments, m_batchBuildStatus);

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
          m_batchBuildStatus.CurrentBatch = ImmediateModeBatcherTypes::CurrentBatchRecord(BatchContentType::VerticesAndIndices, materialId,
                                                                                          m_batchBuildStatus.CurrentSegment.VertexCount,
                                                                                          m_batchBuildStatus.CurrentSegment.IndexCount);
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

      return raw_mesh_builder_type(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity,
                                   m_indices.data() + m_batchBuildStatus.IndexCount, indexCapacity, m_batchBuildStatus.CurrentSegment.VertexCount,
                                   m_batchBuildStatus.ZPos, color);
    }
    raw_mesh_builder_type BeginMeshBuildCustomZ(const BatchMaterialId materialId, const size_type vertexCapacity, const size_type indexCapacity,
                                                const float manualZPos, const color_type color = Colors::White())
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
             (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity || m_meshLimits.OnlyOneEntryPerBatch))
        {
          Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeBatchInfo(m_batches, m_batchBuildStatus);

          // Check if we need to force generate a new segment
          if ((m_batchBuildStatus.CurrentSegment.VertexCount + vertexCapacity) > m_meshLimits.TriggerNewSegmentVertexCapacity ||
              (m_batchBuildStatus.CurrentSegment.IndexCount + indexCapacity) > m_meshLimits.MaxIndexCapacity || m_meshLimits.OnlyOneBatchPerSegment)
          {
            Graphics2D::Internal::FlexibleImmediateModeBatcherUtil::FinalizeSegmentInfo(m_segments, m_batchBuildStatus);

            // Prepare to track a new segment
            m_batchBuildStatus.CurrentSegment = ImmediateModeBatcherTypes::CurrentSegementRecord(
              m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount);
          }

          // Prepare the new batch
          m_batchBuildStatus.CurrentBatch = ImmediateModeBatcherTypes::CurrentBatchRecord(BatchContentType::VerticesAndIndices, materialId,
                                                                                          m_batchBuildStatus.CurrentSegment.VertexCount,
                                                                                          m_batchBuildStatus.CurrentSegment.IndexCount);
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

      return raw_mesh_builder_type(m_vertices.data() + m_batchBuildStatus.VertexCount, vertexCapacity,
                                   m_indices.data() + m_batchBuildStatus.IndexCount, indexCapacity, m_batchBuildStatus.CurrentSegment.VertexCount,
                                   manualZPos, color);
    }
    void EndMeshBuild(raw_mesh_builder_type& rBuilder)
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
      return {SpanUtil::UncheckedAsReadOnlySpan(m_vertices, record.VertexSpanRange.Start, record.VertexSpanRange.Length),
              SpanUtil::UncheckedAsReadOnlySpan(m_indices, record.IndexSpanRange.Start, record.IndexSpanRange.Length)};
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

    FlexibleImmediateModeBatcherStats GetStats() const noexcept
    {
      return {m_batchBuildStatus.SegmentCount, m_batchBuildStatus.BatchCount, m_batchBuildStatus.VertexCount, m_batchBuildStatus.IndexCount};
    }
  };
}

#endif
