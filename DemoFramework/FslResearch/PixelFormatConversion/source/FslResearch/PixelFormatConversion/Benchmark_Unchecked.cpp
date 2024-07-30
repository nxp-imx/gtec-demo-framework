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

#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/Bitmap/UncheckedRawBitmapTransformer.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <benchmark/benchmark.h>
#include <random>


using namespace Fsl;

namespace
{
  TightBitmap CreateSrcBitmap(const PixelFormat pixelFormat)
  {
    return {PxSize2D::Create(4000, 3000), pixelFormat, BitmapOrigin::UpperLeft};
  }

  uint8_t TestOp(const uint8_t val)
  {
    return val + 10;
  }


  // NOLINTNEXTLINE(readability-identifier-naming)
  void TransformChannelsRAW_ThreeChannels_NoMemoryOverlap(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      UncheckedRawBitmapTransformer::TransformChannelsRAWNoMemoryOverlap<uint8_t, uint8_t, 3>(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap(),
                                                                                              TestOp);
    }
  }


  // NOLINTNEXTLINE(readability-identifier-naming)
  void TransformThreeChannelsRAW_NoMemoryOverlap(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      UncheckedRawBitmapTransformer::TransformThreeChannelsRAW<uint8_t, uint8_t>(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap(), TestOp);
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  // NOLINTNEXTLINE(readability-identifier-naming)
  void TransformChannelsRAW_FourChannels_NoMemoryOverlap(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8A8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      UncheckedRawBitmapTransformer::TransformChannelsRAWNoMemoryOverlap<uint8_t, uint8_t, 4>(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap(),
                                                                                              TestOp);
    }
  }


  // NOLINTNEXTLINE(readability-identifier-naming)
  void TransformFourChannelsRAW_NoMemoryOverlap(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8A8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      UncheckedRawBitmapTransformer::TransformFourChannelsRAW<uint8_t, uint8_t>(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap(), TestOp);
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8SrgbToR16G16B16UNorm(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8A8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8SrgbToR16G16B16Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8A8SrgbToR16G16B16A16Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8A8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8SrgbToR32G32B32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR8G8B8A8SrgbToR32G32B32A32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R8G8B8A8_SRGB));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR16G16B16FloatToR32G32B32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R16G16B16_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }


  void UncheckedR32G32B32FloatToR16G16B16Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R32G32B32_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR16G16B16A16FloatToR32G32B32A32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R16G16B16A16_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }


  void UncheckedR32G32B32A32FloatToR16G16B16A16Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R32G32B32A32_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R32G32B32A32_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  void UncheckedR16G16B16A16UNormToR32G32B32A32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R16G16B16A16_UNORM));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }


  void UncheckedR32G32B32FloatToR16G16B16UNorm(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R32G32B32_SFLOAT));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

  void UncheckedR16G16B16UNormToR32G32B32Float(benchmark::State& state)
  {
    TightBitmap srcBitmap(CreateSrcBitmap(PixelFormat::R16G16B16_UNORM));
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

    for (auto _ : state)
    {
      // This code gets timed
      FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    }
  }

}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

BENCHMARK(TransformChannelsRAW_ThreeChannels_NoMemoryOverlap);
BENCHMARK(TransformThreeChannelsRAW_NoMemoryOverlap);

BENCHMARK(TransformChannelsRAW_FourChannels_NoMemoryOverlap);
BENCHMARK(TransformFourChannelsRAW_NoMemoryOverlap);

BENCHMARK(UncheckedR8G8B8SrgbToR16G16B16UNorm);
BENCHMARK(UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm);

BENCHMARK(UncheckedR8G8B8SrgbToR16G16B16Float);
BENCHMARK(UncheckedR8G8B8A8SrgbToR16G16B16A16Float);

BENCHMARK(UncheckedR8G8B8SrgbToR32G32B32Float);
BENCHMARK(UncheckedR8G8B8A8SrgbToR32G32B32A32Float);

BENCHMARK(UncheckedR16G16B16A16FloatToR32G32B32A32Float);
BENCHMARK(UncheckedR32G32B32A32FloatToR16G16B16A16Float);

BENCHMARK(UncheckedR16G16B16FloatToR32G32B32Float);
BENCHMARK(UncheckedR32G32B32FloatToR16G16B16Float);

BENCHMARK(UncheckedR16G16B16A16UNormToR32G32B32A32Float);
BENCHMARK(UncheckedR32G32B32A32FloatToR16G16B16A16UNorm);

BENCHMARK(UncheckedR16G16B16UNormToR32G32B32Float);
BENCHMARK(UncheckedR32G32B32FloatToR16G16B16UNorm);
