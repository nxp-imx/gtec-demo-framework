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

#include <FslBase/Math/BasicWindowMetrics.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslSimpleUI/Base/System/UIManager.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBufferEx.hpp>
#include <FslSimpleUI/Render/Stub/RenderSystem.hpp>

using namespace Fsl;

namespace
{
  class TestUIManager : public TestFixtureFslBase
  {
  protected:
    std::shared_ptr<Fsl::DataBinding::DataBindingService> m_dataBindingService;
    Fsl::UI::UIManager m_manager;

  public:
    TestUIManager()
      : m_dataBindingService(std::make_shared<Fsl::DataBinding::DataBindingService>())
      , m_manager(m_dataBindingService, std::make_unique<UI::RenderStub::RenderSystem>(), false,
                  BasicWindowMetrics(PxExtent2D::Create(800, 600), Vector2(160, 160), 160))
    {
    }
  };
}


TEST_F(TestUIManager, Construct)
{
}


TEST_F(TestUIManager, GetWindowManager)
{
  auto winManager = m_manager.GetWindowManager();
  ASSERT_TRUE(winManager);
}


TEST_F(TestUIManager, GetEventPool)
{
  auto pool = m_manager.GetEventPool();
  ASSERT_TRUE(pool);
}


TEST_F(TestUIManager, GetEventSender)
{
  auto eventSender = m_manager.GetEventSender();
  ASSERT_TRUE(eventSender);
}


TEST_F(TestUIManager, SendMouseButtonEventEmpty)
{
  auto isHandled = m_manager.SendMouseButtonEvent(PxPoint2::Create(0, 0), true, false);

  ASSERT_FALSE(isHandled);
}


TEST_F(TestUIManager, SendMouseMoveEventEmpty)
{
  auto pos = PxPoint2::Create(0, 0);
  auto isHandled = m_manager.SendMouseMoveEvent(pos, false);

  ASSERT_FALSE(isHandled);
}


TEST_F(TestUIManager, ResizedEmpty)
{
  m_manager.Resized(BasicWindowMetrics(PxExtent2D::Create(640, 480), Vector2(160, 160), 160));
}


TEST_F(TestUIManager, FixedUpdateEmpty)
{
  m_manager.FixedUpdate(TimeSpan::FromMicroseconds(1));
}


TEST_F(TestUIManager, UpdateEmpty)
{
  m_manager.Update(TimeSpan::FromMicroseconds(1));
}


TEST_F(TestUIManager, DrawEmpty)
{
  // m_manager.Draw();
  FSLLOG3_WARNING("DrawEmpty disabled");
}


// TEST_F(Test_UIManager, RegisterEventListener)
//{
//  // void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
//  m_manager.RegisterEventListener();
//}

// TEST_F(Test_UIManager, UnregisterEventListener)
//{
//  // void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
//  m_manager.UnregisterEventListener();
//}
