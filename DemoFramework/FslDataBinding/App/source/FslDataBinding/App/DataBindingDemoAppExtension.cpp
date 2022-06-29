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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDataBinding/App/DataBindingDemoAppExtension.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>

namespace Fsl
{
  DataBindingDemoAppExtension::DataBindingDemoAppExtension()
    : m_dataBinding(std::make_shared<DataBinding::DataBindingService>())
  {
  }


  DataBindingDemoAppExtension::~DataBindingDemoAppExtension()
  {
    m_dataBinding->MarkShutdownIntend();
  }

  const std::shared_ptr<DataBinding::DataBindingService>& DataBindingDemoAppExtension::GetDataBinding() const
  {
    return m_dataBinding;
  }

  std::shared_ptr<DataBinding::DataBindingService> DataBindingDemoAppExtension::GetDataBinding()
  {
    return m_dataBinding;
  }


  void DataBindingDemoAppExtension::OnKeyEvent(const KeyEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::OnTimeStateEvent(const TimeStateEvent& event)
  {
    FSL_PARAM_NOT_USED(event);
  }

  void DataBindingDemoAppExtension::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }

  void DataBindingDemoAppExtension::Begin(const DemoAppExtensionCallOrder callOrder)
  {
    if (callOrder == DemoAppExtensionCallOrder::PostApp)
    {
      m_dataBinding->ExecuteChanges();
    }
  }

  void DataBindingDemoAppExtension::PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_dataBinding->ExecuteChanges();
    }
  }

  void DataBindingDemoAppExtension::FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_dataBinding->ExecuteChanges();
    }
  }

  void DataBindingDemoAppExtension::Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_dataBinding->ExecuteChanges();
    }
  }

  void DataBindingDemoAppExtension::PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_dataBinding->ExecuteChanges();
    }
  }

  void DataBindingDemoAppExtension::Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    if (callOrder == DemoAppExtensionCallOrder::PreApp)
    {
      m_dataBinding->ExecuteChanges();
    }

    if (callOrder == DemoAppExtensionCallOrder::PostApp && m_dataBinding->PendingChanges() > 0)
    {
      FSLLOG3_WARNING("Pending changes found after 'Resolve' this should not happen");
    }
  }

  void DataBindingDemoAppExtension::OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);
    if (m_dataBinding->PendingChanges() > 0)
    {
      FSLLOG3_WARNING_IF(callOrder == DemoAppExtensionCallOrder::PreApp, "Pending changes found before 'OnDrawSkipped' this should not happen");
      FSLLOG3_WARNING_IF(callOrder == DemoAppExtensionCallOrder::PostApp, "Pending changes found after 'OnDrawSkipped' this should not happen");
    }
  }

  void DataBindingDemoAppExtension::End(const DemoAppExtensionCallOrder callOrder)
  {
    FSL_PARAM_NOT_USED(callOrder);
  }

}
