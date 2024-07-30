/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"
#include "UTReadOnlyDependencyObject.hpp"

using namespace Fsl;

namespace
{
  using Test_GetRSet_TwoWay = TestFixtureFslBase;
}

TEST(Test_GetRSet_TwoWay, SetBinding_SourceCanNotBeTargetOfOneWayBinding)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  // A source of a two-way binding can not be the target of a one-way binding
  EXPECT_THROW(t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay),
               DataBinding::TwoWayBindingSourceException);
}

TEST(Test_GetRSet_TwoWay, SetBinding_OneWayBindTargetCanNotBeTwoWayBindingSource)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // A one-way binding target can not be the source of a two way binding
  EXPECT_THROW(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)),
               DataBinding::TwoWayBindingSourceException);
}


TEST(Test_GetRSet_TwoWay, SetBinding_DepProperty_DepProperty_ChangeFirstProperty_A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  // const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_THROW(
    t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0), DataBinding::BindingMode::TwoWay),
    DataBinding::TwoWayBindingReadOnlySourceException);
}
