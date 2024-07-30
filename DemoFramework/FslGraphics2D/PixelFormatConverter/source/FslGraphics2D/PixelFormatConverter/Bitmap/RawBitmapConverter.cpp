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

#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverter.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <array>

namespace Fsl::FslGraphics2D::RawBitmapConverter
{
  namespace
  {
    constexpr std::array<SupportedConversion, 27> SupportedConversions = {
      SupportedConversion(PixelFormat::R8G8B8_SRGB, PixelFormat::R16G16B16_UNORM),
      SupportedConversion(PixelFormat::R8G8B8_SRGB, PixelFormat::R16G16B16_SFLOAT),
      SupportedConversion(PixelFormat::R8G8B8_SRGB, PixelFormat::R32G32B32_SFLOAT),
      SupportedConversion(PixelFormat::R8G8B8A8_SRGB, PixelFormat::R16G16B16A16_UNORM),
      SupportedConversion(PixelFormat::R8G8B8A8_SRGB, PixelFormat::R16G16B16A16_SFLOAT),
      SupportedConversion(PixelFormat::R8G8B8A8_SRGB, PixelFormat::R32G32B32A32_SFLOAT),
      SupportedConversion(PixelFormat::R16G16B16_SFLOAT, PixelFormat::R32G32B32_SFLOAT),
      SupportedConversion(PixelFormat::R16G16B16_UNORM, PixelFormat::R32G32B32_SFLOAT),
      SupportedConversion(PixelFormat::R16G16B16A16_SFLOAT, PixelFormat::R32G32B32A32_SFLOAT),
      SupportedConversion(PixelFormat::R16G16B16A16_UNORM, PixelFormat::R32G32B32A32_SFLOAT),
      SupportedConversion(PixelFormat::R32G32B32_SFLOAT, PixelFormat::R16G16B16_SFLOAT),
      SupportedConversion(PixelFormat::R32G32B32_SFLOAT, PixelFormat::R16G16B16_UNORM),
      SupportedConversion(PixelFormat::R32G32B32_SFLOAT, PixelFormat::R16G16B16A16_SFLOAT),
      SupportedConversion(PixelFormat::R32G32B32A32_SFLOAT, PixelFormat::R16G16B16A16_SFLOAT),
      SupportedConversion(PixelFormat::R32G32B32A32_SFLOAT, PixelFormat::R16G16B16A16_UNORM),
      SupportedConversion(PixelFormat::R16G16B16_UNORM, PixelFormat::R8G8B8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16_SFLOAT, PixelFormat::R8G8B8_SRGB),
      SupportedConversion(PixelFormat::R32G32B32_SFLOAT, PixelFormat::R8G8B8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16A16_UNORM, PixelFormat::R8G8B8A8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16A16_SFLOAT, PixelFormat::R8G8B8A8_SRGB),
      SupportedConversion(PixelFormat::R32G32B32A32_SFLOAT, PixelFormat::R8G8B8A8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16_UNORM, PixelFormat::B8G8R8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16_SFLOAT, PixelFormat::B8G8R8_SRGB),
      SupportedConversion(PixelFormat::R32G32B32_SFLOAT, PixelFormat::B8G8R8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16A16_UNORM, PixelFormat::B8G8R8A8_SRGB),
      SupportedConversion(PixelFormat::R16G16B16A16_SFLOAT, PixelFormat::B8G8R8A8_SRGB),
      SupportedConversion(PixelFormat::R32G32B32A32_SFLOAT, PixelFormat::B8G8R8A8_SRGB),
    };

    bool UncheckedR8G8B8SrgbToR16G16B16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
    {
      try
      {
        RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap, srcBitmap);
        return true;
      }
      catch (...)
      {
        return false;
      }
    }

    bool UncheckedR8G8B8A8SrgbToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
    {
      try
      {
        RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap, srcBitmap);
        return true;
      }
      catch (...)
      {
        return false;
      }
    }

    bool UncheckedR16G16B16FloatToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
    {
      try
      {
        RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR8G8B8Srgb(dstBitmap, srcBitmap);
        return true;
      }
      catch (...)
      {
        return false;
      }
    }

    bool UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
    {
      try
      {
        RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(dstBitmap, srcBitmap);
        return true;
      }
      catch (...)
      {
        return false;
      }
    }


    bool UncheckedTryTransform(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
    {
      // We support the 'reverse' of the src format conversions also
      if (dstBitmap.GetPixelFormat() == PixelFormat::R8G8B8_SRGB)
      {
        switch (srcBitmap.GetPixelFormat())
        {
        case PixelFormat::R16G16B16_UNORM:
          RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR8G8B8Srgb(dstBitmap, srcBitmap);
          return true;
        case PixelFormat::R16G16B16_SFLOAT:
          UncheckedR16G16B16FloatToR8G8B8Srgb(dstBitmap, srcBitmap);
          return true;
        case PixelFormat::R32G32B32_SFLOAT:
          RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR8G8B8Srgb(dstBitmap, srcBitmap);
          return true;
        default:
          return false;
        }
      }

      if (dstBitmap.GetPixelFormat() == PixelFormat::R8G8B8A8_SRGB)
      {
        switch (srcBitmap.GetPixelFormat())
        {
        case PixelFormat::R16G16B16A16_UNORM:
          RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(dstBitmap, srcBitmap);
          return true;
        case PixelFormat::R16G16B16A16_SFLOAT:
          UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(dstBitmap, srcBitmap);
          return true;
        case PixelFormat::R32G32B32A32_SFLOAT:
          RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(dstBitmap, srcBitmap);
          return true;
        default:
          return false;
        }
      }

      if (dstBitmap.GetPixelFormat() == PixelFormat::B8G8R8_SRGB)
      {
        switch (srcBitmap.GetPixelFormat())
        {
        case PixelFormat::R16G16B16_UNORM:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8_SRGB);
            RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR8G8B8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle24(dstBitmap, 2, 1, 0);
          }
          return true;
        case PixelFormat::R16G16B16_SFLOAT:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8_SRGB);
            UncheckedR16G16B16FloatToR8G8B8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle24(dstBitmap, 2, 1, 0);
          }
          return true;
        case PixelFormat::R32G32B32_SFLOAT:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8_SRGB);
            RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR8G8B8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle24(dstBitmap, 2, 1, 0);
          }
          return true;
        default:
          return false;
        }
      }

      if (dstBitmap.GetPixelFormat() == PixelFormat::B8G8R8A8_SRGB)
      {
        switch (srcBitmap.GetPixelFormat())
        {
        case PixelFormat::R16G16B16A16_UNORM:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8A8_SRGB);
            RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle(dstBitmap, 2, 1, 0, 3);
          }
          return true;
        case PixelFormat::R16G16B16A16_SFLOAT:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8A8_SRGB);
            UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle(dstBitmap, 2, 1, 0, 3);
          }
          return true;
        case PixelFormat::R32G32B32A32_SFLOAT:
          {
            auto dstBitmapMod = RawBitmapEx::UncheckedPatchWithCompatiblePixelFormat(dstBitmap, PixelFormat::R8G8B8A8_SRGB);
            RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(dstBitmapMod, srcBitmap);
            RawBitmapUtil::Swizzle(dstBitmap, 2, 1, 0, 3);
          }
          return true;
        default:
          return false;
        }
      }
      return false;
    }
  }


  ReadOnlySpan<SupportedConversion> GetSupportedConversions() noexcept
  {
    return SpanUtil::AsReadOnlySpan(SupportedConversions);
  }


  bool TryTransform(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    if (dstBitmap.GetOrigin() != srcBitmap.GetOrigin())
    {
      // We only support conversion between the same bitmap origins
      return false;
    }
    if (dstBitmap.GetSize() != srcBitmap.GetSize())
    {
      // We only support conversion between the same bitmap sizes
      return false;
    }
    if (!RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap))
    {
      // We only support converting between bitmaps that obeys the above rules
      return false;
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R8G8B8_SRGB)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R16G16B16_UNORM:
        RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16UNorm(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R16G16B16_SFLOAT:
        UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R32G32B32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR32G32B32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R8G8B8A8_SRGB)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R16G16B16A16_UNORM:
        RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R16G16B16A16_SFLOAT:
        UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R32G32B32A32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR32G32B32A32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R16G16B16_SFLOAT)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R32G32B32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR32G32B32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R16G16B16_UNORM)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R32G32B32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR32G32B32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R16G16B16A16_SFLOAT)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R32G32B32A32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR32G32B32A32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R16G16B16A16_UNORM)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R32G32B32A32_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR32G32B32A32Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R32G32B32_SFLOAT)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R16G16B16_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16Float(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R16G16B16_UNORM:
        RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16UNorm(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R16G16B16A16_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16A16Float(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R32G32B32A32_SFLOAT)
    {
      switch (dstBitmap.GetPixelFormat())
      {
      case PixelFormat::R16G16B16A16_SFLOAT:
        RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16Float(dstBitmap, srcBitmap);
        return true;
      case PixelFormat::R16G16B16A16_UNORM:
        RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(dstBitmap, srcBitmap);
        return true;
      default:
        break;
      }
    }
    return UncheckedTryTransform(dstBitmap, srcBitmap);
  }
}
