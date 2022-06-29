#ifndef FSLDATABINDING_APP_DATABINDINGDEMOAPPEXTENSION_HPP
#define FSLDATABINDING_APP_DATABINDINGDEMOAPPEXTENSION_HPP
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

#include <FslDemoApp/Base/DemoAppExtension.hpp>
#include <memory>

namespace Fsl
{
  namespace DataBinding
  {
    class DataBindingService;
  }

  class DataBindingDemoAppExtension : public IDemoAppExtension
  {
    std::shared_ptr<DataBinding::DataBindingService> m_dataBinding;

  public:
    DataBindingDemoAppExtension();
    ~DataBindingDemoAppExtension() override;

    const std::shared_ptr<DataBinding::DataBindingService>& GetDataBinding() const;
    std::shared_ptr<DataBinding::DataBindingService> GetDataBinding();

    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    void OnRawMouseMoveEvent(const RawMouseMoveEvent& event) override;
    void OnTimeStateEvent(const TimeStateEvent& event) override;
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;

    void Begin(const DemoAppExtensionCallOrder callOrder) override;
    void PreUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void FixedUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void Update(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void PostUpdate(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void Resolve(const DemoAppExtensionCallOrder callOrder, const DemoTime& demoTime) override;
    void OnDrawSkipped(const DemoAppExtensionCallOrder callOrder, const FrameInfo& frameInfo) override;
    void End(const DemoAppExtensionCallOrder callOrder) override;
  };
}

#endif
