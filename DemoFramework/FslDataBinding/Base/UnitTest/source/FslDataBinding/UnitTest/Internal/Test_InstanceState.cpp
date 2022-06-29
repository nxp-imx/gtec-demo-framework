/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslDataBinding/Base/Internal/InstanceState.hpp>

using namespace Fsl;

namespace
{
  using TestInternal_InstanceState = TestFixtureFslBase;
}


TEST(Test_DataBinding, Create)
{
  DataBinding::Internal::InstanceState state;

  EXPECT_FALSE(state.IsObservable());
  EXPECT_FALSE(state.HasPendingChanges());
  EXPECT_EQ(DataBinding::DataBindingInstanceState::Alive, state.GetState());
  EXPECT_EQ(DataBinding::DataBindingInstanceType::DependencyObject, state.GetType());
  EXPECT_EQ(DataBinding::Internal::PropertyMethodsImplType::NotAvailable, state.GetPropertyMethodsImplType());
}


TEST(Test_DataBinding, CreateWithType)
{
  constexpr auto type = DataBinding::DataBindingInstanceType::DependencyObserverProperty;
  constexpr auto implType = DataBinding::Internal::PropertyMethodsImplType::ATypedDependencyProperty;
  DataBinding::Internal::InstanceState state(type, implType);

  EXPECT_FALSE(state.IsObservable());
  EXPECT_FALSE(state.HasPendingChanges());
  EXPECT_EQ(DataBinding::DataBindingInstanceState::Alive, state.GetState());
  EXPECT_EQ(type, state.GetType());
  EXPECT_EQ(implType, state.GetPropertyMethodsImplType());
}


TEST(Test_DataBinding, CreateWithTypeAndFlags)
{
  constexpr auto type = DataBinding::DataBindingInstanceType::DependencyObserverProperty;
  constexpr auto implType = DataBinding::Internal::PropertyMethodsImplType::ATypedDependencyProperty;
  constexpr auto flag = DataBinding::Internal::InstanceState::Flags::Observable;
  DataBinding::Internal::InstanceState state(type, implType, flag);

  EXPECT_TRUE(state.IsObservable());
  EXPECT_FALSE(state.HasPendingChanges());
  EXPECT_EQ(DataBinding::DataBindingInstanceState::Alive, state.GetState());
  EXPECT_EQ(type, state.GetType());
  EXPECT_EQ(implType, state.GetPropertyMethodsImplType());
  EXPECT_TRUE(state.IsEnabled(flag));
}


TEST(Test_DataBinding, SetDataBindingInstanceState)
{
  DataBinding::Internal::InstanceState state;

  auto newState = DataBinding::DataBindingInstanceState::DestroyScheduled;
  state.SetDataBindingInstanceState(newState);
  EXPECT_EQ(newState, state.GetState());

  newState = DataBinding::DataBindingInstanceState::Destroyed;
  state.SetDataBindingInstanceState(newState);
  EXPECT_EQ(newState, state.GetState());
}
