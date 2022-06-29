#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALHASHGRID2D_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALHASHGRID2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <unordered_map>
#include <vector>

namespace Fsl
{
  class SpatialHashGrid2D
  {
    struct Record
    {
      std::vector<uint32_t> Bucket;
    };

    std::vector<Record> m_entries;
    std::vector<uint32_t> m_lookup;
    uint16_t m_gridCellCountX{};
    uint16_t m_gridCellCountY{};
    uint8_t m_shiftX{};
    uint8_t m_shiftY{};

  public:
    SpatialHashGrid2D(const uint16_t gridCellCountX, const uint16_t gridCellCountY, const uint8_t shiftX, const uint8_t shiftY)
      : m_entries(gridCellCountX * gridCellCountY)
      , m_gridCellCountX(gridCellCountX)
      , m_gridCellCountY(gridCellCountY)
      , m_shiftX(shiftX)
      , m_shiftY(shiftY)
    {
      if (gridCellCountX <= 0)
      {
        throw std::invalid_argument("gridCellCountX must be > 0");
      }
      if (gridCellCountY <= 0)
      {
        throw std::invalid_argument("gridCellCountY must be > 0");
      }
    }

    void Clear() noexcept
    {
      for (auto& rEntry : m_entries)
      {
        rEntry.Bucket.clear();
      }
      m_lookup.clear();
    }

    int32_t GetCellCountX() const noexcept
    {
      return UncheckedNumericCast<int32_t>(m_gridCellCountX);
    }

    int32_t GetCellCountY() const noexcept
    {
      return UncheckedNumericCast<int32_t>(m_gridCellCountY);
    }

    struct Range
    {
      uint16_t Start;
      uint16_t End;
    };

    constexpr Range ToXCell(const float left, const float right) const noexcept
    {
      int32_t startCellX = static_cast<int32_t>(left) >> m_shiftX;
      int32_t endCellX = (static_cast<int32_t>(right + 1.0f) >> m_shiftX) + 1;
      return {static_cast<uint16_t>(startCellX), static_cast<uint16_t>(endCellX)};
    }

    constexpr Range ToYCell(const float top, const float bottom) const noexcept
    {
      int32_t startCellX = static_cast<int32_t>(top) >> m_shiftY;
      int32_t endCellX = (static_cast<int32_t>(bottom + 1.0f) >> m_shiftY) + 1;
      return {static_cast<uint16_t>(startCellX), static_cast<uint16_t>(endCellX)};
    }

    bool TryAdd(const PxAreaRectangleF& rectangle, const uint32_t originalZPos)
    {
      const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
      const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
      const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
      const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

      const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
      if (add)
      {
        const uint32_t startX = startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0;
        const uint32_t startY = startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0;
        const uint32_t endX = endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX;
        const uint32_t endY = endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY;
        assert(startX <= endX);
        assert(startY <= endY);
        Record* pDst = m_entries.data();
        for (uint32_t y = startY; y < endY; ++y)
        {
          for (uint32_t x = startX; x < endX; ++x)
          {
            assert((x + (y * m_gridCellCountX)) < m_entries.size());
            pDst[x + (y * m_gridCellCountX)].Bucket.push_back(originalZPos);
          }
        }
        m_lookup.push_back(originalZPos);
      }
      return add;
    }


    bool TryInsertAfterZPos(const uint32_t insertAfterRemappedZPos, const PxAreaRectangleF& rectangle, const uint32_t originalZPos)
    {
      const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
      const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
      const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
      const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

      const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
      if (add)
      {
        const uint32_t startX = startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0;
        const uint32_t startY = startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0;
        const uint32_t endX = endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX;
        const uint32_t endY = endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY;
        assert(startX <= endX);
        assert(startY <= endY);

        Record* pEntries = m_entries.data();
        for (uint32_t y = startY; y < endY; ++y)
        {
          for (uint32_t x = startX; x < endX; ++x)
          {
            assert((x + (y * m_gridCellCountX)) < m_entries.size());
            Record& rDst = pEntries[x + (y * m_gridCellCountX)];
            // Do a push back to just make room for the new entry (then shift everything back until we hit the desired z-pos to insert at
            rDst.Bucket.push_back(0xFFFFFFFF);
            uint32_t* pDst = rDst.Bucket.data();
            const std::size_t bucketSize = rDst.Bucket.size();
            std::size_t i = bucketSize - 1;
            while (i > 0 && m_lookup[pDst[i - 1]] > insertAfterRemappedZPos)
            {
              assert(i > 0 && i < bucketSize);
              pDst[i] = pDst[i - 1];
              --i;
            }
            pDst[i] = originalZPos;
          }
        }
        m_lookup.push_back(insertAfterRemappedZPos);
      }
      return add;
    }

    Span<uint32_t> LookupSpan()
    {
      return SpanUtil::AsSpan(m_lookup);
    }

    inline uint32_t ToRemappedZPos(const uint32_t originalIndex) const noexcept
    {
      return m_lookup[originalIndex];
    }


    //! @brief This returns original zPositions
    ReadOnlySpan<uint32_t> TryGetChunkEntries(const uint16_t chunkX, const uint16_t chunkY) const noexcept
    {
      return (chunkX < m_gridCellCountX && chunkY < m_gridCellCountY)
               ? ReadOnlySpanUtil::AsSpan(m_entries[chunkX + (chunkY * m_gridCellCountX)].Bucket)
               : ReadOnlySpan<uint32_t>();
    }

    //! @brief This returns original zPositions
    ReadOnlySpan<uint32_t> UncheckedGetChunkEntries(const uint16_t chunkX, const uint16_t chunkY) const noexcept
    {
      assert(chunkX < m_gridCellCountX);
      assert(chunkY < m_gridCellCountY);
      return ReadOnlySpanUtil::AsSpan(m_entries[chunkX + (chunkY * m_gridCellCountX)].Bucket);
    }

    /*
        void SanityCheckEntry(const PxAreaRectangleF& rectangle, const uint32_t originalZPos) const
        {
          const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
          const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
          const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
          const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

          const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
          if (add)
          {
            const uint32_t startX = startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0;
            const uint32_t startY = startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0;
            const uint32_t endX = endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX;
            const uint32_t endY = endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY;
            assert(startX <= endX);
            assert(startY <= endY);

            const Record* const pEntries = m_entries.data();
            for (uint32_t y = 0; y < m_gridCellCountY; ++y)
            {
              for (uint32_t x = 0; x < m_gridCellCountY; ++x)
              {
                assert((x + (y * m_gridCellCountX)) < m_entries.size());
                const Record& src = pEntries[x + (y * m_gridCellCountX)];
                bool found = false;
                for (const auto& bucketEntry : src.Bucket)
                {
                  if (bucketEntry == originalZPos)
                    found = true;
                }

                if (x >= startX && x < endX && y >= startY && y < endY)
                {
                  if (!found)
                    throw InternalErrorException("Grid is not consistent, did not find the entry in the expected cells");
                }
                else if (found)
                {
                  throw InternalErrorException("Grid is not consistent, found the entry outside the expected cells1");
                }
              }
            }
          }
          else
          {
            // The ID should not be part of any grid entry
            for (const auto& entry : m_entries)
            {
              for (const auto& bucketEntry : entry.Bucket)
              {
                if (bucketEntry == originalZPos)
                  throw InternalErrorException("Grid is not consistent, found the entry outside the expected cells2");
              }
            }
          }
        }
        */
  };
}

#endif
