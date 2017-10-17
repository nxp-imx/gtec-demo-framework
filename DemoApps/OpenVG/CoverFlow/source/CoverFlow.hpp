#ifndef OPENVG_COVERFLOW_COVERFLOW_HPP
#define OPENVG_COVERFLOW_COVERFLOW_HPP
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

#include <FslDemoApp/OpenVG/DemoAppVG.hpp>
#include "VG/openvg.h"
#include "VG/vgu.h"

namespace Fsl
{
     struct CoverAlbum
    {
        public:
        VGImage AlbumCF;
        VGImage AlbumBF;
        uint8_t PosAnimation;
        uint8_t ScaleAnimation;
        uint8_t RealIndex;
        uint8_t CurrentIndex;
        uint8_t PastIndex;
        VGfloat IncrementX;
        VGfloat IncrementY;
        VGfloat CurrentX;
        VGfloat CurrentY;
        VGfloat ScaleX;
        VGfloat ScaleY;
        VGfloat ScaleIncrementX;
        VGfloat ScaleIncrementY;
        VGfloat AngleIncrement;
        VGfloat CurrentAngle;
        uint32_t AnimationDuration;
        uint32_t AnimationState;
        uint32_t CoverFlowState;

        CoverAlbum()
        {
        }
     };


  class CoverFlow : public DemoAppVG
  {
    std::vector<CoverAlbum> m_coverAlbums;
    std::vector<CoverAlbum> m_coverBigAlbums;
    uint32_t m_frame;

  public:
    CoverFlow(const DemoAppConfig& config);
    ~CoverFlow();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  };
}

#endif
