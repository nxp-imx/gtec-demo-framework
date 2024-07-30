#ifndef SHARED_SPATIALHASHGRID2D_BASICSPATIALHASHGRID2D_HPP
#define SHARED_SPATIALHASHGRID2D_BASICSPATIALHASHGRID2D_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslSimpleUI/App/UIDemoAppExtension.hpp>
#include <FslSimpleUI/Base/Layout/Layout.hpp>
#include <vector>

namespace Fsl
{
  template <uint32_t TMaxEntriesPerChunk>
  class BasicSpatialHashGrid2D
  {
  public:
    struct Record
    {
      uint32_t Id{0};
    };

  private:
    std::vector<Record> m_grid;
    uint16_t m_gridCellCountX{};
    uint16_t m_gridCellCountY{};
    uint8_t m_shiftX{};
    uint8_t m_shiftY{};

  public:
    BasicSpatialHashGrid2D() = default;

    BasicSpatialHashGrid2D(const uint16_t gridCellCountX, const uint16_t gridCellCountY, const uint8_t shiftX, const uint8_t shiftY)
      : m_grid(static_cast<std::size_t>(gridCellCountX) * gridCellCountY * TMaxEntriesPerChunk)
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

    void Clear()
    {
      for (std::size_t i = 0; i < m_grid.size(); i += TMaxEntriesPerChunk)
      {
        m_grid[i].Id = 0;
      }
    }

    int32_t GetCellCountX() const
    {
      return UncheckedNumericCast<int32_t>(m_gridCellCountX);
    }

    int32_t GetCellCountY() const
    {
      return UncheckedNumericCast<int32_t>(m_gridCellCountY);
    }

    int32_t ToXCell(const int32_t position) const
    {
      return (position >> m_shiftX);
    }

    int32_t ToYCell(const int32_t position) const
    {
      return (position >> m_shiftY);
    }

    bool TryAdd(const Rectangle2D& rect, const uint32_t id)
    {
      int32_t startCellX = (rect.Left() >> m_shiftX);
      int32_t endCellX = (rect.Right() >> m_shiftX) + 1;
      int32_t startCellY = (rect.Top() >> m_shiftY);
      int32_t endCellY = (rect.Bottom() >> m_shiftY) + 1;

      if (startCellX < m_gridCellCountX && endCellX > 0 && startCellY < m_gridCellCountY && endCellY > 0)
      {
        startCellX = startCellX >= 0 ? startCellX : 0;
        startCellY = startCellY >= 0 ? startCellY : 0;
        endCellX = endCellX <= m_gridCellCountX ? endCellX : m_gridCellCountX;
        endCellY = endCellY <= m_gridCellCountY ? endCellY : m_gridCellCountY;
        assert(startCellX >= 0);
        assert(startCellY >= 0);
        assert(startCellX <= endCellX);
        assert(startCellY <= endCellY);

        const constexpr uint32_t ChunkSizeX = TMaxEntriesPerChunk;
        const uint32_t gridStride = (m_gridCellCountX * ChunkSizeX);
        const uint32_t startOffsetY = static_cast<uint32_t>(startCellY) * gridStride;
        const uint32_t endOffsetY = static_cast<uint32_t>(endCellY) * gridStride;
        const uint32_t startOffsetX = static_cast<uint32_t>(startCellX) * ChunkSizeX;
        const uint32_t endOffsetX = static_cast<uint32_t>(endCellX) * ChunkSizeX;

        for (uint32_t y = startOffsetY; y < endOffsetY; y += gridStride)
        {
          for (uint32_t x = startOffsetX; x < endOffsetX; x += ChunkSizeX)
          {
            // The first entry in the main chunk is the count
            uint32_t& rEntryCount = m_grid[y + x].Id;
            // We foolishly assume that there will be no overflow (and with this controlled data we are actually sure)
            assert(rEntryCount <= 0xFFFFFFFE);
            ++rEntryCount;
            if (rEntryCount < ChunkSizeX)
            {
              assert((x + y + rEntryCount) <= m_grid.size());
              m_grid[y + x + rEntryCount].Id = id;
            }
            else
            {
              // Nasty, quick exit if chunk was full
              // This really ought to at least cleanup what was inserted, but for the demo purpose this should never happen
              return false;
            }
          }
        }
      }
      return true;
    }

    ReadOnlySpan<Record> GetChunkEntries(const uint32_t chunkX, const uint32_t chunkY) const
    {
      const uint32_t offsetX = chunkX * TMaxEntriesPerChunk;
      const uint32_t offsetY = chunkY * (m_gridCellCountX * TMaxEntriesPerChunk);
      const Record* pRecord = m_grid.data() + offsetY + offsetX;
      return ReadOnlySpan<Record>(pRecord + 1, pRecord[0].Id);
    }
  };

}

#endif
