#ifndef FSLSIMPLEUI_RENDER_BASE_COMMAND_CUSTOMDRAWNINESLICEINFO_HPP
#define FSLSIMPLEUI_RENDER_BASE_COMMAND_CUSTOMDRAWNINESLICEINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderOptimizedNineSliceInfo.hpp>
#include <FslSimpleUI/Render/Base/DrawClipContext.hpp>
#include <memory>
#include <utility>

namespace Fsl::UI
{
  class UIRawMeshBuilder2D;
  class ICustomDrawData;
  using FnDrawCustomNineSliceMesh = void (*)(UIRawMeshBuilder2D&, const PxVector2, const PxSize2D, const DrawClipContext&,
                                             const RenderOptimizedNineSliceInfo&, const ICustomDrawData* const);

  struct CustomDrawNineSliceInfo
  {
    FnDrawCustomNineSliceMesh FnDraw{nullptr};
    std::shared_ptr<ICustomDrawData> CustomData;

    CustomDrawNineSliceInfo() noexcept = default;
    explicit CustomDrawNineSliceInfo(const FnDrawCustomNineSliceMesh fnDraw, std::shared_ptr<ICustomDrawData> customData) noexcept
      : FnDraw(fnDraw)
      , CustomData(std::move(customData))
    {
    }
  };
}

#endif
