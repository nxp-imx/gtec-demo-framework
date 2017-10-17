#ifndef G2D_EIGHTLAYERS_EIGHTLAYERS_HPP
#define G2D_EIGHTLAYERS_EIGHTLAYERS_HPP
//#define OLD_BOARD
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

#include <FslDemoApp/G2D/DemoAppG2D.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <vector>
#include <g2d.h>

#ifdef OLD_BOARD
struct g2d_surface_pair
{
  struct g2d_surface s;
  struct g2d_surface d;
};
#endif

namespace Fsl
{
  struct ScopedFBHandle;
  struct ScopedG2DHandle;
  class G2DSurface;

  class EightLayers : public DemoAppG2D
  {
    int m_layerCount;
    std::shared_ptr<ScopedFBHandle> m_fb;
    std::shared_ptr<ScopedG2DHandle> m_g2d;

    std::shared_ptr<G2DSurface> m_tmp;
    std::vector<std::shared_ptr<G2DSurface> > m_srcSurfaces;
    std::vector<g2d_surface_pair> m_layers;
    std::vector<g2d_surface_pair*> m_layerPointers;

    Rectangle m_srcRect;
    bool m_useDummyMode;
    bool m_forceVSync;
    bool m_cache;
    bool m_animate;
    bool m_use8Blit;
    int m_test;
    int m_test2;
    float m_angle;
  public:
    EightLayers(const DemoAppConfig& config);
    ~EightLayers();
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void UpdateInfo();
  };
}

#endif
