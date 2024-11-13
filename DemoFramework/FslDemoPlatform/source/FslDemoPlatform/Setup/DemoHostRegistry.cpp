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

#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeature.hpp>
#include <FslDemoPlatform/Setup/DemoHostRegistry.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <iterator>
#include <utility>

namespace Fsl
{
  namespace
  {
    bool CanSupport(const std::deque<DemoHostFeatureName::Enum>& availableFeatures, const std::deque<DemoHostFeature>& requiredFeatures)
    {
      auto itr = requiredFeatures.begin();
      while (itr != requiredFeatures.end())
      {
        if (std::find(availableFeatures.begin(), availableFeatures.end(), itr->Name) == availableFeatures.end())
        {
          return false;
        }
        ++itr;
      }
      return true;
    }
  }


  DemoHostRegistry::DemoHostRegistry() = default;


  DemoHostSetup DemoHostRegistry::GetSetup(const std::deque<DemoHostFeature>& features) const
  {
    // Try to locate a host that supports all the required features
    {
      auto itr = m_records.begin();
      while (itr != m_records.end())
      {
        if (CanSupport(itr->Features, features))
        {
          return itr->HostSetup;
        }
        ++itr;
      }
    }

    // None exists, so lets output a nice useful error message.
    {
      fmt::memory_buffer errorMessage;

      fmt::format_to(std::back_inserter(errorMessage), "There is no host available that supports: ");
      auto itr = features.begin();
      while (itr != features.end())
      {
        fmt::format_to(std::back_inserter(errorMessage), "{}", DemoHostFeatureName::ToString(itr->Name));
        ++itr;
        if (itr != features.end())
        {
          fmt::format_to(std::back_inserter(errorMessage), ", ");
        }
      }
      throw NotSupportedException(fmt::to_string(errorMessage));
    }
  }


  void DemoHostRegistry::Register(const DemoHostFeatureName::Enum feature, const DemoHostSetup& hostSetup)
  {
    m_records.emplace_back(feature, hostSetup);
  }


  void DemoHostRegistry::Register(const std::deque<DemoHostFeatureName::Enum>& features, const DemoHostSetup& hostSetup)
  {
    m_records.emplace_back(features, hostSetup);
  }


  DemoHostRegistry::Record::Record(const DemoHostFeatureName::Enum feature, DemoHostSetup hostSetup)
    : HostSetup(std::move(hostSetup))

  {
    Features.push_back(feature);
  }


  DemoHostRegistry::Record::Record(std::deque<DemoHostFeatureName::Enum> features, DemoHostSetup hostSetup)
    : HostSetup(std::move(hostSetup))
    , Features(std::move(features))
  {
  }
}
