#ifndef FSLGRAPHICS_RENDER_STRATEGY_STRATEGYBATCHBYSTATE_HPP
#define FSLGRAPHICS_RENDER_STRATEGY_STRATEGYBATCHBYSTATE_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2023 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslGraphics/NativeQuadTextureCoords.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/Render/BatchSdfRenderConfig.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Strategy/BatchSegmentInfo.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include <algorithm>
#include <cassert>
#include <vector>

namespace Fsl
{
  // The most basic batching implementation that 'batches' by things with the same state.
  // This means that anytime the blend-state or texture is changed we create a new batch.
  // This also means that everything will be rendered using a standard back-to-front painters algorithm so no depth buffer is required to render it.
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
  class StrategyBatchByState
  {
  public:
    static constexpr const int32_t VERTICES_PER_QUAD = 4;

    using texture_info_type = TTextureInfo;
    using segment_type = BatchSegmentInfo<texture_info_type>;
    using vertex_type = VertexPositionColorTexture;
    using vertex_span_type = VertexSpan<vertex_type>;

  private:
    static constexpr const uint32_t EXPAND_QUAD_GROWTH = 1024;
    static constexpr const uint32_t SAFETY = 1;
    static_assert(SAFETY > 0, "Safety must be > 0");


    // Pointers for optimal access to tracking areas
    struct AddQuadPointers
    {
      vertex_type* pNextDstVertex{nullptr};
      segment_type* pCurrentDstSegment{nullptr};

      AddQuadPointers() = default;

      AddQuadPointers(vertex_type* pDstVertex1, segment_type* pDstSegment1)
        : pNextDstVertex(pDstVertex1)
        , pCurrentDstSegment(pDstSegment1)
      {
      }

      bool IsValid(const std::vector<vertex_type>& /*quadVertices*/, const std::vector<segment_type>& /*segments*/) const
      {
        return (pNextDstVertex != nullptr && pCurrentDstSegment != nullptr);
      }
    };


    std::vector<vertex_type> m_quadVertices;
    std::vector<segment_type> m_segments;
    AddQuadPointers m_addQuad;

  public:
    StrategyBatchByState(const StrategyBatchByState&) = delete;
    StrategyBatchByState(StrategyBatchByState&& other) = delete;
    StrategyBatchByState& operator=(const StrategyBatchByState&) = delete;
    StrategyBatchByState& operator=(StrategyBatchByState&& other) = delete;

    explicit StrategyBatchByState(const uint32_t quadCapacity = 4096)
      : m_quadVertices((std::max(quadCapacity, 1u) + SAFETY) * VERTICES_PER_QUAD)
      , m_segments(std::max(quadCapacity, 1u) + SAFETY)
      , m_addQuad(AddQuadPointers(m_quadVertices.data(), m_segments.data()))
    {
      assert(IsValid());
    }

    ~StrategyBatchByState() = default;


    BlendState GetActiveBlendState() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert(m_addQuad.pCurrentDstSegment != nullptr);

      return m_addQuad.pCurrentDstSegment->ActiveBlendState;
    }

    const BatchSdfRenderConfig& GetActiveSdfRenderConfig() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert(m_addQuad.pCurrentDstSegment != nullptr);

      return m_addQuad.pCurrentDstSegment->SdfRenderConfig;
    }

    const texture_info_type& GetActiveTexture() const
    {
      assert(IsValid());
      assert(m_addQuad.pCurrentDstSegment != nullptr);

      return m_addQuad.pCurrentDstSegment->TextureInfo;
    }


    uint32_t GetCapacity() const
    {
      assert(IsValid());
      return static_cast<uint32_t>(m_segments.size() - SAFETY);
    }


    uint32_t GetSegmentCount() const
    {
      assert(IsValid());
      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));

      auto segments = static_cast<uint32_t>((m_addQuad.pCurrentDstSegment - m_segments.data()));
      return segments + (m_addQuad.pCurrentDstSegment->VertexCount > 0 ? 1 : 0);
    }


    uint32_t GetVertexCount() const
    {
      assert(IsValid());
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert(m_addQuad.pNextDstVertex < (m_quadVertices.data() + m_quadVertices.size()));

      return static_cast<uint32_t>(m_addQuad.pNextDstVertex - m_quadVertices.data());
    }


    uint32_t GetQuadCount() const
    {
      // If these assert fire it means we have a internal error
      assert(IsValid());
      assert((GetVertexCount() % VERTICES_PER_QUAD) == 0);
      return GetVertexCount() / VERTICES_PER_QUAD;
    }


    vertex_span_type GetSpan() const
    {
      // If this assert fire it means we have a internal error
      assert(IsValid());
      return vertex_span_type(m_quadVertices.data(), GetVertexCount());
    }

    const segment_type& GetSegment(const uint32_t index) const
    {
      assert(index < GetSegmentCount());
      // If this assert fire it means we have a internal error
      assert(IsValid());

      return m_segments[index];
    }

    //! @brief clear everything
    //! @note  This also resets the blendState and texture!
    void Clear()
    {
      assert(IsValid());

      ClearSegmentTextureInfo();

      m_addQuad = AddQuadPointers(m_quadVertices.data(), m_segments.data());
    }


    void SetBlendState(const BlendState blendState)
    {
      assert(IsValid());

      // Read the current texture info so we have it available before we update pointers
      UpdateSegmentState(GetActiveTexture(), blendState, GetActiveSdfRenderConfig());
    }


    inline void SetBatchSdfRenderConfig(const BatchSdfRenderConfig& config)
    {
      assert(IsValid());
      UpdateSegmentState(GetActiveTexture(), GetActiveBlendState(), config);
    }


    //! Make sure you call SetBlendState before SetTexture!
    inline void SetTexture(const texture_info_type& textureInfo)
    {
      UpdateSegmentState(textureInfo, GetActiveBlendState(), GetActiveSdfRenderConfig());
    }

    inline void AddQuad(const Vector2& vec0, const Vector2& vec1, const Vector2& vec2, const Vector2& vec3, const Vector2& texCoords0,
                        const Vector2& texCoords1, const Color& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert((m_addQuad.pNextDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size()));

      m_addQuad.pNextDstVertex->Position.X = vec0.X;
      m_addQuad.pNextDstVertex->Position.Y = vec0.Y;
      m_addQuad.pNextDstVertex->TextureCoordinate.X = texCoords0.X;
      m_addQuad.pNextDstVertex->TextureCoordinate.Y = texCoords0.Y;
      m_addQuad.pNextDstVertex->Color = color;

      (m_addQuad.pNextDstVertex + 1)->Position.X = vec1.X;
      (m_addQuad.pNextDstVertex + 1)->Position.Y = vec1.Y;
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate.X = texCoords1.X;
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate.Y = texCoords0.Y;
      (m_addQuad.pNextDstVertex + 1)->Color = color;

      (m_addQuad.pNextDstVertex + 2)->Position.X = vec2.X;
      (m_addQuad.pNextDstVertex + 2)->Position.Y = vec2.Y;
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate.X = texCoords0.X;
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate.Y = texCoords1.Y;
      (m_addQuad.pNextDstVertex + 2)->Color = color;

      (m_addQuad.pNextDstVertex + 3)->Position.X = vec3.X;
      (m_addQuad.pNextDstVertex + 3)->Position.Y = vec3.Y;
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate.X = texCoords1.X;
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate.Y = texCoords1.Y;
      (m_addQuad.pNextDstVertex + 3)->Color = color;

      // Update our records
      m_addQuad.pNextDstVertex += VERTICES_PER_QUAD;

      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      m_addQuad.pCurrentDstSegment->VertexCount += VERTICES_PER_QUAD;
    }


    inline void AddQuad(const PxVector2& vec0, const PxVector2& vec1, const PxVector2& vec2, const PxVector2& vec3, const Vector2& texCoords0,
                        const Vector2& texCoords1, const Color& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert((m_addQuad.pNextDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size()));

      m_addQuad.pNextDstVertex->Position.X = vec0.X.Value;
      m_addQuad.pNextDstVertex->Position.Y = vec0.Y.Value;
      m_addQuad.pNextDstVertex->TextureCoordinate.X = texCoords0.X;
      m_addQuad.pNextDstVertex->TextureCoordinate.Y = texCoords0.Y;
      m_addQuad.pNextDstVertex->Color = color;

      (m_addQuad.pNextDstVertex + 1)->Position.X = vec1.X.Value;
      (m_addQuad.pNextDstVertex + 1)->Position.Y = vec1.Y.Value;
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate.X = texCoords1.X;
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate.Y = texCoords0.Y;
      (m_addQuad.pNextDstVertex + 1)->Color = color;

      (m_addQuad.pNextDstVertex + 2)->Position.X = vec2.X.Value;
      (m_addQuad.pNextDstVertex + 2)->Position.Y = vec2.Y.Value;
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate.X = texCoords0.X;
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate.Y = texCoords1.Y;
      (m_addQuad.pNextDstVertex + 2)->Color = color;

      (m_addQuad.pNextDstVertex + 3)->Position.X = vec3.X.Value;
      (m_addQuad.pNextDstVertex + 3)->Position.Y = vec3.Y.Value;
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate.X = texCoords1.X;
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate.Y = texCoords1.Y;
      (m_addQuad.pNextDstVertex + 3)->Color = color;

      // Update our records
      m_addQuad.pNextDstVertex += VERTICES_PER_QUAD;

      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      m_addQuad.pCurrentDstSegment->VertexCount += VERTICES_PER_QUAD;
    }

    inline void AddQuad(const Vector2& vec0, const Vector2& vec1, const Vector2& vec2, const Vector2& vec3,
                        const NativeQuadTextureCoords& textureCoords, const Color& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert((m_addQuad.pNextDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size()));

      m_addQuad.pNextDstVertex->Position.X = vec0.X;
      m_addQuad.pNextDstVertex->Position.Y = vec0.Y;
      m_addQuad.pNextDstVertex->TextureCoordinate = textureCoords.TopLeft;
      m_addQuad.pNextDstVertex->Color = color;

      (m_addQuad.pNextDstVertex + 1)->Position.X = vec1.X;
      (m_addQuad.pNextDstVertex + 1)->Position.Y = vec1.Y;
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate = textureCoords.TopRight;
      (m_addQuad.pNextDstVertex + 1)->Color = color;

      (m_addQuad.pNextDstVertex + 2)->Position.X = vec2.X;
      (m_addQuad.pNextDstVertex + 2)->Position.Y = vec2.Y;
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate = textureCoords.BottomLeft;
      (m_addQuad.pNextDstVertex + 2)->Color = color;

      (m_addQuad.pNextDstVertex + 3)->Position.X = vec3.X;
      (m_addQuad.pNextDstVertex + 3)->Position.Y = vec3.Y;
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate = textureCoords.BottomRight;
      (m_addQuad.pNextDstVertex + 3)->Color = color;

      // Update our records
      m_addQuad.pNextDstVertex += VERTICES_PER_QUAD;

      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      m_addQuad.pCurrentDstSegment->VertexCount += VERTICES_PER_QUAD;
    }

    inline void AddQuad(const PxAreaRectangleF& dstRect, const NativeTextureArea& srcArea, const Color& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert((m_addQuad.pNextDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size()));

      m_addQuad.pNextDstVertex->Position.X = dstRect.RawLeft();
      m_addQuad.pNextDstVertex->Position.Y = dstRect.RawTop();
      m_addQuad.pNextDstVertex->TextureCoordinate = Vector2(srcArea.X0, srcArea.Y0);
      m_addQuad.pNextDstVertex->Color = color;

      (m_addQuad.pNextDstVertex + 1)->Position.X = dstRect.RawRight();
      (m_addQuad.pNextDstVertex + 1)->Position.Y = dstRect.RawTop();
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate = Vector2(srcArea.X1, srcArea.Y0);
      (m_addQuad.pNextDstVertex + 1)->Color = color;

      (m_addQuad.pNextDstVertex + 2)->Position.X = dstRect.RawLeft();
      (m_addQuad.pNextDstVertex + 2)->Position.Y = dstRect.RawBottom();
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate = Vector2(srcArea.X0, srcArea.Y1);
      (m_addQuad.pNextDstVertex + 2)->Color = color;

      (m_addQuad.pNextDstVertex + 3)->Position.X = dstRect.RawRight();
      (m_addQuad.pNextDstVertex + 3)->Position.Y = dstRect.RawBottom();
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate = Vector2(srcArea.X1, srcArea.Y1);
      (m_addQuad.pNextDstVertex + 3)->Color = color;

      // Update our records
      m_addQuad.pNextDstVertex += VERTICES_PER_QUAD;

      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      m_addQuad.pCurrentDstSegment->VertexCount += VERTICES_PER_QUAD;
    }

    inline void AddQuad(const PxAreaRectangleF& dstRect, const NativeQuadTextureCoords& srcArea, const Color& color)
    {
      static_assert(VERTICES_PER_QUAD == 4, "we assume four vertices per quad here");

      assert(IsValid());

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pNextDstVertex != nullptr);
      assert(m_addQuad.pNextDstVertex >= m_quadVertices.data());
      assert((m_addQuad.pNextDstVertex + VERTICES_PER_QUAD) <= (m_quadVertices.data() + m_quadVertices.size()));

      m_addQuad.pNextDstVertex->Position.X = dstRect.RawLeft();
      m_addQuad.pNextDstVertex->Position.Y = dstRect.RawTop();
      m_addQuad.pNextDstVertex->TextureCoordinate = srcArea.TopLeft;
      m_addQuad.pNextDstVertex->Color = color;

      (m_addQuad.pNextDstVertex + 1)->Position.X = dstRect.RawRight();
      (m_addQuad.pNextDstVertex + 1)->Position.Y = dstRect.RawTop();
      (m_addQuad.pNextDstVertex + 1)->TextureCoordinate = srcArea.TopRight;
      (m_addQuad.pNextDstVertex + 1)->Color = color;

      (m_addQuad.pNextDstVertex + 2)->Position.X = dstRect.RawLeft();
      (m_addQuad.pNextDstVertex + 2)->Position.Y = dstRect.RawBottom();
      (m_addQuad.pNextDstVertex + 2)->TextureCoordinate = srcArea.BottomLeft;
      (m_addQuad.pNextDstVertex + 2)->Color = color;

      (m_addQuad.pNextDstVertex + 3)->Position.X = dstRect.RawRight();
      (m_addQuad.pNextDstVertex + 3)->Position.Y = dstRect.RawBottom();
      (m_addQuad.pNextDstVertex + 3)->TextureCoordinate = srcArea.BottomRight;
      (m_addQuad.pNextDstVertex + 3)->Color = color;

      // Update our records
      m_addQuad.pNextDstVertex += VERTICES_PER_QUAD;

      assert(m_addQuad.pCurrentDstSegment != nullptr);
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      m_addQuad.pCurrentDstSegment->VertexCount += VERTICES_PER_QUAD;
    }


    inline void EnsureCapacity(const std::size_t desiredQuadCapacity)
    {
      if ((desiredQuadCapacity + SAFETY) > m_segments.size())
      {
        GrowCapacity(desiredQuadCapacity);
      }
    }

    inline void EnsureCapacityFor(const std::size_t elementsToEnsureCapacityFor)
    {
      auto desiredQuadCapacity = (GetQuadCount() + elementsToEnsureCapacityFor);
      if ((desiredQuadCapacity + SAFETY) > m_segments.size())
      {
        GrowCapacity(desiredQuadCapacity);
      }
    }

  private:
    void GrowCapacity(std::size_t newMinimumQuadCapacity)
    {
      assert(IsValid());

      newMinimumQuadCapacity += SAFETY;


      // auto vertexCount = GetVertexCount();
      auto diffNextDstVertex = (m_addQuad.pNextDstVertex - m_quadVertices.data());
      auto diffCurrentDstSegment = (m_addQuad.pCurrentDstSegment - m_segments.data());

      m_quadVertices.resize(newMinimumQuadCapacity * VERTICES_PER_QUAD);
      m_segments.resize(newMinimumQuadCapacity);

      // Update the pointers to match the new buffer
      m_addQuad = AddQuadPointers(m_quadVertices.data(), m_segments.data());
      // then patch it with the old offsets
      m_addQuad.pNextDstVertex += diffNextDstVertex;
      m_addQuad.pCurrentDstSegment += diffCurrentDstSegment;

      assert(IsValid());
    }


    void ClearSegmentTextureInfo()
    {
      auto* pDst = m_segments.data();
      const auto* const pDstEnd = pDst + GetSegmentCount();

      assert(pDstEnd <= (m_segments.data() + m_segments.size()));
      while (pDst < pDstEnd)
      {
        // We only clear the relevant fields
        pDst->TextureInfo = {};
        pDst->VertexCount = 0;
        ++pDst;
      }
    }

    inline bool IsValid() const
    {
      // Sanity checks
      return m_addQuad.IsValid(m_quadVertices, m_segments);
    }

    void UpdateSegmentState(const texture_info_type& textureInfo, const BlendState blendState, const BatchSdfRenderConfig& sdfRenderConfig)
    {
      assert(IsValid());
      // If this assert fire it means we have a internal error
      assert(m_addQuad.pCurrentDstSegment != nullptr);

      // If there are vertices in the current segment
      // then we can only reuse it if the active texture and blend state matches
      if (m_addQuad.pCurrentDstSegment->VertexCount <= 0)
      {
        // As there are zero vertices, we check to see if there is a segment before this one to see if the new 'state' is compatible with it
        // This ensures we get a minimum amount of segments.
        if (m_addQuad.pCurrentDstSegment > m_segments.data())
        {
          // Previous segment found, lets examine it
          auto pPreviousDstSegment = m_addQuad.pCurrentDstSegment - 1;
          assert(pPreviousDstSegment >= m_segments.data());
          assert(pPreviousDstSegment < (m_segments.data() + m_segments.size()));

          if (textureInfo == pPreviousDstSegment->TextureInfo && blendState == pPreviousDstSegment->ActiveBlendState &&
              (blendState != BlendState::Sdf || sdfRenderConfig == pPreviousDstSegment->SdfRenderConfig))
          {
            // The state match so we can reuse it, so we clear the now unused segment and move back to the previous one
            //            m_addQuad.pCurrentDstSegment = {};

            // Move to the previous segment and quick exit
            m_addQuad.pCurrentDstSegment = pPreviousDstSegment;
            return;
          }
        }
        // No previous compatible segment so we reuse the active segment with the new state, so lets update it.
        m_addQuad.pCurrentDstSegment->TextureInfo = textureInfo;
        m_addQuad.pCurrentDstSegment->ActiveBlendState = blendState;
        m_addQuad.pCurrentDstSegment->SdfRenderConfig = sdfRenderConfig;
        assert(IsValid());
        return;
      }

      // There are vertices so we check if the state is compatible
      if (textureInfo == m_addQuad.pCurrentDstSegment->TextureInfo && blendState == m_addQuad.pCurrentDstSegment->ActiveBlendState &&
          (blendState != BlendState::Sdf || sdfRenderConfig == m_addQuad.pCurrentDstSegment->SdfRenderConfig))
      {
        assert(IsValid());
        return;
      }

      // No match -> move to the next segment
      ++m_addQuad.pCurrentDstSegment;

      // We expect the user called ensure capacity before starting to use this
      assert(m_addQuad.pCurrentDstSegment >= m_segments.data());
      assert(m_addQuad.pCurrentDstSegment < (m_segments.data() + m_segments.size()));
      // Update the segment and exit
      (*m_addQuad.pCurrentDstSegment) = segment_type(textureInfo, blendState, sdfRenderConfig);

      assert(IsValid());
    }
  };

  // FIX-LATER: Remove once we move to C++17
  template <typename TTextureInfo>
  const int32_t StrategyBatchByState<TTextureInfo>::VERTICES_PER_QUAD;    // NOLINT(readability-redundant-declaration)

  // FIX-LATER: Remove once we move to C++17
  template <typename TTextureInfo>
  const uint32_t StrategyBatchByState<TTextureInfo>::EXPAND_QUAD_GROWTH;    // NOLINT(readability-redundant-declaration)

  // FIX-LATER: Remove once we move to C++17
  template <typename TTextureInfo>
  const uint32_t StrategyBatchByState<TTextureInfo>::SAFETY;    // NOLINT(readability-redundant-declaration)
}

#endif
