#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALGRIDPREPROCESSOR_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_SPATIALGRID_SPATIALGRIDPREPROCESSOR_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <vector>
#include "../../MeshManager.hpp"
#include "../PreprocessResult.hpp"
#include "../PreprocessUtil.hpp"
#include "../ProcessedCommandRecord.hpp"
#include "../Basic/ReorderUtil_Linear.hpp"
#include "SpatialHashGrid2D.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace RenderIMBatch
    {
      class SpatialGridPreprocessor
      {
        SpatialHashGrid2D<8, 8, 32> m_grid;
        PreprocessResult m_result;

      public:
        explicit SpatialGridPreprocessor(const PxSize2D sizePx)
          : m_grid(sizePx)
        {
        }


        inline void Process(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords, ReadOnlySpan<EncodedCommand> commandSpan,
                            const MeshManager& meshManager)
        {
          assert(!commandSpan.empty());
          const std::size_t capacity = commandSpan.size();
          {
            if (capacity > rProcessedCommandRecords.size())
            {
              rProcessedCommandRecords.resize(capacity + PreprocessConfig::ProcessedGrowBy);
            }
          }

          const auto count = UncheckedNumericCast<uint32_t>(commandSpan.size());
          assert(count > 0u);

          ProcessedCommandRecord* const pDst = rProcessedCommandRecords.data();
          uint32_t dstTransparentIndex = 0;

          for (uint32_t i = 0; i < count; ++i)
          {
            const EncodedCommand& command = commandSpan[i];
            // The queue should never contain a Nop command
            assert(command.Type != DrawCommandType::Nop);
            const uint32_t hMesh = HandleCoding::GetOriginalHandle(command.Mesh);

            switch (HandleCoding::GetType(command.Mesh))
            {
            case RenderDrawSpriteType::Dummy:
              break;
            case RenderDrawSpriteType::BasicImageSprite:
            {
              const auto& meshRecord = meshManager.FastGetBasicImageSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.MaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.MaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            case RenderDrawSpriteType::BasicNineSliceSprite:
            {
              const auto& meshRecord = meshManager.FastGetBasicNineSliceSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.MaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.MaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            // case RenderDrawSpriteType::BasicOptimizedNineSliceSprite:
            //  currentMaterialHandle = meshManager.FastGet(hMesh).MaterialHandle;
            //  break;
            case RenderDrawSpriteType::ImageSprite:
            {
              const auto& meshRecord = meshManager.FastGetImageSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.MaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.MaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            case RenderDrawSpriteType::NineSliceSprite:
            {
              const auto& meshRecord = meshManager.FastGetNineSliceSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.MaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.MaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            case RenderDrawSpriteType::SpriteFont:
            {
              const auto& meshRecord = meshManager.FastGetSpriteFont(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.MaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.MaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            case RenderDrawSpriteType::OptimizedNineSliceSprite:
            {
              const auto& meshRecord = meshManager.FastGetOptimizedNineSliceSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex].MaterialHandle = meshRecord.TransparentMaterialHandle;
              pDst[dstTransparentIndex].OriginalCommandIndex = i;
              ++dstTransparentIndex;
              if (!m_grid.TryAdd(PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y, static_cast<float>(command.DstSizePx.Width()),
                                                  static_cast<float>(command.DstSizePx.Height()), OptimizationCheckFlag::NoCheck),
                                 i, meshRecord.TransparentMaterialHandle.Value))
              {
                FSLLOG3_ERROR("No room");
              }
              break;
            }
            default:
              FSLLOG3_WARNING("Unsupported type: {}", HandleCoding::GetType(command.Mesh));
              break;
            }
          }
          assert(dstTransparentIndex <= capacity);
          m_result = PreprocessResult{0u, 0u, 0u, dstTransparentIndex};
        }

        inline Span<ProcessedCommandRecord> GetOpaqueSpan(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords) const
        {
          return SpanUtil::AsSubSpan(rProcessedCommandRecords, m_result.OpaqueStartIndex, m_result.OpaqueCount);
        }

        inline Span<ProcessedCommandRecord> GetTransparentSpan(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords) const
        {
          return SpanUtil::AsSubSpan(rProcessedCommandRecords, m_result.TransparentStartIndex, m_result.TransparentCount);
        }
      };
    }
  }
}
#endif
#endif
