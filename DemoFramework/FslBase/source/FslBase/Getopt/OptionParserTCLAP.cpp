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

#include "OptionParserTCLAP.hpp"
#include <tclap/CmdLine.h>

#define DEFAULT_STR "-not-"

namespace Fsl
{
  namespace
  {

    void ToTCLAPArguments(std::deque<OptionParserTCLAP::ArgRecord>& rArgs, const std::deque<Option>& combinedOptions, TCLAP::CmdLine& rCmd)
    {
      int dstIdx = 0;
      std::deque<Option>::const_iterator srcItr = combinedOptions.begin();
      while (srcItr != combinedOptions.end())
      {
        // This slices the extended Option classes (which is what we want)
        std::shared_ptr<TCLAP::Arg> arg;

        switch(srcItr->HasArg)
        {
        case OptionArgument::OptionNone:
          arg = std::make_shared<TCLAP::SwitchArg>(srcItr->ShortName, srcItr->Name, srcItr->Description, rCmd, false);
          break;
        //case OptionArgument::OptionOptional:
          //arg = std::make_shared<TCLAP::ValueArg<std::string> >(srcItr->ShortName, srcItr->Name, srcItr->Description, false, DEFAULT_STR, "", rCmd);
          //  break;
        case OptionArgument::OptionRequired:
          if ( ! srcItr->IsPositional )
            arg = std::make_shared<TCLAP::ValueArg<std::string> >(srcItr->ShortName, srcItr->Name, srcItr->Description, false, DEFAULT_STR, "", rCmd);
          else
          {
            const bool isRequired = srcItr->HasArg == OptionArgument::OptionRequired;
            arg = std::make_shared<TCLAP::UnlabeledValueArg<std::string> >(srcItr->Name, srcItr->Description, isRequired, DEFAULT_STR, "", rCmd);
          }
          break;
        }

        rArgs.push_back(OptionParserTCLAP::ArgRecord(srcItr->CmdId, arg));
        ++srcItr;
        ++dstIdx;
      }
    }

    class MyOutput : public TCLAP::StdOutput
    {

      virtual void failure(TCLAP::CmdLineInterface& c, TCLAP::ArgException& e)
      {
        throw e;
      }


      virtual void usage(TCLAP::CmdLineInterface& c)
      {
      }

      virtual void version(TCLAP::CmdLineInterface& c)
      {
      }
    };
  }


  OptionParserTCLAP::OptionParserTCLAP(int argc, char** argv, const std::deque<Option>& options)
    : m_index(0)
  {
    try
    {
      TCLAP::CmdLine cmd("Command description message", ' ', "0.0", false);

      MyOutput my;
      cmd.setOutput(&my);

      // Convert to TCLAP arguments
      ToTCLAPArguments(m_args, options, cmd);


      cmd.parse(argc, argv);
    }
    catch (TCLAP::ArgException&)  // catch any exceptions
    {
      //std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
      throw;
    }
  }


  OptionParserTCLAP::~OptionParserTCLAP()
  {
  }


  bool OptionParserTCLAP::Next(int& rValue, std::string& rStrOptArg)
  {
    bool bContinue;
    do
    {
      bContinue = false;
      rValue = 0;
      rStrOptArg.clear();

      if (m_index < m_args.size())
      {
        rValue = m_args[m_index].Value;

        //TCLAP::UnlabeledValueArg<std::string>* pUnlabeledVal = dynamic_cast<TCLAP::UnlabeledValueArg<std::string>*>(m_args[m_index].Arg.get());
        //if (pUnlabeledVal != nullptr)
        //{
        //  rStrOptArg = pUnlabeledVal->getValue();
        //  if (rStrOptArg == DEFAULT_STR)
        //    bContinue = true;
        //}
        //else
        {
          TCLAP::ValueArg<std::string>* pVal = dynamic_cast<TCLAP::ValueArg<std::string>*>(m_args[m_index].Arg.get());
          if (pVal != nullptr)
          {
            rStrOptArg = rStrOptArg = pVal->getValue();
            if (rStrOptArg == DEFAULT_STR)
              bContinue = true;
          }
          else
          {
            TCLAP::SwitchArg* pVal2 = dynamic_cast<TCLAP::SwitchArg*>(m_args[m_index].Arg.get());
            if (pVal2 != nullptr)
            {
              if (!pVal2->getValue())
                bContinue = true;
            }
          }
        }
      }
      ++m_index;
    } while (bContinue && m_index <= m_args.size());
    return m_index <= m_args.size();
  }

}
