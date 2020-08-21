#ifdef __linux__
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

#include <FslDemoService/CpuStats/Impl/Adapter/Linux/ProcFileUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringToValue.hpp>

namespace Fsl
{
  namespace ProcFileUtil
  {
    enum class CPUParseState
    {
      FindCPU,
      FindNextLine,
      ParseCPULine,
      ParseCPULineSkipSpace,
      ParseCPULineCompleted,
      ParseCPULineDone,
    };

    //! cpuX user nice system idle ... ... ...
    //!   user = normal processes executing in user mode
    //!   nice = nice processes executing in user mode
    //!   system = processes executing in kernel mode
    //!   idle = twiddling thumbs
    bool TryParseCPUStats(std::vector<CPUInfo>& rParsed, BasicFileReader& file, BufferedFileParser<4096>& fileParser)
    {
      rParsed.clear();
      fileParser.Clear();
      if (!file.IsGood())
      {
        FSLLOG3_DEBUG_WARNING("Failed to read file");
        return false;
      }

      std::array<char, 32> lineBuffer{};
      uint32_t parseIndex = 0u;
      CPUParseState parseState = CPUParseState::FindCPU;
      std::array<uint64_t, 5> parsedValues{};
      while (fileParser.IsGood(file))
      {
        switch (parseState)
        {
        case CPUParseState::FindCPU:
          parseState = fileParser.StartsWith(file, "cpu") ? CPUParseState::ParseCPULine : CPUParseState::FindNextLine;
          break;
        case CPUParseState::FindNextLine:
          // We ignore the return value as if it returns false the while loop should stop due to "IsGood() == false".
          fileParser.TrySkipToNewLine(file);
          parseState = CPUParseState::FindCPU;
          break;
        case CPUParseState::ParseCPULine:
        {
          std::size_t written = 0;
          if (fileParser.TryReadLineUntil(written, file, lineBuffer.data(), lineBuffer.size() - 1, ' ') && written > 0)
          {
            // zero terminate by appending a zero (which we always reserved room for above)
            lineBuffer[written] = 0;
            // FSLLOG3_INFO("parse entry: '{0}'", lineBuffer.data());
            switch (parseIndex)
            {
            case 0:    // CPU
              if (written > 3)
              {
                assert(lineBuffer[0] == 'c' && lineBuffer[1] == 'p' && lineBuffer[2] == 'u');
                if (!StringToValue::TryParse(parsedValues[parseIndex], StringViewLite(lineBuffer.data() + 3, written - 3)))
                {
                  FSLLOG3_DEBUG_WARNING("Failed to parse entry {0} containing '{1}'", parseIndex, lineBuffer.data());
                  parseState = CPUParseState::ParseCPULineDone;
                }
                else
                {
                  ++parseIndex;
                  parseState = CPUParseState::ParseCPULineSkipSpace;
                }
              }
              else
              {
                parseState = CPUParseState::FindNextLine;
              }
              break;
            case 1:    // User
            case 2:    // Nice
            case 3:    // System
            case 4:    // Idle
              if (!StringToValue::TryParse(parsedValues[parseIndex], StringViewLite(lineBuffer.data(), written)))
              {
                FSLLOG3_DEBUG_WARNING("Failed to parse entry {0} containing '{1}'", parseIndex, lineBuffer.data());
                parseState = CPUParseState::ParseCPULineDone;
              }
              else
              {
                ++parseIndex;
                parseState = parseIndex <= 4 ? CPUParseState::ParseCPULineSkipSpace : CPUParseState::ParseCPULineCompleted;
              }
              break;
            default:
              parseState = CPUParseState::ParseCPULineCompleted;
              break;
            }
          }
          else
          {
            FSLLOG3_DEBUG_WARNING("Failed to parse cpu entry");
            parseState = CPUParseState::ParseCPULineDone;
          }
          break;
        }
        case CPUParseState::ParseCPULineSkipSpace:
          fileParser.TrySkipCharacter(file, ' ');
          parseState = CPUParseState::ParseCPULine;
          break;
        case CPUParseState::ParseCPULineCompleted:
        {
          rParsed.emplace_back(CPUInfo{parsedValues[0], parsedValues[1], parsedValues[2], parsedValues[3], parsedValues[4]});
          parseState = CPUParseState::ParseCPULineDone;
          break;
        }
        case CPUParseState::ParseCPULineDone:
          parseState = CPUParseState::FindNextLine;
          parseIndex = 0u;
          break;
        default:
          throw NotSupportedException("unsupported parse state");
        }
      }
      return true;
    }

    enum class RAMParseState
    {
      FindVmRSS,
      FindNextLine,
      ParseVmRSSLine,
      ParseVmRSSLineSkipWhitespaces,
      ParseVmRSSLineCompleted,
      ParseVmRSSLineFailed
    };


    bool TryParseRAMStats(RAMInfo& rParsed, BasicFileReader& file, BufferedFileParser<4096>& fileParser)
    {
      rParsed = {};
      fileParser.Clear();

      std::array<char, 32> lineBuffer{};
      uint32_t parseIndex = 0u;
      RAMParseState parseState = RAMParseState::FindVmRSS;
      uint64_t resultVmRSS = 0;
      while (fileParser.IsGood(file))
      {
        switch (parseState)
        {
        case RAMParseState::FindVmRSS:
          parseState = fileParser.StartsWith(file, "VmRSS:") ? RAMParseState::ParseVmRSSLine : RAMParseState::FindNextLine;
          break;
        case RAMParseState::FindNextLine:
          // We ignore the return value as if it returns false the while loop should stop due to "IsGood() == false".
          fileParser.TrySkipToNewLine(file);
          parseState = RAMParseState::FindVmRSS;
          break;
        case RAMParseState::ParseVmRSSLine:
        {
          std::size_t written = 0;
          if (fileParser.TryReadLineUntil(written, file, lineBuffer.data(), lineBuffer.size() - 1, ' ', true) && written > 0)
          {
            // zero terminate by appending a zero (which we always reserved room for above)
            lineBuffer[written] = 0;
            // FSLLOG3_INFO("parse entry: '{0}'", lineBuffer.data());
            switch (parseIndex)
            {
            case 0:    // VmRSS:
              if (written > 6)
              {
                assert(lineBuffer[0] == 'V' && lineBuffer[1] == 'm' && lineBuffer[2] == 'R' && lineBuffer[3] == 'S' && lineBuffer[4] == 'S' &&
                       lineBuffer[5] == ':');
                ++parseIndex;
                parseState = RAMParseState::ParseVmRSSLineSkipWhitespaces;
              }
              else
              {
                parseState = RAMParseState::FindNextLine;
              }
              break;
            case 1:    // memory
              if (!StringToValue::TryParse(resultVmRSS, StringViewLite(lineBuffer.data(), written)))
              {
                FSLLOG3_DEBUG_WARNING("Failed to parse entry {0} containing '{1}'", parseIndex, lineBuffer.data());
                parseState = RAMParseState::ParseVmRSSLineFailed;
              }
              else
              {
                ++parseIndex;
                parseState = RAMParseState::ParseVmRSSLineSkipWhitespaces;
              }
              break;
            case 2:    // memory unit type
              ++parseIndex;
              if (StringViewLite(lineBuffer.data(), written) == "kB")
              {
                // Convert to bytes
                resultVmRSS *= 1000;
                parseState = RAMParseState::ParseVmRSSLineCompleted;
              }
              else
              {
                FSLLOG3_DEBUG_WARNING("VmRSS was not using the expected unit type");
                parseState = RAMParseState::ParseVmRSSLineFailed;
              }
              break;
            default:
              parseState = RAMParseState::ParseVmRSSLineCompleted;
              break;
            }
          }
          else
          {
            FSLLOG3_DEBUG_WARNING("Failed to parse VmRSS entry");
            parseState = RAMParseState::ParseVmRSSLineFailed;
          }
          break;
        }
        case RAMParseState::ParseVmRSSLineSkipWhitespaces:
          fileParser.TrySkipCharacter(file, ' ');
          parseState = RAMParseState::ParseVmRSSLine;
          break;
        case RAMParseState::ParseVmRSSLineCompleted:
        {
          rParsed = RAMInfo{resultVmRSS};
          parseState = RAMParseState::ParseVmRSSLineFailed;
          return true;
        }
        case RAMParseState::ParseVmRSSLineFailed:
          return false;
        default:
          throw NotSupportedException("unsupported parse state");
        }
      }
      FSLLOG3_DEBUG_WARNING("Failed to to locate entry");
      return false;
    }
  }
}

#endif
