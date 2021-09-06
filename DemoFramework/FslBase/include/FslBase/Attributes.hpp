#ifndef FSLBASE_ATTRIBUTES_HPP
#define FSLBASE_ATTRIBUTES_HPP
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

// Defines:
// FSL_FUNC_WARN_UNUSED_RESULT            Issue a warning if the result is unused (do not use together with FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT)
// FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT    Issue a warning if the result is unused (deprecated use FSL_FUNC_WARN_UNUSED_RESULT instead, do not use
// both)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201710L

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT [[nodiscard]]
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT

#else

#if defined(__clang__)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT [[nodiscard]]
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

#elif defined(_MSC_VER)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT    // _Check_return_
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT

#elif defined(__GNUC__)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

//#if __GNUC__ <= 5
//#error GCC5 is unsupported as it has known issues with constexpr.
//#endif

#elif defined(__QNXNTO__)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT

#else

#pragma message("WARNING: It would be a good idea to implement FSL_FUNC_WARN_UNUSED_RESULT for this compiler")
#pragma message("WARNING: It would be a good idea to implement FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT for this compiler")
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_WARN_UNUSED_RESULT
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT

#endif
#endif

#endif
