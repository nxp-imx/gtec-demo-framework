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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialVariableDeclarationArray.hpp>
#include <FslGraphics3D/BasicRender/Material/BasicMaterialManager.hpp>
#include <FslGraphics3D/BasicRender/Shader/IBasicShaderManager.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

namespace Fsl::Graphics3D
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto LogType = Fsl::LogType::Verbose5;
    }

    uint32_t FindIndexOfMaterial(const HandleVector<BasicMaterialRecord>& vector, const BasicMaterialCreateInfo createInfo,
                                 BasicShaderHandle createInfoVertexShaderHandle, const BasicShaderHandle createInfoFragmentShaderHandle,
                                 const std::shared_ptr<INativeTexture2D>& texture)
    {
      for (uint32_t i = 0; i < vector.Count(); ++i)
      {
        const BasicMaterialRecord& record = vector[i];
        if (record.Details.MaterialInfo == createInfo.MaterialInfo && record.Texture == texture &&
            record.Details.VertexShaderHandle == createInfoVertexShaderHandle &&
            record.Details.FragmentShaderHandle == createInfoFragmentShaderHandle &&
            record.Details.VertexDecl.AsSpan() == createInfo.VertexDeclaration)
        {
          return i;
        }
      }
      return vector.Count();
    }
  }

  BasicMaterialManager::BasicMaterialManager(const uint32_t maxFramesInFlight, std::shared_ptr<INativeMaterialFactory> factory,
                                             IBasicShaderManager& basicShaderManager)
    : m_maxFramesInFlight(maxFramesInFlight)
    , m_factory(std::move(factory))
    , m_basicShaderManager(basicShaderManager)
    , m_nativeMaterialManager(basicShaderManager)
  {
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::BasicMaterialManager({})", maxFramesInFlight);
    if (maxFramesInFlight <= 0)
    {
      throw std::invalid_argument("maxFramesInFlight needs to be at least 1");
    }
    if (!m_factory)
    {
      throw std::invalid_argument("factory can not be null");
    }
  }


  BasicMaterialManager::~BasicMaterialManager()
  {
    // Force free everything
    try
    {
      ForceFreeAll();
      FSLLOG3_WARNING_IF(!m_records.Empty(), "BasicMaterialManager: There are still {} material objects allocated", m_records.Count());
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("BasicMaterialManager::Destruct throw exception: {}", ex.what());
    }
  }


  void BasicMaterialManager::CreateDependentResources()
  {
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::CreateDependentResources()");
    // As we are currently creating dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
    CollectGarbage(0);

    try
    {    // Create native materials for all currently registered materials
      m_nativeMaterialManager.Init(m_factory, m_records.Count(), m_maxFramesInFlight);

      if (!m_records.Empty())
      {
        constexpr auto recordElementByteSize = HandleVector<BasicMaterialRecord>::ElementByteSize;
        m_nativeMaterialManager.CreateMaterials(
          TypedFlexSpan<BasicNativeMaterialRecord>(&m_records[0].Native, m_records.Count(), recordElementByteSize),
          ReadOnlyTypedFlexSpan<BasicMaterialDetailsRecord>(&m_records[0].Details, m_records.Count(), recordElementByteSize));
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Create dependent resources threw exception: {}", ex.what());
      DestroyDependentResources();
      throw;
    }
  }

  void BasicMaterialManager::DestroyDependentResources()
  {
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::DestroyDependentResources");

    // Remove the handle from the original records
    for (uint32_t i = 0; i < m_records.Count(); ++i)
    {
      m_records[i].Native = {};
    }

    m_nativeMaterialManager.Shutdown();

    // As we are currently destroying dependent resources, we dont have any rendering operation pending, so we can just use a defer count of zero
    CollectGarbage(0, true);
  }


  void BasicMaterialManager::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
  {
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::OnRenderSystemEvent({})", static_cast<int>(theEvent));
    switch (theEvent)
    {
    case BasicRenderSystemEvent::SwapchainLost:
      // We know the device is idle when this occurs so we can just force free everything (and therefore also use a defer count of zero)
      CollectGarbage(0, true);
      break;
    case BasicRenderSystemEvent::SwapchainRecreated:
      break;
    default:
      FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
      break;
    }
  }

  BasicMaterial BasicMaterialManager::CreateMaterial(const BasicMaterialCreateInfo& createInfo, std::shared_ptr<INativeTexture2D> texture,
                                                     const bool isDynamic)
  {
    if (!createInfo.IsValid())
    {
      throw std::invalid_argument("CreateMaterial: createInfo is invalid");
    }
    if (!texture)
    {
      throw std::invalid_argument("CreateMaterial: texture can not be null");
    }
    assert(m_factory);

    // Lookup the actual shaders handles if no shader is supplied.
    const auto fragmentShaderType =
      createInfo.MaterialInfo.Blend != BlendState::Sdf ? PredefinedShaderType::Fragment : PredefinedShaderType::FragmentSdf;
    const BasicShaderHandle actualVertexShaderHandle = createInfo.CustomVertexShader.IsValid()
                                                         ? createInfo.CustomVertexShader
                                                         : m_basicShaderManager.QueryPredefinedShaderHandle(PredefinedShaderType::Vertex);
    const BasicShaderHandle actualFragmentShaderHandle = createInfo.CustomFragmentShader.IsValid()
                                                           ? createInfo.CustomFragmentShader
                                                           : m_basicShaderManager.QueryPredefinedShaderHandle(fragmentShaderType);

    const uint32_t materialIndex =
      !isDynamic ? FindIndexOfMaterial(m_records, createInfo, actualVertexShaderHandle, actualFragmentShaderHandle, texture) : m_records.Count();

    auto hMaterial = HandleVectorConfig::InvalidHandle;
    std::shared_ptr<BasicMaterialTracker> materialTracker;
    if (materialIndex >= m_records.Count())
    {
      // For now we just support one material type, but the backend material creation interface is flexible
      // If we introduce support for more material types then the FindIndexOfMaterial code needs to be updated as well

      constexpr auto pushConstantDeclArray = BasicMaterial::GetPushConstantDeclarationArray();
      auto pushConstantDeclSpan = pushConstantDeclArray.AsReadOnlySpan();

      // The given configuration do not exist, creating new material
      materialTracker = std::make_shared<BasicMaterialTracker>();


      const BasicShaderHandle hVertexShader = m_basicShaderManager.ReferenceShader(actualVertexShaderHandle);
      try
      {
        const BasicShaderHandle hFragShader = m_basicShaderManager.ReferenceShader(actualFragmentShaderHandle);
        try
        {
          hMaterial = m_records.Add(
            BasicMaterialRecord(createInfo, hVertexShader, hFragShader, std::move(texture), isDynamic, pushConstantDeclSpan, materialTracker));
        }
        catch (const std::exception&)
        {
          m_basicShaderManager.DestroyShader(hFragShader);
          throw;
        }
      }
      catch (const std::exception&)
      {
        m_basicShaderManager.DestroyShader(hVertexShader);
        throw;
      }
    }
    else
    {
      // we found a existing configuration, reusing the material
      BasicMaterialRecord& rRecord = m_records[materialIndex];
      assert(!isDynamic);
      assert(rRecord.IsDynamic == false);
      hMaterial = m_records.FastIndexToHandle(materialIndex);
      materialTracker = rRecord.BasicUserObjectTracker.lock();
      if (materialTracker)
      {
        // The user object is still valid so we can just reuse it
        FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::CreateMaterial reusing material and user-object");
      }
      else
      {
        FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::CreateMaterial reusing material, recreating user-object");
        materialTracker = std::make_shared<BasicMaterialTracker>();
        rRecord.BasicUserObjectTracker = materialTracker;
      }
    }

    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager: CreateMaterial({}) count: {}", hMaterial, m_records.Count());

    if (m_nativeMaterialManager.IsValid())
    {
      const auto newMaterialIndex = m_records.FastHandleToIndex(hMaterial);
      m_records[newMaterialIndex].Native = m_nativeMaterialManager.CreateMaterial(m_records[newMaterialIndex].Details);
    }
    return BasicMaterial(materialTracker, BasicMaterialHandle(hMaterial));
  }


  BasicMaterial BasicMaterialManager::CloneMaterial(const BasicMaterial& sourceMaterial, const std::shared_ptr<INativeTexture2D>& texture,
                                                    const bool isDynamic)
  {
    BasicMaterialHandle hSource = sourceMaterial.TryGetHandle();
    const BasicMaterialRecord& record = m_records.Get(hSource.Value);
    BasicMaterialCreateInfo createInfo(record.Details.MaterialInfo, record.Details.VertexDecl.AsSpan());
    return CreateMaterial(createInfo, texture, record.IsDynamic || isDynamic);
  }


  BasicMaterial BasicMaterialManager::CloneMaterial(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo, const bool isDynamic)
  {
    BasicMaterialHandle hSource = sourceMaterial.TryGetHandle();
    const BasicMaterialRecord& record = m_records.Get(hSource.Value);
    BasicMaterialCreateInfo createInfo(materialInfo, record.Details.VertexDecl.AsSpan());
    return CreateMaterial(createInfo, record.Texture, record.IsDynamic || isDynamic);
  }


  BasicMaterialInfo BasicMaterialManager::GetMaterialInfo(const BasicMaterial& sourceMaterial) const
  {
    BasicMaterialHandle hSource = sourceMaterial.TryGetHandle();
    const BasicMaterialRecord& record = m_records.Get(hSource.Value);
    return record.Details.MaterialInfo;
  }


  void BasicMaterialManager::SetMaterialInfo(const BasicMaterial& sourceMaterial, const BasicMaterialInfo& materialInfo)
  {
    BasicMaterialHandle hSource = sourceMaterial.TryGetHandle();
    const BasicMaterialRecord* pRecord = m_records.TryGet(hSource.Value);
    if (pRecord == nullptr)
    {
      throw std::invalid_argument("Unknown source material");
    }

    BasicMaterialRecord& rRecord = m_records.Get(hSource.Value);
    if (!rRecord.IsDynamic)
    {
      throw UsageErrorException("SetMaterialInfo can only be called on materials marked as dynamic");
    }

    if (materialInfo != rRecord.Details.MaterialInfo)
    {
      rRecord.Details.MaterialInfo = materialInfo;

      // If we have native resources
      if (m_nativeMaterialManager.IsValid())
      {
        // Destroy the old material
        if (rRecord.Native.IsValid())
        {
          m_nativeMaterialManager.ScheduleRemove(rRecord.Native.InternalHandle);
          rRecord.Native = {};
        }
        // Create a new one
        rRecord.Native = m_nativeMaterialManager.CreateMaterial(rRecord.Details);
      }
    }
  }


  std::shared_ptr<INativeTexture2D> BasicMaterialManager::TryGetMaterialTexture(const BasicMaterial& sourceMaterial)
  {
    BasicMaterialHandle hSource = sourceMaterial.TryGetHandle();
    const BasicMaterialRecord* pRecord = m_records.TryGet(hSource.Value);
    return pRecord != nullptr ? pRecord->Texture : std::shared_ptr<INativeTexture2D>();
  }


  BasicNativeMaterialHandle BasicMaterialManager::TryGetNativeHandle(const BasicMaterial& material) const
  {
    BasicMaterialHandle handle = material.TryGetHandle();
    const BasicMaterialRecord* pRecord = m_records.TryGet(handle.Value);
    return pRecord != nullptr ? pRecord->Native.NativeHandle : BasicNativeMaterialHandle::Invalid();
  }

  void BasicMaterialManager::PreUpdate()
  {
    // If the dependent resources are invalid then we can instantly collect all garbage
    const uint32_t deferCount = m_nativeMaterialManager.IsValid() ? m_maxFramesInFlight : 0u;
    CollectGarbage(deferCount);
    // Give the native material manager a chance to cleanup
    m_nativeMaterialManager.PreUpdate();
  }

  void BasicMaterialManager::CollectGarbage(const uint32_t deferCount, const bool force)
  {
    FSL_PARAM_NOT_USED(deferCount);
    FSL_PARAM_NOT_USED(force);

    if (m_records.Empty())
    {
      return;
    }
    const auto initialMaterialCount = m_records.Count();
    // Do the actual freeing of the material
    for (uint32_t indexOneTooLarge = m_records.Count(); indexOneTooLarge > 0; --indexOneTooLarge)
    {
      assert(indexOneTooLarge > 0);
      uint32_t i = indexOneTooLarge - 1;
      BasicMaterialRecord& rRecord = m_records[i];

      if (rRecord.BasicUserObjectTracker.expired())
      {
        FSLLOG3(LocalConfig::LogType, "BasicMaterialManager: Destroying material ({},{},{})", m_records.FastIndexToHandle(i),
                rRecord.Native.InternalHandle.Value, rRecord.Native.NativeHandle.Value);
        if (rRecord.Native.IsValid())
        {
          m_nativeMaterialManager.ScheduleRemove(rRecord.Native.InternalHandle);
          rRecord.Native = {};
        }
        if (rRecord.Details.VertexShaderHandle.IsValid())
        {
          m_basicShaderManager.DestroyShader(rRecord.Details.VertexShaderHandle);
          rRecord.Details.VertexShaderHandle = {};
        }
        if (rRecord.Details.FragmentShaderHandle.IsValid())
        {
          m_basicShaderManager.DestroyShader(rRecord.Details.FragmentShaderHandle);
          rRecord.Details.FragmentShaderHandle = {};
        }
        m_records.RemoveAt(i);
      }
    }
    assert(initialMaterialCount >= m_records.Count());
    FSLLOG3_VERBOSE4_IF(initialMaterialCount != m_records.Count(),
                        "BasicMaterialManager: Garbage collected {} materials. There are {} active materials.",
                        initialMaterialCount - m_records.Count(), m_records.Count());
  }

  void BasicMaterialManager::ForceFreeAll()
  {
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager: ForceFreeAll begin {} materials", m_records.Count());

    if (m_nativeMaterialManager.IsValid())
    {
      FSLLOG3_WARNING("BasicMaterialManager: Dependent resources still allocated, trying to free them");
      m_nativeMaterialManager.Shutdown();
    }

    if (!m_records.Empty())
    {
      // Force free all normal resources
      for (uint32_t i = 0; i < m_records.Count(); ++i)
      {
        FSLLOG3(Fsl::LogType::Verbose5, "Destroying material ({})", m_records.FastIndexToHandle(i));
        if (m_records[i].Details.VertexShaderHandle.IsValid())
        {
          m_basicShaderManager.DestroyShader(m_records[i].Details.VertexShaderHandle);
          m_records[i].Details.VertexShaderHandle = {};
        }
        if (m_records[i].Details.FragmentShaderHandle.IsValid())
        {
          m_basicShaderManager.DestroyShader(m_records[i].Details.FragmentShaderHandle);
          m_records[i].Details.FragmentShaderHandle = {};
        }
      }
      m_records.Clear();
    }
    FSLLOG3(LocalConfig::LogType, "BasicMaterialManager::ForceFreeAll done {} materials", m_records.Count());
  }
}
