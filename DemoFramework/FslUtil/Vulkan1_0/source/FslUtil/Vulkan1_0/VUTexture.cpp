/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUTexture& VUTexture::operator = (VUTexture&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_image = std::move(other.m_image);
        m_sampler = std::move(other.m_sampler);

        // Remove the data from other
      }
      return *this;
    }


    VUTexture::VUTexture(VUTexture&& other)
      : m_image(std::move(other.m_image))
      , m_sampler(std::move(other.m_sampler))
    {
      // Remove the data from other
    }


    VUTexture::VUTexture()
      : m_image()
      , m_sampler()
    {
    }


    VUTexture::VUTexture(VUImage&& image, const VkSamplerCreateInfo& createInfo)
      : VUTexture()
    {
      Reset(std::move(image), createInfo);
    }


    VUTexture::VUTexture(VUImage&& image, RapidVulkan::Sampler&& sampler)
      : VUTexture()
    {
      Reset(std::move(image), std::move(sampler));
    }


    void VUTexture::Reset()
    {
      if (!IsValid())
        return;

      assert(m_image.IsValid());
      assert(m_sampler.IsValid());

      DoReset();
    }


    void VUTexture::Reset(VUImage&& image, const VkSamplerCreateInfo& createInfo)
    {
      if (IsValid())
        Reset();

      try
      {
        m_image = std::move(image);
        m_sampler.Reset(m_image.GetDevice(), createInfo);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }


    void VUTexture::Reset(VUImage&& image, RapidVulkan::Sampler&& sampler)
    {
      if (IsValid())
        Reset();

      try
      {
        m_image = std::move(image);
        m_sampler = std::move(sampler);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }


    void VUTexture::DoReset()
    {
      m_sampler.Reset();
      m_image.Reset();
    }
  }
}
