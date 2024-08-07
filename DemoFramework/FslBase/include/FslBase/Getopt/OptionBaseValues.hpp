#ifndef FSLBASE_GETOPT_OPTIONBASEVALUES_HPP
#define FSLBASE_GETOPT_OPTIONBASEVALUES_HPP
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

namespace Fsl
{
  //! The base value for Demo HostManager Option Parsers (frame work internal)
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_HOSTMANAGER_OPTION_BASE_INTERNAL = 0x00100000;
  //! The base value for Demo Host Option Parsers (frame work internal)
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_HOST_OPTION_BASE_INTERNAL = 0x00200000;
  //! The base value for Demo App Option Parsers (framework internal)
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_APP_OPTION_BASE_INTERNAL = 0x00300000;
  //! The base value for Demo Host Option Parsers
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_HOST_OPTION_BASE = 0x01000000;
  //! The base value for Demo App Option Parsers
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_APP_OPTION_BASE = 0x02000000;
  //! The base value for demo service options
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_SERVICE_OPTION_BASE = 0x40000000;

  //! The base value for demo service options intervals
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr const int DEMO_SERVICE_OPTION_INTERVAL = 0x00010000;
}

#endif
