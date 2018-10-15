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

#if defined(_MSC_VER)
#define FSL_ATTR_DEPRECATED __declspec(deprecated)
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
#elif defined(__GNUC__)
#if __cplusplus > 201103
#define FSL_ATTR_DEPRECATED [[deprecated]]
#else
#define FSL_ATTR_DEPRECATED
#endif
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
// __attribute__ ((deprecated)) but its a postfix thing again :(
// So its better to wait for [[deprecated]](C++14)
#elif defined(__QNXNTO__)
#define FSL_ATTR_DEPRECATED
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
#else
#pragma message("WARNING: It would be a good idea to implement FSL_ATTR_DEPRECATED, FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT for this compiler")
#define FSL_ATTR_DEPRECATED
#define FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
#endif

#endif
