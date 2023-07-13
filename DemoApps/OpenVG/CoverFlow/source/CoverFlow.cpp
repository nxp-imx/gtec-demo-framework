/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "CoverFlow.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenVG/VGCheck.hpp>
#include <VG/openvg.h>
#include <array>
#include <iostream>
#include "CoverAlbumBitmap.hpp"

namespace Fsl
{
  // using namespace OpenVG;

  namespace
  {
    constexpr const int ANIMATION_IDLE = 0;
    constexpr const int ANIMATION_EXECUTING_POS = 1;
    constexpr const int ANIMATION_EXECUTING_SCALE = 2;
    constexpr const int FRAMES_PER_TRANSITION = 30;

    std::vector<Vector2> slot_coordinates(8);
    std::vector<Vector2> scale_values(2);
    std::vector<Vector2> special_slot_coordinates(1);


    void VGAnimationStateMachine(CoverAlbum& rObject, Vector2 startPos, Vector2 endPos, Vector2 startScale, Vector2 endScale)
    {
      switch (rObject.CoverFlowState)
      {
      case ANIMATION_IDLE:
        if ((rObject.AnimationDuration - 1) == 0)
        {
          rObject.AnimationDuration = 0;
        }
        if (rObject.PosAnimation != 0u)
        {
          // Animation Requested

          // Calculate next position and animation increments
          rObject.IncrementX = ((endPos.X - startPos.X) / static_cast<float>(rObject.AnimationDuration - 1));
          rObject.IncrementY = ((endPos.Y - startPos.Y) / static_cast<float>(rObject.AnimationDuration - 1));

          rObject.AngleIncrement = (360.0f / static_cast<float>(rObject.AnimationDuration - 1));
          // Define Animations duration
          rObject.AnimationState = 0;
          rObject.CoverFlowState = ANIMATION_EXECUTING_POS;
        }
        else
        {
          // No Animation, print default values
          rObject.CurrentX = endPos.X;
          rObject.CurrentY = endPos.Y;
        }
        if (rObject.ScaleAnimation != 0u)
        {
          rObject.ScaleAnimation = 0;
          rObject.ScaleX = 1;
          rObject.ScaleY = 1;
          rObject.ScaleIncrementX = (endScale.X - startScale.X) / static_cast<float>(rObject.AnimationDuration - 1);
          rObject.ScaleIncrementY = (endScale.Y - startScale.Y) / static_cast<float>(rObject.AnimationDuration - 1);

          rObject.AnimationState = 0;
          rObject.CoverFlowState = ANIMATION_EXECUTING_SCALE;
        }
        else
        {
          rObject.ScaleIncrementX = 0;
          rObject.ScaleIncrementY = 0;
        }
        break;

      case ANIMATION_EXECUTING_POS:
        if (rObject.AnimationDuration > 0)
        {
          rObject.CurrentX = startPos.X + (rObject.IncrementX * static_cast<float>(rObject.AnimationState));
          rObject.CurrentY = startPos.Y + (rObject.IncrementY * static_cast<float>(rObject.AnimationState));
          rObject.CurrentAngle = rObject.AngleIncrement * static_cast<float>(rObject.AnimationState);
          rObject.AnimationState++;
          rObject.AnimationDuration--;
        }
        else
        {
          // Prepare for next position animation
          rObject.CurrentX = endPos.X;
          rObject.CurrentY = endPos.Y;
          rObject.PosAnimation = 0;
          rObject.CoverFlowState = ANIMATION_IDLE;
          rObject.CurrentAngle = 0;
        }
        break;

      case ANIMATION_EXECUTING_SCALE:
        if (rObject.AnimationDuration > 0)
        {
          rObject.ScaleX = startScale.X + (rObject.ScaleIncrementX * static_cast<float>(rObject.AnimationState));
          rObject.ScaleY = startScale.Y + (rObject.ScaleIncrementY * static_cast<float>(rObject.AnimationState));
          rObject.AnimationState++;
          rObject.AnimationDuration--;
        }
        else
        {
          rObject.ScaleAnimation = 0;
          rObject.CoverFlowState = ANIMATION_IDLE;
        }
        break;
      }
    }
  }


  CoverFlow::CoverFlow(const DemoAppConfig& config)
    : DemoAppVG(config)
    , m_bitMapArray{
        nullptr,
        CoverAlbumBitmaps::CarnivalBitmap,
        CoverAlbumBitmaps::EncoreBitmap,
        CoverAlbumBitmaps::KajiuraBitmap,
        CoverAlbumBitmaps::MozartBitmap,
        CoverAlbumBitmaps::StratovariusBitmap,
        CoverAlbumBitmaps::TemerariosBitmap,
        CoverAlbumBitmaps::TigresBitmap,
        CoverAlbumBitmaps::TristaniaBitmap,
      }
    , m_coverAlbums(9)
    , m_coverBigAlbums(2)
    , m_frame(0)
  {
    int j = 0;
    std::array<VGfloat, 4> afClearColour = {0.0f, 0.0f, 0.0f, 1.0f};
    vgSetfv(VG_CLEAR_COLOR, UncheckedNumericCast<VGint>(afClearColour.size()), afClearColour.data());
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    for (j = 1; j < 9; j++)
    {
      m_coverAlbums[j].AlbumCF =
        vgCreateImage(VG_sARGB_8888, 128, 128, VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_NONANTIALIASED | VG_IMAGE_QUALITY_FASTER);
      m_coverAlbums[j].AlbumBF =
        vgCreateImage(VG_sARGB_8888, 128, 128, VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_NONANTIALIASED | VG_IMAGE_QUALITY_FASTER);
      vgImageSubData(m_coverAlbums[j].AlbumCF, m_bitMapArray[j], -1 * 4 * 128, VG_sARGB_8888, 0, 0, 128, 128);
      FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
      vgGaussianBlur(m_coverAlbums[j].AlbumBF, m_coverAlbums[j].AlbumCF, 3, 3, VG_TILE_FILL);
      FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
      m_coverAlbums[j].CurrentAngle = 0.0;
      m_coverAlbums[j].CurrentIndex = j;
      if (m_coverAlbums[j].CurrentIndex >= 8)
      {
        m_coverAlbums[j].CurrentIndex = 1;
      }
      m_coverAlbums[j].RealIndex = j;
      m_coverAlbums[j].PastIndex = 0;
      m_coverAlbums[j].ScaleAnimation = 0;
      m_coverAlbums[j].AnimationDuration = 0;
      m_coverAlbums[j].CoverFlowState = 0;
    }

    m_coverBigAlbums[0].AlbumCF =
      vgCreateImage(VG_sARGB_8888, 128, 128, VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_NONANTIALIASED | VG_IMAGE_QUALITY_FASTER);
    // Big Image
    m_coverBigAlbums[1].AlbumCF =
      vgCreateImage(VG_sARGB_8888, 256, 256, VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_NONANTIALIASED | VG_IMAGE_QUALITY_FASTER);

    vgImageSubData(m_coverBigAlbums[0].AlbumCF, m_bitMapArray[3], -1 * 4 * 128, VG_sARGB_8888, 0, 0, 128, 128);
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

    m_coverBigAlbums[0].ScaleX = 1;
    m_coverBigAlbums[0].ScaleY = 1;
    m_coverBigAlbums[0].CoverFlowState = 0;

    m_coverBigAlbums[1].ScaleX = 1;
    m_coverBigAlbums[1].ScaleY = 1;
    m_coverBigAlbums[1].CoverFlowState = 0;
    const PxSize2D currentSizePx = GetWindowSizePx();
    vgClear(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());
    FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
    // slot_coordinates.resize(8);
    slot_coordinates[0] = Vector2(0.0f, 0.0f);
    // Slot 0
    slot_coordinates[1] = Vector2(550.0f, 550.0f);
    // Slot 1
    slot_coordinates[2] = Vector2(600.0f, 400.0f);
    // Slot 2
    slot_coordinates[3] = Vector2(550.0f, 250.0f);
    // Slot 3
    Vector2(600.0f, 100.0f),
      // Slot 4
      slot_coordinates[4] = Vector2(550.0f, -50.0f);
    // Slot 5
    slot_coordinates[5] = Vector2(600.0f, -200.0f);
    // Slot 6
    slot_coordinates[6] = Vector2(550.0f, -350.0f);
    // Slot 7
    slot_coordinates[7] = Vector2(600.0f, -550.0f);

    scale_values[0] = Vector2(1.0f, 1.0f);
    scale_values[1] = Vector2(2.0f, 2.0f);

    special_slot_coordinates[0] = Vector2(250.0f, 250.0f);
  }


  CoverFlow::~CoverFlow() = default;


  void CoverFlow::Update(const DemoTime& demoTime)
  {
    int j = 0;
    m_frame++;
    for (j = 1; j < 9; j++)
    {
      // Animation State Machine to decide where to print
      if (0 == m_frame % 80)
      {
        m_coverAlbums[j].PosAnimation = 1;
        m_coverAlbums[j].AnimationDuration = static_cast<uint32_t>(3000 * demoTime.DeltaTime);
        m_coverAlbums[j].PastIndex = m_coverAlbums[j].CurrentIndex;
        m_coverAlbums[j].CurrentIndex++;
        if (m_coverAlbums[j].CurrentIndex >= 8)
        {
          m_coverAlbums[j].CurrentIndex = 1;
        }
        if (3 == m_coverAlbums[j].CurrentIndex)
        {
          m_coverBigAlbums[0].ScaleAnimation = 1;
          m_coverBigAlbums[0].AnimationDuration = static_cast<uint32_t>(3000 * demoTime.DeltaTime);
        }
      }
      VGAnimationStateMachine(m_coverAlbums[j], slot_coordinates[m_coverAlbums[j].PastIndex], slot_coordinates[m_coverAlbums[j].CurrentIndex],
                              Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f));
    }
    VGAnimationStateMachine(m_coverBigAlbums[0], Vector2(0.0f, 0.0f), special_slot_coordinates[0], scale_values[0], scale_values[1]);
  }


  void CoverFlow::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    int j = 0;
    const PxSize2D currentSizePx = GetWindowSizePx();
    vgClear(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());
    // Set matrix transformation mode
    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

    for (j = 1; j < 9; j++)
    {
      // Animation State Machine to decide where to print
      if (0 == m_frame % 80)
      {
        if (3 == m_coverAlbums[j].CurrentIndex)
        {
          // Start Special Slot Animation!!
          vgLoadIdentity();
          vgImageSubData(m_coverBigAlbums[0].AlbumCF, m_bitMapArray[m_coverAlbums[j].RealIndex], -1 * 4 * 128, VG_sARGB_8888, 0, 0, 128, 128);
        }
      }
      vgLoadIdentity();
      vgTranslate(m_coverAlbums[j].CurrentX, m_coverAlbums[j].CurrentY);
      if (m_coverAlbums[j].AnimationDuration != 0u)
      {
        vgTranslate(64, 64);
        vgRotate(m_coverAlbums[j].CurrentAngle);
        vgTranslate(-64, -64);
      }
      if (3 != m_coverAlbums[j].CurrentIndex)
      {
        vgDrawImage(m_coverAlbums[j].AlbumBF);
      }
      else
      {
        vgDrawImage(m_coverAlbums[j].AlbumCF);
      }
    }
    vgLoadIdentity();
    vgTranslate(180, 180);
    vgScale(m_coverBigAlbums[0].ScaleX, m_coverBigAlbums[0].ScaleY);
    vgDrawImage(m_coverBigAlbums[0].AlbumCF);
    vgFinish();
  }
}
