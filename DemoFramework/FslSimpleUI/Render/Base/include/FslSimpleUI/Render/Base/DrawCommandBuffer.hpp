#ifndef FSLSIMPLEUI_RENDER_BASE_DRAWCOMMANDBUFFER_HPP
#define FSLSIMPLEUI_RENDER_BASE_DRAWCOMMANDBUFFER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomNineSliceAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawCustomTextAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CommandDrawRot90CWAtOffsetAndSize.hpp>
#include <FslSimpleUI/Render/Base/Command/CustomDrawBasicImageBasicMeshInfo.hpp>
#include <FslSimpleUI/Render/Base/Command/CustomDrawBasicImageInfo.hpp>
#include <FslSimpleUI/Render/Base/Command/CustomDrawNineSliceInfo.hpp>
#include <FslSimpleUI/Render/Base/Command/CustomDrawTextInfo.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>
#include <FslSimpleUI/Render/Base/MeshHandle.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <utility>
#include <vector>

namespace Fsl::UI
{
  class DrawCommandBuffer
  {
    std::vector<EncodedCommand> m_commandRecords;
    std::size_t m_commandCount{0};
    std::vector<CustomDrawBasicImageInfo> m_customDrawBasicImage;
    std::vector<CustomDrawBasicImageBasicMeshInfo> m_customDrawBasicImageBasicMesh;
    std::vector<CustomDrawNineSliceInfo> m_customDrawNineSlice;
    std::vector<CustomDrawTextInfo> m_customDrawText;
    uint32_t m_customDrawBasicImageCount{0};
    uint32_t m_customDrawBasicImageBasicMeshCount{0};
    uint32_t m_customDrawNineSliceCount{0};
    uint32_t m_customDrawTextCount{0};

  public:
    DrawCommandBuffer()
      : m_commandRecords(2048u)
      , m_customDrawBasicImage(128u)
      , m_customDrawBasicImageBasicMesh(128u)
      , m_customDrawNineSlice(128u)
      , m_customDrawText(128u)
    {
    }

    //! @brief Get the current command count
    std::size_t Count() const
    {
      return m_commandCount;
    }

    //! @brief Get the current command capacity
    std::size_t Capacity() const
    {
      return m_commandRecords.size();
    }

    inline void Draw(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                     const DrawClipContext& clipContext)
    {
      if (Check(hMesh, dstColor, dstSizePx))
      {
        AddCommand(CommandDrawAtOffsetAndSize::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext));
      }
    }

    inline void DrawRotated90CW(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                                const DrawClipContext& clipContext)
    {
      if (Check(hMesh, dstColor, dstSizePx))
      {
        AddCommand(CommandDrawRot90CWAtOffsetAndSize::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext));
      }
    }

    inline void DrawCustom(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                           const DrawClipContext& clipContext, FnDrawCustomBasicImageMesh fnDrawCustomMesh,
                           const std::shared_ptr<ICustomDrawData>& customData)
    {
      if (Check(hMesh, dstColor, dstSizePx) && fnDrawCustomMesh != nullptr)
      {
        const uint32_t customDrawIndex = AddCustomDraw(CustomDrawBasicImageInfo(fnDrawCustomMesh, customData));
        AddCommand(CommandDrawCustomBasicImageAtOffsetAndSize::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext, customDrawIndex));
      }
    }

    inline void DrawCustom(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                           const DrawClipContext& clipContext, FnDrawCustomBasicImageBasicMesh fnDrawCustomMesh,
                           const std::shared_ptr<ICustomDrawData>& customData)
    {
      if (Check(hMesh, dstColor, dstSizePx) && fnDrawCustomMesh != nullptr)
      {
        const uint32_t customDrawIndex = AddCustomDraw(CustomDrawBasicImageBasicMeshInfo(fnDrawCustomMesh, customData));
        AddCommand(
          CommandDrawCustomBasicImageAtOffsetAndSizeBasicMesh::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext, customDrawIndex));
      }
    }

    inline void DrawCustom(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                           const DrawClipContext& clipContext, FnDrawCustomNineSliceMesh fnDrawCustomMesh,
                           const std::shared_ptr<ICustomDrawData>& customData)
    {
      if (Check(hMesh, dstColor, dstSizePx) && fnDrawCustomMesh != nullptr)
      {
        const uint32_t customDrawIndex = AddCustomDraw(CustomDrawNineSliceInfo(fnDrawCustomMesh, customData));
        AddCommand(CommandDrawCustomNineSliceAtOffsetAndSize::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext, customDrawIndex));
      }
    }

    inline void DrawCustom(const MeshHandle hMesh, const PxVector2 dstPositionPxf, const PxSize2D dstSizePx, const UIRenderColor dstColor,
                           const DrawClipContext& clipContext, FnDrawCustomTextMesh fnDrawCustomMesh,
                           const std::shared_ptr<ICustomDrawData>& customData)
    {
      if (Check(hMesh, dstColor, dstSizePx) && fnDrawCustomMesh != nullptr)
      {
        const uint32_t customDrawIndex = AddCustomTextDraw(CustomDrawTextInfo(fnDrawCustomMesh, customData));
        AddCommand(CommandDrawCustomTextAtOffsetAndSize::Encode(hMesh, dstPositionPxf, dstSizePx, dstColor, clipContext, customDrawIndex));
      }
    }

    // inline void Draw(const MeshHandle hMesh, const PxAreaRectangleF& dstAreaRectanglePxf, const UIRenderColor dstColor)
    //{
    //   if (Check(hMesh, dstColor, dstAreaRectanglePxf.Size()))
    //   {
    //     AddCommand(CommandDrawAtOffsetAndSize::Encode(hMesh, dstAreaRectanglePxf.Location(), dstAreaRectanglePxf.GetSize(), dstColor));
    //   }
    // }


  protected:
    inline constexpr static bool Check(const MeshHandle hMesh, const UIRenderColor dstColor, const PxSize2D dstSizePx) noexcept
    {
      return hMesh.IsValid() && dstColor.RawA() > 0 && dstSizePx.RawWidth() > 0 && dstSizePx.RawHeight() > 0;
    }

    // inline constexpr static bool Check(const MeshHandle hMesh, const UIRenderColor dstColor, const PxSize2DF& dstSizePxf) noexcept
    //{
    //   return hMesh.IsValid() && dstColor.RawA() > 0 && dstSizePxf.RawWidth() > 0 && dstSizePxf.RawHeight() > 0;
    // }

    void DoClear()
    {
      for (uint32_t i = 0; i < m_customDrawBasicImageCount; ++i)
      {
        m_customDrawBasicImage[i] = {};
      }
      for (uint32_t i = 0; i < m_customDrawBasicImageBasicMeshCount; ++i)
      {
        m_customDrawBasicImageBasicMesh[i] = {};
      }
      for (uint32_t i = 0; i < m_customDrawNineSliceCount; ++i)
      {
        m_customDrawNineSlice[i] = {};
      }
      for (uint32_t i = 0; i < m_customDrawTextCount; ++i)
      {
        m_customDrawText[i] = {};
      }
      m_commandCount = 0;
      m_customDrawBasicImageCount = 0;
      m_customDrawBasicImageBasicMeshCount = 0;
      m_customDrawNineSliceCount = 0;
      m_customDrawTextCount = 0;
    }

    inline ReadOnlySpan<EncodedCommand> DoAsReadOnlySpan() const
    {
      return ReadOnlySpan<EncodedCommand>(m_commandRecords.data(), m_commandCount);
    }

    inline const CustomDrawBasicImageInfo& DoFastGetCustomDrawBasicImageInfo(const uint32_t index) const noexcept
    {
      assert(index < m_customDrawBasicImageCount);
      return m_customDrawBasicImage[index];
    }

    inline const CustomDrawBasicImageBasicMeshInfo& DoFastGetCustomDrawBasicImageBasicMeshInfo(const uint32_t index) const noexcept
    {
      assert(index < m_customDrawBasicImageBasicMeshCount);
      return m_customDrawBasicImageBasicMesh[index];
    }

    inline const CustomDrawNineSliceInfo& DoFastGetCustomDrawNineSliceInfo(const uint32_t index) const noexcept
    {
      assert(index < m_customDrawNineSliceCount);
      return m_customDrawNineSlice[index];
    }

    inline const CustomDrawTextInfo& DoFastGetCustomDrawTextInfo(const uint32_t index) const noexcept
    {
      assert(index < m_customDrawTextCount);
      return m_customDrawText[index];
    }

  private:
    inline void AddCommand(EncodedCommand record)
    {
      if (m_commandCount >= m_commandRecords.size())
      {
        m_commandRecords.resize(m_commandRecords.size() + 2048u);
      }
      m_commandRecords[m_commandCount] = record;
      ++m_commandCount;
    }

    inline uint32_t AddCustomDraw(CustomDrawBasicImageInfo customRecord)
    {
      if (m_customDrawBasicImageCount >= m_customDrawBasicImage.size())
      {
        m_customDrawBasicImage.resize(m_customDrawBasicImage.size() + 2048u);
      }
      m_customDrawBasicImage[m_customDrawBasicImageCount] = std::move(customRecord);
      ++m_customDrawBasicImageCount;
      return m_customDrawBasicImageCount - 1;
    }

    inline uint32_t AddCustomDraw(CustomDrawBasicImageBasicMeshInfo customRecord)
    {
      if (m_customDrawBasicImageBasicMeshCount >= m_customDrawBasicImageBasicMesh.size())
      {
        m_customDrawBasicImageBasicMesh.resize(m_customDrawBasicImageBasicMesh.size() + 2048u);
      }
      m_customDrawBasicImageBasicMesh[m_customDrawBasicImageBasicMeshCount] = std::move(customRecord);
      ++m_customDrawBasicImageBasicMeshCount;
      return m_customDrawBasicImageBasicMeshCount - 1;
    }


    inline uint32_t AddCustomDraw(CustomDrawNineSliceInfo customRecord)
    {
      if (m_customDrawNineSliceCount >= m_customDrawNineSlice.size())
      {
        m_customDrawNineSlice.resize(m_customDrawNineSlice.size() + 2048u);
      }
      m_customDrawNineSlice[m_customDrawNineSliceCount] = std::move(customRecord);
      ++m_customDrawNineSliceCount;
      return m_customDrawNineSliceCount - 1;
    }

    inline uint32_t AddCustomTextDraw(CustomDrawTextInfo customRecord)
    {
      if (m_customDrawTextCount >= m_customDrawText.size())
      {
        m_customDrawText.resize(m_customDrawText.size() + 2048u);
      }
      m_customDrawText[m_customDrawTextCount] = std::move(customRecord);
      ++m_customDrawTextCount;
      return m_customDrawTextCount - 1;
    }
  };
}

#endif
