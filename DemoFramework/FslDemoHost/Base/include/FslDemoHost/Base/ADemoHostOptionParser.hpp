#ifndef FSLDEMOHOST_BASE_ADEMOHOSTOPTIONPARSER_HPP
#define FSLDEMOHOST_BASE_ADEMOHOSTOPTIONPARSER_HPP
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

#include <FslBase/Getopt/IOptionParser.hpp>
#include <FslBase/ITag.hpp>
#include <FslNativeWindow/Base/NativeWindowConfig.hpp>
#include <memory>

namespace Fsl
{
  enum class DemoHostOptionConfig
  {
    ConsoleApp,
    WindowApp
  };

  class ADemoHostOptionParser : public IOptionParser
  {
    NativeWindowConfig m_nativeWindowConfig;
    std::shared_ptr<ITag> m_nativeWindowTag;
    DemoHostOptionConfig m_demoHostOptionConfig;
    bool m_userControlledWindow;

  public:
    ADemoHostOptionParser(const ADemoHostOptionParser&) = delete;
    ADemoHostOptionParser& operator=(const ADemoHostOptionParser&) = delete;
    explicit ADemoHostOptionParser(const DemoHostOptionConfig config);


    std::string GetName() const override
    {
      return {"DemoHostOptionParser"};
    }

    void ArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult Parse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    bool ParsingComplete() override;

    //! @brief Get the native window config
    NativeWindowConfig GetNativeWindowConfig() const;

    //! @brief Set the native window tag
    void SetNativeWindowTag(const std::shared_ptr<ITag>& tag);

    //! @brief Get the native window tag
    std::shared_ptr<ITag> GetNativeWindowTag() const;
  };
}

#endif
