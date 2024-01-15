/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/Internal/PropertySetInfo.hpp>
#include <FslDataBinding/Base/Internal/TypedPropertyMethodsUtil.hpp>
#include <tuple>
#include <utility>

using namespace Fsl;

namespace
{
  using Test_MultiConvertBack = TestFixtureFslBase;

  std::tuple<float, float, float, float> ConvertBackViaTuple(const uint32_t color)
  {
    return std::make_tuple(static_cast<float>(color & 0xFF) / 255.0f, static_cast<float>((color >> 8) & 0xFF) / 255.0f,
                           static_cast<float>((color >> 16) & 0xFF) / 255.0f, static_cast<float>((color >> 24) & 0xFF) / 255.0f);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  template <typename TSet>
  void DoCallSet(DataBinding::Internal::PropertySetInfo setter, const TSet& value)
  {
  }


  template <size_t I, typename... TSetType>
  void CallSetters(const std::tuple<TSetType...>& values, const ReadOnlySpan<DataBinding::Internal::PropertySetInfo> setters)
  {
    if constexpr (I < sizeof...(TSetType))
    {
      // Call DoCallSet for the I-th element of the tuple and corresponding setter
      DoCallSet(setters[I], std::get<I>(values));

      // Recursively call CallSetters for the next element
      CallSetters<I + 1, TSetType...>(values, setters);
    }
  }


  template <class... TSetType>
  void TestConvertBackHelper(const uint32_t srcValue, const ReadOnlySpan<DataBinding::Internal::PropertySetInfo> setters)
  {
    CallSetters<0, TSetType...>(ConvertBackViaTuple(srcValue), setters);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------


  template <size_t TFixedIndex, typename... TSetType>
  void CallSetters2(Span<DataBinding::Internal::PropertySetResult> result, const std::tuple<TSetType...>& values,
                    const ReadOnlySpan<DataBinding::Internal::PropertySetInfo> setters)
  {
    if constexpr (TFixedIndex < sizeof...(TSetType))
    {
      // Call DoCallSet for the I-th element of the tuple and corresponding setter
      result[TFixedIndex] = DataBinding::Internal::TypedPropertyMethodsUtil::SetByRef(setters[TFixedIndex].ImplType, setters[TFixedIndex].pSet,
                                                                                      std::get<TFixedIndex>(values));

      // Recursively call CallSetters for the next element
      CallSetters2<TFixedIndex + 1, TSetType...>(result, values, setters);
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  std::array<DataBinding::Internal::PropertySetResult, 10> TestConvertBackHelper2(const uint32_t srcValue,
                                                                                  const ReadOnlySpan<DataBinding::Internal::PropertySetInfo> setters)
  {
    std::array<DataBinding::Internal::PropertySetResult, 10> result{};
    CallSetters2<0>(SpanUtil::AsSpan(result), ConvertBackViaTuple(srcValue), setters);
    return result;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

}

TEST(Test_MultiConvertBack, Construct)
{
  std::array<DataBinding::Internal::PropertySetInfo, 4> setters{};

  TestConvertBackHelper(0x10203040u, ReadOnlySpanUtil::AsSpan(setters));
  auto result = TestConvertBackHelper2(0x10203040u, ReadOnlySpanUtil::AsSpan(setters));
  ASSERT_EQ(result.size(), 10u);
}
