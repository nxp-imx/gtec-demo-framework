#ifndef FSLBASE_GETOPT_IOPTIONPARSER_HPP
#define FSLBASE_GETOPT_IOPTIONPARSER_HPP
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

#include <deque>
#include <string>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Getopt/Option.hpp>
#include <FslBase/Getopt/OptionParseResult.hpp>

namespace Fsl
{
  //! @brief This is a very thin layer that allows multiple sources to specify the getopt options they need
  class IOptionParser
  {
  public:
    virtual ~IOptionParser() = default;

    //! @brief Return the name of this option parser
    virtual std::string GetName() const = 0;

    //! @brief Configure the input arguments
    //! @param rOptions [in] Options.
    virtual void ArgumentSetup(std::deque<Option>& rOptions) = 0;

    //! @brief Parse a input option.
    virtual OptionParseResult::Enum Parse(const int32_t cmdId, const char* const pszOptArg) = 0;

    //! @brief Parse a input option.
    //! @return true to continue, false to exit
    virtual bool ParsingComplete() = 0;
  };
}

#endif
