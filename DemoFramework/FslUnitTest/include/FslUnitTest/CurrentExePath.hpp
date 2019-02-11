#ifndef FSLUNITTEST_CURRENTEXEPATH_HPP
#define FSLUNITTEST_CURRENTEXEPATH_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <gtest/gtest.h>

namespace Fsl
{
  // Nasty global to keep the current exe path
  class CurrentExePath
  {
    static const char* g_pszEXE;

  public:
    // For this to work remember to insert the ScopedExePath line before calling Google test init
    // - Fsl::CurrentExePath::ScopedExePath exeScope(argc > 0 ? argv[0] : nullptr);
    static const char* TryGetCurrentExePath()
    {
      return g_pszEXE;
    }

    class ScopedExePath
    {
    public:
      ScopedExePath(const char* psz)
      {
        CurrentExePath::SetCurrentExePath(psz);
      }

      ~ScopedExePath()
      {
        CurrentExePath::SetCurrentExePath(nullptr);
      }
    };

  private:
    static void SetCurrentExePath(const char* psz);
  };
}

#endif
