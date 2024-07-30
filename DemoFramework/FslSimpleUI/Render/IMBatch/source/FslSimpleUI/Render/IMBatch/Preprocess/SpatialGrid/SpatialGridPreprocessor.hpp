#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALGRIDPREPROCESSOR_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALGRIDPREPROCESSOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2022-2023 NXP
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
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <FslSimpleUI/Render/IMBatch/DrawReorderMethod.hpp>
#include <unordered_map>
#include <vector>
#include "../../MaterialStats.hpp"
#include "../../MeshManager.hpp"
#include "../Linear/MaterialCache.hpp"
#include "../Linear/PreprocessUtil2_ForceTransparent.hpp"
#include "../Linear/PreprocessUtil2_TwoQueues.hpp"
#include "../PreprocessResult.hpp"
#include "../ProcessedCommandRecord.hpp"
#include "SpatialHashGrid2D.hpp"

namespace Fsl::UI::RenderIMBatch
{

  class SpatialGridPreprocessor
  {
    SpatialHashGrid2D m_grid;
    MaterialCache m_cache;
    PxSize2D m_windowSizePx;
    std::vector<ProcessedCommandRecord> m_finalEntries;
    uint32_t m_finalOpaqueCount{0};
    uint32_t m_finalTransparentCount{0};
    bool m_allowDepthBuffer{false};


    // static SpatialHashGrid2D CreateGrid(const PxSize2D windowSizePx, const int32_t cellsWidthPx, const int32_t cellsHeightPx)
    //{
    //   const uint32_t desiredStepSizeX = BitsUtil::NextPowerOfTwo(cellsWidthPx);
    //   const uint32_t desiredStepSizeY = BitsUtil::NextPowerOfTwo(cellsHeightPx);
    //   const uint32_t shiftX = BitsUtil::IndexOf(desiredStepSizeX);
    //   const uint32_t shiftY = BitsUtil::IndexOf(desiredStepSizeY);
    //   const uint32_t stepsX = (windowSizePx.Width() / desiredStepSizeX) + ((windowSizePx.Width() % desiredStepSizeX) > 0 ? 1 : 0);
    //   const uint32_t stepsY = (windowSizePx.Height() / desiredStepSizeY) + ((windowSizePx.Height() % desiredStepSizeY) > 0 ? 1 : 0);
    //   return SpatialHashGrid2D(UncheckedNumericCast<uint16_t>(stepsX), UncheckedNumericCast<uint16_t>(stepsY), shiftX, shiftY);
    // }
    SpatialHashGrid2D CreateGrid(const PxSize2D windowSizePx, const int32_t cellsX, const int32_t cellsY)
    {
      FSLLOG3_VERBOSE5("Width:{} Height:{} cellsX:{} cellsY:{}", windowSizePx.RawWidth(), windowSizePx.RawHeight(), cellsX, cellsY);
      const uint32_t desiredStepSizeX = BitsUtil::NextPowerOfTwo(windowSizePx.RawWidth() / cellsX);
      const uint32_t desiredStepSizeY = BitsUtil::NextPowerOfTwo(windowSizePx.RawHeight() / cellsY);
      const uint32_t shiftX = BitsUtil::IndexOf(desiredStepSizeX);
      const uint32_t shiftY = BitsUtil::IndexOf(desiredStepSizeY);
      const uint32_t stepsX = (windowSizePx.RawWidth() / desiredStepSizeX) + ((windowSizePx.RawWidth() % desiredStepSizeX) > 0 ? 1 : 0);
      const uint32_t stepsY = (windowSizePx.RawHeight() / desiredStepSizeY) + ((windowSizePx.RawHeight() % desiredStepSizeY) > 0 ? 1 : 0);

      return {UncheckedNumericCast<uint16_t>(stepsX), UncheckedNumericCast<uint16_t>(stepsY), UncheckedNumericCast<uint8_t>(shiftX),
              UncheckedNumericCast<uint8_t>(shiftY)};
    }

  public:
    // 0  1  2  3  4   5   6   7    8
    // 1, 2, 4, 8, 16, 32, 64, 128, 256
    explicit SpatialGridPreprocessor(const PxSize2D windowSizePx, const bool allowDepthBuffer)
      //: m_grid(1, 1, 16, 16)
      // : m_grid(4, 4, 9, 9)
      //: m_grid(8, 8, 8, 8)
      //: m_grid(16, 16, 7, 7)
      //: m_grid(32, 32, 6, 6)
      : m_grid(CreateGrid(windowSizePx, 8, 8))
      , m_windowSizePx(windowSizePx)
      , m_allowDepthBuffer(allowDepthBuffer)
    {
    }

    void OnConfigurationChanged(const PxSize2D windowSizePx)
    {
      m_windowSizePx = windowSizePx;
      m_grid = CreateGrid(windowSizePx, 8, 8);
    }

    void SetAllowDepthBuffer(const bool allowDepthBuffer)
    {
      m_allowDepthBuffer = allowDepthBuffer;
    }

    void Process(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords, ReadOnlySpan<EncodedCommand> commandSpan,
                 const MeshManager& meshManager)
    {
      if (commandSpan.empty())
      {
        m_finalOpaqueCount = 0;
        m_finalTransparentCount = 0;
        return;
      }

      // Ensure that we have enough space in the material cache
      const uint32_t currentMaterialCount = meshManager.GetMaterialLookup().GetCount();
      m_cache.EnsureCapacity(currentMaterialCount);
      Span<MaterialCacheRecord> opaqueMaterialCache = m_cache.GetOpaqueCacheSpan(currentMaterialCount);
      Span<MaterialCacheRecord> transparentMaterialCache = m_cache.GetTransparentCacheSpan(currentMaterialCount);


      PreprocessResult result = m_allowDepthBuffer
                                  ? PreprocessUtil2::PreprocessTwoQueues(rProcessedCommandRecords, opaqueMaterialCache, transparentMaterialCache,
                                                                         commandSpan, meshManager, m_windowSizePx)
                                  : PreprocessUtil2::PreprocessForceTransparent(rProcessedCommandRecords, opaqueMaterialCache,
                                                                                transparentMaterialCache, commandSpan, meshManager, m_windowSizePx);

      const uint32_t totalCount = result.OpaqueCount + result.TransparentCount;
      if (totalCount > m_finalEntries.size())
      {
        m_finalEntries.resize(static_cast<std::size_t>(totalCount) + PreprocessConfig::ProcessedGrowBy);
      }

      if (result.OpaqueCount > 0u)
      {
        Reorder(opaqueMaterialCache, SpanUtil::UncheckedAsSpan(m_finalEntries, 0u, result.OpaqueCount),
                SpanUtil::UncheckedAsReadOnlySpan(rProcessedCommandRecords, result.OpaqueStartIndex, result.OpaqueCount));
      }
      if (result.TransparentCount > 0u)
      {
        Reorder(transparentMaterialCache, SpanUtil::UncheckedAsSpan(m_finalEntries, result.OpaqueCount, result.TransparentCount),
                SpanUtil::UncheckedAsReadOnlySpan(rProcessedCommandRecords, result.TransparentStartIndex, result.TransparentCount));
      }
      m_finalOpaqueCount = result.OpaqueCount;
      m_finalTransparentCount = result.TransparentCount;
    }

    inline ReadOnlySpan<ProcessedCommandRecord> GetOpaqueSpan(std::vector<ProcessedCommandRecord>& /*rProcessedCommandRecords*/) const noexcept
    {
      return SpanUtil::UncheckedAsReadOnlySpan(m_finalEntries, 0u, m_finalOpaqueCount);
    }

    inline ReadOnlySpan<ProcessedCommandRecord> GetTransparentSpan(std::vector<ProcessedCommandRecord>& /*rProcessedCommandRecords*/) const noexcept
    {
      return SpanUtil::UncheckedAsReadOnlySpan(m_finalEntries, m_finalOpaqueCount, m_finalTransparentCount);
    }

  private:
    // static void SanityCheck(const SpatialHashGrid2D& grid, Span<ProcessedCommandRecord> dstSpan, ReadOnlySpan<ProcessedCommandRecord> srcSpan,
    //                         const uint32_t count)
    //{
    //   for (uint32_t i = 0; i < count; ++i)
    //   {
    //     const auto originalCommandIndex = dstSpan[i].OriginalCommandIndex;
    //     const ProcessedCommandRecord& rCurrent = srcSpan[originalCommandIndex];
    //     grid.SanityCheckEntry(rCurrent.DstAreaRectanglePxf, originalCommandIndex);

    //    if (i != grid.ToRemappedZPos(originalCommandIndex))
    //      throw InternalErrorException("remapping out of sync");
    //  }
    //}


    //! @brief Reorder
    //! @param materialCache A initialized material cache that will point to the first index of the material (or the invalid material id)
    //! @param dstSpan
    //! @param srcSpan
    void Reorder(Span<MaterialCacheRecord> materialCache, Span<ProcessedCommandRecord> dstSpan, ReadOnlySpan<ProcessedCommandRecord> srcSpan)
    {
      m_grid.Clear();
      assert(!srcSpan.empty());

      BatchMaterialId previousMaterialId = srcSpan[0].MaterialId;
      const auto count = UncheckedNumericCast<uint32_t>(srcSpan.size());
      assert(BatchMaterialIdConfig::Invalid >= count);
      materialCache[srcSpan[0].MaterialId.Value].Index = 0;
      dstSpan[0].OriginalCommandIndex = 0;
      m_grid.UncheckedAdd(srcSpan[0].DstAreaRectanglePxf, 0);

      const auto clipWidthPxf = static_cast<float>(m_windowSizePx.RawWidth());
      const auto clipHeightPxf = static_cast<float>(m_windowSizePx.RawHeight());

      for (uint32_t i = 1; i < count; ++i)
      {
        // SanityCheck(m_grid, dstSpan, srcSpan, i);

        // Here we always read the next entry in the src span (as it has not been reordered yet)
        const ProcessedCommandRecord& src = srcSpan[i];
        if (src.MaterialId != previousMaterialId)
        {
          assert(i > 0);
          uint32_t targetIndex = static_cast<int32_t>(i) - 1;
          const uint32_t lastKnownMaterialIndex = materialCache[src.MaterialId.Value].Index;
          if (lastKnownMaterialIndex < targetIndex)
          {
            float clippedDstRawL = src.DstAreaRectanglePxf.RawLeft();
            float clippedDstRawR = src.DstAreaRectanglePxf.RawRight();
            float clippedDstRawT = src.DstAreaRectanglePxf.RawTop();
            float clippedDstRawB = src.DstAreaRectanglePxf.RawBottom();
            if (clippedDstRawL < 0)
            {
              clippedDstRawL = 0;
            }
            if (clippedDstRawR >= clipWidthPxf)
            {
              clippedDstRawR = clipWidthPxf;
            }
            if (clippedDstRawT < 0)
            {
              clippedDstRawT = 0;
            }
            if (clippedDstRawB >= clipHeightPxf)
            {
              clippedDstRawB = clipHeightPxf;
            }

            {
              // We expect that all fully outside bounds elements have been removed
              assert(clippedDstRawL < clippedDstRawR && clippedDstRawT < clippedDstRawB);
              // The previous material did not match and we have a previous material entry, so we check all collision candidates to
              // see if there is a collision
              auto rangeX = m_grid.ToXCell(clippedDstRawL, clippedDstRawR);
              auto rangeY = m_grid.ToYCell(clippedDstRawT, clippedDstRawB);
              bool collision = false;
              for (uint16_t gridY = rangeY.Start; gridY < rangeY.End; ++gridY)
              {
                for (uint16_t gridX = rangeX.Start; gridX < rangeX.End; ++gridX)
                {
                  ReadOnlySpan<uint32_t> candidates = m_grid.UncheckedGetChunkEntries(gridX, gridY);
                  for (std::size_t candidateIndex = candidates.size(); candidateIndex > 0; --candidateIndex)
                  {
                    const uint32_t srcIndex = candidates[candidateIndex - 1];
                    const uint32_t remappedSrcIndex = m_grid.ToRemappedZPos(srcIndex);
                    if (remappedSrcIndex <= lastKnownMaterialIndex)
                    {
                      assert((remappedSrcIndex != lastKnownMaterialIndex) ||
                             (remappedSrcIndex == lastKnownMaterialIndex && src.MaterialId == srcSpan[srcIndex].MaterialId));
                      break;
                    }
                    if (src.DstAreaRectanglePxf.Intersects(srcSpan[srcIndex].DstAreaRectanglePxf))
                    {
                      collision = true;
                      goto on_collission_exit;
                    }
                  }
                }
              }
            on_collission_exit:
              // lastKnownMaterialIndex is based on the remapped cached index (so the target index is also the remapped index)
              targetIndex = !collision ? lastKnownMaterialIndex : 0xFFFFFFFF;
            }
          }
          // Here we are interested in using the 're-ordered' elements while we backtrack, so we need to lookup the original index
          // to get access to the src record
          if (targetIndex == lastKnownMaterialIndex)
          {    // We found a previous entry with the same material and the draw commands do not overlap, so we can reorder the draw calls
               // without issues
            // SanityCheck(m_grid, dstSpan, srcSpan, i);
            assert(targetIndex < i);
            assert(srcSpan[dstSpan[targetIndex].OriginalCommandIndex].MaterialId == src.MaterialId);
            assert(targetIndex == lastKnownMaterialIndex);
            // assert(targetIndex == static_cast<uint32_t<(compareTargetIndex));

            assert(static_cast<uint32_t>(targetIndex + 1) < i);
            // If this fires the cache is no longer in sync with what we found
            assert(materialCache[srcSpan[dstSpan[targetIndex].OriginalCommandIndex].MaterialId.Value].Index == targetIndex);

            // We need to insert this draw operation just after the target
            const auto insertAtIndex = UncheckedNumericCast<uint32_t>(targetIndex + 1);

            m_grid.TryInsertAfterZPos(targetIndex, src.DstAreaRectanglePxf, i);
            auto lookupSpan = m_grid.LookupSpan();
            for (uint32_t moveIndex = i; moveIndex > insertAtIndex; --moveIndex)
            {
              const ProcessedCommandRecord& recordToMove = dstSpan[moveIndex - 1];
              dstSpan[moveIndex].OriginalCommandIndex = recordToMove.OriginalCommandIndex;
              // Remap the grid lookup table
              lookupSpan[recordToMove.OriginalCommandIndex] = moveIndex;

              // Remap the material cache index
              MaterialCacheRecord& rMaterialCacheEntry = materialCache[srcSpan[recordToMove.OriginalCommandIndex].MaterialId.Value];
              if (rMaterialCacheEntry.Index < moveIndex)
              {
                rMaterialCacheEntry.Index = moveIndex;
              }
            }
            materialCache[src.MaterialId.Value].Index = insertAtIndex;
            dstSpan[insertAtIndex].OriginalCommandIndex = i;
            lookupSpan[dstSpan[insertAtIndex].OriginalCommandIndex] = insertAtIndex;

            previousMaterialId = srcSpan[dstSpan[i].OriginalCommandIndex].MaterialId;
            // SanityCheck(m_grid, dstSpan, srcSpan, i + 1);
          }
          else
          {
            // SanityCheck(m_grid, dstSpan, srcSpan, i);
            materialCache[src.MaterialId.Value].Index = i;
            dstSpan[i].OriginalCommandIndex = i;
            m_grid.UncheckedAdd(src.DstAreaRectanglePxf, i);
            previousMaterialId = src.MaterialId;
            // SanityCheck(m_grid, dstSpan, srcSpan, i + 1);
          }
        }
        else
        {
          // SanityCheck(m_grid, dstSpan, srcSpan, i);
          materialCache[src.MaterialId.Value].Index = i;
          dstSpan[i].OriginalCommandIndex = i;
          m_grid.UncheckedAdd(src.DstAreaRectanglePxf, i);
          // SanityCheck(m_grid, dstSpan, srcSpan, i + 1);
        }
      }

      // Finally we write everything in the correct order
      for (uint32_t i = 0; i < count; ++i)
      {
        dstSpan[i] = srcSpan[dstSpan[i].OriginalCommandIndex];
      }
    }
  };
}
#endif
