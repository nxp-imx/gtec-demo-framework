#ifndef FSLDEMOAPP_BASE_SERVICE_OPTIONS_OPTIONS_HPP
#define FSLDEMOAPP_BASE_SERVICE_OPTIONS_OPTIONS_HPP
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

#include <FslDemoApp/Base/Service/Options/IOptions.hpp>
#include <functional>

namespace Fsl
{
  //! @brief Allows access to the various active option parsers so alternative configuration information can be read if need be.
  class Options
  {
    std::weak_ptr<IOptions> m_options;

  public:
    explicit Options(std::weak_ptr<IOptions> options);

    std::shared_ptr<IOptionParser> GetOptionParser(const OptionParserId& optionParserId) const;
    std::shared_ptr<IOptionParser> TryGetOptionParser(const OptionParserId& optionParserId) const;
    std::shared_ptr<IOptionParser> GetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const;
    std::shared_ptr<IOptionParser> TryGetOptionParser(const std::function<bool(const std::shared_ptr<IOptionParser>&)>& matchFunction) const;

    template <typename T>
    std::shared_ptr<T> GetOptionParser() const
    {
      auto parser =
        GetOptionParser([](const std::shared_ptr<IOptionParser>& value) -> bool { return std::dynamic_pointer_cast<T>(value).get() != nullptr; });
      return std::dynamic_pointer_cast<T>(parser);
    }

    template <typename T>
    std::shared_ptr<T> TryGetOptionParser() const
    {
      auto parser =
        TryGetOptionParser([](const std::shared_ptr<IOptionParser>& value) -> bool { return std::dynamic_pointer_cast<T>(value).get() != nullptr; });
      return std::dynamic_pointer_cast<T>(parser);
    }

  private:
    template <typename T>
    static bool CanCast(const std::shared_ptr<T>& value)
    {
      return std::dynamic_pointer_cast<T>(value);
    }
  };
}

#endif
