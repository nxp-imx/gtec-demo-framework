/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <FslBase/Log/Log.hpp>
#include <FslDemoHost/Base/Service/Options/OptionsService.hpp>

namespace Fsl
{
  OptionsService::OptionsService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  std::shared_ptr<IOptionParser> OptionsService::GetOptionParser(const OptionParserId& optionParserId) const
  {
    auto optionParser = TryGetOptionParser(optionParserId);
    if (!optionParser)
    {
      throw NotFoundException("The requested option parser was not found");
    }
    return optionParser;
  }


  std::shared_ptr<IOptionParser> OptionsService::TryGetOptionParser(const OptionParserId& optionParserId) const
  {
    const auto itr = m_optionParsers.find(optionParserId);
    return itr != m_optionParsers.end() ? itr->second : std::shared_ptr<IOptionParser>();
  }


  std::shared_ptr<IOptionParser>
    OptionsService::GetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const
  {
    auto optionParser = TryGetOptionParser(matchFunction);
    if (!optionParser)
    {
      throw NotFoundException("The requested option parser was not found");
    }
    return optionParser;
  }


  std::shared_ptr<IOptionParser>
    OptionsService::TryGetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const
  {
    if (!matchFunction)
    {
      throw std::invalid_argument("matchFunction must be valid");
    }

    for (const auto& keyValue : m_optionParsers)
    {
      if (matchFunction(keyValue.second))
      {
        return keyValue.second;
      }
    }
    return std::shared_ptr<IOptionParser>();
  }


  void OptionsService::AddOptionParser(const std::shared_ptr<IOptionParser>& optionParser)
  {
    if (!optionParser)
    {
      throw std::invalid_argument("optionParser can not be null");
    }

    auto p = optionParser.get();

    const OptionParserId optionParserId(typeid(*p));

    FSLLOG_WARNING_IF(m_optionParsers.find(optionParserId) != m_optionParsers.end(),
                      "A option parser for the given id has already been registered, request ignored");

    m_optionParsers[optionParserId] = optionParser;
  }
}
