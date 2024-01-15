#ifndef FSLSIMPLEUI_APP_UIDEMOAPPRENDERCREATEINFO_HPP
#define FSLSIMPLEUI_APP_UIDEMOAPPRENDERCREATEINFO_HPP
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

#include <FslSimpleUI/App/UIDemoAppMaterialConfig.hpp>
#include <FslSimpleUI/App/UIDemoAppMaterialCreateInfo.hpp>
#include <FslSimpleUI/App/UIDemoAppRenderCapacityInfo.hpp>

namespace Fsl
{
  struct UIDemoAppRenderCreateInfo
  {
    UIDemoAppRenderCapacityInfo Capacity;
    //! Material create info that can only be configured during creation
    UIDemoAppMaterialCreateInfo MaterialCreateInfo;
    //! control various material settings.
    UIDemoAppMaterialConfig MaterialConfig;

    constexpr UIDemoAppRenderCreateInfo() noexcept = default;


    explicit constexpr UIDemoAppRenderCreateInfo(const UIDemoAppRenderCapacityInfo capacity,
                                                 const UIDemoAppMaterialCreateInfo materialCreateInfo = {},
                                                 const UIDemoAppMaterialConfig materialConfig = {}) noexcept
      : Capacity(capacity)
      , MaterialCreateInfo(materialCreateInfo)
      , MaterialConfig(materialConfig)
    {
    }


    explicit constexpr UIDemoAppRenderCreateInfo(const UIDemoAppMaterialCreateInfo materialCreateInfo,
                                                 const UIDemoAppMaterialConfig materialConfig = {}) noexcept
      : UIDemoAppRenderCreateInfo(UIDemoAppRenderCapacityInfo(), materialCreateInfo, materialConfig)
    {
    }

    constexpr bool operator==(const UIDemoAppRenderCreateInfo& rhs) const noexcept
    {
      return Capacity == rhs.Capacity && MaterialCreateInfo == rhs.MaterialCreateInfo && MaterialConfig == rhs.MaterialConfig;
    }


    constexpr bool operator!=(const UIDemoAppRenderCreateInfo& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
