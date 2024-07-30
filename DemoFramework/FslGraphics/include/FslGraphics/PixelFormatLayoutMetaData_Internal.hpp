#ifndef FSLGRAPHICS_PIXELFORMATLAYOUTMETADATA_INTERNAL_HPP
#define FSLGRAPHICS_PIXELFORMATLAYOUTMETADATA_INTERNAL_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormatLayout.hpp>
#include <array>
#include <cassert>

namespace Fsl::PixelFormatLayoutMetaData::Internal
{
  enum class SwizzleCompatibilityId
  {
    Undefined,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R4G4B4A4_PACK16,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G6B5_PACK16,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G5B5A1_PACK16,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8,
  };


  // A mirror layout is considered to be 'swizzle' compatible with the associated format
  struct MirrorLayout
  {
    PixelFormatLayout Layout1{PixelFormatLayout::Undefined};
    PixelFormatLayout Layout2{PixelFormatLayout::Undefined};

    constexpr MirrorLayout() noexcept = default;

    constexpr explicit MirrorLayout(const PixelFormatLayout layout) noexcept
      : Layout1(layout)
    {
    }

    constexpr MirrorLayout(const PixelFormatLayout layout1, const PixelFormatLayout layout2) noexcept
      : Layout1(layout1)
      , Layout2(layout2)
    {
    }
  };

  struct PixelFormatLayoutInfo
  {
  private:
    uint8_t m_channelCount{0};
    uint8_t m_hasAlphaChannel{0};

  public:
    constexpr PixelFormatLayoutInfo() noexcept = default;
    explicit constexpr PixelFormatLayoutInfo(const uint8_t channelCount, const bool hasAlphaChannel = false) noexcept
      : m_channelCount(channelCount)
      , m_hasAlphaChannel(hasAlphaChannel ? 1u : 0u)
    {
    }

    constexpr uint8_t GetChannelCount() const noexcept
    {
      return m_channelCount;
    }

    constexpr bool HasAlphaChannel() const noexcept
    {
      return m_hasAlphaChannel != 0u;
    }
  };


  struct PixelFormatLayoutRecord
  {
    PixelFormatLayout Layout{PixelFormatLayout::Undefined};
    PixelChannelOrder ChannelOrder{PixelChannelOrder::Undefined};
    SwizzleCompatibilityId SwizzleCompatibility{SwizzleCompatibilityId::Undefined};
    MirrorLayout Mirror;
    PixelFormatLayoutInfo Info;

    constexpr PixelFormatLayoutRecord() noexcept = default;

    constexpr PixelFormatLayoutRecord(const PixelFormatLayout layout, const PixelFormatLayoutInfo info, const PixelChannelOrder channelOrder) noexcept
      : Layout(layout)
      , ChannelOrder(channelOrder)
      , Info(info)
    {
    }

    constexpr PixelFormatLayoutRecord(const PixelFormatLayout layout, const PixelFormatLayoutInfo info, const PixelChannelOrder channelOrder,
                                      const SwizzleCompatibilityId swizzleCompatibilityId) noexcept
      : Layout(layout)
      , ChannelOrder(channelOrder)
      , SwizzleCompatibility(swizzleCompatibilityId)
      , Info(info)
    {
    }


    constexpr PixelFormatLayoutRecord(const PixelFormatLayout layout, const PixelFormatLayoutInfo info, const PixelChannelOrder channelOrder,
                                      const MirrorLayout& mirror) noexcept
      : Layout(layout)
      , ChannelOrder(channelOrder)
      , Mirror(mirror)
      , Info(info)
    {
      assert(layout != mirror.Layout1 || layout == PixelFormatLayout::Undefined);
      assert(layout != mirror.Layout2 || layout == PixelFormatLayout::Undefined);
    }

    constexpr PixelFormatLayoutRecord(const PixelFormatLayout layout, const PixelFormatLayoutInfo info, const PixelChannelOrder channelOrder,
                                      const SwizzleCompatibilityId swizzleCompatibilityId, const MirrorLayout& mirror) noexcept
      : Layout(layout)
      , ChannelOrder(channelOrder)
      , SwizzleCompatibility(swizzleCompatibilityId)
      , Mirror(mirror)
      , Info(info)
    {
      assert(layout != mirror.Layout1 || layout == PixelFormatLayout::Undefined);
      assert(layout != mirror.Layout2 || layout == PixelFormatLayout::Undefined);
    }
  };

  inline constexpr const std::array<PixelFormatLayoutRecord, 66> PixelFormatLayouts = {
    PixelFormatLayoutRecord(),
    PixelFormatLayoutRecord(PixelFormatLayout::R4G4, PixelFormatLayoutInfo(2), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R4G4B4A4_PACK16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA,
                            SwizzleCompatibilityId::R4G4B4A4_PACK16, MirrorLayout(PixelFormatLayout::B4G4R4A4_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::B4G4R4A4_PACK16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::BGRA,
                            SwizzleCompatibilityId::R4G4B4A4_PACK16, MirrorLayout(PixelFormatLayout::R4G4B4A4_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::R5G6B5_PACK16, PixelFormatLayoutInfo(3), PixelChannelOrder::RGBA,
                            SwizzleCompatibilityId::R5G6B5_PACK16, MirrorLayout(PixelFormatLayout::B5G6R5_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::B5G6R5_PACK16, PixelFormatLayoutInfo(3), PixelChannelOrder::BGRA,
                            SwizzleCompatibilityId::R5G6B5_PACK16, MirrorLayout(PixelFormatLayout::R5G6B5_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::R5G5B5A1_PACK16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA,
                            SwizzleCompatibilityId::R5G6B5_PACK16,
                            MirrorLayout(PixelFormatLayout::B5G5R5A1_PACK16, PixelFormatLayout::A1R5G5B5_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::B5G5R5A1_PACK16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::BGRA,
                            SwizzleCompatibilityId::R5G6B5_PACK16,
                            MirrorLayout(PixelFormatLayout::R5G5B5A1_PACK16, PixelFormatLayout::A1R5G5B5_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::A1R5G5B5_PACK16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::ARGB,
                            MirrorLayout(PixelFormatLayout::R5G5B5A1_PACK16, PixelFormatLayout::B5G5R5A1_PACK16)),
    PixelFormatLayoutRecord(PixelFormatLayout::R8, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::R8G8, PixelFormatLayoutInfo(2), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R8G8B8, PixelFormatLayoutInfo(3), PixelChannelOrder::RGBA, SwizzleCompatibilityId::R8G8B8,
                            MirrorLayout(PixelFormatLayout::B8G8R8)),
    PixelFormatLayoutRecord(PixelFormatLayout::B8G8R8, PixelFormatLayoutInfo(3), PixelChannelOrder::BGRA, SwizzleCompatibilityId::R8G8B8,
                            MirrorLayout(PixelFormatLayout::R8G8B8)),
    PixelFormatLayoutRecord(PixelFormatLayout::R8G8B8A8, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA, SwizzleCompatibilityId::R8G8B8A8,
                            MirrorLayout(PixelFormatLayout::B8G8R8A8)),
    PixelFormatLayoutRecord(PixelFormatLayout::B8G8R8A8, PixelFormatLayoutInfo(4, true), PixelChannelOrder::BGRA, SwizzleCompatibilityId::R8G8B8A8,
                            MirrorLayout(PixelFormatLayout::R8G8B8A8)),
    PixelFormatLayoutRecord(PixelFormatLayout::A8B8G8R8_PACK32, PixelFormatLayoutInfo(4, true), PixelChannelOrder::ABGR),
    PixelFormatLayoutRecord(PixelFormatLayout::A2R10G10B10_PACK32, PixelFormatLayoutInfo(4, true), PixelChannelOrder::ARGB,
                            MirrorLayout(PixelFormatLayout::A2B10G10R10_PACK32)),
    PixelFormatLayoutRecord(PixelFormatLayout::A2B10G10R10_PACK32, PixelFormatLayoutInfo(4, true), PixelChannelOrder::ABGR,
                            MirrorLayout(PixelFormatLayout::A2R10G10B10_PACK32)),
    PixelFormatLayoutRecord(PixelFormatLayout::R16, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::R16G16, PixelFormatLayoutInfo(2), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R16G16B16, PixelFormatLayoutInfo(3), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R16G16B16A16, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R32, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::R32G32, PixelFormatLayoutInfo(2), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R32G32B32, PixelFormatLayoutInfo(3), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R32G32B32A32, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R64, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::R64G64, PixelFormatLayoutInfo(2), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R64G64B64, PixelFormatLayoutInfo(3), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::R64G64B64A64, PixelFormatLayoutInfo(4, true), PixelChannelOrder::RGBA),
    PixelFormatLayoutRecord(PixelFormatLayout::B10G11R11_PACK32, PixelFormatLayoutInfo(3), PixelChannelOrder::BGRA),
    PixelFormatLayoutRecord(PixelFormatLayout::E5B9G9R9_PACK32, PixelFormatLayoutInfo(4), PixelChannelOrder::BGRA),
    PixelFormatLayoutRecord(PixelFormatLayout::D16, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::X8_D24_PACK32, PixelFormatLayoutInfo(2), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::D32, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::S8, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::D16_S8, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::D24_S8, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::D32_S8, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::BC1_RGB_BLOCK, PixelFormatLayoutInfo(3), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::BC1_RGBA_BLOCK, PixelFormatLayoutInfo(4, true), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::BC2_BLOCK, PixelFormatLayoutInfo(4, true), PixelChannelOrder::Undefined),    // Something like R5G6B5A4
    PixelFormatLayoutRecord(PixelFormatLayout::BC3_BLOCK, PixelFormatLayoutInfo(4, true), PixelChannelOrder::Undefined),    // Something like R5G6B5A8
    PixelFormatLayoutRecord(PixelFormatLayout::BC4_BLOCK, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::BC5_BLOCK, PixelFormatLayoutInfo(2), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::BC6H_BLOCK, PixelFormatLayoutInfo(3),
                            PixelChannelOrder::Undefined),    // Something like R16G16B16 half float
    PixelFormatLayoutRecord(PixelFormatLayout::BC7_BLOCK, PixelFormatLayoutInfo(4, true),
                            PixelChannelOrder::Undefined),    // Something like R4-7, G4-7, B4-7 A8
    PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8_BLOCK, PixelFormatLayoutInfo(3), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK, PixelFormatLayoutInfo(4, true), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK, PixelFormatLayoutInfo(4, true), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::EAC_R11_BLOCK, PixelFormatLayoutInfo(1), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::EAC_R11G11_BLOCK, PixelFormatLayoutInfo(2), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_4x4_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_5x4_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_5x5_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_6x5_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_6x6_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x5_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x6_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x8_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x5_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x6_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x8_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x10_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_12x10_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
    PixelFormatLayoutRecord(PixelFormatLayout::ASTC_12x12_BLOCK, PixelFormatLayoutInfo(0), PixelChannelOrder::Undefined),
  };

  static_assert((sizeof(PixelFormatLayouts) / sizeof(PixelFormatLayoutRecord)) == static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE),
                "g_pixelFormatLayouts needs to match the size of the enum range");
}

#endif
