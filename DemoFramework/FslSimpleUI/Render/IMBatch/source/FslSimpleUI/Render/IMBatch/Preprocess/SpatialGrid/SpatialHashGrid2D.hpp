#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALHASHGRID2D_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALHASHGRID2D_HPP
#if 0
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <vector>

namespace Fsl
{
  template <uint16_t TShiftX, uint16_t TShiftY, uint32_t TMaxEntriesPerChunk>
  class SpatialHashGrid2D
  {
    struct GridCellsInfo
    {
      uint16_t CountX{};
      uint16_t CountY{};
    };

  public:
    struct Record
    {
      uint32_t Id{0};
      uint32_t MaterialId{0};
      PxAreaRectangleF RectPxf;
    };

  private:
    GridCellsInfo m_info;
    std::vector<Record> m_grid;

  public:
    SpatialHashGrid2D() = default;

    SpatialHashGrid2D(const PxSize2D sizePx)
      : m_info(CalcInfo(sizePx))
      , m_grid(std::size_t(m_info.CountX) * m_info.CountY * TMaxEntriesPerChunk)
    {
      if (m_info.CountX <= 0)
      {
        throw std::invalid_argument("m_info.CountX must be > 0");
      }
      if (m_info.CountY <= 0)
      {
        throw std::invalid_argument("m_info.CountY must be > 0");
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
      return UncheckedNumericCast<int32_t>(m_info.CountX);
    }

    int32_t GetCellCountY() const
    {
      return UncheckedNumericCast<int32_t>(m_info.CountY);
    }

    int32_t ToXCell(const int32_t position) const
    {
      return (position >> TShiftX);
    }

    int32_t ToYCell(const int32_t position) const
    {
      return (position >> TShiftY);
    }

    enum class AddResult
    {
      NoCollissions,
      StartNewBatch,
      Merge,
    };

    bool TryAdd(const PxAreaRectangleF& rect, const uint32_t id, const uint32_t materialId)
    {
      int32_t startCellX = (static_cast<int32_t>(rect.Left()) >> TShiftX);
      int32_t endCellX = (static_cast<int32_t>(rect.Right()) >> TShiftY) + 1;
      int32_t startCellY = (static_cast<int32_t>(rect.Top()) >> TShiftX);
      int32_t endCellY = (static_cast<int32_t>(rect.Bottom()) >> TShiftY) + 1;

      if (startCellX < m_info.CountX && endCellX > 0 && startCellY < m_info.CountY && endCellY > 0)
      {
        startCellX = startCellX >= 0 ? startCellX : 0;
        startCellY = startCellY >= 0 ? startCellY : 0;
        endCellX = endCellX <= m_info.CountX ? endCellX : m_info.CountX;
        endCellY = endCellY <= m_info.CountY ? endCellY : m_info.CountY;
        assert(startCellX >= 0);
        assert(startCellY >= 0);
        assert(startCellX <= endCellX);
        assert(startCellY <= endCellY);

        const constexpr uint32_t chunkSizeX = TMaxEntriesPerChunk;
        const uint32_t gridStride = (m_info.CountX * chunkSizeX);
        const uint32_t startOffsetY = static_cast<uint32_t>(startCellY) * gridStride;
        const uint32_t endOffsetY = static_cast<uint32_t>(endCellY) * gridStride;
        const uint32_t startOffsetX = static_cast<uint32_t>(startCellX) * chunkSizeX;
        const uint32_t endOffsetX = static_cast<uint32_t>(endCellX) * chunkSizeX;

        uint32_t foundId = 0;
        AddResult result = AddResult::NoCollissions;
        for (uint32_t y = startOffsetY; y < endOffsetY; y += gridStride)
        {
          for (uint32_t x = startOffsetX; x < endOffsetX; x += chunkSizeX)
          {
            // The first entry in the main chunk is the count
            uint32_t& rEntryCount = m_grid[std::size_t(y) + x].Id;

            // Check for collisions
            if (rEntryCount > 0 && result != AddResult::StartNewBatch)
            {
              const uint32_t count = rEntryCount;
              // travel backwards to the list (last inserted order)
              for (uint32_t entryIndex = count; entryIndex > 0; --entryIndex)
              {
                const Record& rSrc = m_grid[std::size_t(y) + x + entryIndex];
                if (rSrc.MaterialId == materialId)
                {
                  result = AddResult::Merge;
                  foundId = rSrc.Id > foundId ? rSrc.Id : foundId;
                }
                else if (rSrc.RectPxf.Intersects(rect))
                {
                  result = AddResult::StartNewBatch;
                  foundId = rSrc.Id > foundId ? rSrc.Id : foundId;
                  break;
                }
              }
            }

            ++rEntryCount;
            // We foolishly assume that there will be no overflow (and with this controlled data we are actually sure)
            assert(rEntryCount >= 0u);
            if (rEntryCount < chunkSizeX)
            {
              assert((std::size_t(y) + x + rEntryCount) <= m_grid.size());
              m_grid[std::size_t(y) + x + rEntryCount] = Record{id, materialId, rect};
            }
            else
            {
              // Nasty, quick exit if chunk was full
              // This really ought to at least cleanup what was inserted, but for the demo purpose this should never happen
              return false;
            }
          }
        }
        switch (result)
        {
        case AddResult::Merge:
          // FSLLOG3_INFO("Merge");
          break;
        case AddResult::NoCollissions:
          break;
        case AddResult::StartNewBatch:
          // FSLLOG3_INFO("Start");
          break;
        default:
          break;
        }
      }
      return true;
    }

    ReadOnlySpan<Record> GetChunkEntries(const uint32_t chunkX, const uint32_t chunkY) const
    {
      const uint32_t offsetX = chunkX * TMaxEntriesPerChunk;
      const uint32_t offsetY = chunkY * (m_info.CountX * TMaxEntriesPerChunk);
      const Record* pRecord = m_grid.data() + offsetY + offsetX;
      return ReadOnlySpan<Record>(pRecord + 1, pRecord[0].Id);
    }

    constexpr static GridCellsInfo CalcInfo(const PxSize2D size)
    {
      constexpr uint32_t cellSizeX = 1u << TShiftX;
      constexpr uint32_t cellSizeY = 1u << TShiftY;

      uint16_t cellCountX = (size.Width() / cellSizeX) + ((size.Width() % cellSizeX) > 0 ? 1 : 0);
      uint16_t cellCountY = (size.Height() / cellSizeY) + ((size.Height() % cellSizeY) > 0 ? 1 : 0);
      return {cellCountX, cellCountY};
    }
  };

}
#endif
#endif
