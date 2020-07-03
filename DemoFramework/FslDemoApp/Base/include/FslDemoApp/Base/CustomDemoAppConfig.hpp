#ifndef FSLDEMOAPP_BASE_CUSTOMDEMOAPPCONFIG_HPP
#define FSLDEMOAPP_BASE_CUSTOMDEMOAPPCONFIG_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/ITag.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  enum class CustomDemoAppConfigRestartFlags : uint32_t
  {
    //! No flags set, so no restart
    Never = 0x00,
    Resize = 0x01,
    DpiChange = 0x02,
    ConfigurationChanged = (Resize | DpiChange),
  };

  constexpr inline CustomDemoAppConfigRestartFlags operator|(const CustomDemoAppConfigRestartFlags lhs, const CustomDemoAppConfigRestartFlags rhs)
  {
    return static_cast<CustomDemoAppConfigRestartFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline CustomDemoAppConfigRestartFlags operator&(const CustomDemoAppConfigRestartFlags lhs, const CustomDemoAppConfigRestartFlags rhs)
  {
    return static_cast<CustomDemoAppConfigRestartFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }


  namespace CustomDemoAppConfigRestartFlagsUtil
  {
    constexpr inline bool IsFlagged(const CustomDemoAppConfigRestartFlags src, const CustomDemoAppConfigRestartFlags flag)
    {
      return (src & flag) == flag;
    }
  };


  struct CustomDemoAppConfig
  {
    CustomDemoAppConfigRestartFlags RestartFlags{CustomDemoAppConfigRestartFlags::ConfigurationChanged};
    std::shared_ptr<ITag> AppRegistrationUserTag;

    CustomDemoAppConfig() = default;

    explicit CustomDemoAppConfig(std::shared_ptr<ITag> appRegistrationUserTag)
      : AppRegistrationUserTag(std::move(appRegistrationUserTag))
    {
    }
  };
}

#endif
