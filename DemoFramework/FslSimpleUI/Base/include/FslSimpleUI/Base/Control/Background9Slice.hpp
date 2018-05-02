#ifndef FSLSIMPLEUI_BASE_CONTROL_BACKGROUND9SLICE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_BACKGROUND9SLICE_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/NineSlice.hpp>
#include <FslBase/Math/ThicknessF.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/Control/ContentControl.hpp>

namespace Fsl
{
  namespace UI
  {
    class Background9Slice : public ContentControl
    {
      AtlasTexture2D m_background;
      NineSlice m_nineSlice;
      ThicknessF m_minimumBackgroundMargin;
    public:
      Background9Slice(const std::shared_ptr<WindowContext>& context);

      const AtlasTexture2D& GetBackground() const
      {
        return m_background;
      }
      void SetBackground(const AtlasTexture2D& value);

      const NineSlice GetNineSlice() const
      {
        return m_nineSlice;
      }
      void SetNineSlice(const NineSlice& value);

      const ThicknessF& GetMinimumBackgroundMargin() const
      {
        return m_minimumBackgroundMargin;
      }
      void SetMinimumBackgroundMargin(const ThicknessF& value);

      virtual void WinDraw(const UIDrawContext& context) override;
    };
  }
}

#endif
