/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>

using namespace Fsl;

namespace
{
  using Test_Binding = TestFixtureFslBase;
}


TEST(Test_Binding, Create_Source)
{
  constexpr DataBinding::DataBindingInstanceHandle SrcHandle(0);
  DataBinding::Binding binding(SrcHandle);

  EXPECT_TRUE(binding.ContainsSource(SrcHandle));
  EXPECT_FALSE(binding.ComplexBinding());

  const auto sourceHandleSpan = binding.SourceHandlesAsSpan();
  ASSERT_FALSE(sourceHandleSpan.empty());
  ASSERT_EQ(1u, sourceHandleSpan.size());
  EXPECT_EQ(SrcHandle, sourceHandleSpan[0]);
}


TEST(Test_Binding, Create_Source_Converter)
{
  constexpr DataBinding::DataBindingInstanceHandle SrcHandle(0);

  auto converterBinding = std::make_shared<DataBinding::ConverterBinding<float, int32_t>>([](const int32_t val) { return static_cast<float>(val); });
  DataBinding::Binding binding(converterBinding, SrcHandle);

  EXPECT_TRUE(binding.ContainsSource(SrcHandle));
  EXPECT_EQ(converterBinding, binding.ComplexBinding());

  const auto sourceHandleSpan = binding.SourceHandlesAsSpan();
  ASSERT_EQ(1u, sourceHandleSpan.size());
  EXPECT_EQ(SrcHandle, sourceHandleSpan[0]);
}


TEST(Test_Binding, Create_Source_MultiConverter)
{
  constexpr DataBinding::DataBindingInstanceHandle SrcHandle0(0);
  constexpr DataBinding::DataBindingInstanceHandle SrcHandle1(1);

  auto converterBinding = std::make_shared<DataBinding::MultiConverterBinding<float, int32_t, int32_t>>([](const int32_t val0, const int32_t val1)
                                                                                                        { return static_cast<float>(val0 + val1); });
  DataBinding::Binding binding(converterBinding, SrcHandle0, SrcHandle1);

  EXPECT_TRUE(binding.ContainsSource(SrcHandle0));
  EXPECT_EQ(converterBinding, binding.ComplexBinding());

  const auto sourceHandleSpan = binding.SourceHandlesAsSpan();
  ASSERT_EQ(2u, sourceHandleSpan.size());
  EXPECT_EQ(SrcHandle0, sourceHandleSpan[0]);
  EXPECT_EQ(SrcHandle1, sourceHandleSpan[1]);
}
