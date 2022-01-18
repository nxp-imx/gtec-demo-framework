#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_BASIC_PREPROCESSUTIL_FORCETRANSPARENT_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_BASIC_PREPROCESSUTIL_FORCETRANSPARENT_HPP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <vector>
#include "../../HandleCoding.hpp"
#include "../../Log/FmtRenderDrawSpriteType.hpp"
#include "../../MeshManager.hpp"
#include "../PreprocessConfig.hpp"
#include "../PreprocessResult.hpp"
#include "../ProcessedCommandRecord.hpp"

namespace Fsl
{
  namespace UI
  {
    namespace RenderIMBatch
    {
      namespace PreprocessUtil
      {
        // Pre-process the draw commands
        // - Split multi material sprites so each queue gets one entry for it, thereby making it appear as a single material for the rest of the
        //   drawing system
        // - Remove dummy entries
        inline PreprocessResult PreprocessForceTransparent(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords,
                                                           ReadOnlySpan<EncodedCommand> commandSpan, const MeshManager& meshManager)
        {
          assert(!commandSpan.empty());
          const MaterialLookup& materialLookup = meshManager.GetMaterialLookup();

          const std::size_t capacity = (commandSpan.size() * 2u);
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
              const auto& meshRecord = meshManager.UncheckedGetBasicImageSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex] = ProcessedCommandRecord(materialLookup.GetMaterialIndex(meshRecord.MaterialHandle),
                                                                 PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y,
                                                                                  static_cast<float>(command.DstSizePx.Width()),
                                                                                  static_cast<float>(command.DstSizePx.Height())),
                                                                 command.DstColor, dstTransparentIndex, i);
              ++dstTransparentIndex;
              break;
            }
            case RenderDrawSpriteType::BasicNineSliceSprite:
            {
              const auto& meshRecord = meshManager.UncheckedGetBasicNineSliceSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex] = ProcessedCommandRecord(materialLookup.GetMaterialIndex(meshRecord.MaterialHandle),
                                                                 PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y,
                                                                                  static_cast<float>(command.DstSizePx.Width()),
                                                                                  static_cast<float>(command.DstSizePx.Height())),
                                                                 command.DstColor, dstTransparentIndex, i);
              ++dstTransparentIndex;
              break;
            }
            // case RenderDrawSpriteType::BasicOptimizedNineSliceSprite:
            //  currentMaterialHandle = meshManager.FastGet(hMesh).MaterialHandle;
            //  break;
            case RenderDrawSpriteType::ImageSprite:
            {
              const auto& meshRecord = meshManager.UncheckedGetImageSprite(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex] = ProcessedCommandRecord(materialLookup.GetMaterialIndex(meshRecord.MaterialHandle),
                                                                 PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y,
                                                                                  static_cast<float>(command.DstSizePx.Width()),
                                                                                  static_cast<float>(command.DstSizePx.Height())),
                                                                 command.DstColor, dstTransparentIndex, i);
              ++dstTransparentIndex;
              break;
            }
            case RenderDrawSpriteType::NineSliceSprite:
            {
              const auto& meshRecord = meshManager.UncheckedGetNineSliceSprite(hMesh);
              assert(meshRecord.Sprite);
              const PxThicknessF& scaledImageTrimMarginPxf = meshRecord.Sprite->GetRenderInfo().ScaledTrimMarginPxf;
              const auto materialId = materialLookup.GetMaterialIndex(meshRecord.MaterialHandle);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex] =
                ProcessedCommandRecord(materialId,
                                       command.Type != DrawCommandType::DrawRot90CWAtOffsetAndSize
                                         ? PxAreaRectangleF(command.DstPositionPxf.X + scaledImageTrimMarginPxf.Left(),
                                                            command.DstPositionPxf.Y + scaledImageTrimMarginPxf.Top(),
                                                            static_cast<float>(command.DstSizePx.Width()) - scaledImageTrimMarginPxf.SumX(),
                                                            static_cast<float>(command.DstSizePx.Height()) - scaledImageTrimMarginPxf.SumY())
                                         : PxAreaRectangleF(command.DstPositionPxf.X + scaledImageTrimMarginPxf.Top(),
                                                            command.DstPositionPxf.Y + scaledImageTrimMarginPxf.Left(),
                                                            static_cast<float>(command.DstSizePx.Width()) - scaledImageTrimMarginPxf.SumY(),
                                                            static_cast<float>(command.DstSizePx.Height()) - scaledImageTrimMarginPxf.SumX()),
                                       command.DstColor, dstTransparentIndex, i);
              ++dstTransparentIndex;
              break;
            }
            case RenderDrawSpriteType::SpriteFont:
            {
              const auto& meshRecord = meshManager.UncheckedGetSpriteFont(hMesh);
              assert(dstTransparentIndex < capacity);
              pDst[dstTransparentIndex] = ProcessedCommandRecord(materialLookup.GetMaterialIndex(meshRecord.MaterialHandle),
                                                                 PxAreaRectangleF(command.DstPositionPxf.X, command.DstPositionPxf.Y,
                                                                                  static_cast<float>(command.DstSizePx.Width()),
                                                                                  static_cast<float>(command.DstSizePx.Height())),
                                                                 command.DstColor, dstTransparentIndex, i);
              ++dstTransparentIndex;
              break;
            }
            case RenderDrawSpriteType::OptimizedNineSliceSprite:
            {
              const auto& meshRecord = meshManager.UncheckedGetOptimizedNineSliceSprite(hMesh);
              assert(meshRecord.Transparency != MeshTransparencyFlags::NoFlags);
              assert(meshRecord.Sprite);
              const PxThicknessF& scaledImageTrimMarginPxf = meshRecord.Sprite->GetRenderInfo().ScaledTrimMarginPxf;
              const auto dstRectanglePxf = command.Type != DrawCommandType::DrawRot90CWAtOffsetAndSize
                                             ? PxAreaRectangleF(command.DstPositionPxf.X + scaledImageTrimMarginPxf.Left(),
                                                                command.DstPositionPxf.Y + scaledImageTrimMarginPxf.Top(),
                                                                static_cast<float>(command.DstSizePx.Width()) - scaledImageTrimMarginPxf.SumX(),
                                                                static_cast<float>(command.DstSizePx.Height()) - scaledImageTrimMarginPxf.SumY())
                                             : PxAreaRectangleF(command.DstPositionPxf.X + scaledImageTrimMarginPxf.Top(),
                                                                command.DstPositionPxf.Y + scaledImageTrimMarginPxf.Left(),
                                                                static_cast<float>(command.DstSizePx.Width()) - scaledImageTrimMarginPxf.SumY(),
                                                                static_cast<float>(command.DstSizePx.Height()) - scaledImageTrimMarginPxf.SumX());
              if (command.DstColor.A() == 0xFF)
              {
                if (MeshTransparencyFlagsUtil::IsEnabled(meshRecord.Transparency, MeshTransparencyFlags::Opaque))
                {
                  const auto materialId = materialLookup.GetMaterialIndex(meshRecord.OpaqueMaterialHandle);
                  assert(dstTransparentIndex < capacity);
                  pDst[dstTransparentIndex] = ProcessedCommandRecord(materialId, dstRectanglePxf, command.DstColor, dstTransparentIndex, i,
                                                                     ProcessedCommandFlags::RenderOpaque);
                  ++dstTransparentIndex;
                }
                if (MeshTransparencyFlagsUtil::IsEnabled(meshRecord.Transparency, MeshTransparencyFlags::Transparent))
                {
                  const auto materialId = materialLookup.GetMaterialIndex(meshRecord.TransparentMaterialHandle);
                  assert(dstTransparentIndex < capacity);
                  pDst[dstTransparentIndex] = ProcessedCommandRecord(materialId, dstRectanglePxf, command.DstColor, dstTransparentIndex, i);
                  ++dstTransparentIndex;
                }
              }
              else
              {
                const auto materialId = materialLookup.GetMaterialIndex(meshRecord.TransparentMaterialHandle);
                assert(dstTransparentIndex < capacity);
                pDst[dstTransparentIndex] = ProcessedCommandRecord(materialId, dstRectanglePxf, command.DstColor, dstTransparentIndex, i,
                                                                   ProcessedCommandFlags::RenderIgnoreOpacity);
                ++dstTransparentIndex;
              }
              break;
            }
            default:
              FSLLOG3_WARNING("Unsupported type: {}", HandleCoding::GetType(command.Mesh));
              break;
            }
          }
          assert(dstTransparentIndex <= capacity);
          return {0u, 0u, 0u, dstTransparentIndex};
        }
      }
    }
  }
}
#endif
