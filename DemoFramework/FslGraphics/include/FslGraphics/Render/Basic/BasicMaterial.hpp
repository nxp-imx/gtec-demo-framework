#ifndef FSLGRAPHICS_RENDER_BASIC_BASICMATERIAL_HPP
#define FSLGRAPHICS_RENDER_BASIC_BASICMATERIAL_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialHandle.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationArray.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationSpan.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariables.hpp>
#include <FslGraphics/Render/Basic/Material/IBasicMaterial.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>

namespace Fsl
{
  //! @brief By encapsulating the shared pointer in a moveable object we can later change the implementation to one that
  //!        does not depend on share pointers if need be.
  class BasicMaterial
  {
    struct PushConstants
    {
      float SdfSmooth{1.0f};
    };
    std::shared_ptr<IBasicMaterial> m_material;

    BasicMaterialHandle m_handle;
    PushConstants m_pushConstants;
    BasicMaterialVariableDeclarationArray<1> m_pushConstantDecl;

  public:
    BasicMaterial() noexcept = default;
    ~BasicMaterial() noexcept = default;
    BasicMaterial(const BasicMaterial&) = delete;
    BasicMaterial& operator=(const BasicMaterial&) = delete;
    BasicMaterial& operator=(BasicMaterial&& other) noexcept = default;
    BasicMaterial(BasicMaterial&& other) noexcept = default;

    float GetSdfSmooth() const
    {
      return m_pushConstants.SdfSmooth;
    }

    void SetSdfSmooth(const float value)
    {
      m_pushConstants.SdfSmooth = value;
    }

    explicit BasicMaterial(const std::shared_ptr<IBasicMaterial>& material, const BasicMaterialHandle handle)
      : m_material(material)
      , m_handle(handle)
      , m_pushConstantDecl(GetPushConstantDeclarationArray())
    {
      if (!material || !handle.IsValid())
      {
        throw std::invalid_argument("invalid args");
      }
    }

    bool IsValid() const noexcept
    {
      return m_material != nullptr;
    }

    BasicMaterialVariables AsPushConstants() const noexcept
    {
      return BasicMaterialVariables(&m_pushConstants, sizeof(PushConstants), AsPushConstantDeclarationSpan(), OptimizationCheckFlag::NoCheck);
    }


    static constexpr BasicMaterialVariableDeclarationArray<1> GetPushConstantDeclarationArray()
    {
      constexpr BasicMaterialVariableDeclarationArray<1> DeclArray(
        std::array<BasicMaterialVariableElement, 1>{
          BasicMaterialVariableElement(offsetof(PushConstants, SdfSmooth), BasicMaterialVariableElementFormat::Float1,
                                       BasicMaterialVariableElementUsage::SdfSmooth, 0),
        },
        sizeof(PushConstants));
      return DeclArray;
    }

    BasicMaterialVariableDeclarationSpan AsPushConstantDeclarationSpan() const noexcept
    {
      return m_pushConstantDecl.AsReadOnlySpan();
    }

    BasicMaterialHandle TryGetHandle() const noexcept
    {
      return m_handle;
    }

    BasicMaterialHandle GetHandle() const
    {
      return m_handle.IsValid() ? m_handle : throw UsageErrorException("handle is invalid");
    }
  };
}

#endif
