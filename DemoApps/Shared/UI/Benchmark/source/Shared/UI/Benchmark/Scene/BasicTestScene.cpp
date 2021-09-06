/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include "BasicTestScene.hpp"
#include <Shared/UI/Benchmark/App/TestAppFactory.hpp>
#include <Shared/UI/Benchmark/App/TestAppHost.hpp>
#include <Shared/UI/Benchmark/UIModule/ICustomWindowInfoModule.hpp>
#include "SceneCreateInfo.hpp"
#include "SceneDemoAppExtensionProxy.hpp"
#include "TestHostForwarderTweak.hpp"

namespace Fsl
{
  BasicTestScene::BasicTestScene(const SceneCreateInfo& createInfo)
    : BasicScene(createInfo)
    , m_forwarder(createInfo.Forwarder)
    , m_testAppHost(std::make_shared<TestAppHost>(createInfo.DemoServiceProvider, m_windowMetrics, true))
    , m_testAppHostExtensionProxy(std::make_shared<SceneDemoAppExtensionProxy>(m_testAppHost->GetWindowInfoModule(), m_testAppHost))
  {
    m_forwarder->SetExtension1(m_testAppHostExtensionProxy);
    m_forwarder->SetTweak(std::make_shared<TestHostForwarderTweak>(m_rootLayout, m_testAppHost));
  }


  BasicTestScene::~BasicTestScene()
  {
    m_forwarder->ClearTweak();
    m_forwarder->ClearExtension1();
    m_testAppHostExtensionProxy.reset();
  }


  void BasicTestScene::OnFrameSequenceBegin()
  {
    BasicScene::OnFrameSequenceBegin();
    m_testAppHost->ClearProfileData();
  }


  void BasicTestScene::Update(const DemoTime& demoTime)
  {
    BasicScene::Update(demoTime);
    m_testAppHost->AppUpdate(demoTime);
  }

  void BasicTestScene::Draw(const DemoTime& demoTime)
  {
    BasicScene::Draw(demoTime);
    m_testAppHost->AppDraw(demoTime);
  }


  const ISceneTestDemoControl& BasicTestScene::GetDemoControl() const
  {
    return *m_testAppHostExtensionProxy;
  }


  ISceneTestDemoControl& BasicTestScene::GetDemoControl()
  {
    return *m_testAppHostExtensionProxy;
  }
}
