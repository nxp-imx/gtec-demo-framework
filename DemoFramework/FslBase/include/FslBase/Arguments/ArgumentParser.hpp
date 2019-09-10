#ifndef FSLBASE_ARGUMENTS_ARGUMENTPARSER_HPP
#define FSLBASE_ARGUMENTS_ARGUMENTPARSER_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Arguments/Command.hpp>
#include <FslBase/Arguments/EncodedCommand.hpp>
#include <FslBase/Arguments/ParseResult.hpp>
#include <cstdint>
#include <deque>

namespace Fsl
{
  namespace Arguments
  {
    struct ParseErrorInfo;

    namespace ArgumentParser
    {
      //! @brief Try to parse the argument list into commands.
      //! @param rEncodedArguments (the parsed argument commands)
      //! @param the argument count (>= 0)
      //! @param the arguments as a list of valid pointers (can not be a nullptr and neither can the elements being pointed to)
      //! @param commands a list of valid commands.
      //! @param pErrorInfo a optional structure that will be filled with extra error information in case a error occurs.
      //! @note BEWARE: this does not skip the initial argument, so if you are parsing parameters directly from the main ensure that you
      //!               do argc-1, argv+1
      ParseResult TryParse(std::deque<EncodedCommand>& rEncodedCommands, const int argCount, const char* const* const ppszArgs,
                           const std::deque<Command>& commands, ParseErrorInfo* pErrorInfo = nullptr);
    };
  }
}

#endif
