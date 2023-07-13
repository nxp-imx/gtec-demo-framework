/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslResearch/SpatialGrid2D/SpatialHashGrid2DFixedBucketSize.hpp>
#include <FslResearch/SpatialGrid2D/SpatialHashGrid2DMap.hpp>
#include <FslResearch/SpatialGrid2D/SpatialHashGrid2DUnorderedMap.hpp>
#include <FslResearch/SpatialGrid2D/SpatialHashGrid2DVector.hpp>
#include <benchmark/benchmark.h>
#include <random>

#define LOCAL_BENCH_TRYADD
#define LOCAL_BENCH_TRYUNCHECKEDADD
#define LOCAL_BENCH_CLEAR
#define LOCAL_BENCH_GETCHUNKENTRIES
#define LOCAL_BENCH_UNCHECKEDGETCHUNKENTRIES
#define LOCAL_BENCH_TRYINSERTAFTERZPOS

#define LOCAL_BENCH_UNORDEREDMAP
#define LOCAL_BENCH_MAP
#define LOCAL_BENCH_VECTOR
#define LOCAL_BENCH_FIXED_BUCKET_SIZE_256
#define LOCAL_BENCH_FIXED_BUCKET_SIZE_32

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr uint32_t ObjectCount = 200;
    constexpr uint32_t RandomChunkCount = 512;
    constexpr uint32_t Seed = 1337;

    constexpr PxSize2D SizePx = PxSize2D::Create(1920, 1080);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  struct DrawRecord
  {
    PxAreaRectangleF Rect;
    uint32_t Id{0};
  };

  void FillVector(std::vector<DrawRecord>& rRecords, const uint32_t seed, const PxSize2D sizePx)
  {
    std::mt19937 random(seed);

    std::uniform_real_distribution<float> randomPositionX(-10.0f, 2000.0f);
    std::uniform_real_distribution<float> randomPositionY(-10.0f, 2000.0f);
    std::uniform_real_distribution<float> randomWidth1(20.0f, 200.0f);
    std::uniform_real_distribution<float> randomWidth2(20.0f, 400.0f);
    std::uniform_real_distribution<float> randomHeight(10.0f, 100.0f);

    const float scaleX = static_cast<float>(sizePx.RawWidth()) / LocalConfig::SizePx.RawWidth();
    const float scaleY = static_cast<float>(sizePx.RawHeight()) / LocalConfig::SizePx.RawHeight();

    uint32_t index = 0;
    for (auto& rRecord : rRecords)
    {
      const float posX = randomPositionX(random) * scaleX;
      const float posY = randomPositionY(random) * scaleY;
      const float width = ((index % 3) != 0 ? randomWidth1(random) : randomWidth2(random)) * scaleX;
      const float height = randomHeight(random) * scaleY;
      rRecord.Rect = PxAreaRectangleF::Create(posX, posY, width, height);
      rRecord.Id = index;
      ++index;
    }
  }

  void FillVector(std::vector<Point2>& rRecords, const uint32_t seed, const uint16_t countX, const uint16_t countY)
  {
    std::mt19937 random(seed);

    std::uniform_int_distribution<uint16_t> randomX(0, countX - 1);
    std::uniform_int_distribution<uint16_t> randomY(0, countY - 1);

    for (auto& rRecord : rRecords)
    {
      rRecord = Point2(randomX(random), randomY(random));
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  // SpatialGrid CreateGrid(const int32_t widthPx, const int32_t heightPx, const int32_t cellsX, const int32_t cellsY)
  //{
  //   const uint32_t desiredStepSizeX = BitsUtil::NextPowerOfTwo(widthPx / cellsX);
  //   const uint32_t desiredStepSizeY = BitsUtil::NextPowerOfTwo(heightPx / cellsY);
  //   const uint32_t shiftX = BitsUtil::IndexOf(desiredStepSizeX);
  //   const uint32_t shiftY = BitsUtil::IndexOf(desiredStepSizeY);
  //   const uint32_t stepsX = 1 << shiftX;
  //   const uint32_t stepsY = 1 << shiftY;
  //   return SpatialGrid(UncheckedNumericCast<uint16_t>(stepsX), UncheckedNumericCast<uint16_t>(stepsY), shiftX, shiftY);
  // }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename TGrid>
  TGrid CreateGrid(const int32_t widthPx, const int32_t heightPx, const int32_t cellsWidthPx, const int32_t cellsHeightPx)
  {
    const uint32_t desiredStepSizeX = BitsUtil::NextPowerOfTwo(cellsWidthPx);
    const uint32_t desiredStepSizeY = BitsUtil::NextPowerOfTwo(cellsHeightPx);
    const uint32_t shiftX = BitsUtil::IndexOf(desiredStepSizeX);
    const uint32_t shiftY = BitsUtil::IndexOf(desiredStepSizeY);
    const uint32_t stepsX = (widthPx / desiredStepSizeX) + ((widthPx % desiredStepSizeX) > 0 ? 1 : 0);
    const uint32_t stepsY = (heightPx / desiredStepSizeY) + ((heightPx % desiredStepSizeY) > 0 ? 1 : 0);
    return TGrid(UncheckedNumericCast<uint16_t>(stepsX), UncheckedNumericCast<uint16_t>(stepsY), shiftX, shiftY);
  }

  template <typename TGrid>
  void TryAdd(TGrid& grid, ReadOnlySpan<DrawRecord> testDataSpan)
  {
    for (const auto& entry : testDataSpan)
    {
      grid.TryAdd(entry.Rect, entry.Id);
    }
  }

  template <typename TGrid>
  void TryUncheckedAdd(TGrid& grid, ReadOnlySpan<DrawRecord> testDataSpan)
  {
    for (const auto& entry : testDataSpan)
    {
      grid.TryUncheckedAdd(entry.Rect, entry.Id);
    }
  }

  template <typename TGrid>
  void TryInsertAfterZPos(benchmark::State& rState, TGrid& grid, ReadOnlySpan<DrawRecord> testDataSpan)
  {
    FSL_PARAM_NOT_USED(rState);
    for (const auto& entry : testDataSpan)
    {
      // rState.PauseTiming();
      // rState.ResumeTiming();

      grid.TryInsertAfterZPos(0, entry.Rect, 1);
    }
  }


  template <typename TGrid>
  void TryGetChunkEntries(TGrid& grid, ReadOnlySpan<Point2> testDataSpan)
  {
    for (const auto& entry : testDataSpan)
    {
      benchmark::DoNotOptimize(grid.TryGetChunkEntries(entry.X, entry.Y));
    }
  }

  template <typename TGrid>
  void UncheckedGetChunkEntries(TGrid& grid, ReadOnlySpan<Point2> testDataSpan)
  {
    for (const auto& entry : testDataSpan)
    {
      benchmark::DoNotOptimize(grid.UncheckedGetChunkEntries(entry.X, entry.Y));
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_TryAdd(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
    FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);

    const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);

    // Perform setup here
    for (auto _ : state)
    {
      state.PauseTiming();
      grid.Clear();
      state.ResumeTiming();

      // This code gets timed
      TryAdd(grid, testDataSpan);
    }
  }

  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_TryUncheckedAdd(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
    FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);

    const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);

    // Perform setup here
    for (auto _ : state)
    {
      state.PauseTiming();
      grid.Clear();
      state.ResumeTiming();

      // This code gets timed
      TryUncheckedAdd(grid, testDataSpan);
    }
  }

  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_TryInsertAfterZPos(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
    FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);

    const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);

    // Perform setup here
    for (auto _ : state)
    {
      state.PauseTiming();
      grid.Clear();
      state.ResumeTiming();

      // This code gets timed
      TryInsertAfterZPos(state, grid, testDataSpan);
    }
  }

  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_Clear(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
    FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);

    const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);

    // Perform setup here
    for (auto _ : state)
    {
      state.PauseTiming();
      TryAdd(grid, testDataSpan);
      state.ResumeTiming();

      // This code gets timed
      grid.Clear();
    }
  }


  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_TryGetChunkEntriesRandom(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    {
      std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
      FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);
      const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);
      TryAdd(grid, testDataSpan);
    }

    std::vector<Point2> testChunks(LocalConfig::RandomChunkCount);
    FillVector(testChunks, LocalConfig::Seed, grid.GetCellCountX(), grid.GetCellCountY());
    const auto testChunkSpan = ReadOnlySpanUtil::AsSpan(testChunks);

    // Perform setup here
    for (auto _ : state)
    {
      // This code gets timed
      TryGetChunkEntries(grid, testChunkSpan);
    }
  }


  template <int32_t TCellSize, typename TGrid>
  void BM_Grid_UncheckedGetChunkEntriesRandom(benchmark::State& state)
  {
    auto grid = CreateGrid<TGrid>(LocalConfig::SizePx.RawWidth(), LocalConfig::SizePx.RawHeight(), TCellSize, TCellSize);
    {
      std::vector<DrawRecord> testData(LocalConfig::ObjectCount);
      FillVector(testData, LocalConfig::Seed, LocalConfig::SizePx);
      const auto testDataSpan = ReadOnlySpanUtil::AsSpan(testData);
      TryAdd(grid, testDataSpan);
    }

    std::vector<Point2> testChunks(LocalConfig::RandomChunkCount);
    FillVector(testChunks, LocalConfig::Seed, grid.GetCellCountX(), grid.GetCellCountY());
    const auto testChunkSpan = ReadOnlySpanUtil::AsSpan(testChunks);

    // Perform setup here
    for (auto _ : state)
    {
      // This code gets timed
      UncheckedGetChunkEntries(grid, testChunkSpan);
    }
  }

}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// TryAdd
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_TRYADD
#ifdef LOCAL_BENCH_UNORDEREDMAP
BENCHMARK(BM_Grid_TryAdd<64, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryAdd<128, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryAdd<256, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryAdd<512, SpatialHashGrid2DUnorderedMap>);
#endif

#ifdef LOCAL_BENCH_MAP
BENCHMARK(BM_Grid_TryAdd<64, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryAdd<128, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryAdd<256, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryAdd<512, SpatialHashGrid2DMap>);
#endif

#ifdef LOCAL_BENCH_VECTOR
BENCHMARK(BM_Grid_TryAdd<64, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryAdd<128, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryAdd<256, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryAdd<512, SpatialHashGrid2DVector>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_TryAdd<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryAdd<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryAdd<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryAdd<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_TryAdd<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryAdd<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryAdd<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryAdd<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// TryUncheckedAdd
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_TRYUNCHECKEDADD
#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_TryUncheckedAdd<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_TryUncheckedAdd<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryUncheckedAdd<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// TryInsertAfterZPos
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_TRYINSERTAFTERZPOS
#ifdef LOCAL_BENCH_UNORDEREDMAP
BENCHMARK(BM_Grid_TryInsertAfterZPos<64, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<128, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<256, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<512, SpatialHashGrid2DUnorderedMap>);
#endif

#ifdef LOCAL_BENCH_MAP
BENCHMARK(BM_Grid_TryInsertAfterZPos<64, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<128, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<256, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<512, SpatialHashGrid2DMap>);
#endif

#ifdef LOCAL_BENCH_VECTOR
BENCHMARK(BM_Grid_TryInsertAfterZPos<64, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<128, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<256, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<512, SpatialHashGrid2DVector>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_TryInsertAfterZPos<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_TryInsertAfterZPos<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryInsertAfterZPos<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_CLEAR
#ifdef LOCAL_BENCH_UNORDEREDMAP
BENCHMARK(BM_Grid_Clear<64, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_Clear<128, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_Clear<256, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_Clear<512, SpatialHashGrid2DUnorderedMap>);
#endif

#ifdef LOCAL_BENCH_MAP
BENCHMARK(BM_Grid_Clear<64, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_Clear<128, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_Clear<256, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_Clear<512, SpatialHashGrid2DMap>);
#endif

#ifdef LOCAL_BENCH_VECTOR
BENCHMARK(BM_Grid_Clear<64, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_Clear<128, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_Clear<256, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_Clear<512, SpatialHashGrid2DVector>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_Clear<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_Clear<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_Clear<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_Clear<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_Clear<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_Clear<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_Clear<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_Clear<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// GetChunkEntries
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_GETCHUNKENTRIES
#ifdef LOCAL_BENCH_UNORDEREDMAP
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<64, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<128, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<256, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<512, SpatialHashGrid2DUnorderedMap>);
#endif

#ifdef LOCAL_BENCH_MAP
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<64, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<128, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<256, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<512, SpatialHashGrid2DMap>);
#endif

#ifdef LOCAL_BENCH_VECTOR
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<64, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<128, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<256, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<512, SpatialHashGrid2DVector>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_TryGetChunkEntriesRandom<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedGetChunkEntries
// ---------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LOCAL_BENCH_UNCHECKEDGETCHUNKENTRIES
#ifdef LOCAL_BENCH_UNORDEREDMAP
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<64, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<128, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<256, SpatialHashGrid2DUnorderedMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<512, SpatialHashGrid2DUnorderedMap>);
#endif

#ifdef LOCAL_BENCH_MAP
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<64, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<128, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<256, SpatialHashGrid2DMap>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<512, SpatialHashGrid2DMap>);
#endif

#ifdef LOCAL_BENCH_VECTOR
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<64, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<128, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<256, SpatialHashGrid2DVector>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<512, SpatialHashGrid2DVector>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_256
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<64, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<128, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<256, SpatialHashGrid2DFixedBucketSize<256>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<512, SpatialHashGrid2DFixedBucketSize<256>>);
#endif

#ifdef LOCAL_BENCH_FIXED_BUCKET_SIZE_32
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<64, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<128, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<256, SpatialHashGrid2DFixedBucketSize<32>>);
BENCHMARK(BM_Grid_UncheckedGetChunkEntriesRandom<512, SpatialHashGrid2DFixedBucketSize<32>>);
#endif
#endif
