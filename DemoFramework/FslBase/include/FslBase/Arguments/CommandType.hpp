#ifndef FSLBASE_ARGUMENTS_COMMANDTYPE_HPP
#define FSLBASE_ARGUMENTS_COMMANDTYPE_HPP
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

namespace Fsl
{
  namespace Arguments
  {
    enum class CommandTypeFlags
    {
      //! The argument type is followed by a value
      Value = 0x1000,
      //! This is a positional value
      Positional = 0x2000,
      //! The argument type can be repeated multiple times
      Multi = 0x4000,
    };


    enum class CommandType
    {
      //! Not used.
      Undefined = 0,
      // A optional switch argument (on/off)
      Switch = 1,
      // A optional switch argument that can be specified multiple times, for example to increase verbosity levels '-v', '-vv' or '-v -v'
      MultiSwitch = (static_cast<int>(Switch) | static_cast<int>(CommandTypeFlags::Multi)),
      // A optional argument with a associated value
      Value = (2 | static_cast<int>(CommandTypeFlags::Value)),
      // A optional argument with a associated value that can be specified multiple times
      MultiValue = (static_cast<int>(Value) | static_cast<int>(CommandTypeFlags::Multi)),
      // A optional positional value argument without a name
      PositionalValue = 3 | static_cast<int>(CommandTypeFlags::Positional),
      // A optional positional value argument without a name that can be specified multiple times
      // MultiPositionalValue = 4 | (static_cast<int>(CommandTypeFlags::Positional) | static_cast<int>(CommandTypeFlags::Multi)),
      // All arguments found after '--' will be classified as unhandled
      Unhandled = 7
    };
  }
}

#endif
