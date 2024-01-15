#ifndef FSLSIMPLEUI_APP_UIDEMOAPPMATERIALCREATEINFO_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPMATERIALCREATEINFO_HPP
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

#include <FslGraphics/PixelFormat.hpp>

namespace Fsl
{
  struct UIDemoAppMaterialCreateInfo
  {
    //! If true the created materials will not use the Opaque material
    bool DisableOpaqueMaterials{false};
    //! If true the materials will be created in a way that allows the viewport to be dynamically modified.
    bool AllowDynamicCustomViewport{false};
    //! The default UI textures pixel format
    PixelFormat DefaultTexturePixelFormat{PixelFormat::R8G8B8A8_UNORM};

    constexpr UIDemoAppMaterialCreateInfo() noexcept = default;


    explicit constexpr UIDemoAppMaterialCreateInfo(const bool disableOpaqueMaterials) noexcept
      : DisableOpaqueMaterials(disableOpaqueMaterials)
    {
    }

    constexpr UIDemoAppMaterialCreateInfo(const bool disableOpaqueMaterials, const bool allowDynamicCustomViewport) noexcept
      : DisableOpaqueMaterials(disableOpaqueMaterials)
      , AllowDynamicCustomViewport(allowDynamicCustomViewport)
    {
    }


    constexpr UIDemoAppMaterialCreateInfo(const bool disableOpaqueMaterials, const bool allowDynamicCustomViewport,
                                          const PixelFormat defaultTexturePixelFormat) noexcept
      : DisableOpaqueMaterials(disableOpaqueMaterials)
      , AllowDynamicCustomViewport(allowDynamicCustomViewport)
      , DefaultTexturePixelFormat(defaultTexturePixelFormat)
    {
    }

    constexpr explicit UIDemoAppMaterialCreateInfo(const PixelFormat defaultTexturePixelFormat) noexcept
      : DefaultTexturePixelFormat(defaultTexturePixelFormat)
    {
    }

    constexpr bool operator==(const UIDemoAppMaterialCreateInfo& rhs) const noexcept
    {
      return DisableOpaqueMaterials == rhs.DisableOpaqueMaterials && AllowDynamicCustomViewport == rhs.AllowDynamicCustomViewport &&
             DefaultTexturePixelFormat == rhs.DefaultTexturePixelFormat;
    }

    constexpr bool operator!=(const UIDemoAppMaterialCreateInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
