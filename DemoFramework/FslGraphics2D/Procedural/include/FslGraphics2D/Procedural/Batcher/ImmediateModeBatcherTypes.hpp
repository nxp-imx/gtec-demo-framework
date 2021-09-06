#ifndef FSLGRAPHICS2D_PROCEDURAL_BATCHER_IMMEDIATEMODEBATCHERTYPES_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BATCHER_IMMEDIATEMODEBATCHERTYPES_HPP
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
#include <FslBase/Math/SpanRange.hpp>
#include <FslGraphics2D/Procedural/Batcher/BatchInfo.hpp>

namespace Fsl
{
  /// <summary>
  /// Allows us to reuse this for the two ImmediateModeBatchers
  /// </summary>
  namespace ImmediateModeBatcherTypes
  {
    struct CurrentSegementRecord
    {
      //! This is the first batch index in this segment
      uint32_t BatchStartIndex{0};
      //! This is the vertex offset in the m_vertices array
      uint32_t VertexOffset{0};
      //! This is the index offset in the m_indices array
      uint32_t IndexOffset{0};
      uint32_t VertexCount{0};
      uint32_t IndexCount{0};
      constexpr CurrentSegementRecord() noexcept = default;
      explicit constexpr CurrentSegementRecord(const uint32_t batchStartIndex, const uint32_t vertexOffset, const uint32_t indexOffset) noexcept
        : BatchStartIndex(batchStartIndex)
        , VertexOffset(vertexOffset)
        , IndexOffset(indexOffset)
      {
      }
    };

    struct CurrentBatchRecord
    {
      BatchInfo Info;
      //! This is the vertex offset in the current segment
      uint32_t SegmentVertexOffset{0};
      //! This is the index offset in the current segment
      uint32_t SegmentIndexOffset{0};
      //! This is the number of vertices in this batch
      uint32_t VertexCount{0};
      //! This is the number of indices in this batch
      uint32_t IndexCount{0};

      constexpr CurrentBatchRecord() noexcept = default;

      explicit constexpr CurrentBatchRecord(const BatchContentType contentType, const BatchMaterialId materialId, const uint32_t segmentVertexOffset,
                                            const uint32_t segmentIndexOffset)
        : Info(contentType, materialId)
        , SegmentVertexOffset(segmentVertexOffset)
        , SegmentIndexOffset(segmentIndexOffset)
      {
      }
    };

    struct BatchBuildStatus
    {
      bool Building{false};
      uint32_t SegmentCount{0};
      uint32_t BatchCount{0};
      uint32_t VertexCount{0};
      uint32_t IndexCount{0};
      float ZPos{0.0f};
      CurrentSegementRecord CurrentSegment;
      CurrentBatchRecord CurrentBatch;

      constexpr BatchBuildStatus() noexcept = default;
      explicit constexpr BatchBuildStatus(const bool building) noexcept
        : Building(building)
      {
      }
    };

    struct SegmentRecord
    {
      SpanRange<uint32_t> BatchRange;
      SpanRange<uint32_t> VertexSpanRange;
      SpanRange<uint32_t> IndexSpanRange;

      constexpr SegmentRecord() noexcept = default;
      constexpr SegmentRecord(const SpanRange<uint32_t> batchRange, const SpanRange<uint32_t>& vertexSpanRange,
                              const SpanRange<uint32_t>& indexSpanRange) noexcept
        : BatchRange(batchRange)
        , VertexSpanRange(vertexSpanRange)
        , IndexSpanRange(indexSpanRange)
      {
      }
    };
  };
}

#endif
