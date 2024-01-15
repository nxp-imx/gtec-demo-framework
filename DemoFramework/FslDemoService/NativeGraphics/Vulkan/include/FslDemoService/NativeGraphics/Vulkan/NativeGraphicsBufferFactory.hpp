#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSBUFFERFACTORY_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSBUFFERFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsBufferRecord.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBufferHandle.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeBufferFactory.hpp>
#include <memory>

namespace Fsl::Vulkan
{
  class VMBufferManager;

  class NativeGraphicsBufferFactory
  {
    std::shared_ptr<VMBufferManager> m_bufferManager;

    HandleVector<NativeGraphicsBufferRecord> m_buffers;

    bool m_isDisposed{false};

  public:
    explicit NativeGraphicsBufferFactory(std::shared_ptr<VMBufferManager> bufferManager);
    ~NativeGraphicsBufferFactory() noexcept;

    void Dispose() noexcept;

    Graphics3D::NativeBufferFactoryCaps GetBufferCaps() const noexcept;

    BasicNativeBufferHandle CreateBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData, const uint32_t bufferElementCapacity,
                                         const bool isDynamic);

    bool DestroyBuffer(const BasicNativeBufferHandle hBuffer) noexcept;

    void SetBufferData(const BasicNativeBufferHandle hBuffer, const uint32_t dstIndex, ReadOnlyFlexSpan bufferData);

    const VMBuffer& GetBuffer(const BasicNativeBufferHandle buffer) noexcept
    {
      return m_buffers.Get(buffer.Value).GetBuffer();
    }
  };
}

#endif
