#ifndef FSLGRAPHICS_RENDER_BASIC_BASICSHADER_HPP
#define FSLGRAPHICS_RENDER_BASIC_BASICSHADER_HPP
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

#include <FslGraphics/Render/Basic/IBasicHandleManager.hpp>
#include <FslGraphics/Render/Basic/Shader/BasicShaderHandle.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace Fsl
{
  //! A basic shader RAII object that controls the lifetime of the basic shader
  class BasicShader
  {
    std::weak_ptr<IBasicHandleManager> m_handleManager;
    BasicShaderHandle m_handle;

  public:
    BasicShader() noexcept = default;


    ~BasicShader() noexcept
    {
      if (m_handle.IsValid())
      {
        auto handleManager = m_handleManager.lock();
        if (handleManager)
        {
          handleManager->DestroyHandle(m_handle);
          m_handle = {};
        }
      }
    }

    BasicShader(const BasicShader&) = delete;
    BasicShader& operator=(const BasicShader&) = delete;

    BasicShader& operator=(BasicShader&& other) noexcept = default;
    BasicShader(BasicShader&& other) noexcept = default;

    BasicShader(std::weak_ptr<IBasicHandleManager> handleManager, const BasicShaderHandle handle)
      : m_handleManager(std::move(handleManager))
      , m_handle(handle)
    {
      if (!handle.IsValid())
      {
        throw std::invalid_argument("invalid args");
      }
    }

    constexpr bool IsValid() const noexcept
    {
      return m_handle.IsValid();
    }

    constexpr BasicShaderHandle TryGetHandle() const noexcept
    {
      return m_handle;
    }
  };
}

#endif
