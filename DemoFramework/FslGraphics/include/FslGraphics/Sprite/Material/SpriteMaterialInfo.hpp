#ifndef FSLGRAPHICS_SPRITE_MATERIAL_SPRITEMATERIALINFO_HPP
#define FSLGRAPHICS_SPRITE_MATERIAL_SPRITEMATERIALINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <memory>

namespace Fsl
{
  class ISpriteMaterial;

  //! @brief A simple immutable render material
  struct SpriteMaterialInfo
  {
    //! The render material id, this only means something to the render implementation
    SpriteMaterialId Id{};

    //! The actual extent of the main texture in pixels
    PxExtent2D ExtentPx;

    //! True if the material is considered opaque
    bool IsOpaque{true};

    std::shared_ptr<ISpriteMaterial> Material;

    SpriteMaterialInfo() = default;

    SpriteMaterialInfo(const SpriteMaterialId spriteMaterialId, const PxExtent2D extentPx, const bool isOpaque,
                       std::shared_ptr<ISpriteMaterial> material);
    ~SpriteMaterialInfo();


    bool IsValid() const
    {
      return static_cast<bool>(Material);
    }

    bool operator==(const SpriteMaterialInfo& rhs) const
    {
      return Id == rhs.Id && ExtentPx == rhs.ExtentPx && IsOpaque == rhs.IsOpaque && Material == rhs.Material;
    }

    bool operator!=(const SpriteMaterialInfo& rhs) const
    {
      return !(*this == rhs);
    }

    static SpriteMaterialInfo Invalid()
    {
      return {};
    }
  };
}

#endif
