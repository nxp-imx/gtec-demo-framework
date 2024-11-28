/****************************************************************************************************************************************************
 * Copyright 2021-2024 NXP
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
#include <FslGraphics/Render/Basic/BasicCameraInfo.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderConverter.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterialUtil.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh.hpp>
#include <FslSimpleUI/Render/Base/Command/Log/FmtDrawCommandType.hpp>
#include <FslSimpleUI/Render/Base/RenderPerformanceCapture.hpp>
#include <FslSimpleUI/Render/Builder/ScopedCustomUITextMeshBuilder2D.hpp>
#include <FslSimpleUI/Render/Builder/UITextMeshBuilder.hpp>
#include "DefaultRenderSystem.hpp"
#include "FlexRenderSystem.hpp"
#include "HandleCoding.hpp"
#include "Log/FmtRenderDrawSpriteType.hpp"
#include "MeshManager.hpp"
#include "Preprocess/Basic/BasicPreprocessor.hpp"
#include "RenderDrawCommandType.hpp"


namespace Fsl::UI::RenderIMBatch
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


    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // BasicImageSprite, ImageSprite
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddImageMesh(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, const MeshManager::ImageMeshRecord& meshRecord)
    {
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        builder.AddRect(processedCmd.DstAreaRectanglePxf, meshRecord.Primitive.RenderInfo.TextureArea);
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddImageMeshWithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                         const MeshManager::ImageMeshRecord& meshRecord, const PxAreaRectangleF& clipRectanglePxf)
    {
      PxAreaRectangleF dstRectanglePxf = processedCmd.DstAreaRectanglePxf;
      NativeTextureArea srcTextureArea = meshRecord.Primitive.RenderInfo.TextureArea;

      if (!Clip2DUtil::Clip(dstRectanglePxf, srcTextureArea, clipRectanglePxf))
      {
        auto builder = rBatcher.BeginMeshBuildCustomZ(
          processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
          static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
        {
          assert(meshRecord.Sprite);
          builder.AddRect(dstRectanglePxf, srcTextureArea);
        }
        rBatcher.EndMeshBuild(builder);
      }
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawCustomBasicImageAtOffsetAndSize

    template <typename TBatcher>
    inline void AddBasicImageSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                    const CommandDrawCustomBasicImageAtOffsetAndSize& cmd, const CustomDrawBasicImageInfo& customDrawInfo,
                                    const MeshManager::ImageMeshRecord& meshRecord, const DrawClipContext& clipContext)
    {
      // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
      // the chosen material
      auto builder =
        rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
                                       static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
      {
        assert(customDrawInfo.FnDraw != nullptr);
        assert(meshRecord.Sprite);
        // Execute the custom draw command
        customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), clipContext,
                              RenderConverter::ToRenderBasicImageInfo(meshRecord.Primitive.RenderInfo), customDrawInfo.CustomData.get());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    template <typename TBatcher>
    inline void AddBasicImageSpriteMesh(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                        const CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh& cmd,
                                        const CustomDrawBasicImageBasicMeshInfo& customDrawInfo, const MeshManager::ImageMeshRecord& meshRecord,
                                        const DrawClipContext& clipContext)
    {
      // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
      // the chosen material

      auto builder =
        rBatcher.BeginBasicMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity,
                                            static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
      {
        assert(customDrawInfo.FnDraw != nullptr);
        assert(meshRecord.Sprite);
        // Execute the custom draw command
        customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), clipContext,
                              RenderConverter::ToRenderBasicImageInfo(meshRecord.Primitive.RenderInfo), customDrawInfo.CustomData.get());
      }
      rBatcher.EndBasicMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // BasicNineSliceSprite, NineSliceSprite
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, const MeshManager::NineSliceMeshRecord& meshRecord)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      assert(meshRecord.Sprite);
      const auto& renderInfo = meshRecord.Primitive.RenderInfo;
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        builder.AddNineSlice(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                             processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                             processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                             processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                             processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                             processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(), renderInfo.TextureArea);
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddNineSliceSpriteWithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                               const MeshManager::NineSliceMeshRecord& meshRecord, const PxAreaRectangleF& clipRectanglePxf)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      assert(meshRecord.Sprite);
      const auto& renderInfo = meshRecord.Primitive.RenderInfo;
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        builder.AddNineSlice(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                             processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                             processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                             processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                             processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                             processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(), renderInfo.TextureArea,
                             clipRectanglePxf);
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawRot90CWAtOffsetAndSize
    template <typename TBatcher>
    inline void AddNineSliceSpriteRot90(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                        const MeshManager::NineSliceMeshRecord& meshRecord)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      assert(meshRecord.Sprite);
      const auto& renderInfo = meshRecord.Primitive.RenderInfo;
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                          processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                          processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                          processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                          processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                          processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                          renderInfo.TextureArea);
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawRot90CWAtOffsetAndSize
    template <typename TBatcher>
    inline void AddNineSliceSpriteRot90WithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                    const MeshManager::NineSliceMeshRecord& meshRecord, const PxAreaRectangleF& clipRectanglePxf)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      assert(meshRecord.Sprite);
      const auto& renderInfo = meshRecord.Primitive.RenderInfo;
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                          processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                          processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                          processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                          processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                          processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                          renderInfo.TextureArea, clipRectanglePxf);
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawCustomNineSliceAtOffsetAndSize
    template <typename TBatcher>
    inline void AddNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                   const CommandDrawCustomNineSliceAtOffsetAndSize& cmd, const CustomDrawNineSliceInfo& customDrawInfo,
                                   const MeshManager::NineSliceMeshRecord& meshRecord, const DrawClipContext clipContext)
    {
      // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
      // the chosen material
      auto builder =
        rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
                                       static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
      {
        assert(meshRecord.Sprite);
        assert(customDrawInfo.FnDraw != nullptr);
        const RenderOptimizedNineSliceInfo& renderInfo = meshRecord.Primitive.RenderInfo;
        // Execute the custom draw command
        customDrawInfo.FnDraw(builder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), clipContext, renderInfo, customDrawInfo.CustomData.get());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // OptimizedNineSliceSprite
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddOptimizedNineSliceSprite(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                            const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

        const float dstX0Pxf = processedCmd.DstAreaRectanglePxf.RawLeft();
        const float dstY0Pxf = processedCmd.DstAreaRectanglePxf.RawTop();
        const float dstX1Pxf = processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft();
        const float dstY1Pxf = processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop();
        const float dstX2Pxf = processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight();
        const float dstY2Pxf = processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom();
        const float dstX3Pxf = processedCmd.DstAreaRectanglePxf.RawRight();
        const float dstY3Pxf = processedCmd.DstAreaRectanglePxf.RawBottom();

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

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddOptimizedNineSliceSpriteWithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                        const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord,
                                                        const PxAreaRectangleF& clipRectanglePxf)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

        const float dstX0Pxf = processedCmd.DstAreaRectanglePxf.RawLeft();
        const float dstY0Pxf = processedCmd.DstAreaRectanglePxf.RawTop();
        const float dstX1Pxf = processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft();
        const float dstY1Pxf = processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop();
        const float dstX2Pxf = processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight();
        const float dstY2Pxf = processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom();
        const float dstX3Pxf = processedCmd.DstAreaRectanglePxf.RawRight();
        const float dstY3Pxf = processedCmd.DstAreaRectanglePxf.RawBottom();

        // Optimization fix: Once we decouple the rendering commands from the original command entry the preprocessor
        //                   can just rewrite the command type and schedule a normal nineslice operation instead since its compatible.
        if (!ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderIgnoreOpacity))
        {
          builder.AddNineSlice(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf, renderInfo.TextureArea,
                               clipRectanglePxf, renderInfo.Flags,
                               ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderOpaque));
        }
        else
        {
          builder.AddNineSlice(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf, renderInfo.TextureArea,
                               clipRectanglePxf);
        }
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawRot90CWAtOffsetAndSize
    template <typename TBatcher>
    inline void AddOptimizedNineSliceSpriteRot90(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                 const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

        // Optimization fix: Once we decouple the rendering commands from the original command entry the preprocessor
        //                   can just rewrite the command type and schedule a normal nineslice operation instead since its compatible.
        if (!ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderIgnoreOpacity))
        {
          builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                            processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                            processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                            processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                            renderInfo.TextureArea, renderInfo.Flags,
                                            ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderOpaque));
        }
        else
        {
          builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                            processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                            processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                            processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                            renderInfo.TextureArea);
        }
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawRot90CWAtOffsetAndSize
    template <typename TBatcher>
    inline void AddOptimizedNineSliceSpriteRot90WithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd,
                                                             const MeshManager::OptimizedNineSliceSpriteMeshRecord& meshRecord,
                                                             const PxAreaRectangleF& clipRectanglePxf)
    {
      assert(processedCmd.DstAreaRectanglePxf.RawWidth() > 0);
      assert(processedCmd.DstAreaRectanglePxf.RawHeight() > 0);
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        const auto& renderInfo = meshRecord.Sprite->GetRenderInfo();

        // Optimization fix: Once we decouple the rendering commands from the original command entry the preprocessor
        //                   can just rewrite the command type and schedule a normal nineslice operation instead since its compatible.
        if (!ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderIgnoreOpacity))
        {
          builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                            processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                            processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                            processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                            renderInfo.TextureArea, clipRectanglePxf, renderInfo.Flags,
                                            ProcessedCommandFlagsUtil::IsEnabled(processedCmd.Flags, ProcessedCommandFlags::RenderOpaque));
        }
        else
        {
          builder.AddNineSliceUVRotated90CW(processedCmd.DstAreaRectanglePxf.RawLeft(), processedCmd.DstAreaRectanglePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawLeft() + renderInfo.ScaledTrimmedNineSlicePxf.RawTop(),
                                            processedCmd.DstAreaRectanglePxf.RawTop() + renderInfo.ScaledTrimmedNineSlicePxf.RawLeft(),
                                            processedCmd.DstAreaRectanglePxf.RawRight() - renderInfo.ScaledTrimmedNineSlicePxf.RawBottom(),
                                            processedCmd.DstAreaRectanglePxf.RawBottom() - renderInfo.ScaledTrimmedNineSlicePxf.RawRight(),
                                            processedCmd.DstAreaRectanglePxf.RawRight(), processedCmd.DstAreaRectanglePxf.RawBottom(),
                                            renderInfo.TextureArea, clipRectanglePxf);
        }
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // SpriteFont
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddSpriteFont(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& textMeshBuilder,
                              const CommandDrawAtOffsetAndSize& cmd, const MeshManager::SpriteFontMeshRecord& meshRecord)
    {
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        textMeshBuilder.AddString(builder, cmd.GetDstPositionPxf(), meshRecord.GetGlyphs());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawAtOffsetAndSize
    template <typename TBatcher>
    inline void AddSpriteFontWithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& textMeshBuilder,
                                          const CommandDrawAtOffsetAndSize& cmd, const MeshManager::SpriteFontMeshRecord& meshRecord)
    {
      auto builder = rBatcher.BeginMeshBuildCustomZ(
        processedCmd.MaterialId, meshRecord.Primitive.MeshVertexCapacity, meshRecord.Primitive.MeshIndexCapacity,
        static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), UIRenderColor::Premultiply(processedCmd.FinalColor));
      {
        assert(meshRecord.Sprite);
        textMeshBuilder.AddString(builder, cmd.GetDstPositionPxf(), meshRecord.GetGlyphs(), cmd.GetClipRectanglePxf());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawCustomTextAtOffsetAndSize
    template <typename TBatcher>
    inline void AddSpriteFont(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& rTextMeshBuilder,
                              const CommandDrawCustomTextAtOffsetAndSize& cmd, const CustomDrawTextInfo& customDrawInfo,
                              const MeshManager::SpriteFontMeshRecord& record)
    {
      // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
      // the chosen material

      auto builder =
        rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, record.Primitive.MeshVertexCapacity, record.Primitive.MeshIndexCapacity,
                                       static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
      {
        assert(record.Sprite);
        ScopedCustomUITextMeshBuilder2D scopedTextBuilder(builder, rTextMeshBuilder, *record.Sprite);
        customDrawInfo.FnDraw(scopedTextBuilder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), customDrawInfo.CustomData.get());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // CommandDrawCustomTextAtOffsetAndSize
    template <typename TBatcher>
    inline void AddSpriteFontWithClipping(TBatcher& rBatcher, const ProcessedCommandRecord& processedCmd, UITextMeshBuilder& rTextMeshBuilder,
                                          const CommandDrawCustomTextAtOffsetAndSize& cmd, const CustomDrawTextInfo& customDrawInfo,
                                          const MeshManager::SpriteFontMeshRecord& record)
    {
      // The custom rendering functions gets called with non-premultiplied colors so its up to them to do the proper color conversion depending on
      // the chosen material

      auto builder =
        rBatcher.BeginMeshBuildCustomZ(processedCmd.MaterialId, record.Primitive.MeshVertexCapacity, record.Primitive.MeshIndexCapacity,
                                       static_cast<float>(LocalConfig::ZStart - processedCmd.LegacyCommandSpanIndex), processedCmd.FinalColor);
      {
        assert(record.Sprite);
        ScopedCustomUITextMeshBuilder2D scopedTextBuilder(builder, rTextMeshBuilder, *record.Sprite, cmd.GetClipRectanglePxf());
        customDrawInfo.FnDraw(scopedTextBuilder, cmd.GetDstPositionPxf(), cmd.GetDstSizePx(), customDrawInfo.CustomData.get());
      }
      rBatcher.EndMeshBuild(builder);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Process
    // -----------------------------------------------------------------------------------------------------------------------------------------------

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
        if (!command.State.IsClipEnabled())
        {
          switch (ToRenderDrawCommandType(HandleCoding::GetType(command.Mesh), command.State.Type()))
          {
          case RenderDrawCommandType::BasicImageSprite_DrawAtOffsetAndSize:
          case RenderDrawCommandType::ImageSprite_DrawAtOffsetAndSize:
            AddImageMesh(rBatcher, record, meshManager.UncheckedGetImageSprite(hMesh));
            break;
          case RenderDrawCommandType::BasicImageSprite_DrawCustomBasicImageAtOffsetAndSize:
            {
              CommandDrawCustomBasicImageAtOffsetAndSize cmdEx(command);
              const CustomDrawBasicImageInfo& customDrawInfo = commandBuffer.FastGetCustomDrawBasicImageInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddBasicImageSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetImageSprite(hMesh), {});
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
                AddBasicImageSpriteMesh(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetImageSprite(hMesh), {});
              }
              break;
            }
          case RenderDrawCommandType::BasicNineSliceSprite_DrawAtOffsetAndSize:
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
                AddNineSliceSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetNineSliceSprite(hMesh), {});
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
            FSLLOG3_ERROR("Not a valid draw command '{}' for for the given mesh type {}", command.State.Type(), HandleCoding::GetType(command.Mesh));
            break;
          }
        }
        else
        {
          switch (ToRenderDrawCommandType(HandleCoding::GetType(command.Mesh), command.State.Type()))
          {
          case RenderDrawCommandType::BasicImageSprite_DrawAtOffsetAndSize:
          case RenderDrawCommandType::ImageSprite_DrawAtOffsetAndSize:
            AddImageMeshWithClipping(rBatcher, record, meshManager.UncheckedGetImageSprite(hMesh), command.ClipRectanglePxf);
            break;
          case RenderDrawCommandType::BasicImageSprite_DrawCustomBasicImageAtOffsetAndSize:
            {
              CommandDrawCustomBasicImageAtOffsetAndSize cmdEx(command);
              const CustomDrawBasicImageInfo& customDrawInfo = commandBuffer.FastGetCustomDrawBasicImageInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddBasicImageSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetImageSprite(hMesh),
                                    DrawClipContext(true, command.ClipRectanglePxf));
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
                AddBasicImageSpriteMesh(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetImageSprite(hMesh),
                                        DrawClipContext(true, command.ClipRectanglePxf));
              }
              break;
            }
          case RenderDrawCommandType::BasicNineSliceSprite_DrawAtOffsetAndSize:
          case RenderDrawCommandType::NineSliceSprite_DrawAtOffsetAndSize:
            AddNineSliceSpriteWithClipping(rBatcher, record, meshManager.UncheckedGetNineSliceSprite(hMesh), command.ClipRectanglePxf);
            break;
          case RenderDrawCommandType::NineSliceSprite_DrawRot90CWAtOffsetAndSize:
            AddNineSliceSpriteRot90WithClipping(rBatcher, record, meshManager.UncheckedGetNineSliceSprite(hMesh), command.ClipRectanglePxf);
            break;
          case RenderDrawCommandType::NineSliceSprite_DrawCustomNineSliceAtOffsetAndSize:
            {
              CommandDrawCustomNineSliceAtOffsetAndSize cmdEx(command);
              const CustomDrawNineSliceInfo& customDrawInfo = commandBuffer.FastGetCustomDrawNineSliceInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddNineSliceSprite(rBatcher, record, cmdEx, customDrawInfo, meshManager.UncheckedGetNineSliceSprite(hMesh),
                                   DrawClipContext(true, command.ClipRectanglePxf));
              }
              break;
            }
          case RenderDrawCommandType::OptimizedNineSliceSprite_DrawAtOffsetAndSize:
            AddOptimizedNineSliceSpriteWithClipping(rBatcher, record, meshManager.UncheckedGetOptimizedNineSliceSprite(hMesh),
                                                    command.ClipRectanglePxf);
            break;
          case RenderDrawCommandType::OptimizedNineSliceSprite_DrawRot90CWAtOffsetAndSize:
            AddOptimizedNineSliceSpriteRot90WithClipping(rBatcher, record, meshManager.UncheckedGetOptimizedNineSliceSprite(hMesh),
                                                         command.ClipRectanglePxf);
            break;
          case RenderDrawCommandType::SpriteFont_DrawAtOffsetAndSize:
            AddSpriteFontWithClipping(rBatcher, record, rTextMeshBuilder, CommandDrawAtOffsetAndSize(command),
                                      meshManager.UncheckedGetSpriteFont(hMesh));
            break;
          case RenderDrawCommandType::SpriteFont_DrawCustomTextAtOffsetAndSize:
            {
              CommandDrawCustomTextAtOffsetAndSize cmdEx(command);
              const CustomDrawTextInfo& customDrawInfo = commandBuffer.FastGetCustomDrawTextInfo(cmdEx.CustomDrawFunctionIndex());
              if (customDrawInfo.FnDraw != nullptr)
              {
                AddSpriteFontWithClipping(rBatcher, record, rTextMeshBuilder, cmdEx, customDrawInfo, meshManager.UncheckedGetSpriteFont(hMesh));
              }
              break;
            }
          default:
            FSLLOG3_ERROR("Not a valid draw command '{}' for for the given mesh type {}", command.State.Type(), HandleCoding::GetType(command.Mesh));
            break;
          }
        }
      }
    }

    template <typename TBatcher>
    UploadStats UploadMeshChanges(std::vector<RenderSystemBufferRecord>& rBuffers, IBasicRenderSystem& renderSystem, const TBatcher& batcher)
    {
      UploadStats stats;
      // Upload all the changes to the buffers (create + resize as required)
      const uint32_t segmentCount = batcher.GetSegmentCount();
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        const typename TBatcher::SegmentSpans info = batcher.GetSegmentSpans(i);
        // FSLLOG3_INFO("Upload batch #{} vertices:{} indices:{}", i, info.Vertices.size(), info.Indices.size());
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
              std::max(info.Vertices.size() + LocalConfig::PureGrowBy, static_cast<std::size_t>(LocalConfig::MinPureVertexCapacity)));

            rBuffers[i] = RenderSystemBufferRecord(
              renderSystem.CreateDynamicBuffer(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck), vertexCapacity),
              vertexCapacity);
          }
          else
          {
            rBuffers[i].VertexBuffer->SetData(ReadOnlyFlexVertexSpanUtil::AsSpan(info.Vertices, OptimizationCheckFlag::NoCheck));
          }
          ++stats.VertexBufferCount;
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
            const auto vertexCapacity = UncheckedNumericCast<uint32_t>(
              std::max(info.Vertices.size() + LocalConfig::GrowBy, static_cast<std::size_t>(LocalConfig::MinVertexCapacity)));
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
          ++stats.VertexBufferCount;
          ++stats.IndexBufferCount;
        }
      }
      return stats;
    }


    template <typename TBatcher>
    UploadStats CalcUploadStats(const TBatcher& batcher)
    {
      UploadStats stats;
      // Upload all the changes to the buffers (create + resize as required)
      const uint32_t segmentCount = batcher.GetSegmentCount();
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        const typename TBatcher::SegmentSpans info = batcher.GetSegmentSpans(i);
        if (info.Indices.empty())
        {
          ++stats.VertexBufferCount;
        }
        else
        {
          assert(!info.Vertices.empty());
          assert(!info.Indices.empty());
          ++stats.VertexBufferCount;
          ++stats.IndexBufferCount;
        }
      }
      return stats;
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

          const auto* pBasicMaterial = BasicSpriteMaterialUtil::TryUpcast(materialInfo.Material.get());
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
                 RenderPerformanceCapture* const pPerformanceCapture, const uint32_t maxDrawCalls, const bool isNewCommandBuffer)
    {
      UploadStats uploadStats;

      if (isNewCommandBuffer)
      {
        if (pPerformanceCapture != nullptr)
        {
          pPerformanceCapture->Begin(RenderPerformanceCaptureId::UpdateBuffers);
        }

        uploadStats = UploadMeshChanges(rBuffers, renderSystem, batcher);

        if (pPerformanceCapture != nullptr)
        {
          pPerformanceCapture->EndThenBegin(RenderPerformanceCaptureId::UpdateBuffers, RenderPerformanceCaptureId::ScheduleDraw);
        }
      }
      else
      {
        // Calculate the stats so they can be shown correctly
        uploadStats = CalcUploadStats(batcher);
        if (pPerformanceCapture != nullptr)
        {
          pPerformanceCapture->Begin(RenderPerformanceCaptureId::UpdateBuffers);
          pPerformanceCapture->EndThenBegin(RenderPerformanceCaptureId::UpdateBuffers, RenderPerformanceCaptureId::ScheduleDraw);
        }
      }

      DrawStats drawStats;
      DrawMeshes(renderSystem, drawStats, batcher, meshManager, SpanUtil::UncheckedAsReadOnlySpan(rBuffers, 0, batcher.GetSegmentCount()), cameraInfo,
                 maxDrawCalls);


      if (pPerformanceCapture != nullptr)
      {
        pPerformanceCapture->End(RenderPerformanceCaptureId::ScheduleDraw);
      }

      UpdateStats(rStats, batcher, meshManager, uploadStats, drawStats);
    }


    template <typename TBatcher, typename TPreprocessor>
    void DoDraw(RenderSystemStats& rStats, IBasicRenderSystem& renderSystem, MeshManager& rMeshManager,
                std::vector<RenderSystemBufferRecord>& rBuffers, std::vector<ProcessedCommandRecord>& rProcessedCommandRecords, TBatcher& rBatcher,
                DrawCommandBufferEx& rCommandBuffer, const BasicCameraInfo& cameraInfo, TPreprocessor& rPreprocessor,
                RenderPerformanceCapture* const pPerformanceCapture, const uint32_t maxDrawCalls, const bool isNewCommandBuffer)
    {
      if (isNewCommandBuffer)
      {
        auto capacity = rMeshManager.GetCapacity();
        rBatcher.EnsureCapacity(capacity.VertexCapacity, capacity.IndexCapacity);
      }

      try
      {
        UITextMeshBuilder& rTextMeshBuilder = rMeshManager.GetTextMeshBuilder();

        if (isNewCommandBuffer)
        {
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

              // FSLLOG3_INFO("commandSpan:{} Opaque:{} Transparent:{}", commandSpan.size(), opaqueSpan.size(), transparentSpan.size());

              if (pPerformanceCapture != nullptr)
              {
                pPerformanceCapture->End(RenderPerformanceCaptureId::GenerateMeshes);
              }
            }
          }
          rBatcher.EndBatch();
        }
        else if (pPerformanceCapture != nullptr)
        {
          pPerformanceCapture->Begin(RenderPerformanceCaptureId::PreprocessDrawCommands);
          pPerformanceCapture->EndThenBegin(RenderPerformanceCaptureId::PreprocessDrawCommands, RenderPerformanceCaptureId::GenerateMeshes);
          pPerformanceCapture->End(RenderPerformanceCaptureId::GenerateMeshes);
        }

        // Time to upload and draw the meshes
        DrawNow(rStats, renderSystem, rMeshManager, rBuffers, rBatcher, cameraInfo, pPerformanceCapture, maxDrawCalls, isNewCommandBuffer);
      }
      catch (std::exception& ex)
      {
        rBatcher.ForceEndBatch();
        FSLLOG3_ERROR("Exception {}", ex.what());
        throw;
      }
    }
  }

  RenderSystem::RenderSystem(const RenderSystemCreateInfo& createInfo)
    : RenderSystemBase(createInfo)
    , m_batcher(createInfo.DefaultVertexCapacity, createInfo.DefaultIndexCapacity)
    , m_preprocessor(GetAllowDepthBuffer(), GetWindowMetrics().GetSizePx())
  {
  }


  void RenderSystem::OnConfigurationChanged(const BasicWindowMetrics& windowMetrics)
  {
    RenderSystemBase::OnConfigurationChanged(windowMetrics);
    m_preprocessor.OnConfigurationChanged(GetWindowMetrics().GetSizePx());
  }


  void RenderSystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
  {
    const bool isNewCommandBuffer = IsNewCommandBuffer();

    const BasicCameraInfo cameraInfo(GetMatrixProjection());

    m_preprocessor.SetAllowDepthBuffer(GetAllowDepthBuffer());

    DoDraw(DoGetStats(), GetRenderSystem(), DoGetMeshManager(), GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
           m_preprocessor, pPerformanceCapture, 0xFFFFFFFF, isNewCommandBuffer);
  }


  DefaultRenderSystem::DefaultRenderSystem(const RenderSystemCreateInfo& createInfo)
    : RenderSystemBase(createInfo)
    , m_batcher(createInfo.DefaultVertexCapacity, createInfo.DefaultIndexCapacity)
    , m_preprocessor(createInfo.AllowDepthBuffer, GetWindowMetrics().GetSizePx())
  {
  }

  void DefaultRenderSystem::OnConfigurationChanged(const BasicWindowMetrics& windowMetrics)
  {
    RenderSystemBase::OnConfigurationChanged(windowMetrics);
    m_preprocessor.OnConfigurationChanged(GetWindowMetrics().GetSizePx());
  }

  void DefaultRenderSystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
  {
    const bool isNewCommandBuffer = IsNewCommandBuffer();
    const BasicCameraInfo cameraInfo(GetMatrixProjection());

    MeshManager& rMeshManager = DoGetMeshManager();
    DoDraw(DoGetStats(), GetRenderSystem(), rMeshManager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
           m_preprocessor, pPerformanceCapture, m_maxDrawCalls, isNewCommandBuffer);
  }


  FlexRenderSystem::FlexRenderSystem(const RenderSystemCreateInfo& createInfo)
    : RenderSystemBase(createInfo)
    , m_batcher(createInfo.DefaultVertexCapacity, createInfo.DefaultIndexCapacity)
    , m_config(true, true, GetAllowDepthBuffer(), DrawReorderMethod::LinearConstrained)
    , m_preprocessor(createInfo.AllowDepthBuffer, GetWindowMetrics().GetSizePx())
    , m_spatialGridPreprocessor(createInfo.WindowMetrics.GetSizePx(), createInfo.AllowDepthBuffer)
  {
    SetConfig(m_config);
  }


  void FlexRenderSystem::OnConfigurationChanged(const BasicWindowMetrics& windowMetrics)
  {
    RenderSystemBase::OnConfigurationChanged(windowMetrics);
    m_preprocessor.OnConfigurationChanged(windowMetrics.GetSizePx());
    m_spatialGridPreprocessor.OnConfigurationChanged(windowMetrics.GetSizePx());
  }

  void FlexRenderSystem::Draw(RenderPerformanceCapture* const pPerformanceCapture)
  {
    const bool isNewCommandBuffer = IsNewCommandBuffer();

    MeshManager& rMeshManager = DoGetMeshManager();
    const bool allowDepthBuffer = m_config.UseDepthBuffer;
    const uint32_t maxDrawCalls = m_maxDrawCalls;

    // const PxSize2D sizePx = GetWindowMetrics().GetSizePx();
    const BasicCameraInfo cameraInfo(GetMatrixProjection());
    if (m_config.ReorderMethod == DrawReorderMethod::Disabled)
    {
      BasicPreprocessor preprocessor(allowDepthBuffer, GetWindowMetrics().GetSizePx());
      DoDraw(DoGetStats(), GetRenderSystem(), rMeshManager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
             preprocessor, pPerformanceCapture, maxDrawCalls, isNewCommandBuffer);
    }
    else if (m_config.ReorderMethod == DrawReorderMethod::LinearConstrained)
    {
      m_preprocessor.SetAllowDepthBuffer(allowDepthBuffer);

      DoDraw(DoGetStats(), GetRenderSystem(), rMeshManager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
             m_preprocessor, pPerformanceCapture, maxDrawCalls, isNewCommandBuffer);
    }
    else if (m_config.ReorderMethod == DrawReorderMethod::SpatialGrid)
    {
      m_spatialGridPreprocessor.SetAllowDepthBuffer(allowDepthBuffer);

      DoDraw(DoGetStats(), GetRenderSystem(), rMeshManager, GetBuffers(), m_processedCommandRecords, m_batcher, GetCommandBuffer(), cameraInfo,
             m_spatialGridPreprocessor, pPerformanceCapture, maxDrawCalls, isNewCommandBuffer);
    }
  }
}
