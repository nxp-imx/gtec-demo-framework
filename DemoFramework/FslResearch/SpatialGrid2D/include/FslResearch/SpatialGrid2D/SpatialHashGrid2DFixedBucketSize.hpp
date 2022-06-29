#ifndef FSLRESEARCH_SPATIALGRID2D_FSLRESEARCH_SPATIALGRID2D_SPATIALHASHGRID2DFIXEDBUCKETSIZE_HPP
#define FSLRESEARCH_SPATIALGRID2D_FSLRESEARCH_SPATIALGRID2D_SPATIALHASHGRID2DFIXEDBUCKETSIZE_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <vector>

namespace Fsl
{
  //! @brief Fixed size and fixed bucket size grid. This ought to be one of the simplest and fastest spatial grids.
  //!        But the fixed bucket size is a severe limitation.
  template <uint32_t TMaxEntriesPerChunk>
  class SpatialHashGrid2DFixedBucketSize
  {
  public:

  private:
    std::vector<uint32_t> m_grid;
    uint32_t m_gridStride{};
    uint16_t m_gridCellCountX{};
    uint16_t m_gridCellCountY{};
    uint8_t m_shiftX{};
    uint8_t m_shiftY{};

  public:
    SpatialHashGrid2DFixedBucketSize() = default;

    SpatialHashGrid2DFixedBucketSize(const uint16_t gridCellCountX, const uint16_t gridCellCountY, const uint8_t shiftX, const uint8_t shiftY)
      : m_grid(std::size_t(gridCellCountX) * gridCellCountY * TMaxEntriesPerChunk)
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
      m_gridStride = (m_gridCellCountX * TMaxEntriesPerChunk);
    }

    void Clear() noexcept
    {
      uint32_t* pData = m_grid.data();
      const auto* pDataEnd = pData + m_grid.size();
      while (pData < pDataEnd)
      {
        *pData = 0;
        pData += TMaxEntriesPerChunk;
      }
    }

    constexpr int32_t GetCellCountX() const noexcept
    {
      return UncheckedNumericCast<int32_t>(m_gridCellCountX);
    }

    constexpr int32_t GetCellCountY() const noexcept
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

    constexpr bool TryAdd(const PxAreaRectangleF& rectangle, const uint32_t id) noexcept
    {
      const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
      const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
      const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
      const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

      const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
      if (add)
      {
        const uint32_t gridStride = m_gridStride;

        const uint32_t startOffsetX = (startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0) * TMaxEntriesPerChunk;
        const uint32_t startOffsetY = (startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0) * gridStride;
        const uint32_t endOffsetX = (endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX) * TMaxEntriesPerChunk;
        const uint32_t endOffsetY = (endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY) * gridStride;
        assert(startOffsetX <= endOffsetX);
        assert(startOffsetY <= endOffsetY);

        auto* pDataStart = m_grid.data();
        for (uint32_t y = startOffsetY; y < endOffsetY; y += gridStride)
        {
          for (uint32_t x = startOffsetX; x < endOffsetX; x += TMaxEntriesPerChunk)
          {
            // The first entry in the main chunk is the count
            uint32_t& rEntryCount = pDataStart[y + x];
            // We foolishly assume that there will be no overflow (and with this controlled data we are actually sure)
            assert(rEntryCount <= 0xFFFFFFFE);
            ++rEntryCount;
            if (rEntryCount < TMaxEntriesPerChunk)
            {
              assert((x + y + rEntryCount) <= m_grid.size());
              pDataStart[y + x + rEntryCount] = id;
            }
            else
            {
              // Nasty, quick exit if chunk was full
              // This really ought to at least cleanup what was inserted, but for the demo purpose this should never happen
              FSLLOG3_DEBUG_WARNING("Bucket full, data does not fit");
              return false;
            }
          }
        }
      }
      return add;
    }

    constexpr bool TryUncheckedAdd(const PxAreaRectangleF& rectangle, const uint32_t id) noexcept
    {
      const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
      const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
      const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
      const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

      const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
      if (add)
      {
        const uint32_t gridStride = m_gridStride;

        const uint32_t startOffsetX = (startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0) * TMaxEntriesPerChunk;
        const uint32_t startOffsetY = (startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0) * gridStride;
        const uint32_t endOffsetX = (endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX) * TMaxEntriesPerChunk;
        const uint32_t endOffsetY = (endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY) * gridStride;
        assert(startOffsetX <= endOffsetX);
        assert(startOffsetY <= endOffsetY);

        auto* pDataStart = m_grid.data();
        for (uint32_t y = startOffsetY; y < endOffsetY; y += gridStride)
        {
          for (uint32_t x = startOffsetX; x < endOffsetX; x += TMaxEntriesPerChunk)
          {
            // The first entry in the main chunk is the count
            uint32_t& rEntryCount = pDataStart[y + x];
            // We foolishly assume that there will be no overflow (and with this controlled data we are actually sure)
            assert(rEntryCount <= 0xFFFFFFFE);
            ++rEntryCount;
            assert(rEntryCount < TMaxEntriesPerChunk);
            assert((x + y + rEntryCount) <= m_grid.size());
            pDataStart[y + x + rEntryCount] = id;
          }
        }
      }
      return add;
    }

    bool TryInsertAfterZPos(const uint32_t insertAfterZPos, const PxAreaRectangleF& rectangle, const uint32_t newZpos)
    {
      const int32_t startCellX = static_cast<int32_t>(rectangle.Left()) >> m_shiftX;
      const int32_t endCellX = (static_cast<int32_t>(rectangle.Right() + 1.0f) >> m_shiftX) + 1;
      const int32_t startCellY = static_cast<int32_t>(rectangle.Top()) >> m_shiftY;
      const int32_t endCellY = (static_cast<int32_t>(rectangle.Bottom() + 1.0f) >> m_shiftY) + 1;

      const bool add = startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0;
      if (add)
      {
        const uint32_t gridStride = m_gridStride;

        const uint32_t startOffsetX = (startCellX >= 0 ? static_cast<uint32_t>(startCellX) : 0) * TMaxEntriesPerChunk;
        const uint32_t startOffsetY = (startCellY >= 0 ? static_cast<uint32_t>(startCellY) : 0) * gridStride;
        const uint32_t endOffsetX = (endCellX <= m_gridCellCountX ? static_cast<uint32_t>(endCellX) : m_gridCellCountX) * TMaxEntriesPerChunk;
        const uint32_t endOffsetY = (endCellY <= m_gridCellCountY ? static_cast<uint32_t>(endCellY) : m_gridCellCountY) * gridStride;
        assert(startOffsetX <= endOffsetX);
        assert(startOffsetY <= endOffsetY);


        for (uint32_t y = startOffsetY; y < endOffsetY; y += gridStride)
        {
          for (uint32_t x = startOffsetX; x < endOffsetX; x += TMaxEntriesPerChunk)
          {
            uint32_t* pRecord = m_grid.data() + y + x;
            // We foolishly assume that there will be no overflow (and with this controlled data we are actually sure)
            // - Increase the entry count (which is the first stored entry)
            assert(*pRecord <= 0xFFFFFFFE);
            ++(*pRecord);

            // Do a push back to just make room for the new entry (then shift everything back until we hit the desired z-pos to insert at
            uint32_t* pDst = pRecord + 1;    // skip the size counter
            const std::size_t bucketSize = *pRecord;
            std::size_t i = bucketSize - 1;
            while (i > 0 && pDst[i - 1] > insertAfterZPos)
            {
              assert(i > 0 && i < bucketSize);
              pDst[i] = pDst[i - 1] + 1;
              --i;
            }
            pDst[i] = newZpos;
          }
        }
      }
      return add;
    }

    constexpr ReadOnlySpan<uint32_t> TryGetChunkEntries(const uint32_t chunkX, const uint32_t chunkY) const noexcept
    {
      const uint32_t offsetX = chunkX * TMaxEntriesPerChunk;
      const uint32_t offsetY = chunkY * m_gridStride;
      const uint32_t* pRecord = m_grid.data() + offsetY + offsetX;
      return (chunkX < m_gridCellCountX && chunkY < m_gridCellCountY) ? ReadOnlySpan<uint32_t>(pRecord + 1, *pRecord, OptimizationCheckFlag::NoCheck)
                                                                      : ReadOnlySpan<uint32_t>();
    }

    constexpr ReadOnlySpan<uint32_t> UncheckedGetChunkEntries(const uint32_t chunkX, const uint32_t chunkY) const noexcept
    {
      const uint32_t offsetX = chunkX * TMaxEntriesPerChunk;
      const uint32_t offsetY = chunkY * m_gridStride;
      const uint32_t* pRecord = m_grid.data() + offsetY + offsetX;
      return ReadOnlySpan<uint32_t>(pRecord + 1, *pRecord, OptimizationCheckFlag::NoCheck);
    }
  };

}

#endif
