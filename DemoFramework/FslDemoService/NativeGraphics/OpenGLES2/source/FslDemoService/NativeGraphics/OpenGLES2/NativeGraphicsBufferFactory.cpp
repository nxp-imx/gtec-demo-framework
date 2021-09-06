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

#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsBufferFactory.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <utility>

namespace Fsl
{
  namespace GLES2
  {
    NativeGraphicsBufferFactory::NativeGraphicsBufferFactory()
    {
      FSLLOG3_VERBOSE3("NativeGraphicsBufferFactory::Construct");
    }

    NativeGraphicsBufferFactory::~NativeGraphicsBufferFactory() noexcept
    {
      FSLLOG3_VERBOSE3("NativeGraphicsBufferFactory::Destruct");
      Dispose();
    }


    Graphics3D::NativeBufferFactoryCaps NativeGraphicsBufferFactory::GetBufferCaps() const
    {
      return Graphics3D::NativeBufferFactoryCaps::Dynamic;
    }

    void NativeGraphicsBufferFactory::Dispose() noexcept
    {
      if (m_isDisposed)
      {
        return;
      }
      FSLLOG3_VERBOSE3("NativeGraphicsBufferFactory::Dispose");
      m_isDisposed = true;
      if (m_buffers.Count() > 0)
      {
        try
        {
          FSLLOG3_WARNING("NativeGraphicsBufferFactory: There are still {} allocated buffers, force freeing them", m_buffers.Count());
          m_buffers.Clear();
        }
        catch (const std::exception& ex)
        {
          // This is the best we can do since this is not supposed to occur
          FSLLOG3_ERROR("NativeGraphicsBufferFactory consumed exception during force free: {}", ex.what());
        }
      }
    }


    BasicNativeBufferHandle NativeGraphicsBufferFactory::CreateBuffer(const BasicBufferType bufferType, ReadOnlyFlexSpan bufferData,
                                                                      const uint32_t bufferElementCapacity, const bool isDynamic)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsBufferFactory::CreateBuffer");
      if (m_isDisposed)
      {
        throw UsageErrorException("Trying to create buffer after dispose");
      }
      if (bufferData.size() > bufferElementCapacity)
      {
        throw NotSupportedException("bufferData does not fit within bufferElementCapacity");
      }

      auto handle = m_buffers.Add(NativeGraphicsBufferRecord(bufferType, bufferData, bufferElementCapacity, isDynamic));
      FSLLOG3_VERBOSE6("NativeGraphicsBufferFactory::CreateBuffer handle: {}", handle);
      return BasicNativeBufferHandle(handle);
    }

    bool NativeGraphicsBufferFactory::DestroyBuffer(const BasicNativeBufferHandle hBuffer)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsBufferFactory::DestroyBuffer({})", hBuffer.Value);
      if (m_isDisposed)
      {
        FSLLOG3_ERROR("hBuffer is invalid, call ignored");
        return false;
      }
      if (!m_buffers.Remove(hBuffer.Value))
      {
        FSLLOG3_ERROR("tried to free a unknown buffer, call ignored");
        return false;
      }
      return true;
    }

    void NativeGraphicsBufferFactory::SetBufferData(const BasicNativeBufferHandle hBuffer, const uint32_t dstIndex, ReadOnlyFlexSpan bufferData)
    {
      FSLLOG3_VERBOSE6("NativeGraphicsBufferFactory::SetBufferData({})", hBuffer.Value);
      if (m_isDisposed)
      {
        throw UsageErrorException("hBuffer is invalid, call ignored");
      }

      NativeGraphicsBufferRecord& rRecord = m_buffers.Get(hBuffer.Value);
      rRecord.SetBufferData(dstIndex, bufferData);
    }
  }
}
