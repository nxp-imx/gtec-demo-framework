#ifndef FSLUNITTEST_TESTCUSTOMCHECKS_HPP
#define FSLUNITTEST_TESTCUSTOMCHECKS_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <stdexcept>

namespace Fsl
{
  // namespace Test
  //{
  //  template <typename T>
  //  inline ::testing::AssertionResult MemoryContentEquals(const T* const pSrc, const std::size_t length, const std::size_t index, const T value)
  //  {
  //    if (pSrc == nullptr)
  //    {
  //      throw std::invalid_argument("pSrc can not be null");
  //    }
  //    if (index >= length)
  //    {
  //      throw std::invalid_argument("index out of bounds");
  //    }

  //    if (pSrc[index] == value)
  //    {
  //      return ::testing::AssertionSuccess();
  //    }
  //    return ::testing::AssertionFailure() << "Expected value at " << static_cast<const void*>(pSrc) << " was " <<
  //    static_cast<uint64_t>(pSrc[index])
  //                                         << " expected " << static_cast<uint64_t>(value);
  //  }

  //  template <typename T>
  //  inline ::testing::AssertionResult MemoryContent3Equals(const T* const pSrc, const std::size_t length, const std::size_t index, const T value0,
  //                                                         const T value1, const T value2)
  //  {
  //    if (pSrc == nullptr)
  //    {
  //      throw std::invalid_argument("pSrc can not be null");
  //    }
  //    if (length < 3u)
  //    {
  //      throw std::invalid_argument("index out of bounds");
  //    }
  //    if (index > (length - 3u))
  //    {
  //      throw std::invalid_argument("index out of bounds");
  //    }

  //    if (pSrc[index] == value0 && pSrc[index + 1] == value1 && pSrc[index + 2] == value2)
  //    {
  //      return ::testing::AssertionSuccess();
  //    }

  //    std::string strError = fmt::format("Memory {0} content at index {1}, {2}, {3}:", static_cast<const void*>(pSrc), index, index + 1, index + 2);
  //    if (pSrc[index] != value0)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 0, static_cast<uint64_t>(value0), static_cast<uint64_t>(pSrc[index + 0]));
  //    }
  //    if (pSrc[index + 1] != value1)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 1, static_cast<uint64_t>(value1), static_cast<uint64_t>(pSrc[index + 1]));
  //    }
  //    if (pSrc[index + 2] != value2)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 2, static_cast<uint64_t>(value2), static_cast<uint64_t>(pSrc[index + 2]));
  //    }

  //    return ::testing::AssertionFailure() << strError;
  //  }

  //  template <typename T>
  //  inline ::testing::AssertionResult MemoryContent4Equals(const T* const pSrc, const std::size_t length, const std::size_t index, const T value0,
  //                                                         const T value1, const T value2, const T value3)
  //  {
  //    if (pSrc == nullptr)
  //    {
  //      throw std::invalid_argument("pSrc can not be null");
  //    }
  //    if (length < 4u)
  //    {
  //      throw std::invalid_argument("index out of bounds");
  //    }
  //    if (index > (length - 4u))
  //    {
  //      throw std::invalid_argument("index out of bounds");
  //    }

  //    if (pSrc[index] == value0 && pSrc[index + 1] == value1 && pSrc[index + 2] == value2 && pSrc[index + 3] == value3)
  //    {
  //      return ::testing::AssertionSuccess();
  //    }

  //    std::string strError =
  //      fmt::format("Memory {0} content at index {1}, {2}, {3}, {4}:", static_cast<const void*>(pSrc), index, index + 1, index + 2, index + 3);
  //    if (pSrc[index] != value0)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 0, static_cast<uint64_t>(value0), static_cast<uint64_t>(pSrc[index + 0]));
  //    }
  //    if (pSrc[index + 1] != value1)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 1, static_cast<uint64_t>(value1), static_cast<uint64_t>(pSrc[index + 1]));
  //    }
  //    if (pSrc[index + 2] != value2)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 2, static_cast<uint64_t>(value2), static_cast<uint64_t>(pSrc[index + 2]));
  //    }
  //    if (pSrc[index + 3] != value3)
  //    {
  //      strError += fmt::format(" [{0}] was {2} expected {1}", index + 3, static_cast<uint64_t>(value3), static_cast<uint64_t>(pSrc[index + 3]));
  //    }

  //    return ::testing::AssertionFailure() << strError;
  //  }
  //}
}

#endif
