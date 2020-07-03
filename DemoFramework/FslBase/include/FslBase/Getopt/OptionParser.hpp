#ifndef FSLBASE_GETOPT_OPTIONPARSER_HPP
#define FSLBASE_GETOPT_OPTIONPARSER_HPP
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
#include <FslBase/ReadOnlySpan.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <deque>

namespace Fsl
{
  class OptionParser
  {
  public:
    // Switched to C++11 way of doing it
    OptionParser(const OptionParser&) = delete;
    OptionParser& operator=(const OptionParser&) = delete;


    struct ParserRecord
    {
      IOptionParser* Parser{nullptr};
      int32_t CmdIdOffset{0};

      ParserRecord() = default;

      ParserRecord(IOptionParser* parser, const int32_t cmdIdOffset)
        : Parser(parser)
        , CmdIdOffset(cmdIdOffset)
      {
      }
    };

    enum class Result
    {
      Failed = 0,
      OK = 1,
      Exit = 2,
    };

    struct ParseResult
    {
      Result Status{Result::Failed};
      uint32_t VerbosityLevel{0};

      constexpr ParseResult() = default;

      constexpr ParseResult(const Result result, const uint32_t verbosityLevel)
        : Status(result)
        , VerbosityLevel(verbosityLevel)
      {
      }
    };

    //! @brief Parse command line arguments we expect the first parameter to be the filename
    static ParseResult Parse(int argc, char** argv, StringViewLite strHelpCaption);
    //! @brief Parse command line arguments we expect the first parameter to be the filename
    static ParseResult Parse(int argc, char** argv, IOptionParser& inputOptionParser, StringViewLite strHelpCaption);
    //! @brief Parse command line arguments we expect the first parameter to be the filename
    static ParseResult Parse(int argc, char** argv, const std::deque<IOptionParser*>& inputOptionParsers, StringViewLite strHelpCaption);
    //! @brief Parse command line arguments we expect the first parameter to be the filename
    static ParseResult Parse(int argc, char** argv, const std::deque<ParserRecord>& inputOptionParsers, StringViewLite strHelpCaption);

    //! @brief Here we expect that the args has been preprocessed a bit and the filename has been stripped so we only look at real args
    static ParseResult Parse(const ReadOnlySpan<StringViewLite> args, StringViewLite strHelpCaption);
    //! @brief Here we expect that the args has been preprocessed a bit and the filename has been stripped so we only look at real args
    static ParseResult Parse(const ReadOnlySpan<StringViewLite> args, IOptionParser& inputOptionParser, StringViewLite strHelpCaption);
    //! @brief Here we expect that the args has been preprocessed a bit and the filename has been stripped so we only look at real args
    static ParseResult Parse(const ReadOnlySpan<StringViewLite> args, const std::deque<IOptionParser*>& inputOptionParsers,
                             StringViewLite strHelpCaption);
    //! @brief Here we expect that the args has been preprocessed a bit and the filename has been stripped so we only look at real args
    static ParseResult Parse(const ReadOnlySpan<StringViewLite> args, const std::deque<ParserRecord>& inputOptionParsers,
                             StringViewLite strHelpCaption);
  };
}

#endif
