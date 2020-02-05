#ifndef FSLGRAPHICS_RENDER_STRATEGY_STRATEGYSORTBYTRANSPARENCY_HPP
#define FSLGRAPHICS_RENDER_STRATEGY_STRATEGYSORTBYTRANSPARENCY_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Math/Point2.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Strategy/BatchSegmentInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include <algorithm>
#include <cassert>
#include <vector>

namespace Fsl
{
  // Begin must be called to initiate a new round of sorting.
  // End must be called to finish it.
  // Before the first AddQuad there must have been one call to SetTexture and SetBlendState.
  // The user should also call EnsureCapacity/GrowCapacity as appropriate.
  // Expected call pattern
  // - Clear
  // - SetBlendState
  // - SetTexture
  // - AddQuad
  // - SetTexture
  // - AddQuad
  // - AddQuad
  // - SetBlendState
  // - AddQuad

  // PERFORMANCE IMPROVEMENT:
  // for texture_info_type that don't contain any dangerous resources like std::shared_ptr etc
  // we can get away with not clearing unused segments. Allowing operations like Clear and EnsureCapacity to execute slightly faster
  //
  // Pointer naming convention
  // - pStart means this is where it starts and it is included
  // - pEnd means this is where it ends (and its not included)
  // - pNext means its the next location to be written into
  // - pCurrent means this is were we are currently writing
  template <typename TTextureInfo>
  class StrategySortByTransparency
  {
  public:
    static constexpr const uint32_t VERTICES_PER_QUAD = 4;

    using texture_info_type = TTextureInfo;
    using segment_type = BatchSegmentInfo<texture_info_type>;
    using vertex_type = VertexPositionColorTexture;
    using vertex_span_type = VertexSpan<vertex_type>;

  private:
    static constexpr const uint32_t EXPAND_QUAD_GROWTH = 1024;
    static constexpr const uint32_t SAFETY = 1;
    static constexpr const uint32_t MIN_QUADS = 1;
    static constexpr const uint32_t NUM_BUFFERS = 2;

    static_assert(MIN_QUADS > 0, "Min quads must be > 0");
    static_assert(SAFETY > 0, "Safety must be > 0");
    static_assert(NUM_BUFFERS == 2, "All of this class is hard-coded to expect two buffers");

    struct BufferPointers
    {
      // So the actual allocated vertices are calculated like this:
      // OpaqueCount      = (pEndVertexOpaque - pNextVertexOpaque) - VERTICES_PER_QUAD;
      // TransparentCount = (pEndVertexTransp - pStartVertexTransp);
      vertex_type* pNextVertexOpaque{nullptr};
      vertex_type* pEndVertexOpaque{nullptr};
      vertex_type* pStartVertexTransp{nullptr};
      vertex_type* pEndVertexTransp{nullptr};
      segment_type* pCurrentSegmentOpaque{nullptr};
      segment_type* pEndSegmentOpaque{nullptr};
      segment_type* pStartSegmentTransp{nullptr};
      segment_type* pCurrentSegmentTransp{nullptr};

      inline bool IsValid(const std::vector<vertex_type>& quadVertices, const std::vector<segment_type>& segments) const
      {
        // Validate the segments
        return IsValidVertexPointers(quadVertices) && IsValidSegmentPointers(segments);
      }

      inline bool IsValidVertexPointers(const std::vector<vertex_type>& quadVertices) const
      {
        return (pNextVertexOpaque != nullptr && pStartVertexTransp != nullptr && pEndVertexOpaque != nullptr && pEndVertexTransp != nullptr &&
                pNextVertexOpaque >= quadVertices.data() && pNextVertexOpaque <= pEndVertexOpaque &&
                pEndVertexOpaque <= (quadVertices.data() + (quadVertices.size() / NUM_BUFFERS)) &&
                pStartVertexTransp >= (quadVertices.data() + (quadVertices.size() / NUM_BUFFERS)) && pStartVertexTransp <= pEndVertexTransp &&
                pEndVertexTransp <= (quadVertices.data() + quadVertices.size()));
      }

      inline bool IsValidSegmentPointers(const std::vector<segment_type>& segments) const
      {
        return (pCurrentSegmentOpaque != nullptr && pStartSegmentTransp != nullptr && pEndSegmentOpaque != nullptr &&
                pCurrentSegmentTransp != nullptr && pCurrentSegmentOpaque >= segments.data() && pCurrentSegmentOpaque <= pEndSegmentOpaque &&
                pEndSegmentOpaque <= (segments.data() + (segments.size() / NUM_BUFFERS)) &&
                pStartSegmentTransp >= (segments.data() + (segments.size() / NUM_BUFFERS)) && pStartSegmentTransp <= pCurrentSegmentTransp &&
                pCurrentSegmentTransp < (segments.data() + segments.size()));
      }
    };

    // Pointers for optimal access to tracking areas
    struct AddQuadPointers
    {
      vertex_type** ppNextDstVertex{nullptr};
      segment_type* pFirstDstSegment{nullptr};
      segment_type** ppCurrentDstSegment{nullptr};
      int32_t VertexAdd{0};
      int32_t SegmentAdd{0};

      bool IsValid(const std::vector<segment_type>& segments) const
      {
        return (ppNextDstVertex != nullptr && pFirstDstSegment != nullptr && ppCurrentDstSegment != nullptr && (*ppNextDstVertex) != nullptr &&
                (*ppCurrentDstSegment) != nullptr && (VertexAdd == VERTICES_PER_QUAD || VertexAdd == -static_cast<int32_t>(VERTICES_PER_QUAD)) &&
                IsValidFirstDstSegment(segments));
      }

      bool IsValidFirstDstSegment(const std::vector<segment_type>& segments) const
      {
        return (SegmentAdd == -1 && pFirstDstSegment == (segments.data() + (segments.size() / 2) - 1)) ||
               (SegmentAdd == 1 && pFirstDstSegment == (segments.data() + (segments.size() / 2)));
      }
    };


    //! Opaque should be drawn front to back, Transparent should be drawn back to front
    //! Since we in theory could draw 'quadCapacity' of either Opaque or Transparent quads we allocate 2xcapacity to handle the worst case.
    //! To keep things close in cache we allocate
    //! - transparent quads from the middle moving forward
    //! - opaque quads from the middle moving back
    //! This gives us a fairly good chance of the data being close and has the additional benefit of allowing us to upload all
    //! vertices as one continuous block.
    //! So if quad capacity is four we allocate like this:   ........
    //! Draw transparent A,B then opaque C,D,E               .EDCAB..
    //! Using one array slightly complicates the resize operation, but its OK we should not be resizing very often.
    //!
    //! To make all 'assert' checks simpler we add a 'safety block' at both ends of the vector
    //! This should ensure that a comprehensive assert check can be done in all methods even when its filled.
    std::vector<vertex_type> m_quadVertices;
    //! To make all 'assert' checks simpler we add a 'safety block' at both ends of the vector.
    //! This block is also needed for the 'SetBlendState' and SetTexture as it might be 'one ahead' of the capacity depending on the
    //! method call ordering.
    std::vector<segment_type> m_segments;

    BufferPointers m_buffers;
    AddQuadPointers m_addQuad;

  public:
    StrategySortByTransparency(const StrategySortByTransparency&) = delete;
    StrategySortByTransparency(StrategySortByTransparency&& other) = delete;
    StrategySortByTransparency& operator=(const StrategySortByTransparency&) = delete;
    StrategySortByTransparency& operator=(StrategySortByTransparency&& other) = delete;

    // * NUM_BUFFERS because we store opaque and transparent in the same buffer
    StrategySortByTransparency(const uint32_t quadCapacity = 4096)
      : m_quadVertices(static_cast<std::size_t>(std::max(quadCapacity, MIN_QUADS) + SAFETY) * VERTICES_PER_QUAD * NUM_BUFFERS)
      , m_segments(static_cast<std::size_t>(std::max(quadCapacity, MIN_QUADS) + SAFETY) * NUM_BUFFERS)
      , m_buffers(CreateBufferPointers(m_quadVertices, m_segments))
      , m_addQuad(CreateAddQuadPointers(m_buffers, BlendState::Opaque))
    {
      assert(IsValid());
    }

    ~StrategySortByTransparency() = default;


    BlendState GetActiveBlendState() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert(m_addQuad.ppCurrentDstSegment != nullptr);
      assert((*m_addQuad.ppCurrentDstSegment) != nullptr);

      return (*m_addQuad.ppCurrentDstSegment)->ActiveBlendState;
    }


    texture_info_type GetActiveTexture() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert(m_addQuad.ppCurrentDstSegment != nullptr);
      assert((*m_addQuad.ppCurrentDstSegment) != nullptr);

      return (*m_addQuad.ppCurrentDstSegment)->TextureInfo;
    }


    uint32_t GetCapacity() const
    {
      assert(IsValid());
      // It must be >= four because we require room for at least one opaque and one transparent quad and one safety allocation in both ends
      assert(m_segments.size() >= (2 * (MIN_QUADS + SAFETY)));
      return static_cast<uint32_t>(m_segments.size() / NUM_BUFFERS) - SAFETY;
    }


    uint32_t GetOpaqueSegmentCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      // While IsValid checks all of this its added here for good measure as it makes it clear what our assumptions are
      assert(m_buffers.pCurrentSegmentOpaque != nullptr);
      assert(m_buffers.pEndSegmentOpaque != nullptr);
      assert(m_buffers.pEndSegmentOpaque > m_buffers.pCurrentSegmentOpaque);

      auto segments = static_cast<uint32_t>((m_buffers.pEndSegmentOpaque - m_buffers.pCurrentSegmentOpaque) - 1);
      return segments + (m_buffers.pCurrentSegmentOpaque->VertexCount > 0 ? 1 : 0);
    }


    uint32_t GetTransparentSegmentCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      // While IsValid checks all of this its added here for good measure as it makes it clear what our assumptions are
      assert(m_buffers.pStartSegmentTransp != nullptr);
      assert(m_buffers.pCurrentSegmentTransp != nullptr);
      assert(m_buffers.pCurrentSegmentTransp >= m_buffers.pStartSegmentTransp);

      auto segments = static_cast<uint32_t>((m_buffers.pCurrentSegmentTransp - m_buffers.pStartSegmentTransp));
      return segments + (m_buffers.pCurrentSegmentTransp->VertexCount > 0 ? 1 : 0);
    }


    uint32_t GetOpaqueVertexCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      // While IsValid checks all of this its added here for good measure as it makes it clear what our assumptions are
      assert(m_buffers.pNextVertexOpaque != nullptr);
      assert(m_buffers.pEndVertexOpaque != nullptr);
      assert(m_buffers.pEndVertexOpaque > m_buffers.pNextVertexOpaque);
      assert((m_buffers.pEndVertexOpaque - VERTICES_PER_QUAD) >= m_buffers.pNextVertexOpaque);

      return static_cast<uint32_t>((m_buffers.pEndVertexOpaque - m_buffers.pNextVertexOpaque) - VERTICES_PER_QUAD);
    }


    uint32_t GetTransparentVertexCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      // While IsValid checks all of this its added here for good measure as it makes it clear what our assumptions are
      assert(m_buffers.pStartVertexTransp != nullptr);
      assert(m_buffers.pEndVertexTransp != nullptr);
      assert(m_buffers.pEndVertexTransp >= m_buffers.pStartVertexTransp);

      return static_cast<uint32_t>((m_buffers.pEndVertexTransp - m_buffers.pStartVertexTransp));
    }

    uint32_t GetOpaqueQuadCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert((GetOpaqueVertexCount() % VERTICES_PER_QUAD) == 0);
      return GetOpaqueVertexCount() / VERTICES_PER_QUAD;
    }

    uint32_t GetTransparentQuadCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert((GetTransparentVertexCount() % VERTICES_PER_QUAD) == 0);
      return GetTransparentVertexCount() / VERTICES_PER_QUAD;
    }

    vertex_span_type GetOpaqueSpan() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert(m_buffers.pNextVertexOpaque != nullptr);

      const auto pStartVertex = m_buffers.pNextVertexOpaque + VERTICES_PER_QUAD;
      const auto vertexCount = GetOpaqueVertexCount();
      // Check that the vertex count is inside the expected range
      assert(vertexCount <= ((m_quadVertices.size() / NUM_BUFFERS) - SAFETY));

      // Verify that the pointers are inside the valid range
      // Opaque vertices are stored from the middle to the beginning of the quad vertex vector.
      assert(vertexCount <= (m_quadVertices.size() / NUM_BUFFERS));
      assert(pStartVertex == (m_quadVertices.data() + (m_quadVertices.size() / NUM_BUFFERS) - vertexCount));
      assert((pStartVertex + vertexCount) == (m_quadVertices.data() + (m_quadVertices.size() / NUM_BUFFERS)));
      assert(pStartVertex >= (m_quadVertices.data() + SAFETY));

      return vertex_span_type(pStartVertex, vertexCount);
    }

    vertex_span_type GetTransparentSpan() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      const auto vertexCount = GetTransparentVertexCount();

      // Check that the vertex count is inside the expected range
      assert(vertexCount <= ((m_quadVertices.size() / NUM_BUFFERS) - SAFETY));

      // Verify that the pointers are inside the valid range
      // Transparent vertices are stored from the middle to the end of the quad vertex vector.
      assert((m_quadVertices.size() % NUM_BUFFERS) == 0);
      assert(m_buffers.pStartVertexTransp != nullptr);
      assert(m_buffers.pStartVertexTransp == (m_quadVertices.data() + (m_quadVertices.size() / NUM_BUFFERS)));
      assert((m_buffers.pStartVertexTransp + vertexCount) == (m_quadVertices.data() + (m_quadVertices.size() / NUM_BUFFERS) + vertexCount));
      assert((m_buffers.pStartVertexTransp + vertexCount) <= (m_quadVertices.data() + m_quadVertices.size() - SAFETY));
      return vertex_span_type(m_buffers.pStartVertexTransp, vertexCount);
    }


    segment_type GetOpaqueSegment(const uint32_t index) const
    {
      assert(index < GetOpaqueSegmentCount());
      // If this assert fire it means we have a internal error
      assert(IsValid());
      assert(m_buffers.pEndSegmentOpaque != nullptr);

      auto pSegment = m_buffers.pEndSegmentOpaque - 1 - index;

      // Opaque segments are stored from the middle to the beginning of the segment vector.
      assert(pSegment >= (m_segments.data() + SAFETY));
      assert(pSegment < (m_segments.data() + (m_segments.size() / NUM_BUFFERS)));

      return *pSegment;
    }

    segment_type GetTransparentSegment(const uint32_t index) const
    {
      assert(index < GetTransparentSegmentCount());
      // If this assert fire it means we have a internal error
      assert(IsValid());
      assert(m_buffers.pStartSegmentTransp != nullptr);

      auto pSegment = m_buffers.pStartSegmentTransp + index;

      // Transparent segments are stored from the middle to the end of the segment vector.
      assert((m_segments.size() % NUM_BUFFERS) == 0);
      assert(pSegment >= (m_segments.data() + (m_segments.size() / NUM_BUFFERS)));
      assert(pSegment < (m_segments.data() + m_segments.size() - SAFETY));
      return *pSegment;
    }


    //! @brief clear everything
    //! @note  This also resets the blendState and texture!
    void Clear()
    {
      // If this assert fire it means we have a internal error
      assert(IsValid());
      assert(m_buffers.pCurrentSegmentOpaque != nullptr);
      assert(m_buffers.pCurrentSegmentTransp != nullptr);
      assert(m_buffers.pCurrentSegmentOpaque <= m_buffers.pCurrentSegmentTransp);

      // Free all segments;
      Clear(m_buffers.pCurrentSegmentOpaque, m_buffers.pCurrentSegmentTransp + 1);

      // If this assert fires it just means we have a internal error
      m_buffers = CreateBufferPointers(m_quadVertices, m_segments);
      m_addQuad = CreateAddQuadPointers(m_buffers, BlendState::Opaque);
    }


    void SetBlendState(const BlendState blendState)
    {
      assert(IsValid());

      // Read the current texture info so we have it available before we update pointers
      auto activeTextureInfo = GetActiveTexture();

      // Switch to the right blend mode
      m_addQuad = CreateAddQuadPointers(m_buffers, blendState);

      UpdateSegmentState(activeTextureInfo, blendState);
    }

    //! Make sure you call SetBlendState before SetTexture!
    inline void SetTexture(const texture_info_type& textureInfo)
    {
      UpdateSegmentState(textureInfo, GetActiveBlendState());
    }


    inline void AddQuad(const Vector2& vec0, const Vector2& vec1, const Vector2& vec2, const Vector2& vec3, const Vector2& texCoords0,
                        const Vector2& texCoords1, const Vector4& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      assert(m_addQuad.ppNextDstVertex != nullptr);
      auto pDstVertex = *m_addQuad.ppNextDstVertex;
      assert(pDstVertex != nullptr);

      // We expect the user called ensure capacity before starting to use this
      assert(pDstVertex >= (m_quadVertices.data() + SAFETY));
      assert((pDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size() - SAFETY));

      pDstVertex->Position.X = vec0.X;
      pDstVertex->Position.Y = vec0.Y;
      pDstVertex->TextureCoordinate.X = texCoords0.X;
      pDstVertex->TextureCoordinate.Y = texCoords0.Y;
      pDstVertex->Color = color;

      (pDstVertex + 1)->Position.X = vec1.X;
      (pDstVertex + 1)->Position.Y = vec1.Y;
      (pDstVertex + 1)->TextureCoordinate.X = texCoords1.X;
      (pDstVertex + 1)->TextureCoordinate.Y = texCoords0.Y;
      (pDstVertex + 1)->Color = color;

      (pDstVertex + 2)->Position.X = vec2.X;
      (pDstVertex + 2)->Position.Y = vec2.Y;
      (pDstVertex + 2)->TextureCoordinate.X = texCoords0.X;
      (pDstVertex + 2)->TextureCoordinate.Y = texCoords1.Y;
      (pDstVertex + 2)->Color = color;

      (pDstVertex + 3)->Position.X = vec3.X;
      (pDstVertex + 3)->Position.Y = vec3.Y;
      (pDstVertex + 3)->TextureCoordinate.X = texCoords1.X;
      (pDstVertex + 3)->TextureCoordinate.Y = texCoords1.Y;
      (pDstVertex + 3)->Color = color;

      pDstVertex += m_addQuad.VertexAdd;

      (*m_addQuad.ppNextDstVertex) = pDstVertex;

      // We add the VERTICES_PER_QUAD here instead of m_addQuad.VertexAdd because this should always increase
      assert(m_addQuad.ppCurrentDstSegment != nullptr);
      assert((*m_addQuad.ppCurrentDstSegment) != nullptr);
      assert((*m_addQuad.ppCurrentDstSegment) >= m_segments.data());
      assert((*m_addQuad.ppCurrentDstSegment) < (m_segments.data() + m_segments.size()));

      (*m_addQuad.ppCurrentDstSegment)->VertexCount += VERTICES_PER_QUAD;
    }

    inline void EnsureCapacity(const std::size_t desiredQuadCapacity)
    {
      // * NUM_BUFFERS because we store both transparent and opaque in the same buffer
      if ((static_cast<std::size_t>(desiredQuadCapacity + SAFETY) * NUM_BUFFERS) > m_segments.size())
      {
        GrowCapacity(desiredQuadCapacity);
      }
    }

    // inline void EnsureCapacityFor(const std::size_t elementsToEnsureCapacityFor)
    //{
    //  auto desiredQuadCapacity = (GetQuadCount() + elementsToEnsureCapacityFor);
    //  if ((desiredQuadCapacity + SAFETY) > m_segments.size())
    //  {
    //    GrowCapacity(desiredQuadCapacity);
    //  }
    //}

  private:
    void GrowCapacity(std::size_t newMinimumQuadCapacity)
    {
      assert(IsValid());

      auto activeBlendState = GetActiveBlendState();

      // we store both transparent and opaque in the same buffer and need room for the safety area
      newMinimumQuadCapacity = (newMinimumQuadCapacity + SAFETY) * NUM_BUFFERS;

      // This should only be called to grow the capacity
      assert(newMinimumQuadCapacity >= m_segments.size());

      const std::size_t newQuadCapacity = newMinimumQuadCapacity + EXPAND_QUAD_GROWTH;
      const std::size_t newVertexCapacity = newQuadCapacity * VERTICES_PER_QUAD;

      assert(m_buffers.IsValid(m_quadVertices, m_segments));

      // We expect the buffer to contain at least two quads
      assert(m_quadVertices.size() >= 2);
      // We expect the buffer buffer to be able to contain exactly 2x quad size
      assert((m_quadVertices.size() & 1) == 0);

      auto vertexCountOpaque = GetOpaqueVertexCount();
      auto vertexCountTransp = GetTransparentVertexCount();

      auto endOffsetVertexOpaque = (m_buffers.pEndVertexOpaque - m_quadVertices.data());
      auto endOffsetVertexTransp = (m_buffers.pEndVertexTransp - m_quadVertices.data());

      auto currentOffsetSegmentOpaque = (m_buffers.pCurrentSegmentOpaque - m_segments.data());
      auto currentOffsetSegmentTransp = (m_buffers.pCurrentSegmentTransp - m_segments.data());
      auto endOffsetSegmentOpaque = (m_buffers.pEndSegmentOpaque - m_segments.data());
      auto startOffsetSegmentTransp = (m_buffers.pStartSegmentTransp - m_segments.data());

      m_quadVertices.resize(newVertexCapacity);
      m_segments.resize(newQuadCapacity);

      m_buffers = CreateBufferPointers(m_quadVertices, m_segments);

      // old start -> offsets   | new start  ->   mod
      // -ab|      -> -ab|      | --|        ->   -ab|
      // ^  E      ->  ^ E      |  ^E        ->   ^  E

      // Its important to move transparent before opaque or we could overwrite content
      {
        // Adjust the opaque start offset so it fits the previous layout
        m_buffers.pNextVertexOpaque -= vertexCountOpaque;
        // Adjust the transparent end offsets to fit the previous layout
        m_buffers.pEndVertexTransp += vertexCountTransp;

        // Time to move the existing data if there is any
        // We start by moving the transparent and we move the 'right side' first to prevent overwrites and then we do the same for the opaque
        SafeMove(m_buffers.pStartVertexTransp, m_quadVertices.data(), endOffsetVertexTransp - vertexCountTransp, vertexCountTransp);
        SafeMove(m_buffers.pNextVertexOpaque + VERTICES_PER_QUAD, m_quadVertices.data(), endOffsetVertexOpaque - vertexCountOpaque,
                 vertexCountOpaque);
      }

      {
        // - Since transparent is from start to current it will not include 'current'
        const auto currentSegmentBlocksTransp = (currentOffsetSegmentTransp - startOffsetSegmentTransp);
        m_buffers.pCurrentSegmentTransp += currentSegmentBlocksTransp;
        SafeMove(m_buffers.pStartSegmentTransp, m_segments.data(), startOffsetSegmentTransp, currentSegmentBlocksTransp + 1);

        // Due to the 'pCurrent' thing we adjust the segment pointers slightly differently
        // - Since opaque is from current to end it will always include all content
        const auto currentSegmentBlocksOpaque = (endOffsetSegmentOpaque - currentOffsetSegmentOpaque);
        m_buffers.pCurrentSegmentOpaque -= (currentSegmentBlocksOpaque - 1);
        SafeMove(m_buffers.pCurrentSegmentOpaque, m_segments.data(), endOffsetSegmentOpaque - currentSegmentBlocksOpaque, currentSegmentBlocksOpaque);
      }

      // Since the cached pointers could change due to the resize we check if there are existing valid pointers and update them if there are.
      if (m_addQuad.ppNextDstVertex != nullptr)
      {
        m_addQuad = CreateAddQuadPointers(m_buffers, activeBlendState);
      }

      // We expect the buffer points to be valid
      assert(m_buffers.IsValid(m_quadVertices, m_segments));
    }

    template <typename T>
    static inline void SafeMove(T* pDst, T* pSrcData, const ptrdiff_t& srcStartOffset, const ptrdiff_t length)
    {
      assert(pDst != nullptr);
      assert(pSrcData != nullptr);
      assert(srcStartOffset >= 0);
      assert(length >= 0);
      const auto pSrcEnd = pSrcData + srcStartOffset;
      auto pSrc = pSrcData + srcStartOffset + length - 1;
      pDst += length - 1;
      while (pSrc >= pSrcEnd)
      {
        *pDst = *pSrc;
        *pSrc = {};
        --pDst;
        --pSrc;
      }
    }


    inline static BufferPointers CreateBufferPointers(std::vector<vertex_type>& rQuadVertices, std::vector<segment_type>& rSegments)
    {
      BufferPointers buffers;
      // We expect the buffer to contain at least two quads
      assert(rQuadVertices.size() >= ((MIN_QUADS + SAFETY) * VERTICES_PER_QUAD));
      // We expect the buffer to be able to contain exactly: (QUAD_CAPACITY + SAFETY) * VERTICES_PER_QUAD * NUM_BUFFERS
      assert((rQuadVertices.size() % NUM_BUFFERS) == 0);
      const auto vertexMidpoint = rQuadVertices.size() / NUM_BUFFERS;
      buffers.pStartVertexTransp = rQuadVertices.data() + vertexMidpoint;
      buffers.pEndVertexTransp = buffers.pStartVertexTransp;
      buffers.pEndVertexOpaque = buffers.pStartVertexTransp;
      buffers.pNextVertexOpaque = buffers.pStartVertexTransp - VERTICES_PER_QUAD;

      // We expect the buffer to contain at least this
      assert(rSegments.size() >= ((MIN_QUADS + SAFETY) * NUM_BUFFERS));
      // We expect the buffer to be able to contain exactly: (QUAD_CAPACITY + SAFETY) * NUM_BUFFERS
      assert((rSegments.size() % NUM_BUFFERS) == 0);

      const auto segmentMidpoint = rSegments.size() / NUM_BUFFERS;
      buffers.pStartSegmentTransp = rSegments.data() + segmentMidpoint;
      buffers.pCurrentSegmentTransp = buffers.pStartSegmentTransp;
      buffers.pEndSegmentOpaque = buffers.pStartSegmentTransp;
      buffers.pCurrentSegmentOpaque = buffers.pStartSegmentTransp - 1;

      // If this fires then the internal state is corrupt
      assert(buffers.IsValid(rQuadVertices, rSegments));
      return buffers;
    }


    inline AddQuadPointers CreateAddQuadPointers(BufferPointers& rBuffers, const BlendState blendState) const
    {
      // If this fires then the internal state is corrupt
      assert(rBuffers.IsValid(m_quadVertices, m_segments));

      AddQuadPointers addQuad;
      if (blendState == BlendState::Opaque)
      {
        addQuad.pFirstDstSegment = rBuffers.pEndSegmentOpaque - 1;
        addQuad.ppNextDstVertex = &rBuffers.pNextVertexOpaque;
        addQuad.ppCurrentDstSegment = &rBuffers.pCurrentSegmentOpaque;
        addQuad.VertexAdd = -static_cast<int32_t>(VERTICES_PER_QUAD);
        addQuad.SegmentAdd = -1;
      }
      else
      {
        addQuad.pFirstDstSegment = rBuffers.pStartSegmentTransp;
        addQuad.ppNextDstVertex = &rBuffers.pEndVertexTransp;
        addQuad.ppCurrentDstSegment = &rBuffers.pCurrentSegmentTransp;
        addQuad.VertexAdd = static_cast<int32_t>(VERTICES_PER_QUAD);
        addQuad.SegmentAdd = 1;
      }

      assert(addQuad.IsValid(m_segments));
      return addQuad;
    }


    void Clear(segment_type* pDst, const segment_type* const pDstEnd)
    {
      assert(pDst != nullptr);
      assert(pDstEnd != nullptr);
      assert(pDst <= pDstEnd);
      while (pDst < pDstEnd)
      {
        *pDst = {};
        ++pDst;
      }
    }

    inline bool IsValid() const
    {
      // Sanity checks
      return (m_segments.size() >= ((MIN_QUADS + SAFETY) * NUM_BUFFERS)) && ((m_quadVertices.size() % NUM_BUFFERS) == 0) &&
             ((m_segments.size() % NUM_BUFFERS) == 0) && ((m_quadVertices.size() / VERTICES_PER_QUAD) == m_segments.size()) &&
             m_buffers.IsValid(m_quadVertices, m_segments) && m_addQuad.IsValid(m_segments);
    }

    void UpdateSegmentState(const texture_info_type& textureInfo, const BlendState blendState)
    {
      assert(IsValid());
      // If this assert fire it means we have a internal error
      assert(m_addQuad.ppCurrentDstSegment != nullptr);
      auto pDstSegment = *m_addQuad.ppCurrentDstSegment;
      assert(pDstSegment != nullptr);

      // If there are vertices in the current segment
      // then we can only reuse it if the active texture and blend state matches
      if (pDstSegment->VertexCount <= 0)
      {
        // As there are zero vertices, we check to see if there is a segment before this one to see if the new 'state' is compatible with it
        // This ensures we get a minimum amount of segments.
        if (pDstSegment != m_addQuad.pFirstDstSegment)
        {
          // Previous segment found, lets examine it
          auto pPreviousDstSegment = pDstSegment - m_addQuad.SegmentAdd;
          assert(pPreviousDstSegment >= m_segments.data());
          assert(pPreviousDstSegment < (m_segments.data() + m_segments.size()));

          if (textureInfo == pPreviousDstSegment->TextureInfo && blendState == pPreviousDstSegment->ActiveBlendState)
          {
            // The state match so we can reuse it, so we clear the now unused segment and move back to the previous one
            pDstSegment = {};

            // Move to the previous segment and quick exit
            (*m_addQuad.ppCurrentDstSegment) = pPreviousDstSegment;
            return;
          }
        }
        // No previous compatible segment so we reuse the active segment with the new state, so lets update it.
        pDstSegment->TextureInfo = textureInfo;
        pDstSegment->ActiveBlendState = blendState;
        assert(IsValid());
        return;
      }

      // There are vertices so we check if the state is compatible
      if ((textureInfo == pDstSegment->TextureInfo && blendState == pDstSegment->ActiveBlendState))
      {
        assert(IsValid());
        return;
      }

      // No match -> move to the next segment
      pDstSegment += m_addQuad.SegmentAdd;
      (*m_addQuad.ppCurrentDstSegment) = pDstSegment;

      // We expect the user called ensure capacity before starting to use this
      assert(pDstSegment >= m_segments.data());
      assert(pDstSegment < (m_segments.data() + m_segments.size()));
      // Update the segment and exit
      (*pDstSegment) = segment_type(textureInfo, blendState);

      assert(IsValid());
      return;
    }
  };

  template <typename TTextureInfo>
  const uint32_t StrategySortByTransparency<TTextureInfo>::VERTICES_PER_QUAD;
  template <typename TTextureInfo>
  const uint32_t StrategySortByTransparency<TTextureInfo>::EXPAND_QUAD_GROWTH;
  template <typename TTextureInfo>
  const uint32_t StrategySortByTransparency<TTextureInfo>::SAFETY;
  template <typename TTextureInfo>
  const uint32_t StrategySortByTransparency<TTextureInfo>::MIN_QUADS;
  template <typename TTextureInfo>
  const uint32_t StrategySortByTransparency<TTextureInfo>::NUM_BUFFERS;
}

#endif
