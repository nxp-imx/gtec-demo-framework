#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_LINEAR_LINEARPREPROCESSOR_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_LINEAR_LINEARPREPROCESSOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2023 NXP
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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <FslSimpleUI/Render/IMBatch/DrawReorderMethod.hpp>
#include <vector>
#include "../../MaterialStats.hpp"
#include "../../MeshManager.hpp"
#include "../PreprocessResult.hpp"
#include "../ProcessedCommandRecord.hpp"
#include "MaterialCache.hpp"
#include "PreprocessUtil2_ForceTransparent.hpp"
#include "PreprocessUtil2_TwoQueues.hpp"

namespace Fsl::UI::RenderIMBatch
{
  class LinearPreprocessor
  {
    MaterialCache m_cache;
    std::vector<ProcessedCommandRecord> m_finalEntries;
    uint32_t m_finalOpaqueCount{0};
    uint32_t m_finalTransparentCount{0};
    uint16_t m_maxBacktracking{32};
    bool m_allowDepthBuffer{false};
    PxSize2D m_windowSizePx;

  public:
    explicit LinearPreprocessor(const bool allowDepthBuffer, const PxSize2D windowSizePx)
      : m_allowDepthBuffer(allowDepthBuffer)
      , m_windowSizePx(windowSizePx)
    {
    }

    void OnConfigurationChanged(const PxSize2D windowSizePx)
    {
      m_windowSizePx = windowSizePx;
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

      // FSLLOG3_INFO("commandSpan:{} Opaque:{} Transparent:{} totalCount:{}", commandSpan.size(), result.OpaqueCount, result.TransparentCount,
      //              totalCount);

      if (totalCount > m_finalEntries.size())
      {
        m_finalEntries.resize(static_cast<std::size_t>(totalCount) + PreprocessConfig::ProcessedGrowBy);
      }

      if (result.OpaqueCount > 0u)
      {
        Reorder(opaqueMaterialCache, SpanUtil::UncheckedAsSpan(m_finalEntries, 0u, result.OpaqueCount),
                SpanUtil::UncheckedAsReadOnlySpan(rProcessedCommandRecords, result.OpaqueStartIndex, result.OpaqueCount), m_maxBacktracking);
      }
      if (result.TransparentCount > 0u)
      {
        Reorder(transparentMaterialCache, SpanUtil::UncheckedAsSpan(m_finalEntries, result.OpaqueCount, result.TransparentCount),
                SpanUtil::UncheckedAsReadOnlySpan(rProcessedCommandRecords, result.TransparentStartIndex, result.TransparentCount),
                m_maxBacktracking);
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
    //! @brief Reorder
    //! @param materialCache A initialized material cache that will point to the first index of the material (or the invalid material id)
    //! @param dstSpan
    //! @param srcSpan
    //! @param maxBacktracking
    static void Reorder(Span<MaterialCacheRecord> materialCache, Span<ProcessedCommandRecord> dstSpan, ReadOnlySpan<ProcessedCommandRecord> srcSpan,
                        const uint16_t maxBacktracking) noexcept
    {
      assert(!srcSpan.empty());

      BatchMaterialId previousMaterialId = srcSpan[0].MaterialId;
      const auto count = UncheckedNumericCast<uint32_t>(srcSpan.size());
      assert(BatchMaterialIdConfig::Invalid >= count);
      materialCache[srcSpan[0].MaterialId.Value].Index = 0;
      dstSpan[0].OriginalCommandIndex = 0;
      for (uint32_t i = 1; i < count; ++i)
      {
        // Here we always read the next entry in the src span (as it has not been reordered yet)
        const ProcessedCommandRecord& src = srcSpan[i];
        if (src.MaterialId != previousMaterialId)
        {
          // The previous material did not match and we have a previous entry in the array, so try to backtrack until we locate a matching
          // material or a overlapping entry. This will only backtrack for "maxBacktracking" entries before giving up on the reordering
          int32_t targetIndex = static_cast<int32_t>(i) - 1;
          const uint32_t lastKnownMaterialIndex = materialCache[src.MaterialId.Value].Index;
          if (lastKnownMaterialIndex < static_cast<uint32_t>(targetIndex) &&
              static_cast<int32_t>(static_cast<uint32_t>(targetIndex) - lastKnownMaterialIndex) <= maxBacktracking)
          {
            const int32_t targetIndexSrcEnd = std::max(targetIndex - maxBacktracking, 0);
            while (targetIndex >= targetIndexSrcEnd)
            {
              // We use the dstSpan[x].OriginalCommandIndex as a temporary to store the original srcSpan index while reordering
              // Here we are interested in using the 're-ordered' elements while we backtrack, so we need to lookup the original index
              // to get access to the src record
              const ProcessedCommandRecord& rPrevious = srcSpan[dstSpan[targetIndex].OriginalCommandIndex];
              assert(static_cast<int32_t>(materialCache[rPrevious.MaterialId.Value].Index) >= targetIndex);

              if (src.MaterialId != rPrevious.MaterialId && !src.DstAreaRectanglePxf.Intersects(rPrevious.DstAreaRectanglePxf))
              {
                --targetIndex;
              }
              else
              {
                break;
              }
            }
          }

          // Here we are interested in using the 're-ordered' elements while we backtrack, so we need to lookup the original index
          // to get access to the src record
          assert(targetIndex < static_cast<int32_t>(i));
          if (targetIndex >= 0 && static_cast<uint32_t>(targetIndex) == lastKnownMaterialIndex)
          {    // We found a previous entry with the same material and the draw commands do not overlap, so we can reorder the draw calls
               // without issues
            assert(srcSpan[dstSpan[targetIndex].OriginalCommandIndex].MaterialId == src.MaterialId);

            assert(static_cast<uint32_t>(targetIndex + 1) < i);
            // If this fires the cache is no longer in sync with what we found
            assert(static_cast<int32_t>(materialCache[srcSpan[dstSpan[targetIndex].OriginalCommandIndex].MaterialId.Value].Index) == targetIndex);

            // We need to insert this draw operation just after the target
            const auto insertAtIndex = UncheckedNumericCast<uint32_t>(targetIndex + 1);

            for (uint32_t moveIndex = i; moveIndex > insertAtIndex; --moveIndex)
            {
              const ProcessedCommandRecord& recordToMove = dstSpan[moveIndex - 1];
              dstSpan[moveIndex].OriginalCommandIndex = recordToMove.OriginalCommandIndex;
              // Remap the material cache index
              MaterialCacheRecord& rMaterialCacheEntry = materialCache[srcSpan[recordToMove.OriginalCommandIndex].MaterialId.Value];
              if (rMaterialCacheEntry.Index < moveIndex)
              {
                rMaterialCacheEntry.Index = moveIndex;
              }
            }
            dstSpan[insertAtIndex].OriginalCommandIndex = i;
            materialCache[src.MaterialId.Value].Index = insertAtIndex;
            previousMaterialId = srcSpan[dstSpan[i].OriginalCommandIndex].MaterialId;
          }
          else
          {
            materialCache[src.MaterialId.Value].Index = i;
            dstSpan[i].OriginalCommandIndex = i;
            previousMaterialId = src.MaterialId;
          }
        }
        else
        {
          materialCache[src.MaterialId.Value].Index = i;
          dstSpan[i].OriginalCommandIndex = i;
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
