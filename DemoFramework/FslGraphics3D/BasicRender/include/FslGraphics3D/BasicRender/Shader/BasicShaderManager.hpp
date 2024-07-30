#ifndef FSLGRAPHICS3D_BASICRENDER_SHADER_BASICSHADERMANAGER_HPP
#define FSLGRAPHICS3D_BASICRENDER_SHADER_BASICSHADERMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslGraphics/Render/Basic/Shader/BasicShaderCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Shader/BasicShaderHandle.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeShaderFactory.hpp>
#include <FslGraphics3D/BasicRender/Shader/BasicShaderRecord.hpp>
#include <FslGraphics3D/BasicRender/Shader/IBasicShaderManager.hpp>
#include <FslGraphics3D/BasicRender/Shader/PredefinedShaderType.hpp>
#include <array>
#include <memory>
#include <utility>

namespace Fsl::Graphics3D
{
  //! Manage the lifetime of all native shaders.
  //! As shaders can be destroyed once a pipeline has been created we do not need to defer the destruction once we know no material needs it anymore
  class BasicShaderManager final : public IBasicShaderManager
  {
    struct PredefinedShaderRecord
    {
      BasicShaderHandle Handle;

      PredefinedShaderRecord() noexcept = default;
      explicit PredefinedShaderRecord(const BasicShaderHandle handle) noexcept
        : Handle(handle)
      {
      }
    };

    std::array<PredefinedShaderRecord, 5> m_predefinedShaders;
    std::shared_ptr<INativeShaderFactory> m_factory;
    HandleVector<BasicShaderRecord> m_records;

  public:
    explicit BasicShaderManager(std::shared_ptr<INativeShaderFactory> factory);
    ~BasicShaderManager() noexcept override;


    BasicShaderHandle CreateShader(const BasicShaderCreateInfo& createInfo);
    BasicShaderHandle QueryPredefinedShaderHandle(const PredefinedShaderType shaderType) final;
    BasicShaderHandle ReferenceShader(const BasicShaderHandle hShader) final;
    bool DestroyShader(const BasicShaderHandle hShader) noexcept final;

    const BasicShaderRecord* TryGetShaderRecord(const BasicShaderHandle handle) noexcept
    {
      return m_records.TryGet(handle.Value);
    }

    BasicNativeShaderHandle TryGetNativeHandle(const BasicShaderHandle handle) const noexcept final
    {
      const BasicShaderRecord* pRecord = m_records.TryGet(handle.Value);
      return pRecord != nullptr ? pRecord->NativeHandle : BasicNativeShaderHandle::Invalid();
    }

    uint32_t ShaderCount() const noexcept
    {
      return m_records.Count();
    }

  private:
    void ForceFreeAll() noexcept;
  };
}

#endif
