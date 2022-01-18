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

#include "RenderSystem.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslGraphics2D/Procedural/Builder/ScopedCustomUITextMeshBuilder2D.hpp>
#include <FslGraphics2D/Procedural/Builder/UITextMeshBuilder.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh.hpp>
#include <FslSimpleUI/Render/Base/Command/Log/FmtDrawCommandType.hpp>
#include <FslSimpleUI/Render/Base/RenderPerformanceCapture.hpp>
#include "FlexRenderSystem.hpp"
#include "HandleCoding.hpp"
#include "MeshManager.hpp"
#include "Log/FmtRenderDrawSpriteType.hpp"
#include "Preprocess/Basic/BasicPreprocessor.hpp"
#include "Preprocess/SpatialGrid/SpatialGridPreprocessor.hpp"
#include "RenderDrawCommandType.hpp"


namespace Fsl
{
  namespace UI
  {
    namespace RenderIMBatch
    {
      namespace
      {
        namespace LocalConfig
        {
          // constexpr LogType Verbosity = LogType::Verbose6;
          constexpr uint32_t GrowBy = 4000;
          constexpr uint32_t InitialCapacity = 2000;
          constexpr uint32_t PureGrowBy = 4000;
          constexpr uint32_t InitialPureCapacity = 0xFFFF;

          constexpr uint32_t MinVertexCapacity = 4 * InitialCapacity;
          constexpr uint32_t MinIndexCapacity = 6 * InitialCapacity;

          constexpr uint32_t MinPureVertexCapacity = 6 * InitialPureCapacity;

          constexpr uint32_t ZStart = 9000;
        }

        struct UploadStats
        {
          uint32_t VertexBufferCount{0};
          uint32_t IndexBufferCount{0};
        };

        struct DrawStats
        {
          uint32_t DrawCalls{0};
          uint32_t DrawIndexCalls{0};
        };


        // -------------------------------------------------------------------------------------------------------------------------------------------
        // BasicImageSprite
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize

        template <typename TBatcher>
        inline void AddBasicImageSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                        const MeshManager::BasicImageSpriteMeshRecord& meshRecord)
        {
          auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                        Color::Premultiply(processedCmd.FinalColor));
          {
            assert(meshRecord.Sprite);
            const RenderBasicImageInfo& renderInfo = meshRecord.Sprite->GetRenderInfo();
            builder.AddRect(processedCmd.DstAreaRectanglePxf, renderInfo.TextureArea);
          }
          rBatcher.EndMeshBuild(builder);
        }

        // CommandDrawCustomBasicImageAtOffsetAndSize

        template <typename TBatcher>
        inline void AddBasicImageSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                        const CommandDrawCustomBasicImageAtOffsetAndSize& cmd, const CustomDrawBasicImageInfo& customDrawInfo,
                                        const MeshManager::BasicImageSpriteMeshRecord& meshRecord)
        {
          // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
          // the chosen material
          auto builder =
            rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                           static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
          {
            assert(customDrawInfo.FnDraw != nullptr);
            assert(meshRecord.Sprite);
            // Execute the custom draw command
            customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), meshRecord.Sprite->GetRenderInfo(),
                                  customDrawInfo.CustomData.get());
          }
          rBatcher.EndMeshBuild(builder);
        }

        template <typename TBatcher>
        inline void AddBasicImageSpriteMesh(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                            const CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh& cmd,
                                            const CustomDrawBasicImageBasicMeshInfo& customDrawInfo,
                                            const MeshManager::BasicImageSpriteMeshRecord& meshRecord)
        {
          // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
          // the chosen material

          auto builder = rBatcher.BeginBasicMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity,
                                                             static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                             processedCmd.FinalColor);
          {
            assert(customDrawInfo.FnDraw != nullptr);
            assert(meshRecord.Sprite);
            // Execute the custom draw command
            customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), meshRecord.Sprite->GetRenderInfo(),
                                  customDrawInfo.CustomData.get());
          }
          rBatcher.EndBasicMeshBuild(builder);
        }

        // -------------------------------------------------------------------------------------------------------------------------------------------
        // BasicNineSliceSprite
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize

        template <typename TBatcher>
        inline void AddBasicNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, const CommandDrawAtOffsetAndSize& cmd,
                                            const MeshManager::BasicNineSliceSpriteMeshRecord& meshRecord)
        {
          auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                        Color::Premultiply(processedCmd.FinalColor));
          {
            const PxSize2D& dstSizePx = cmd.GetDstSizePx();
            // The zero sized elements should already have been removed
            assert(dstSizePx.Width() > 0 && dstSizePx.Height() > 0);
            assert(meshRecord.Sprite);
            const PxVector2& dstPositionPxf = cmd.GetDstPositionPxf();
            const RenderBasicNineSliceInfo& renderInfo = meshRecord.Sprite->GetRenderInfo();
            const auto& scaledNineSlicePx = renderInfo.ScaledNineSlicePx;
            const float dstX0Pxf = dstPositionPxf.X;
            const float dstY0Pxf = dstPositionPxf.Y;
            const float dstX1Pxf = dstX0Pxf + static_cast<float>(scaledNineSlicePx.Left());
            const float dstY1Pxf = dstY0Pxf + static_cast<float>(scaledNineSlicePx.Top());
            const float dstX2Pxf = dstX0Pxf + static_cast<float>(dstSizePx.Width() - scaledNineSlicePx.Right());
            const float dstY2Pxf = dstY0Pxf + static_cast<float>(dstSizePx.Height() - scaledNineSlicePx.Bottom());
            const float dstX3Pxf = dstX0Pxf + static_cast<float>(dstSizePx.Width());
            const float dstY3Pxf = dstY0Pxf + static_cast<float>(dstSizePx.Height());

            builder.AddNineSlice(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf, renderInfo.TextureArea);
          }
          rBatcher.EndMeshBuild(builder);
        }


        // -------------------------------------------------------------------------------------------------------------------------------------------
        // ImageSprite
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize

        template <typename TBatcher>
        inline void AddImageSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, const CommandDrawAtOffsetAndSize& cmd,
                                   const MeshManager::ImageSpriteMeshRecord& meshRecord)
        {
          auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                        Color::Premultiply(processedCmd.FinalColor));
          {
            assert(meshRecord.Sprite);
            const RenderImageInfo& renderInfo = meshRecord.Sprite->GetRenderInfo();

            const PxVector2& dstPositionPxf = cmd.GetDstPositionPxf();
            const PxSize2D& dstSizePx = cmd.GetDstSizePx();
            if (dstSizePx.Width() > 0 && dstSizePx.Height() > 0)
            {
              if (dstSizePx == renderInfo.ScaledSizePx)
              {
                // Create a unscaled destination rectangle (applying the trim to the offset)
                PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + renderInfo.ScaledTrimMarginPxf.Left(),
                                                 dstPositionPxf.Y + renderInfo.ScaledTrimMarginPxf.Top(), renderInfo.ScaledTrimmedSizePxf.Width(),
                                                 renderInfo.ScaledTrimmedSizePxf.Height());
                builder.AddRect(dstRectanglePxf, renderInfo.TextureArea);
              }
              else
              {
                // We need to apply the scaling and trim
                const float finalScalingX = static_cast<float>(dstSizePx.Width()) / static_cast<float>(renderInfo.ScaledSizePx.Width());
                const float finalScalingY = static_cast<float>(dstSizePx.Height()) / static_cast<float>(renderInfo.ScaledSizePx.Height());

                PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + (renderInfo.ScaledTrimMarginPxf.Left() * finalScalingX),
                                                 dstPositionPxf.Y + (renderInfo.ScaledTrimMarginPxf.Top() * finalScalingY),
                                                 static_cast<float>(dstSizePx.Width()) - (renderInfo.ScaledTrimMarginPxf.SumX() * finalScalingX),
                                                 static_cast<float>(dstSizePx.Height()) - (renderInfo.ScaledTrimMarginPxf.SumY() * finalScalingY));

                builder.AddRect(dstRectanglePxf, renderInfo.TextureArea);
              }
            }
          }
          rBatcher.EndMeshBuild(builder);
        }

        // -------------------------------------------------------------------------------------------------------------------------------------------
        // NineSliceSprite
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize
        // FIX: since the trim handling was moved to pre-process, this means that the rendering code is equal how we render the basic nine slice mesh!

        template <typename TBatcher>
        inline void AddNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                       const MeshManager::NineSliceSpriteMeshRecord& meshRecord)
        {
          // assert(processedCmd.DstAreaRectanglePxf.Width() > 0);
          // assert(processedCmd.DstAreaRectanglePxf.Height() > 0);
          // FIX: remove this check, it should be done during pre-process, this will save a lot more
          if (processedCmd.DstAreaRectanglePxf.Width() > 0 && processedCmd.DstAreaRectanglePxf.Height() > 0)
          {
            assert(meshRecord.Sprite);
            const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();
            auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                          static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                          Color::Premultiply(processedCmd.FinalColor));
            {
              builder.AddNineSlice(processedCmd.DstAreaRectanglePxf.Left(), processedCmd.DstAreaRectanglePxf.Top(),
                                   processedCmd.DstAreaRectanglePxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Left(),
                                   processedCmd.DstAreaRectanglePxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Top(),
                                   processedCmd.DstAreaRectanglePxf.Right() - renderInfo.ScaledTrimmedNineSlicePxf.Right(),
                                   processedCmd.DstAreaRectanglePxf.Bottom() - renderInfo.ScaledTrimmedNineSlicePxf.Bottom(),
                                   processedCmd.DstAreaRectanglePxf.Right(), processedCmd.DstAreaRectanglePxf.Bottom(), renderInfo.TextureArea);
            }
            rBatcher.EndMeshBuild(builder);
          }
        }

        // CommandDrawRot90CWAtOffsetAndSize

        template <typename TBatcher>
        inline void AddNineSliceSpriteRot90(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                            const MeshManager::NineSliceSpriteMeshRecord& meshRecord)
        {
          // assert(processedCmd.DstAreaRectanglePxf.Width() > 0);
          // assert(processedCmd.DstAreaRectanglePxf.Height() > 0);
          // FIX: remove this check, it should be done during pre-process, this will save a lot more
          if (processedCmd.DstAreaRectanglePxf.Width() > 0 && processedCmd.DstAreaRectanglePxf.Height() > 0)
          {
            assert(meshRecord.Sprite);
            const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();
            auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                          static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                          Color::Premultiply(processedCmd.FinalColor));
            {
              builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.Left(), processedCmd.DstAreaRectanglePxf.Top(),
                                                processedCmd.DstAreaRectanglePxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Top(),
                                                processedCmd.DstAreaRectanglePxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Left(),
                                                processedCmd.DstAreaRectanglePxf.Right() - renderInfo.ScaledTrimmedNineSlicePxf.Bottom(),
                                                processedCmd.DstAreaRectanglePxf.Bottom() - renderInfo.ScaledTrimmedNineSlicePxf.Right(),
                                                processedCmd.DstAreaRectanglePxf.Right(), processedCmd.DstAreaRectanglePxf.Bottom(),
                                                renderInfo.TextureArea);
            }
            rBatcher.EndMeshBuild(builder);
          }
        }

        // CommandDrawCustomNineSliceAtOffsetAndSize

        template <typename TBatcher>
        inline void AddNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                       const CommandDrawCustomNineSliceAtOffsetAndSize& cmd, const CustomDrawNineSliceInfo& customDrawInfo,
                                       const MeshManager::NineSliceSpriteMeshRecord& meshRecord)
        {
          // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
          // the chosen material
          auto builder =
            rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                           static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
          {
            assert(meshRecord.Sprite);
            assert(customDrawInfo.FnDraw != nullptr);
            const RenderNineSliceInfo& renderInfo = meshRecord.Sprite->GetRenderInfo();
            // Execute the custom draw command
            customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), renderInfo, customDrawInfo.CustomData.get());
          }
          rBatcher.EndMeshBuild(builder);
        }

        // -------------------------------------------------------------------------------------------------------------------------------------------
        // OptimizedNineSliceSprite
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize

        template <typename TBatcher>
        inline void AddOptimizedNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord)
        {
          // assert(processedCmd.DstAreaRectanglePxf.Width() > 0);
          // assert(processedCmd.DstAreaRectanglePxf.Height() > 0);
          // FIX: remove this check, it should be done during pre-process, this will save a lot more
          if (processedCmd.DstAreaRectanglePxf.Width() > 0 && processedCmd.DstAreaRectanglePxf.Height() > 0)
          {
            auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                          static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                          Color::Premultiply(processedCmd.FinalColor));
            {
              assert(meshRecord.Sprite);
              const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

              const float dstX0Pxf = processedCmd.DstAreaRectanglePxf.Left();
              const float dstY0Pxf = processedCmd.DstAreaRectanglePxf.Top();
              const float dstX1Pxf = processedCmd.DstAreaRectanglePxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Left();
              const float dstY1Pxf = processedCmd.DstAreaRectanglePxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Top();
              const float dstX2Pxf = processedCmd.DstAreaRectanglePxf.Right() - renderInfo.ScaledTrimmedNineSlicePxf.Right();
              const float dstY2Pxf = processedCmd.DstAreaRectanglePxf.Bottom() - renderInfo.ScaledTrimmedNineSlicePxf.Bottom();
              const float dstX3Pxf = processedCmd.DstAreaRectanglePxf.Right();
              const float dstY3Pxf = processedCmd.DstAreaRectanglePxf.Bottom();

              // Optimization fix: Once we decouple the rendering commands from the original command entry the preprocessor
              //                   can just rewrite the command type and schedule a normal nineslice operation instead since its compatible.
              if (!ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderIgnoreOpacity))
              {
                builder.AddNineSlice(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf, renderInfo.TextureArea,
                                     renderInfo.Flags, ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderOpaque));
              }
              else
              {
                builder.AddNineSlice(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf, renderInfo.TextureArea);
              }
            }
            rBatcher.EndMeshBuild(builder);
          }
        }

        // CommandDrawRot90CWAtOffsetAndSize

        template <typename TBatcher>
        inline void AddOptimizedNineSliceSpriteRot90(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                     const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord)
        {
          // assert(processedCmd.DstAreaRectanglePxf.Width() > 0);
          // assert(processedCmd.DstAreaRectanglePxf.Height() > 0);
          // FIX: remove this check, it should be done during pre-process, this will save a lot more
          if (processedCmd.DstAreaRectanglePxf.Width() > 0 && processedCmd.DstAreaRectanglePxf.Height() > 0)

          {
            auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                          static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                          Color::Premultiply(processedCmd.FinalColor));
            {
              assert(meshRecord.Sprite);
              const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

              // Optimization fix: Once we decouple the rendering commands from the original command entry the preprocessor
              //                   can just rewrite the command type and schedule a normal nineslice operation instead since its compatible.
              if (!ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderIgnoreOpacity))
              {
                builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.Left(), processedCmd.DstAreaRectanglePxf.Top(),
                                                  processedCmd.DstAreaRectanglePxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Top(),
                                                  processedCmd.DstAreaRectanglePxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Left(),
                                                  processedCmd.DstAreaRectanglePxf.Right() - renderInfo.ScaledTrimmedNineSlicePxf.Bottom(),
                                                  processedCmd.DstAreaRectanglePxf.Bottom() - renderInfo.ScaledTrimmedNineSlicePxf.Right(),
                                                  processedCmd.DstAreaRectanglePxf.Right(), processedCmd.DstAreaRectanglePxf.Bottom(),
                                                  renderInfo.TextureArea, renderInfo.Flags,
                                                  ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderOpaque));
              }
              else
              {
                builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.Left(), processedCmd.DstAreaRectanglePxf.Top(),
                                                  processedCmd.DstAreaRectanglePxf.Left() + renderInfo.ScaledTrimmedNineSlicePxf.Top(),
                                                  processedCmd.DstAreaRectanglePxf.Top() + renderInfo.ScaledTrimmedNineSlicePxf.Left(),
                                                  processedCmd.DstAreaRectanglePxf.Right() - renderInfo.ScaledTrimmedNineSlicePxf.Bottom(),
                                                  processedCmd.DstAreaRectanglePxf.Bottom() - renderInfo.ScaledTrimmedNineSlicePxf.Right(),
                                                  processedCmd.DstAreaRectanglePxf.Right(), processedCmd.DstAreaRectanglePxf.Bottom(),
                                                  renderInfo.TextureArea);
              }
            }
            rBatcher.EndMeshBuild(builder);
          }
        }


        // -------------------------------------------------------------------------------------------------------------------------------------------
        // SpriteFont
        // -------------------------------------------------------------------------------------------------------------------------------------------

        // CommandDrawAtOffsetAndSize

        template <typename TBatcher>
        inline void AddSpriteFont(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& textMeshBuilder,
                                  const CommandDrawAtOffsetAndSize& cmd, const MeshManager::SpriteFontMeshRecord& meshRecord)
        {
          auto builder = rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.VertexCapacity, meshRecord.IndexCapacity,
                                                        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex),
                                                        Color::Premultiply(processedCmd.FinalColor));
          {
            assert(meshRecord.Sprite);
            textMeshBuilder.AddString(builder, *meshRecord.Sprite, cmd.GetDstPositionPxf(), StringViewLiteUtil::AsStringViewLite(meshRecord.Text));
          }
          rBatcher.EndMeshBuild(builder);
        }

        // CommandDrawCustomTextAtOffsetAndSize

        template <typename TBatcher>
        inline void AddSpriteFont(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& rTextMeshBuilder,
                                  const CommandDrawCustomTextAtOffsetAndSize& cmd, const CustomDrawTextInfo& customDrawInfo,
                                  const MeshManager::SpriteFontMeshRecord& record)
        {
          // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
          // the chosen material

          auto builder =
            rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, record.VertexCapacity, record.IndexCapacity,
                                           static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
          {
            assert(record.Sprite);
            ScopedCustomUITextMeshBuilder2D scopedTextBuilder(builder, rTextMeshBuilder, *record.Sprite);
            customDrawInfo.FnDraw(scopedTextBuilder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), customDrawInfo.CustomData.get());
          }
          rBatcher.EndMeshBuild(builder);
        }

        // -------------------------------------------------------------------------------------------------------------------------------------------
        // Process
        // -------------------------------------------------------------------------------------------------------------------------------------------

        template <typename TBatcher>
        void ProcessDrawCommands(TBatcher& rBatcher, const MeshManager& meshManager, UITextMeshBuilder& rTextMeshBuilder,
                                 const ReadOnlySpan<ProcessedCommandRecord> orderedSpan, const ReadOnlySpan<EncodedCommand> commandSpan,
                                 const DrawCommandBufferEx& commandBuffer)
        {
          const std::size_t count = orderedSpan.size();
          for (std::size_t i = 0; i < count; ++i)
          {
            const ProcessedCommandRecord& record = orderedSpan[i];
            const EncodedCommand& command = commandSpan[record.LegacyCommandSpanIndex];
            const auto hMesh = HandleCoding::GetOriginalHandle(command.Mesh);
            switch (ToRenderDrawCommandType(HandleCoding::GetType(command.Mesh), command.Type))
            {
            case RenderDrawCommandType::BasicImageSprite_DrawAtOffsetAndSize:
              AddBasicImageSprite(rBatcher, record, meshManager.UncheckedGetBasicImageSprite(hMesh));
              break;
            case RenderDrawCommandType::BasicImageSprite_DrawCustomBasicImageAtOffsetAndSize:
            {
              CommandDrawCustomBasicImageAtOffsetAndSize cmdEx(command);
              const CustomDrawBasicImageInfo& customDrawInfo = commandBuffer.FastGetCustomDrawBasicImageInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddBasicImageSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetBasicImageSprite(hMesh));
              }
              break;
            }
            case RenderDrawCommandType::BasicImageSprite_DrawCustomBasicImageAtOffsetAndSizeBasicMesh:
            {
              CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh cmdEx(command);
              const CustomDrawBasicImageBasicMeshInfo& customDrawInfo =
                commandBuffer.FastGetCustomDrawBasicImageBasicMeshInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddBasicImageSpriteMesh(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetBasicImageSprite(hMesh));
              }
              break;
            }
            case RenderDrawCommandType::BasicNineSliceSprite_DrawAtOffsetAndSize:
              AddBasicNineSliceSprite(rBatcher, record, CommandDrawAtOffsetAndSize(command), meshManager.UncheckedGetBasicNineSliceSprite(hMesh));
              break;
            case RenderDrawCommandType::ImageSprite_DrawAtOffsetAndSize:
              AddImageSprite(rBatcher, record, CommandDrawAtOffsetAndSize(command), meshManager.UncheckedGetImageSprite(hMesh));
              break;
            case RenderDrawCommandType::NineSliceSprite_DrawAtOffsetAndSize:
              AddNineSliceSprite(rBatcher, record, meshManager.UncheckedGetNineSliceSprite(hMesh));
              break;
            case RenderDrawCommandType::NineSliceSprite_DrawRot90CWAtOffsetAndSize:
              AddNineSliceSpriteRot90(rBatcher, record, meshManager.UncheckedGetNineSliceSprite(hMesh));
              break;
            case RenderDrawCommandType::NineSliceSprite_DrawCustomNineSliceAtOffsetAndSize:
            {
              CommandDrawCustomNineSliceAtOffsetAndSize cmdEx(command);
              const CustomDrawNineSliceInfo& customDrawInfo = commandBuffer.FastGetCustomDrawNineSliceInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddNineSliceSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetNineSliceSprite(hMesh));
              }
              break;
            }
            case RenderDrawCommandType::OptimizedNineSliceSprite_DrawAtOffsetAndSize:
              AddOptimizedNineSliceSprite(rBatcher, record, meshManager.UncheckedGetOptimizedNineSliceSprite(hMesh));
              break;
            case RenderDrawCommandType::OptimizedNineSliceSprite_DrawRot90CWAtOffsetAndSize:
              AddOptimizedNineSliceSpriteRot90(rBatcher, record, meshManager.UncheckedGetOptimizedNineSliceSprite(hMesh));
              break;
            case RenderDrawCommandType::SpriteFont_DrawAtOffsetAndSize:
              AddSpriteFont(rBatcher, record, rTextMeshBuilder, CommandDrawAtOffsetAndSize(command), meshManager.UncheckedGetSpriteFont(hMesh));
              break;
            case RenderDrawCommandType::SpriteFont_DrawCustomTextAtOffsetAndSize:
            {
              CommandDrawCustomTextAtOffsetAndSize cmdEx(command);
              const CustomDrawTextInfo& customDrawInfo = commandBuffer.FastGetCustomDrawTextInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddSpriteFont(rBatcher, record, rTextMeshBuilder, cmdEx, customDrawInfo, meshManager.UncheckedGetSpriteFont(hMesh));
              }
              break;
            }
            default:
              FSLLOG3_ERROR("Not a valid draw command '{}' for for the given mesh type {}", command.Type, HandleCoding::GetType(command.Mesh));
              break;
            }
          }
        }

        template <typename TBatcher>
        void UploadMeshChanges(std::vector<RenderSystemBufferRecord>& rBuffers, UploadStats& rStats, IBasicRenderSystem& renderSystem,
                               const TBatcher& batcher)
        {
          // Upload all the changes to the buffers (create + resize as required)
          const uint32_t segmentCount = batcher.GetSegmentCount();
          for (uint32_t i = 0; i < segmentCount; ++i)
          {
            const typename TBatcher::SegmentSpans info = batcher.GetSegmentSpans(i);
            // FSLLOG3_INFO("Batch #:{} vertices:{} indices:{}", i, info.Vertices.size(), info.Indices.size());
            if (info.Indices.empty())
            {
              assert(!info.Vertices.empty());
              if (i >= rBuffers.size())
              {
                const auto vertexCapacity = std::max(UncheckedNumericCast<uint32_t>(info.Vertices.size()), LocalConfig::MinPureVertexCapacity);
                rBuffers.emplace_back(
                  renderSystem.CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck), vertexCapacity),
                  vertexCapacity);
              }
              else if (info.Vertices.size() > rBuffers[i].VertexCapacity)
              {
                const auto vertexCapacity = UncheckedNumericCast<uint32_t>(
                  std::max(info.Vertices.size() + LocalConfig::PureGrowBy, std::size_t(LocalConfig::MinPureVertexCapacity)));

                rBuffers[i] = RenderSystemBufferRecord(
                  renderSystem.CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck), vertexCapacity),
                  vertexCapacity);
              }
              else
              {
                rBuffers[i].VertexBuffer->SetData(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck));
              }
              ++rStats.VertexBufferCount;
            }
            else
            {
              assert(!info.Vertices.empty());
              assert(!info.Indices.empty());
              if (i >= rBuffers.size())
              {
                const auto vertexCapacity = std::max(UncheckedNumericCast<uint32_t>(info.Vertices.size()), LocalConfig::MinVertexCapacity);
                const auto indexCapacity = std::max(UncheckedNumericCast<uint32_t>(info.Indices.size()), LocalConfig::MinIndexCapacity);
                rBuffers.emplace_back(
                  renderSystem.CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck), vertexCapacity),
                  vertexCapacity, renderSystem.CreateDynamicBuffer(info.Indices, indexCapacity), indexCapacity);
              }
              else if (info.Vertices.size() > rBuffers[i].VertexCapacity || info.Indices.size() > rBuffers[i].IndexCapacity)
              {
                const auto vertexCapacity =
                  UncheckedNumericCast<uint32_t>(std::max(info.Vertices.size() + LocalConfig::GrowBy, std::size_t(LocalConfig::MinVertexCapacity)));
                const auto indexCapacity = UncheckedNumericCast<uint32_t>(info.Indices.size() + LocalConfig::GrowBy);
                rBuffers[i] = RenderSystemBufferRecord(
                  renderSystem.CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck), vertexCapacity),
                  vertexCapacity, renderSystem.CreateDynamicBuffer(info.Indices, indexCapacity), indexCapacity);
              }
              else
              {
                rBuffers[i].VertexBuffer->SetData(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck));
                rBuffers[i].IndexBuffer->SetData(info.Indices);
              }
              ++rStats.VertexBufferCount;
              ++rStats.IndexBufferCount;
            }
          }
        }

        template <typename TBatcher>
        void DrawMeshes(IBasicRenderSystem& renderSystem, DrawStats& rStats, const TBatcher& batcher, const MeshManager& meshManager,
                        const ReadOnlySpan<RenderSystemBufferRecord> buffers, const BasicCameraInfo& cameraInfo, const uint32_t maxDrawCalls)
        {
          renderSystem.BeginCmds();
          renderSystem.CmdSetCamera(cameraInfo);

          const auto& materialLookup = meshManager.GetMaterialLookup();

          const uint32_t segmentCount = batcher.GetSegmentCount();
          assert(buffers.size() == segmentCount);
          for (uint32_t segmentIndex = 0; segmentIndex < segmentCount && (rStats.DrawCalls + rStats.DrawIndexCalls) < maxDrawCalls; ++segmentIndex)
          {
            assert(segmentIndex < buffers.size());
            // Bind the buffers used by the segment
            const RenderSystemBufferRecord& bufferRecord = buffers[segmentIndex];
            renderSystem.CmdBindVertexBuffer(bufferRecord.VertexBuffer);
            if (bufferRecord.IndexCapacity > 0u)
            {
              assert(bufferRecord.IndexBuffer);
              renderSystem.CmdBindIndexBuffer(bufferRecord.IndexBuffer);
            }
            // Draw all the batches stored in this segment
            auto segmentBatchInfo = batcher.GetSegmentBatchInfo(segmentIndex);
            const uint32_t batchEndIndex = segmentBatchInfo.BatchRange.End();
            for (uint32_t batchIndex = segmentBatchInfo.BatchRange.Start;
                 batchIndex < batchEndIndex && (rStats.DrawCalls + rStats.DrawIndexCalls) < maxDrawCalls; ++batchIndex)
            {
              const BatchRecord& batchRecord = batcher.GetBatchRecord(batchIndex);
              const SpriteMaterialInfo& materialInfo = materialLookup.FastGetSpriteMaterialInfo(batchRecord.Info.MaterialId);

              const auto* pBasicMaterial = dynamic_cast<const BasicSpriteMaterial*>(materialInfo.Material.get());
              if (pBasicMaterial != nullptr)
              {
                renderSystem.CmdBindMaterial(pBasicMaterial->Material);
                switch (batchRecord.Info.ContentType)
                {
                case BatchContentType::Vertices:
                {
                  assert(batchRecord.VertexSpanRange.Start <= bufferRecord.VertexCapacity);
                  assert(batchRecord.VertexSpanRange.End() <= bufferRecord.VertexCapacity);
                  renderSystem.CmdDraw(batchRecord.VertexSpanRange.Length, batchRecord.VertexSpanRange.Start);
                  ++rStats.DrawCalls;
                  break;
                }
                case BatchContentType::VerticesAndIndices:
                {
                  assert(batchRecord.IndexSpanRange.Start <= bufferRecord.IndexCapacity);
                  assert(batchRecord.IndexSpanRange.End() <= bufferRecord.IndexCapacity);
                  renderSystem.CmdDrawIndexed(batchRecord.IndexSpanRange.Length, batchRecord.IndexSpanRange.Start);
                  ++rStats.DrawIndexCalls;
                  break;
                }
                default:
                  FSLLOG3_ERROR("Unsupported content type");
                  break;
                }
              }
            }
          }
          renderSystem.EndCmds();
        }


        template <typename TBatcher>
        inline void UpdateStats(RenderSystemStats& rStats, const TBatcher& batcher, const MeshManager& meshManager, const UploadStats& uploadStats,
                                const DrawStats& drawStats)
        {    // Update the stats
          auto batcherStats = batcher.GetStats();
          rStats.MeshCount = meshManager.GetMeshCount();
          rStats.BatchCount = batcherStats.BatchCount;
          rStats.VertexCount = batcherStats.VertexCount;
          rStats.IndexCount = batcherStats.IndexCount;
          rStats.VertexBufferCount = uploadStats.VertexBufferCount;
          rStats.IndexBufferCount = uploadStats.IndexBufferCount;
          rStats.DrawCalls = drawStats.DrawCalls;
          rStats.DrawIndexCalls = drawStats.DrawIndexCalls;
        }

        template <typename TBatcher>
        void DrawNow(RenderSystemStats& rStats, IBasicRenderSystem& renderSystem, MeshManager& meshManager,
                     std::vector<RenderSystemBufferRecord>& rBuffers, const TBatcher& batcher, const BasicCameraInfo& cameraInfo,
                     RenderPerformanceCapture* const pPerformanceCapture, const uint32_t maxDrawCalls)
        {
          UploadStats uploadStats;
          DrawStats drawStats;

          if (pPerformanceCapture != nullptr)
          {
            pPerformanceCapture->Begin(RenderPerformanceCaptureId::UpdateBuffers);
          }

          UploadMeshChanges(rBuffers, uploadStats, renderSystem, batcher);

          if (pPerformanceCapture != nullptr)
          {
            pPerformanceCapture->EndThenBegin(RenderPerformanceCaptureId::UpdateBuffers, RenderPerformanceCaptureId::ScheduleDraw);
          }

          DrawMeshes(renderSystem, drawStats, batcher, meshManager,
                     ReadOnlySpanUtil::AsSpan(rBuffers, 0, batcher.GetSegmentCount(), OptimizationCheckFlag::NoCheck), cameraInfo, maxDrawCalls);


          if (pPerformanceCapture != nullptr)
          {
            pPerformanceCapture->End(RenderPerformanceCaptureId::ScheduleDraw);
          }

          UpdateStats(rStats, batcher, meshManager, uploadStats, drawStats);
        }


        template <typename TBatcher, typename TPreprocessor>
        void DoDraw(RenderSystemStats& rStats, IBasicRenderSystem& renderSystem, MeshManager& rMeshManager,
                    std::vector<RenderSystemBufferRecord>& rBuffers, std::vector<ProcessedCommandRecord>& rProcessedCommandRecords,
                    TBatcher& rBatcher, DrawCommandBufferEx& rCommandBuffer, const BasicCameraInfo& cameraInfo, TPreprocessor& rPreprocessor,
                    RenderPerformanceCapture* const pPerformanceCapture, const uint32_t maxDrawCalls)
        {
          {
            auto capacity = rMeshManager.GetCapacity();
            rBatcher.EnsureCapacity(capacity.VertexCapacity, capacity.IndexCapacity);
          }

          try
          {
            UITextMeshBuilder& rTextMeshBuilder = rMeshManager.GetTextMeshBuilder();

            // Process the draw commands which generate all the meshes using a given 'batch' strategy.
            rBatcher.BeginBatch();
            {
              auto commandSpan = rCommandBuffer.AsReadOnlySpan();
              if (!commandSpan.empty())
              {
                if (pPerformanceCapture != nullptr)
                {
                  pPerformanceCapture->Begin(RenderPerformanceCaptureId::PreprocessDrawCommands);
                }

                rPreprocessor.Process(rProcessedCommandRecords, commandSpan, rMeshManager);

                if (pPerformanceCapture != nullptr)
                {
                  pPerformanceCapture->EndThenBegin(RenderPerformanceCaptureId::PreprocessDrawCommands, RenderPerformanceCaptureId::GenerateMeshes);
                }

                ReadOnlySpan<ProcessedCommandRecord> opaqueSpan = rPreprocessor.GetOpaqueSpan(rProcessedCommandRecords);
                ProcessDrawCommands(rBatcher, rMeshManager, rTextMeshBuilder, opaqueSpan, commandSpan, rCommandBuffer);
                ReadOnlySpan<ProcessedCommandRecord> transparentSpan = rPreprocessor.GetTransparentSpan(rProcessedCommandRecords);
                ProcessDrawCommands(rBatcher, rMeshManager, rTextMeshBuilder, transparentSpan, commandSpan, rCommandBuffer);

                if (pPerformanceCapture != nullptr)
                {
                  pPerformanceCapture->End(RenderPerformanceCaptureId::GenerateMeshes);
                }
              }
              rCommandBuffer.Clear();
            }
            rBatcher.EndBatch();


            // Time to upload and draw the meshes
            DrawNow(rStats, renderSystem, rMeshManager, rBuffers, rBatcher, cameraInfo, pPerformanceCapture, maxDrawCalls);
          }
          catch (std::exception& ex)
          {
            rCommandBuffer.Clear();
            rBatcher.ForceEndBatch();
            FSLLOG3_ERROR("Exception {}", ex.what());
            throw;
          }
        }
      }

      RenderSystem::RenderSystem(const RenderSystemCreateInfo& createInfo)
        : RenderSystemBase(createInfo)
        , m_batcher(createInfo.DefaultVertexCapacity, createInfo.DefaultIndexCapacity)
      {
      }


      void RenderSystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
      {
        const BasicCameraInfo cameraInfo(GetMatrixProjection());
        LinearPreprocessor preprocessor(GetAllowDepthBuffer());
        DoDraw(DoGetStats(), GetRenderSystem(), DoGetMeshManager(), GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(),
               cameraInfo, preprocessor, pPerformanceCapture, 0xFFFFFFFF);
      }


      void FlexRenderSystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
      {
        MeshManager& rMeshmanager = DoGetMeshManager();

        const bool allowDepthBuffer = m_config.UseDepthBuffer;
        const uint32_t maxDrawCalls = m_maxDrawCalls;

        // const PxSize2D sizePx = GetWindowMetrics().GetSizePx();
        const BasicCameraInfo cameraInfo(GetMatrixProjection());
        if (m_config.ReorderMethod == DrawReorderMethod::Disabled)
        {
          BasicPreprocessor preprocessor(allowDepthBuffer);
          DoDraw(DoGetStats(), GetRenderSystem(), rMeshmanager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
                 preprocessor, pPerformanceCapture, maxDrawCalls);
        }
        else
        {
          m_preprocessor.SetAllowDepthBuffer(allowDepthBuffer);

          DoDraw(DoGetStats(), GetRenderSystem(), rMeshmanager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
                 m_preprocessor, pPerformanceCapture, maxDrawCalls);

          //  // DoDraw(DoGetStats(), GetRenderSystem(), rMeshmanager), GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(),
          //  //       cameraInfo, SpatialGridPreprocessor(sizePx));
        }
      }
    }
  }
}
