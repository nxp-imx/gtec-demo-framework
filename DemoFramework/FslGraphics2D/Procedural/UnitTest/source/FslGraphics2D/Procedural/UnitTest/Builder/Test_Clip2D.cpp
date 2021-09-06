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

#include <FslGraphics2D/Procedural/Builder/Clip2DUtil.hpp>
#include <FslBase/Log/Math/Pixel/LogPxVector2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxAreaRectangleF.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslBase/Exceptions.hpp>
#include <array>

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr float X0 = 1.0f;
    constexpr float X1 = 2.0f;
    constexpr float X2 = 4.0f;
    // constexpr float X3 = 8.0f;
    constexpr float U0 = 10.0f;
    constexpr float U1 = 20.0f;
    constexpr float U2 = 40.0f;
    // constexpr float U3 = 80.0f;
  }

  using TestBuilder_Clip2D = TestFixtureFslGraphics;
}

// segments:     A---B
// clip:         C---D
TEST(TestBuilder_Clip2D, Clip2_ExactMatch)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X1);
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), coordsPxf.size());
  for (uint32_t i = 0; i < clippedEntries.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], clippedEntries[i]);
  }
}

// segments:     A---B
// clip:     C---D
TEST(TestBuilder_Clip2D, Clip2_ClipAreaLeftOf)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 10, LocalConfig::X0);
  std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---B
// clip:             C---D
TEST(TestBuilder_Clip2D, Clip2_ClipAreaRightOf)
{
  const PxVector2 clipRangePxf(LocalConfig::X1, LocalConfig::X1 + 10);
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---B
// clip:         C-D
TEST(TestBuilder_Clip2D, Clip2_PartialLeft0)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}

// segments:     A---B
// clip:        C--D
TEST(TestBuilder_Clip2D, Clip2_PartialLeft1)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 1, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}


// segments:     A---B
// clip:           C-D
TEST(TestBuilder_Clip2D, Clip2_PartialRight0)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2), LocalConfig::X1);
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---B
// clip:           C--D
TEST(TestBuilder_Clip2D, Clip2_PartialRight)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2), LocalConfig::X1 + 1);
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---B
// clip:          C-D
TEST(TestBuilder_Clip2D, Clip2_Inside)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + 0.25f, LocalConfig::X1 - 0.25f);
  const std::array<PxVector2, 2> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1)};
  std::array<PxVector2, 2> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[0].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentageL = deltaNewL / delta.X;
  float percentageR = deltaNewR / delta.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], clippedR);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


// segments:     A---B---C
// clip:         C-------D
TEST(TestBuilder_Clip2D, Clip3_ExactMatch)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X2);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), coordsPxf.size());
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], clippedEntries[i]);
  }
}

// segments:     A---B---C
// clip:     D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaLeftOf)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 10, LocalConfig::X0);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---B---C
// clip:                 D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaRightOf)
{
  const PxVector2 clipRangePxf(LocalConfig::X2, LocalConfig::X2 + 10);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---B---C
// clip:         D-E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft0)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}

// segments:     A---B---C
// clip:        D--E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft1)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 1, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}


// segments:     A---B---C
// clip:               D-E
TEST(TestBuilder_Clip2D, Clip3_PartialRight0)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + ((LocalConfig::X2 - LocalConfig::X1) / 2), LocalConfig::X2);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---B---C
// clip:               D--E
TEST(TestBuilder_Clip2D, Clip3_PartialRight)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + ((LocalConfig::X2 - LocalConfig::X1) / 2), LocalConfig::X2 + 1);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---B---C
// clip:          D-E
TEST(TestBuilder_Clip2D, Clip3_InsideAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + 0.25f, LocalConfig::X1 - 0.35f);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);
  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[0].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentageL = deltaNewL / delta.X;
  float percentageR = deltaNewR / delta.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], clippedR);
}

// segments:     A---B---C
// clip:           D---E
TEST(TestBuilder_Clip2D, Clip3_InsideAC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + 0.25f, LocalConfig::X2 - 0.35f);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);
  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 3u);

  auto deltaL = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaR = orgCoordsPxf[2] - orgCoordsPxf[1];

  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[0].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[1].X;

  float percentageL = deltaNewL / deltaL.X;
  float percentageR = deltaNewR / deltaR.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[0].Y + (deltaL.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[1].Y + (deltaR.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], orgCoordsPxf[1]);
  EXPECT_EQ(clippedEntries[2], clippedR);
}

// segments:     A---B---C
// clip:              D-E
TEST(TestBuilder_Clip2D, Clip3_InsideBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + 0.25f, LocalConfig::X2 - 0.35f);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);
  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);

  auto deltaR = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[1].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[1].X;
  float percentageL = deltaNewL / deltaR.X;
  float percentageR = deltaNewR / deltaR.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[1].Y + (deltaR.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[1].Y + (deltaR.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], clippedR);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


// segments:     AB---C
// clip:         C-------D
TEST(TestBuilder_Clip2D, Clip3_ExactMatch_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X2);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), coordsPxf.size());
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], clippedEntries[i]);
  }
}

// segments:     AB---C
// clip:     D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaLeftOf_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 10, LocalConfig::X0);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     AB---C
// clip:              D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaRightOf_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X2, LocalConfig::X2 + 10);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     AB---C
// clip:         D-E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft0_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 3u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[2].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[2], clipped);
}

// segments:     AB---C
// clip:        D--E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft1_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 1, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 3u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[2].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[2], clipped);
}


// segments:     AB---C
// clip:             D-E
TEST(TestBuilder_Clip2D, Clip3_PartialRight0_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + ((LocalConfig::X2 - LocalConfig::X1) / 2), LocalConfig::X2);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     AB---C
// clip:            D--E
TEST(TestBuilder_Clip2D, Clip3_PartialRight_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + ((LocalConfig::X2 - LocalConfig::X1) / 2), LocalConfig::X2 + 1);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[1].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[1].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     AB---C
// clip:           D-E
TEST(TestBuilder_Clip2D, Clip3_InsideBC_OverlappingCoordsAB)
{
  const PxVector2 clipRangePxf(LocalConfig::X1 + 0.25f, LocalConfig::X2 - 0.35f);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X0, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X2, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);
  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);

  auto deltaR = orgCoordsPxf[2] - orgCoordsPxf[1];
  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[1].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[1].X;
  float percentageL = deltaNewL / deltaR.X;
  float percentageR = deltaNewR / deltaR.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[1].Y + (deltaR.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[1].Y + (deltaR.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], clippedR);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


// segments:     A---BC
// clip:         C----D
TEST(TestBuilder_Clip2D, Clip3_ExactMatch_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X2);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), coordsPxf.size());
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], clippedEntries[i]);
  }
}

// segments:     A---BC
// clip:     D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaLeftOf_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 10, LocalConfig::X0);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---BC
// clip:              D---E
TEST(TestBuilder_Clip2D, Clip3_ClipAreaRightOf_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X2, LocalConfig::X2 + 10);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 0u);
  for (uint32_t i = 0; i < orgCoordsPxf.size(); ++i)
  {
    EXPECT_EQ(orgCoordsPxf[i], coordsPxf[i]);
  }
}

// segments:     A---BC
// clip:         D-E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft0_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}

// segments:     A---BC
// clip:        D--E
TEST(TestBuilder_Clip2D, Clip3_PartialLeft1_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 - 1, LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2));
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);
  EXPECT_EQ(clippedEntries[0].X, orgCoordsPxf[0].X);
  EXPECT_EQ(clippedEntries[1].X, clipRangePxf.Y);
  EXPECT_EQ(clippedEntries[0].Y, orgCoordsPxf[0].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[1], clipped);
}


// segments:     A---BC
// clip:            D-E
TEST(TestBuilder_Clip2D, Clip3_PartialRight0_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2), LocalConfig::X1);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 3u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[2].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);
  EXPECT_EQ(clippedEntries[2].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---BC
// clip:            D--E
TEST(TestBuilder_Clip2D, Clip3_PartialRight_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + ((LocalConfig::X1 - LocalConfig::X0) / 2), LocalConfig::X1 + 1);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);

  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 3u);
  EXPECT_EQ(clippedEntries[0].X, clipRangePxf.X);
  EXPECT_EQ(clippedEntries[1].X, orgCoordsPxf[1].X);
  EXPECT_EQ(clippedEntries[2].X, orgCoordsPxf[2].X);
  EXPECT_EQ(clippedEntries[1].Y, orgCoordsPxf[1].Y);
  EXPECT_EQ(clippedEntries[2].Y, orgCoordsPxf[2].Y);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNew = clipRangePxf.X - orgCoordsPxf[0].X;
  float percentage = deltaNew / delta.X;
  PxVector2 clipped(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentage));
  EXPECT_EQ(clippedEntries[0], clipped);
}

// segments:     A---B---C
// clip:          D-E
TEST(TestBuilder_Clip2D, Clip3_InsideAB_OverlappignCoordsBC)
{
  const PxVector2 clipRangePxf(LocalConfig::X0 + 0.25f, LocalConfig::X1 - 0.35f);
  const std::array<PxVector2, 3> orgCoordsPxf = {PxVector2(LocalConfig::X0, LocalConfig::U0), PxVector2(LocalConfig::X1, LocalConfig::U1),
                                                 PxVector2(LocalConfig::X1, LocalConfig::U2)};
  std::array<PxVector2, 3> coordsPxf(orgCoordsPxf);
  ReadOnlySpan<PxVector2> clippedEntries = Clip2DUtil::Clip(coordsPxf, clipRangePxf);

  EXPECT_EQ(clippedEntries.size(), 2u);

  auto delta = orgCoordsPxf[1] - orgCoordsPxf[0];
  auto deltaNewL = clipRangePxf.X - orgCoordsPxf[0].X;
  auto deltaNewR = clipRangePxf.Y - orgCoordsPxf[0].X;
  float percentageL = deltaNewL / delta.X;
  float percentageR = deltaNewR / delta.X;

  PxVector2 clippedL(clipRangePxf.X, orgCoordsPxf[0].Y + (delta.Y * percentageL));
  PxVector2 clippedR(clipRangePxf.Y, orgCoordsPxf[0].Y + (delta.Y * percentageR));

  EXPECT_EQ(clippedEntries[0], clippedL);
  EXPECT_EQ(clippedEntries[1], clippedR);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
