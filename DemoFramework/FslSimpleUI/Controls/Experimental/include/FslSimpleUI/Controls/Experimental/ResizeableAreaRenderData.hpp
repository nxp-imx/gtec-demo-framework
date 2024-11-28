#ifndef FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_RESIZEABLEAREARENDERDATA_HPP
#define FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_RESIZEABLEAREARENDERDATA_HPP
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

#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslSimpleUI/Controls/Experimental/Logic/ResizeableAreaDragCorner.hpp>
#include <FslSimpleUI/Controls/Experimental/ResizeableAreaDragHandleLocation.hpp>
#include <FslSimpleUI/Render/Base/ICustomDrawData.hpp>
#include <FslSimpleUI/Render/Base/UIRenderColor.hpp>
#include <array>

namespace Fsl::UI
{
  struct ResizeableAreaRenderData : public ICustomDrawData
  {
    struct DragHandleRecord
    {
      ResizeableAreaDragCorner Corner;
      bool Hover{false};
      PxRectangle Rectangle;
    };

    bool ShowRenderHandles{false};

    //! The actual clip rectangle (window coordinates)
    PxRectangle ClipRectanglePx;
    //! The actual render offset of the window
    PxVector2 RenderOffsetPxf;
    //! The render rectangle (window coordinates)
    PxRectangle RenderRectanglePx;

    std::array<DragHandleRecord, 4> DragHandles;

    UIRenderColor DragHandleColor;

    ResizeableAreaRenderData();

    void SetRenderData(const PxVector2 renderOffsetPxf, const PxRectangle& rectanglePx, const PxSize1D dragHandleSizePx,
                       const ResizeableAreaDragHandleLocation dragHandleLocation, const bool showRenderHandles, const UIRenderColor dragHandleColor);

    int32_t TryGetFindHitIndex(const PxPoint2 positionPx) const noexcept;
  };
}
#endif
